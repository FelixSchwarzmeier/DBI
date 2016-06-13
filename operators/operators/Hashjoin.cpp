#include "hashjoin.hpp"

Hashjoin::HashJoin( Operator* _op1; unsigned _col1, Operator* _op2, unsigned _col2 ) {
    op1 = _op1;
    col1 = _col1;
    op2 = _op2;
    col2 = _col2;
    
}

void Hashjoin::open() {
    //Hashmap der kleinere Relation bauen
    bool has_next = op1.next();
    while ( has_next ) {
        vector<Register> ret = op1->getOutput();
        small_table.insert( std::make_pair<Register, vector<Register>>(ret[col1], ret));
        op1.next();
    }
}

bool Hashjoin::next() {
    
    if( queue.empty() ) {
        //Nächste Zeile der Spalte und Ausgabe dieser
        bool has_next = op2.next();
        vector<Register> tmp = op2->getOutput();
        
        auto range = small_table.equal_range(tmp[col2]);
        for_each (
            range.first,
            range.second,
            [](stringmap::value_type& x){
                vector<Register> q = tmp;
                q.insert(q.end(), x.second.begin(), x.second.end());
                queue.push( q );
            }
        );
        
        ret = queue.front();
        queue.pop();

    } else {
        //gepufferte Werte die noch ausgegeben werden müssen
        ret = queue.front();
        queue.pop();
    }
}

vector<Register> Hashjoin::getOutput() {    
    return ret;
}

void Hashjoin::close() {
    //close notwendig?
}
