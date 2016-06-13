

#ifndef Btree_hpp
#define Btree_hpp

#include <stdio.h>
#include <algorithm>
#include <iostream>
#include "BufferFrame.hpp"
#include "BufferManager.hpp"

#include "TID.hpp"
/* Implement a B+-Tree index for your database system. Your tree should support different (opaque) key types. Parameterize the B+-Tree with a key type and a comparator. You can assume that keys are unique and all key types have fixed length. The B+-Trees must support the following reentrant operations:
    insert: Inserts a new key/TID pair into the tree.
    erase: Deletes a specified key. You may simplify the logic by accepting under full pages.
    lookup: Returns a TID or indicates that the key was not found.*/

template <class K, class LESS = std::less<K>>
class BTree{
    LESS less;
    
    int id;
    
    class Node {
    public:
        
        // the node is a leaf
        bool leaf;
        // number of entries
        uint32_t entries;
        
        Node(bool leaf) : leaf(leaf), entries(0) {};
        
        virtual bool isFull() = 0;
        bool isLeaf() { return leaf; }
    };
    
    class InnerNode : public Node {

        static const size_t n = (blocksize - sizeof(Node) - sizeof(uint64_t)) / (sizeof(K) + sizeof(uint64_t));
        
        K keys[n];
        uint64_t children[n+1];
        
        // inner node is not a leaf
        InnerNode() : Node(false) {};
        
    public:
        InnerNode(uint64_t leftChildNode, uint64_t rightChildNode, K seperator) : Node(false) {
            children[0] = leftChildNode;
            children[1] = rightChildNode;
            keys[0] = seperator;
            (*this).entries = 2;
        };
        
        bool isFull() {
            return (*this).entries == n+1;
        }

        K maxKey() {
            return keys[(*this).entries-2];
        }
        
        unsigned getKeyIndex(K key) {
            LESS less;
            return std::lower_bound(keys, keys+(*this).entries-1, key, less)-keys;
        }
        
        uint64_t getChild(K key) {
            return children[getKeyIndex(key)];
        }
        
        // get middle for the seperator entry
        K split(BufferFrame& bufferFrame) {
            InnerNode* innerNode = new (bufferFrame.getData()) InnerNode();
            
            // get the middle
            unsigned  middle = (*this).entries / 2;
            (*this).entries -= middle;
            (*innerNode).entries = middle;
            
            memcpy((*innerNode).keys, keys+(*this).entries, (middle-1) * sizeof(K));
            memcpy((*innerNode).children, children+(*this).entries, middle * sizeof(uint64_t));
            
            // return the middle
            return keys[(*this).entries-1];
        }
        
        void insert(K key, uint64_t child) {
            unsigned int i = getKeyIndex(key);
            
            if (i < (*this).entries - 1) {
                // is it less?
                LESS less;
                if (!less(key, keys[i])) {
                    // overwriting
                    children[i] = child;
                    return;
                } else {
                    // move the entries
                    memmove(keys+i+1, keys+i, ((*this).entries-i-1) * sizeof(K));
                    memmove(children+i+2, children+i+1, ((*this).entries-i-1) * sizeof(uint64_t));
                }
            }
        
            // insert
            keys[i] = key;
            children[i+1] = child;
            (*this).entries++;
        }
    };
    
    class LeafNode : public Node {
        // calculate n
        static const size_t n = (blocksize - sizeof(Node)) / (sizeof(K) + sizeof(TID));
        
        K keys[n];
        TID tids[n];
        
    public:
        LeafNode() : Node(true) {};
        
        inline bool isFull() {
            return (*this).entries == n;
        }
        
        inline K maxKey() {
            return keys[(*this).entries-1];
        }
        
        // returns index of the key which is smaller than the given key
        unsigned getKeyIndex(K key) {
            LESS less;
            return std::lower_bound(keys, keys+(*this).entries, key, less)-keys;
        }
        
        bool getTid(K key, TID& tid) {
            unsigned i = getKeyIndex(key);
            
            LESS less;
            
            if(i == (*this).entries || less(key, keys[i])) {
                return false;
            }
            
            tid = tids[i];
            return true;
        }
        
