#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream pipe("mypipe"); // Opening the named pipe for reading
    if (!pipe.is_open()) {
        std::cerr << "Failed to open the named pipe" << std::endl;
        return 1;
    }

    std::string data;
    while (true) {
         // Reading from the named pipe
        if (std::getline(pipe, data)) {
            std::cout << data << std::endl;
            //std::cout << "Error state of the pipe: " << pipe.rdstate() << std::endl;
        }
        
        pipe.clear();
        //pipe.seekg(0, std::ios::beg);
        if (data == "end" || data == "Internet is not connected.") {
            break;
        }
    }
    pipe.close();
    return 0;
}