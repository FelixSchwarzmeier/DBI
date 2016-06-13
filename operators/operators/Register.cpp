#include "Register.hpp"

Register::Register( int i) {
    zahl = true;
    num = i;
}

Register::Register( std::string i) {
    zahl = false;
    str = i;;
}

bool Register::is_num() {
    return zahl;
}


std::string& Register::getString() {
    return str;
}

int Register::getInteger() {
    return num;
}

void Register::setInteger( int i) {
    zahl = true;
    str = "";
    num = i;
    
}
void Register::setString(const std::string& s) {
    zahl = false;
    num = 0;
    str = s;
}

bool Register::operator==(Register& r) {
    return num == r.getInteger() && str.compare(r.getString()) == 0;
}