        void insert(K key, TID tid) {

            unsigned i = getKeyIndex(key);
            if (i < (*this).entries) {
                LESS less;
                // is it less?
                if (!less(key, keys[i])) {
                    // overwriting
                    tids[i] = tid;
                    return;
                } else {
                    // move the entries
                    memmove(keys+i+1, keys+i, ((*this).entries-i) * sizeof(K));
                    memmove(tids+i+1, tids+i, ((*this).entries-i) * sizeof(TID));
                }
            }
            
            // insert
            keys[i] = key;
            tids[i] = tid;
            (*this).entries++;
        }

        bool eraseLeafNode(K key) {
          
            unsigned i = getKeyIndex(key);
            if (i == (*this).entries) {
                // key not found
                return false;
            }
            
            memmove(keys+i, keys+i+1, ((*this).entries-i-1) * sizeof(K));
            memmove(tids+i, tids+i+1, ((*this).entries-i-1) * sizeof(TID));
            (*this).entries--;
            return true;
        }
        
        K split(BufferFrame& bf) {
            LeafNode* leafNode = new (bf.getData()) LeafNode();
            
            unsigned middle = (*this).entries / 2;
            (*this).entries    -= middle;
            (*leafNode).entries  = middle;
            
            // move the entries to the new leaf
            K*   keyStart = keys+(*this).entries;
            TID* tidStart = tids+(*this).entries;
            std::copy(keyStart, keyStart+middle, (*leafNode).keys);
            std::copy(tidStart, tidStart+middle, (*leafNode).tids);
            
            return maxKey();
        }
    };
    
    
public:

    BufferManager& bm;
    BTree(BufferManager& bm, uint64_t id): bm(bm), root(0){
        
        std::cout << "Btree created" << std::endl;
        
        // builds a new empty tree
        BufferFrame& bufferFrame = bm.fixPage(root, true);
        void* dataPointer = bufferFrame.getData();
        new (dataPointer) LeafNode();
        bm.unfixPage(bufferFrame, true);
        id = 1;
    };
    
    /* insert: Inserts a new key/TID pair into the tree.
     
     Perform a search to determine what bucket the new record should go into.
     
     If the bucket is not full (at most b - 1 entries after the insertion), add the record.
     Otherwise, split the bucket.
     Allocate new leaf and move half the bucket's elements to the new bucket.
     Insert the new leaf's smallest key and address into the parent.
     If the parent is full, split it too.
     Add the middle key to the parent node.
     Repeat until a parent is found that need not split.
     If the root splits, create a new root which has one key and two pointers. */
    
    
    void insert(K key, TID tid) {
        std::cout << "insert" << std::endl;
        BufferFrame* bufferFrame = &bm.fixPage(root, true);
        bool bufferFrameDirty = false;
        Node* node = static_cast<Node*>((*bufferFrame).getData());
        
        // the root has no parent node
        BufferFrame* bufferFrameParent = NULL;
        bool parentDirty = false;
        
        // split all full nodes
        while(true) {
            //std::cout << "here1" << std::endl;
            
            // if the node is full, split it
            if ((*node).isFull()) {
                // make a new page for the splitting
                //std::cout << "if" << std::endl;
                uint64_t     newId = id++;
                BufferFrame* newBufferFrame = &bm.fixPage(newId, true);
                
                K separator;
                if ((*node).isLeaf()) {
                    // make a new leaf and move half of the entries
                    LeafNode* oldLeaf = reinterpret_cast<LeafNode*>(node);
                    separator = (*oldLeaf).split(*newBufferFrame);
                } else {
                    // make a new inner node and move half of the entries
                    InnerNode* oldInnerNode = reinterpret_cast<InnerNode*>(node);
                    separator = (*oldInnerNode).split(*newBufferFrame);
                }
                
                // is it the root?
                if (bufferFrameParent == NULL) {
                    // make page for the old root
                    uint64_t moveID = id++;
                    BufferFrame* bfMove = &bm.fixPage(moveID, true);
                    
                    // move the root to the page
                    memcpy((*bfMove).getData(), (*bufferFrame).getData(), blocksize);
                    
                    // make a new root and make the old root the child left of it
                    new ((*bufferFrame).getData()) InnerNode(id, (*newBufferFrame).getID(), separator);
                    
                    bufferFrameParent = bufferFrame;
                    bufferFrame = bfMove;
                    bufferFrameDirty  = true;
                    parentDirty = true;
                } else {
                    // update the parent
                    InnerNode* parent = static_cast<InnerNode*>((*bufferFrameParent).getData());
                    
                    (*parent).insert(separator, (*newBufferFrame).getID());
                    bufferFrameDirty  = true;
                    parentDirty = true;
                }
                
                // insert
                if (!less(separator, key)) {
                    bm.unfixPage(*newBufferFrame, true);
                    newBufferFrame = NULL;
                } else {
                    bm.unfixPage(*newBufferFrame, true);
                    bufferFrame = newBufferFrame;
                }
                node  = static_cast<Node*>((*bufferFrame).getData());
                
                // not full
            } else {
                //std::cout << "else" << std::endl;
                
                if ((*node).isLeaf()) {
                    // the leaf where the entry has to be inserted
                    LeafNode* leaf = reinterpret_cast<LeafNode*>(node);
                    (*leaf).insert(key, tid);
                    
                    if (bufferFrameParent != NULL) {
                        bm.unfixPage(*bufferFrameParent, parentDirty);
                    }
                    
                    bm.unfixPage(*bufferFrame, true);
                    return;
                    // no leaf
                } else {
                    
                    InnerNode* inner  = reinterpret_cast<InnerNode*>(node);
                    uint64_t   nextID = (*inner).getChild(key);
                    
                    // lock coupling
                    BufferFrame* newBufferFrame = &bm.fixPage(nextID, true);
                    if (bufferFrameParent != NULL) {
                        bm.unfixPage(*bufferFrameParent, parentDirty);
                    }
                    
                    bufferFrameParent = bufferFrame;
                    bufferFrame = newBufferFrame;
                    parentDirty = bufferFrameDirty;
                    parentDirty = false;
                    
                    node = static_cast<Node*>((*bufferFrame).getData());
                }
            }
        }
    };
    
