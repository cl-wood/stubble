#include "user.cpp"
#include <iostream>
using namespace std;

int main() 
{
    user u("user", "pw");
    //user u("user", "pw", "info", 12, 20);

    cout << u.stats() << endl;


    cout << registerUser("clark", "blah") << endl;
    cout << registerUser("clark", "blah") << endl;



}
