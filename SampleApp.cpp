//Dedicated to Hatsune Miku

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

SOCKET clientSocket = INVALID_SOCKET;
std::mutex mymutex;
std::condition_variable mycond;
bool m_bStop = true;
bool m_bRegistered = false;
bool processed = false;
string joinedChannel;
string lastCommand;
string sentMessage;
string nick;
int sentMsgFlag = 0;
int dualPass = 0;
uint64_t lastReportingTime;
std::random_device dev;
std::mt19937 rng(dev());

void writeMsg(HANDLE &namedPipe, string inpt, DWORD &dwNoBytesWrite) {
    //cout<<inpt<<endl;
    bool ret = WriteFile(
        namedPipe,
        inpt.c_str(),
        inpt.length(),
        &dwNoBytesWrite,
        NULL
        );
    if (ret == FALSE) {
        cout<<"WriteMsg Failed = "<<GetLastError()<<endl;
    }  
    FlushFileBuffers(namedPipe);
    return;
}

uint64_t timeSinceEpochMillisec() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

bool IsInternetConnected() {
    DWORD dwFlag = 0;
    return InternetGetConnectedState(&dwFlag, 0);
}

int sendData(string buffer) {
    int sbyteCount = send(clientSocket, buffer.c_str(), (int)buffer.length(), 0);
    if (sbyteCount == SOCKET_ERROR) {
        cout << "Client send error: " << WSAGetLastError() << std::endl;
        return -1;
    } else {
        //cout << "Client: Sent " << sbyteCount << " bytes" << std::endl;
    }
    return 1;
}

void sendThread(HANDLE &namedPipe) {
    DWORD dwNoBytesWriteSend;
    sendData("PASS none\r\n");
    sendData("USER guest 0 * :Coding Challenges Client\r\n");
    string iniNick = "cod";
    sendData("NICK " + iniNick + "\r\n");
    do {
        sendData("NICK " + iniNick + "\r\n");
        //pipe<<iniNick<<endl;
        std::uniform_int_distribution<std::mt19937::result_type> dist9(1,9);
        char ext = (char)('0' + dist9(rng));
        iniNick.push_back(ext);
        std::unique_lock<std::mutex> lock(mymutex);
        mycond.wait(lock, [] {
           return processed;
        });
        processed = false;
    } while (!m_bRegistered);
    nick = iniNick;
    nick.pop_back();

    

    while(m_bStop) {
        //std::unique_lock<std::mutex> writeLock(writeMutex);
        string inpt;
        char ch = '#';
        while (m_bStop) {
            if ( kbhit() ) {
    
                // Stores the pressed key in ch
                ch = (char)getch();
                //pipe<<ch<<endl;
                // Terminates the loop
                // when enter is pressed
                if ((int)ch == 13) {
                    cout<<'\n';
                    break;
                } else if (ch == '\b') {
                    if (!inpt.empty()) {
                        cout << "\b \b"; // Erase previous character from the screen
                        inpt.pop_back(); // Remove last character from the string
                    }
                } else {
                    cout << ch; // Print the pressed key
                    inpt.push_back(ch); // Add the character to the string
                }
    
            } else if (inpt.empty()) {
                break;
            }
        }
        
        __SIZE_TYPE__ whiteSpacePos = 0;
        string command;
        string param;
        whiteSpacePos = inpt.find_first_of(' ');
        if (whiteSpacePos != std::string::npos) {
            command = inpt.substr(0, whiteSpacePos);
            param = inpt.substr(whiteSpacePos + 1);
        } else {
            command = inpt.substr(0);
        }

        if (!command.empty() and command[0] != '/' and m_bStop) {
            //writeLock.lock();
           // pipe<<"Incorrect command format"<<endl;
            writeMsg(namedPipe, "Incorrect command format", dwNoBytesWriteSend);
            //writeLock.unlock();
        } else {
            if (command == "/join") {
                std::unique_lock<std::mutex> lock(mymutex);
                joinedChannel = param;
                lastCommand = command;
                lock.unlock();
                sendData("JOIN " + param + "\r\n");
            }
            if (command == "/part") {
                std::unique_lock<std::mutex> lock(mymutex);
                lastCommand = command;
                lock.unlock();
                sendData("PART "+ joinedChannel + "\r\n");
            }
            if (command == "/nick") {
                std::unique_lock<std::mutex> lock(mymutex);
                lastCommand = command;
                lock.unlock();
                sendData("NICK "+ param + "\r\n");
            }
            if (command == "/msg") {
                std::unique_lock<std::mutex> lock(mymutex);
                lastCommand = command;
                sentMessage = nick + ": " + param + "\n";
                //cout<<param + "dhoka"<<endl;                
                sendData("PRIVMSG "+ joinedChannel + " :" + param + "\r\n");
                
                lock.unlock();
            }
            if (command == "/quit") {
                sendData("QUIT\r\n");
                //m_bStop = false;
                break;
            }
        }

        //pipe<<"Going out of sending thread"<<endl;
        //lock.unlock();
        //std::this_thread::sleep_for(std::chrono::milliseconds(200));
        //mycond.notify_all();
    }
    return;
}

