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


/*
 *  User Constructor
 */
user::user(string n, 
           string pw,
           string i = "<none>",
           int w = 0,
           int l = 0)
{
    name = n;
    password = pw;
    info = i;
    wins = w;
    losses = l;
}


/*
 *  Register a new user.
 *  Returns string informing success/failure.
 *  Only the guest user may register new users.
 *  Usernames should be unique. 
 */
string registerUser(string n, string pw)
{

    for (vector<user>::iterator it = users.begin(); it != users.end(); ++it) {

        if (it->getName() == n) {
            return it->getName() + " registered already.";    
        } 
    }

    // Create new user with no wins, losses, or info.
    user newUser(n, pw);    
    users.push_back(newUser);    
    return "User created.";

}

/*
 *  Print a user's stats.
 *  Returns string.
 */ 
string user::stats()
{
    string res = "";
    res += "User: " + this->getName() + "\n";
    res += "Info: " + this->getInfo() + "\n";
    res += "Rating: ";
    res += "0.000";
    res += "\n";
    res += "Wins: ";
    res += static_cast<ostringstream*>( &(ostringstream() << this->getWins() ) )->str();
    res += ", Losses: ";
    res += static_cast<ostringstream*>( &(ostringstream() << this->getLosses() ) )->str();
    res += "\n";

    /* TODO use this when we have a way to determine if user is online or not.
    res += "\n";
    res += this->getName();
    res += " ";
    res += "is currently online\n";
    */

    return res;
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

