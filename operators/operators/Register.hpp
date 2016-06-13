#ifndef REGISTER_H
#define REGISTER_H

class Register {

private:
    bool zahl;
    int num;
    string str;
    
    
public:
    
    bool is_num();
    
    const string& getString();
    int getInteger();
    
    void setInteger( int i);
    void setString(const string& s);
    
    bool operator==(Register& r);
    
    
};


#endif