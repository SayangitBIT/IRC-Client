# IRC-Client

### Introduction:
Hi all, this is a basic IRC client made for Windows Platform. It allows one to
connect to any IRC server out there and interact with it. 

I would like to thank John Crickett for his amazing website.

https://codingchallenges.fyi/

https://www.linkedin.com/in/johncrickett/

which lists a bunch of amazing projects for people to try out.

Relevant challenge - https://codingchallenges.fyi/challenges/challenge-irc/


---


### Steps to launch the client:
- Compile SampleApp.cpp with the linker flags -lwsock32 and -lwininet 
- Compile WindowsPipeClient.cpp with the same linker flags.
- Run the SampleApp exe first followed by WindowsPipeClient exe.
- Wait for IRC server to connect. It will send welcome messages on successful connection.
- You will require two separate interactive consoles for this. Something like cmd or Powershell works fine.


---

### Note:
I use VScode as my IDE and C/C++ Runner as the extension. 

https://marketplace.visualstudio.com/items?itemName=franneck94.c-cpp-runner

https://github.com/franneck94/Vscode-C-Cpp-Runner/tree/main

This extension makes compiling C++ applications less tedious on VScode. 
Obviously for something larger one may have to look for alternative solutions 
like using CMake.

---
### Commands:
/join #channelName - Allows you to join a channel which goes by the name "channelName".

/part - Allows you to leave a joined channel. 

/nick newNick - Change your current nickname to "newNick".

/msg - Send a message using this command on a joined channel.

/quit - Exit the client.

---
### Limitations:
- You can join atmost one channel at a time.
- Sometimes the client pipe application may not output newly received messages. Pressing the
  enter key on the console usually fixes it.
- When the IRC client is idle for too long, the socket throws a recv error with error code: 10054.
  Refer to Windows Socket error codes for more information.
- Client pipe application may sometimes abruptly shutdown and WriteFile function in the SampleApp throws an error code: 232.
- IRC Client protocol lists huge number of error responses for commands and several of them are not implemented. So it's possible
  that such a response is received and it would cause unpredictable behaviour in the client.
- Synchronization issues between sent messages and received responses might occur. (I have done my best to handle several cases)
- Random freezes of either exe's might occur as a possibility.

---
### Concluding Remarks
Thank you for visiting my IRC Client. When I started off with the project, I thought it would be fairly 
simple to develop. The steps provided in the challenge link seemed simple and I thought of this project 
merely as a parsing server messages application. I was very wrong (lol). As I kept testing more and more
I came across more issues and needless to say I had to wrack my brains to solve them. It was a great learning
experience for me.

I want to sign off by saying that this is a v1.0 of the final thing I want to make. It's pretty amateurish
from any experienced developer's point of view, perhaps even very untidy in terms of code styling and several issues in general.
If you would like to provide me feedback on improvement please message me at

https://www.linkedin.com/in/sayan-maiti-a500271bb/.

The ircArchive.xlsx lists resources which helped me. Please do refer to them.

Thank you visitor!
