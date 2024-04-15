#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <windows.h>
#include <winbase.h>
using namespace std;


bool func(HANDLE &namedPipe, string &inpt, DWORD &dwNoBytesWrite) {
  return WriteFile(
    namedPipe,
    inpt.c_str(),
    inpt.length(),
    &dwNoBytesWrite,
    NULL
    );
}

std::unique_ptr <HANDLE> createPipe(DWORD &dwszOutputBuffer, DWORD &dwszInputBuffer) {
  return std::make_unique <HANDLE> (CreateNamedPipe (
      "\\\\.\\pipe\\MYNAMEDPIPE",
      PIPE_ACCESS_DUPLEX,
      PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
      PIPE_UNLIMITED_INSTANCES,
      dwszOutputBuffer,
      dwszInputBuffer,
      0,
      NULL
      ));
}


int main() {
    cout<<"Named Pipe Server!!!!"<<endl;
    //Named Local Pipe Variable
    std::unique_ptr <HANDLE> hCreateNamedPipe;
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

    //CreateNamedPipe - STEP 1
     hCreateNamedPipe = createPipe(dwszOutputBuffer, dwszInputBuffer);

    if (*hCreateNamedPipe == INVALID_HANDLE_VALUE) {
      cout<<"Failed pipe creation & Error No "<<GetLastError()<<endl;
    }
    cout<<"Named Pipe Creation Success"<<endl;
    //ConnectNamedPipe - STEP 2

    bConnectNamedPipe = ConnectNamedPipe(*hCreateNamedPipe, NULL);
    if (bConnectNamedPipe == FALSE) {
      cout<<"Connection Failed & Error No "<<GetLastError()<<endl;
    }
    cout<<"Connection Success"<<endl;
    //WriteFile Operation - STEP 3

    string inpt;
    do {
      getline(cin, inpt);
      bWriteFile = func(*hCreateNamedPipe, inpt, dwNoBytesWrite);
      
      if (bWriteFile == FALSE) {
        cout<<"WriteFile Failed = "<<GetLastError()<<endl;
      }
      cout<<"WriteFile Success"<<endl;
      //Flush the File Buffer - STEP 4

      bFlushFileBuffer = FlushFileBuffers(*hCreateNamedPipe);
      // if (bFlushFileBuffer == FALSE) {
      //   cout<<"FlushFileBuffer Failed & Error No "<<GetLastError()<<endl;
      // }
      // cout<<"FlushFileBuffer Success"<<endl;
      //ReadFile Operation - STEP 5
      if (inpt == "fin") {
        break;
      }
    } while (true);

    // bReadFile = ReadFile(
    //     hCreateNamedPipe,
    //     szReadFileBuffer,
    //     dwReadBufferSize,
    //     &dwNoBytesRead,
    //     NULL
    //     );

    // if (bReadFile == FALSE) {
    //   cout<<"ReadFile Failed = "<<GetLastError()<<endl;
    // }
    // cout<<"ReadFile Success"<<endl;
    //DisconnectNamedPipe - STEP 6
    DisconnectNamedPipe(*hCreateNamedPipe);
    //CloseHandle - STEP 7
    CloseHandle(*hCreateNamedPipe);

return 0;
}