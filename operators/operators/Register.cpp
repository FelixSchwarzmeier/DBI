#include "Register.hpp"

bool Register::is_num() {
    return zahl;
}


string& Register::getString() {
    return str;
}

int Register::getInteger() {
    return num;
}

void setInteger( int i) {
    zahl = true;
    str = "";
    num = i;
    
}
void setString(const string& s) {
    zahl = false;
    i = 0;
    str = s;
}

bool operator==(Register& r) {
    return num == r.getInteger() && str.compare(r.getString()) == 0;
}

