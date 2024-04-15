/* This cpp file reads from SampleApp.cpp via a named Pipe
I used pipes as using sending and receiving messages over the same
interactive console would cause messages to interleave which results in bad user
experience */

/*I will become better in web dev / I/O methods eventually to 
find a better solution*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <winbase.h>
using namespace std;

int main() {
    cout<<"Named File Client!!!!"<<endl;
    //Named Local Pipe Variable
    HANDLE hCreateFile;
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
    vector <char>   szReadFileBuffer(1023);
    DWORD   dwReadBufferSize       = (int)szReadFileBuffer.size();
    DWORD   dwNoBytesRead;

    //CreateNamedPipe - STEP 1
    hCreateFile = CreateFile (
                        "\\\\.\\pipe\\MYNAMEPIPE",
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL
                        );
    if (hCreateFile == INVALID_HANDLE_VALUE) {
      cout<<"File creation & Error No "<<GetLastError()<<endl;
    }
    cout<<"File Creation Success"<<endl;
    //ConnectNamedPipe - STEP 2

    // bConnectNamedPipe = ConnectNamedPipe(hCreateNamedPipe, NULL);
    // if (bConnectNamedPipe == FALSE) {
    //   cout<<"Connection Failed & Error No "<<GetLastError()<<endl;
    // }
    // cout<<"Connection Success"<<endl;
    //WriteFile Operation - STEP 3

    // bWriteFile = WriteFile(
    //     hCreateFile,
    //     szWriteFileBuffer,
    //     dwWriteBufferSize,
    //     &dwNoBytesWrite,
    //     NULL
    //     );

    // if (bWriteFile == FALSE) {
    //   cout<<"WriteFile Failed = "<<GetLastError()<<endl;
    // }
    // cout<<"WriteFile Success"<<endl;
    //Flush the File Buffer - STEP 4

    // bFlushFileBuffer = FlushFileBuffers(hCreateNamedPipe);
    // if (bFlushFileBuffer == FALSE) {
    //   cout<<"FlushFileBuffer Failed & Error No "<<GetLastError()<<endl;
    // }
    // cout<<"FlushFileBuffer Success"<<endl;
    //ReadFile Operation - STEP 5

    //For our purpose only STEP - 5 is useful since we want to read from the pipe
    do {
        bReadFile = ReadFile(
            hCreateFile,
            szReadFileBuffer.data(),
            dwReadBufferSize,
            &dwNoBytesRead,
            NULL
            );
        szReadFileBuffer[dwNoBytesRead] = '\0';
        // if (bReadFile == FALSE) {
        //   cout<<"ReadFile Failed = "<<GetLastError()<<endl;
        // }
        // cout<<"ReadFile Success"<<endl;
        cout<<szReadFileBuffer.data()<<endl;
        if (string(szReadFileBuffer.begin(), szReadFileBuffer.end()).substr(0,3) == "fin") {
            break;
        }
    } while (true);
    //DisconnectNamedPipe - STEP 6
    //DisconnectNamedPipe(hCreateNamedPipe);
    //CloseHandle - STEP 7
    CloseHandle(hCreateFile);

return 0;
}