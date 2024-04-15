// kbhit is a wonderful way to track characters typed on the keyboard
//I used this cpp file to handle the case if IRC server closes the socket connection
//and we are still blocked on console input. khbit() is non blocking and comes to the rescue!

#include <iostream>
#include <conio.h>
#include <string>
using namespace std;
int main()
{
    char ch;
    string s;
    while (1) {
 
        if ( kbhit() ) {
 
            // Stores the pressed key in ch
            ch = getch();
            s.push_back(ch);

            // Terminates the loop
            // when escape is pressed
            if ((int)ch == 13)
                break;
            else if (ch == '\b') {
                if (!s.empty()) {
                    std::cout << "\b \b"; // Erase previous character from the screen
                    s.pop_back(); // Remove last character from the string
                }
            } else {
               std::cout << ch; // Print the pressed key
               s += ch; // Add the character to the string
            }
 
            //cout << "\nKey pressed= " << ch;
        } else if (s.empty()) {
            cout<< "sleeping"<<'\n';
        }
    }
    return 0;
}