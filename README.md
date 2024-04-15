# IRC-Client

### Introduction:
Hi all, this is a basic IRC client made for Windows Platform. It allows one to
connect to any IRC server out there and interact with it. 

I would like to thank John Crickett for his amazing website

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
