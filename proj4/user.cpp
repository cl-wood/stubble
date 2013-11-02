/*
 *  User class.
 *  Contains function definitions for users and user-related global variables
 *
 */

#ifndef __USER_CPP__
#define __USER_CPP__

#include "user.h"
#include <vector>

using namespace std;

// Globals
vector<user> users;

user::user(string n, string pw)
{
    name = n;
    password = pw;
    wins = 0;
    losses = 0;
    isOnline = false;
}



/*
 *  Register a new user.
 *  Only the guest user may register new users.
 *  Usernames should be unique. 
 */
void user::registerUser(string n, string pw)
{
    if (this->getName() != "guest") {
        return;
    }

    for (vector<user>::iterator it = users.begin(); it != users.end(); ++it) {

        if (it->getName() == n) {
            return;    
        } 
    }

    user newUser(n, pw);    
    users.push_back(newUser);    
    return;

}

#endif


