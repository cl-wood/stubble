#ifndef __USER_H__
#define __USER_H__
#include<string>
#include<iostream>
#include<sstream>

using namespace std;


// General purpose functions
string registerUser(string n, string pw);


class user {

    public:
        user(string n, 
             string pw,
             string info,
              int wins,
              int losses);

        // Accessor methods
        string getName()                        {return name;};
        string getPassword()                    {return password;};
        string getInfo()                        {return info;};
        int getWins()                  {return wins;};
        int getLosses()                {return losses;};
        // getMessages; 

        // Mutator methods
        bool setName(string n);
        bool setPassword(string pw);            // Used for passwd command
        bool setInfo(string info);              // Used for info command
        bool setWins( int i);
        bool setLosses( int i);
        // setMessages;

        // Other functions
        void wonGame()                          {wins++;};
        void LostGame()                         {losses++;};
        string stats();
        void observe( int gameNumber);
        void unobserve();

    private:
        string name;
        string password;
        string info;
        int wins;
        int losses;
        // messages;

};


#endif

