#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User{
public:
    User(int id = -1,string name = "",string password = "",string state = "offline"){
        this->id = id;
        this->name = name;
        this->password = password;
        this->state = state;
    }

    void setId(int id){
        this->id = id;
    }
    void setName(string name){
        this->name = name;
    }
    void setPassword(string password){
        this->password = password;
    }
    void setState(string state){
        this->state = state;
    }
    
    string getName(){
        return this->name;
    }
    string getPassword(){
        return this->password;
    }
    string getState(){
        return this->state;
    }
    int getId(){
        return this->id;
    }
private:
    int id;
    string name;
    string password;    
    string state;
};


#endif