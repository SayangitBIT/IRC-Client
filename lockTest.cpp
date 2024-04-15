#include <winsock2.h>
#include "iostream"
#include <vector>
#include <chrono>
#include <string>
#include <ctime>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <fstream>
#include <wininet.h>
#include <memory>
//#include <sstream>
#include <conio.h>
using namespace std;

std::mutex mymutex;

int main() {
    for(int i = 0; i < 20; i++) {
        std::unique_lock<std::mutex> lock(mymutex);
        cout<<5<<endl;
        continue;
    }
}