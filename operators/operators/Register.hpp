#ifndef REGISTER_H
#define REGISTER_H

#include <string>


class Register {

private:
    bool zahl;
    int num;
    std::string str;
    
    
public:
    
    bool is_num();
    
    std::string& getString();
    int getInteger();
    
    void setInteger( int i);
    void setString(const std::string& s);
    
    bool operator==(Register& r);
    
    Register( int i );
    Register( std::string i );
    Register();
    
    
};

namespace std {
    template <> 
    struct hash<Register> {
        std::size_t operator()(const Register&  reg) const {
            if( reg.is_num() ) {
                return std::hash<int>()(reg.getInteger());
            }  else {
                return std::hash<std::string>()(reg.getString());
            }
        }
    };
};


#endif