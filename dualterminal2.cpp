//You can mimic pipe utility using C++ fstream objects too

#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::string input;
    std::ofstream pipe("mypipe"); // Creating the named pipe
    if (!pipe.is_open()) {
        std::cerr << "Failed to open the named pipe" << std::endl;
        return 1;
    }

    while (true) {
    std::cout << "Enter data to send: ";
    std::getline(std::cin, input);

      pipe << input << std::endl; // Writing to the named pipe
      if (input == "end") {
        break;
      }
    }
    pipe.close();
    return 0;
}