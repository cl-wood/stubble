#ifndef __USER_H__
#define __USER_H__
#include<string>

using namespace std;

class user {

    public:
        user(string n, string pw);
        ~user();

        // Accessor methods
        string getName() {return name;};
        string getPassword() {return password;};
        unsigned int getWins() {return wins;};
        unsigned int getLosses() {return losses;};
        // getMessages; 
        bool getIsOnline() {return isOnline;};

        // Mutator methods
        void setName(string n);
        void setPassword(string pw);
        void setWins(unsigned int i);
        void setLosses(unsigned int i);
        // getMessages;
        void setIsOnline(bool b);

        // Other functions
        void registerUser(string n, string pw);

    private:
        //const static string guest = "guest";
        string name;
        string password;
        unsigned int wins;
        unsigned int losses;
        // messages;
        bool isOnline;

};


#endif