int main() {

    //Pipe variables
    HANDLE hCreateNamedPipe;
    char szInputBuffer[1023];
    char szOutputBuffer[1023];
    DWORD dwszInputBuffer = sizeof(szInputBuffer);
    DWORD dwszOutputBuffer = sizeof(szOutputBuffer);
    BOOL bConnectNamedPipe;

    //WriteFile Local Variable
    BOOL    bWriteFile;
    char    szWriteFileBuffer[1023] = "Hello from Pipe Server!";
    DWORD   dwWriteBufferSize       = sizeof(szWriteFileBuffer);
    DWORD   dwNoBytesWrite;

    //FlushBuffer
    BOOL bFlushFileBuffer;

    //ReadFile Local Variable
    BOOL    bReadFile;
    char    szReadFileBuffer[1023];
    DWORD   dwReadBufferSize       = sizeof(szReadFileBuffer);
    DWORD   dwNoBytesRead;
    //Pipe variables end

    //Pipe Creation
    hCreateNamedPipe = CreateNamedPipe (
      "\\\\.\\pipe\\MYNAMEPIPE",
      PIPE_ACCESS_DUPLEX,
      PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
      PIPE_UNLIMITED_INSTANCES,
      dwszOutputBuffer,
      dwszInputBuffer,
      0,
      NULL
      );
    if (hCreateNamedPipe == INVALID_HANDLE_VALUE) {
      cout<<"Failed pipe creation & Error No "<<GetLastError()<<endl;
      WSACleanup();
    }
    cout<<"Named Pipe Creation Success"<<endl;

    bConnectNamedPipe = ConnectNamedPipe(hCreateNamedPipe, NULL);
    if (bConnectNamedPipe == FALSE) {
      cout<<"Pipe connection Failed & Error No "<<GetLastError()<<endl;
      WSACleanup();
    }
    cout<<"Pipe Connection Success"<<endl;


    int iResult;
    WSADATA wsaData;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    u_long iMode = 1;

    // Create a socket. This will be set to non blocking mode in lines 304 - 314
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        writeMsg(hCreateNamedPipe, "fin", dwNoBytesWrite);
        DisconnectNamedPipe(hCreateNamedPipe);
        CloseHandle(hCreateNamedPipe);
        return 0;
    } else {
        cout << "Socket is OK!" << std::endl;
    }

    ioctlsocket(clientSocket, FIONBIO, &iMode);
    if (iResult != NO_ERROR) {
        cout << "ioctlsocket failed with error: " << iResult;
        WSACleanup();
        writeMsg(hCreateNamedPipe, "fin", dwNoBytesWrite);
        DisconnectNamedPipe(hCreateNamedPipe);
        CloseHandle(hCreateNamedPipe);
        return 0;
    }

    // Bind the socket to an IP address and port number
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    //clientService.sin_addr.s_addr = inet_addr("207.148.28.126");  // Freenode IRC
    clientService.sin_addr.s_addr = inet_addr("176.58.122.119");    // Libera IRC
    clientService.sin_port = htons(6667);  // Choose a port number
    

    connect(clientSocket, reinterpret_cast<SOCKADDR*>(&clientService), sizeof(clientService)); // reinterpret_cast works as sockaddr and sockaddr_in
    fd_set wrset;
    fd_set exset;
    FD_ZERO(&wrset);
    FD_ZERO(&exset);

    FD_SET(clientSocket, &wrset);
    FD_SET(clientSocket, &exset);

    //wait till connect finishes
    int res = select(0, 0, &wrset, &exset, 0);

    if (res > 0) {
        cout << "Client: Connect() is OK!" << std::endl;
        cout << "Client: Can start sending and receiving data..." << std::endl;
    } else {
        cout << "Client: connect() - Failed to connect: " << WSAGetLastError() << std::endl;                    // share the same first member and connect () only compares these members  
        WSACleanup();
        writeMsg(hCreateNamedPipe, "fin", dwNoBytesWrite);
        DisconnectNamedPipe(hCreateNamedPipe);
        CloseHandle(hCreateNamedPipe);
        return 0;
    }

    do {
        vector <char> firstReceiveBuffer(512,'\0');
        int firstRbyteCount = recv(clientSocket, firstReceiveBuffer.data(), (int)firstReceiveBuffer.size(), 0);
        if (firstRbyteCount > 0) {
            writeMsg(hCreateNamedPipe, "Client: Received data: " + string(firstReceiveBuffer.begin(), firstReceiveBuffer.end()), dwNoBytesWrite);
            break;
        }
    } while(true);

    //Sending initial two messages as per challenge requirement̥̥ 
    
    std::thread sendingThread(sendThread, std::ref(hCreateNamedPipe));
    string toDisplay; //Used to parse each complete message sent from IRC
    string firstCommand; //Command corresponding to the sent message
    string firsttrailling; //Trailing string corresponding to the sent message.
    //std::unique_lock<std::mutex> writeLock(writeMutex);
    //writeLock.unlock();


    vector <char> receiveBuffer(512, '\0');
    do {
        if (!IsInternetConnected()) {
            cout << "Internet is not connected." << std::endl;
            m_bStop = false;
            writeMsg(hCreateNamedPipe, "fin", dwNoBytesWrite);
            break;
        }

        int rbyteCount = recv(clientSocket, receiveBuffer.data(), (int)receiveBuffer.size(), 0);
         
        //cout<<"We are here "<<receiveBuffer.size()<<" "<<rbyteCount<<endl;
        if (rbyteCount < 0 and WSAEWOULDBLOCK != WSAGetLastError()) {
            cout << "Client recv error: " << WSAGetLastError() << std::endl;
            writeMsg(hCreateNamedPipe, "fin", dwNoBytesWrite);
            break;
        } else if (rbyteCount == 0) {
            m_bStop = false;
            writeMsg(hCreateNamedPipe, "fin", dwNoBytesWrite);
            break;
        } else {
            //pipe << receiveBuffer.data() << std::endl;
        }

        /*Lines 373 - 411 use ugly logic for synchronization. For any
        message sent in a channel chat, IRC does not send a response unless it's
        an incorrect message format.
        A response to an incorrect message takes roughly 150 ms. The dualPass variable
        is a fail-safe to ensure our message has been sent correctly to the channel*/

        if (rbyteCount < 0) {
            std::unique_lock<std::mutex> lock(mymutex);
            if (lastCommand == "/msg" and joinedChannel != "") {
                //cout<<"Faster than speed of light: "<<rbyteCount<<endl;

                if (lastReportingTime == 0) {
                    lastReportingTime = timeSinceEpochMillisec();
                }
                if (sentMsgFlag == 0 and timeSinceEpochMillisec() - lastReportingTime >= 350) {
                    //cout<<rbyteCount<<" "<<joinedChannel<<" "<<sentMessage<<endl;
                    if (dualPass == 1) {
                        writeMsg(hCreateNamedPipe, sentMessage, dwNoBytesWrite);
                        //writeMsg(hCreateNamedPipe, "inside " + to_string(timeSinceEpochMillisec() - lastReportingTime), dwNoBytesWrite);
                        //cout<<"Entity -1: "<<"I killed your command"<<endl;
                        lastCommand = "";
                        firstCommand = "";
                        dualPass = 0;
                        sentMessage = "";
                        lastReportingTime = 0;
                    }
                    dualPass = 1;
                }
                if (sentMsgFlag == 2) {
                    //sentMessage = "";
                    sentMsgFlag = 0;
                    lastCommand = "";
                    firstCommand = "";
                    dualPass = 0;
                    sentMessage = "";
                    lastReportingTime = 0;
                }
                
            }
            //lock.unlock();
            continue;
        }

        
        string receiveBufferStr(receiveBuffer.begin(), receiveBuffer.begin() + rbyteCount + 1);
        receiveBufferStr.back() = '\0';
        // cout<<"Entity: " + receiveBufferStr<<endl;
        // cout<<"Entity 1: "<<lastCommand<<endl;
        //fill(receiveBuffer.begin(), receiveBuffer.end(), '\0');
        //receiveBufferStr[rbyteCount] = '\0';

        __SIZE_TYPE__ start = 0;
        __SIZE_TYPE__ pos = 0;
        
        while (true) {
            pos = receiveBufferStr.find("\r\n", start);
            if (pos == std::string::npos) {
                break;
            }
            toDisplay += receiveBufferStr.substr(start, pos - start);
            //cout<<toDisplay<<endl<<endl;
            string origin;
            string command;
            vector <string> parameter;
            string trailling;
            bool flag = false;

            __SIZE_TYPE__ startIndex = 0;
            if (toDisplay[0] == ':') {
                __SIZE_TYPE__ whiteSpacePos = toDisplay.find_first_of(' ', 0);
                origin = toDisplay.substr(0, whiteSpacePos);
                startIndex = whiteSpacePos + 1;
            }

            /* command  block */ { 
                __SIZE_TYPE__ whiteSpacePos = toDisplay.find_first_of(' ', startIndex);
                command = toDisplay.substr(startIndex, whiteSpacePos - startIndex);
                startIndex = whiteSpacePos + 1;
            }

            while (toDisplay[startIndex] != ':') {
                __SIZE_TYPE__ whiteSpacePos = toDisplay.find_first_of(' ', startIndex);
                parameter.push_back(toDisplay.substr(startIndex, whiteSpacePos - startIndex));
                if (whiteSpacePos != std::string::npos) {
                    startIndex = whiteSpacePos + 1;
                } else {
                    startIndex -= 1;
                    break;
                }
            }

            /*trailing block*/ {
                trailling = toDisplay.substr(startIndex + 1);
                //cout<<trailling<<endl;
            }

            if (command == "001" || command == "002" || command == "003" || command == "004"
            || command == "372" || command == "NOTICE" || command == "ERROR") {
                //writeLock.lock();
                flag = true;
                writeMsg(hCreateNamedPipe, trailling, dwNoBytesWrite);
                //writeLock.unlock();
            }
            if (command == "353" || command == "332") {
                ////writeLock.lock();
                flag = true;
                writeMsg(hCreateNamedPipe, trailling + "\n", dwNoBytesWrite);
                //writeLock.unlock();
            }
            

            //Lines 496-506 are meant for successful nickname registration
            if (!m_bRegistered) {
                if (command == "433") {
                    processed = true;
                    mycond.notify_one();
                } else if (command == "001") {
                    processed = true;
                    m_bRegistered = true;
                    mycond.notify_one();
                }
            }

            if (command == "PING") {
                flag = true;
                sendData("PONG " + trailling + "\r\n");
            }

            if (command == "NICK") {
                __SIZE_TYPE__ findExclaim = origin.find_first_of('!');
                string receivedNick = origin.substr(1,findExclaim - 1);
                if (receivedNick != nick) {
                    flag = true;
                    //writeLock.lock();
                    writeMsg(hCreateNamedPipe, receivedNick + " is now known as " + trailling + "\n", dwNoBytesWrite);
                    //writeLock.unlock();
                }
            }

            if (command == "PRIVMSG") {
                flag = true;
                __SIZE_TYPE__ findExclaim = origin.find_first_of('!');
                string receivedNick = origin.substr(1,findExclaim - 1);
                //writeLock.lock();
                writeMsg(hCreateNamedPipe, receivedNick + ": " + trailling + "\n", dwNoBytesWrite);
                //writeLock.unlock();
            }

            std::unique_lock<std::mutex> lock(mymutex);
            // cout<<"Entity 2: "<<lastCommand<<" "<<firstCommand<<" "<<flag<<endl;
            // cout<<"Entity 3: "<<command<<endl;
            if (firstCommand == "" and lastCommand != "" and !flag) {
                //writeMsg(hCreateNamedPipe, "Last command is " + lastCommand, dwNoBytesWrite);
                firstCommand = command;
                //writeMsg(hCreateNamedPipe, "Corresponding command is " + firstCommand, dwNoBytesWrite);
                firsttrailling = trailling;
                //writeMsg(hCreateNamedPipe, "Trailing is " + firsttrailling + "\n", dwNoBytesWrite);
            }
            if (lastCommand == "/join") {
                if (firstCommand == "476" || firstCommand == "471" || firstCommand == "472" || firstCommand == "461" || firstCommand == "477") {
                    lastCommand = "";
                    firstCommand = "";
                    joinedChannel = "";
                    //writeLock.lock();
                    writeMsg(hCreateNamedPipe, trailling + "\n", dwNoBytesWrite);
                    //writeLock.unlock();
                } 
                if (firstCommand == "JOIN") {
                    lastCommand = "";
                    firstCommand = "";
                    //writeLock.lock();
                    writeMsg(hCreateNamedPipe, "Joined channel " + firsttrailling + "\n", dwNoBytesWrite);
                    //writeLock.unlock();
                }
            }
            if (lastCommand == "/part") {
                if (firstCommand == "461" || firstCommand == "442") {
                    lastCommand = "";
                    firstCommand = "";
                    //writeLock.lock();
                    writeMsg(hCreateNamedPipe,  firsttrailling + "\n", dwNoBytesWrite);
                    //writeLock.unlock();
                }
                if (firstCommand == "PART") {
                    lastCommand = "";
                    firstCommand = "";
                    //writeLock.lock();
                    writeMsg(hCreateNamedPipe, "Left channel " + firsttrailling + "\n", dwNoBytesWrite);
                    //writeLock.unlock();
                    joinedChannel = "";
                    lastReportingTime = 0;
                }

            }
            if (lastCommand == "/nick") {
                if (firstCommand == "431" || firstCommand == "433" || firstCommand == "432" || firstCommand == "436") {
                    lastCommand = "";
                    firstCommand = "";
                    //writeLock.lock();
                    writeMsg(hCreateNamedPipe,  firsttrailling + "\n", dwNoBytesWrite);
                    //writeLock.unlock();
                }
                if (firstCommand == "NICK") {
                    lastCommand = "";
                    firstCommand = "";
                    //writeLock.lock();
                    writeMsg(hCreateNamedPipe,  "You are now known as " + firsttrailling + "\n", dwNoBytesWrite);
                    //writeLock.unlock();
                    nick = firsttrailling;
                }
            }
            if (lastCommand == "/msg") {
                //cout<<firstCommand<<endl;
                if (firstCommand == "411" || firstCommand == "404" || firstCommand == "412" || firstCommand == "461") {    
                    //writeLock.lock();
                    writeMsg(hCreateNamedPipe,  firsttrailling + "\n", dwNoBytesWrite);
                    if (joinedChannel != "") {
                        sentMsgFlag = 2;
                    }
                    //writeMsg(hCreateNamedPipe, to_string(timeSinceEpochMillisec() - lastReportingTime), dwNoBytesWrite);
                } 
                if (joinedChannel == "") {
                    lastCommand = "";
                    firstCommand = "";
                    sentMessage = "";
                }
            }
            toDisplay = {};
            start = pos + 2;
        }
        if (start < receiveBufferStr.length()) {
            __SIZE_TYPE__ posnull = receiveBufferStr.find_first_of('\0', start);
            toDisplay = receiveBufferStr.substr(start, posnull - start);
        }
        
    } while (m_bStop);
        
    sendingThread.join();
    WSACleanup();
    DisconnectNamedPipe(hCreateNamedPipe);
    CloseHandle(hCreateNamedPipe);
    return 0;
}