    /* erase: Deletes a specified key. You may simplify the logic by accepting under full pages.
     
     Start at root, find leaf L where entry belongs.
     Remove the entry.
     If L is at least half-full, done!
     If L has fewer entries than it should,
     If sibling (adjacent node with same parent as L) is more than half-full, re-distribute, borrowing an entry from it.
     Otherwise, sibling is exactly half-full, so we can merge L and sibling.
     If merge occurred, must delete entry (pointing to L or sibling) from parent of L.
     Merge could propagate to root, decreasing height.
     
     */
    bool erase(K key) {
        std::cout << "erase" << std::endl;
        
        LeafNode* leaf;
        BufferFrame& bufferFrame = findLeaf(key, true, &leaf);
        
        bool found = (*leaf).eraseLeafNode(key);
        
        bm.unfixPage(bufferFrame, found);
        // TODO
        return found;
    };
    

    /* lookup: Returns a TID or indicates that the key was not found.
     
     We are looking for a value k in the B+ Tree. Starting from the root, we are looking for the leaf which may contain the value k. At each node, we figure out which internal pointer we should follow. An internal B+ Tree node has at most d ≤ b children, where every one of them represents a different sub-interval. We select the corresponding node by searching on the key values of the node.
     */
    
      bool lookup(K key, TID& tid) {
        std::cout << "lookup" << std::endl;
          
        LeafNode*    leafNode;
        BufferFrame& bufferFrame = findLeaf(key, false, &leafNode);
        
        bool found = (*leafNode).getTid(key, tid);
        bm.unfixPage(bufferFrame, false);
        return found;
    }
    
    uint64_t root;
    
    // gets the bufferframe of the leaf for a key

    BufferFrame& findLeaf(K& key, bool exclusive, LeafNode** leafPointer) {
        // get root node
        BufferFrame* bufferFrame = &bm.fixPage(root, exclusive);
        Node* node = static_cast<Node*>((*bufferFrame).getData());
        
        // find the leaf
        while(!(*node).isLeaf()) {
            InnerNode* inner  = reinterpret_cast<InnerNode*>(node);
            uint64_t   nextID = (*inner).getChild(key);
            
            // lock coupling
            BufferFrame* newBufferFrame = &bm.fixPage(nextID, exclusive);
            bm.unfixPage(*bufferFrame, false);
            bufferFrame = newBufferFrame;
            
            node = static_cast<Node*>((*bufferFrame).getData());
        }
        
        *leafPointer = reinterpret_cast<LeafNode*>(node);
        return *bufferFrame;
    }

};


#endif /* Btree_hpp */
