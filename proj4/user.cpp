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
// TODO every 5 minutes, write this to a file
vector<user> users;

// TODO
// parseInput() for commands, receives string from main.cpp user input
//                      returns string for main.cpp to handle
// 

user::user(string n, 
           string pw,
           string info,
           unsigned int wins,
           unsigned int losses)
{
    name = n;
    password = pw;
    wins = 0;
    losses = 0;
}


/*
 *  Register a new user.
 *  Only the guest user may register new users.
 *  Usernames should be unique. 
 */
void registerUser(string n, string pw)
{

    for (vector<user>::iterator it = users.begin(); it != users.end(); ++it) {

        if (it->getName() == n) {
            return;    
        } 
    }

    // Create new user with no wins, losses, or info.
    user newUser(n, pw, "", 0, 0);    
    users.push_back(newUser);    
    return;

}

/*
 *  Print a user's stats
 *  TODO check tesla when it's back up so we can see how sample works
 *
 */ 
string user::stats()
{
    string res = "";
    res += "User: " + this->getName() + "\n";
    res += "Info: " + this->getInfo() + "\n";
    res += "Rating: " + "0.000" + "\n";
    //res += "Wins: " + itoa(this->getWins()) + ", Losses: " + itoa(this->getLosses()) + "\n";;
    //Info: <none>

    //cwood is currently online.
    return info;
}


// Mutator methods
bool user::setName(string n)
{
    if (n == "") {
        return false;
    }

    name = n;
    return true;
}

bool user::setPassword(string pw)
{
    if (pw == "") {
        return false;
    }

    password = pw;
    return true;

}

bool user::setInfo(string i)   // Used for info command
{
    if (i == "") {
        return false;
    }

    info = i;
    return true;

}




#endif


