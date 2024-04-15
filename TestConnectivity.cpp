#include <windows.h>
#include <wininet.h>
#include <iostream>

bool IsInternetConnected() {
    DWORD dwFlag = 0;
    return InternetGetConnectedState(&dwFlag, 0);
}

int main() {
    if (IsInternetConnected()) {
        std::cout << "Internet is connected." << std::endl;
    } else {
      std::cout << "Internet is not connected." << std::endl;
    }
    return 0;
}