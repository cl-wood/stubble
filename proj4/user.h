#ifndef __USER_H__
#define __USER_H__
#include<string>
#include<cstdlib>

using namespace std;

// TODO
// error checking for NULL strings for user data get/set
// 

// General purpose function
void registerUser(string n, string pw);


class user {

    public:
        user(string n, 
             string pw,
             string info,
             unsigned int wins,
             unsigned int losses);

        // Accessor methods
        string getName()                        {return name;};
        string getPassword()                    {return password;};
        string getInfo()                        {return info;};
        unsigned int getWins()                  {return wins;};
        unsigned int getLosses()                {return losses;};
        // getMessages; 

        // Mutator methods
        bool setName(string n);
        bool setPassword(string pw);            // Used for passwd command
        bool setInfo(string info);              // Used for info command
        bool setWins(unsigned int i);
        bool setLosses(unsigned int i);
        // setMessages;

        // Other functions
        void wonGame()                          {wins++;};
        void LostGame()                         {losses++;};
        string stats();
        void observe(unsigned int gameNumber);
        void unobserve();

    private:
        string name;
        string password;
        string info;
        unsigned int wins;
        unsigned int losses;
        // messages;

};


#endif

