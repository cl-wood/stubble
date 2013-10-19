#include<stdio.h>

// For basic mutation, 2 possible paths based on user input
    
void main(int argc, char* argv) 
{
    // Get 1 char of user input
    char choice;
    choice = getchar();

    // Assign value to x based on input
    int x;
    if (choice == '1') {
        x = 111;
    } else {
        x = 222;
    }
 
    // Print x
    printf("%d\n", x);
}
