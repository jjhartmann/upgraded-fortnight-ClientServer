#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

using namespace std;

// SIGCHLD Handler
void handle_sigchld(int sig)
{
    // cout << "SENT SIGCHLD" << endl;
    int serrno = errno;
    while (waitpid((pid_t) -1, 0, WNOHANG) > 0) {
        // Do until all child processes are reaped.
    }
    errno = serrno;
}

// Error Function
void error(string errorMessage)
{
    perror(errorMessage.c_str());
    exit(1);
}

// Forward declarations
void StartServer(int portNumber);
void InfiniteRun(int csfd);
void StartClient(string hostname, int portnumber);

///////////////////////////////////////////////////////////////
// Main
int main(int argc, char *argv[])
{
    cout << INADDR_ANY << endl;
     cout << "Simple Client Server" << endl;
    int type = -1;
    string strType;
    if (argc <= 1)
    {
        cout << "Please enter parameters:\nClient=1//Server=0: ";
        cin >> strType;
        type = atoi(strType.c_str());
    }
    else
    {
        // Assume convertible to int. TODO: error cases.
        type = atoi(argv[1]);
    }

    if (type > 1) return 1;

    if (!type)
    {
        int portnum = -1;
        // Call server
        if (argc < 2)
        {
            string strpnum;
            cout << "Provide a portnumber for server: ";
            cin >> strpnum;
            portnum = atoi(strpnum.c_str());
        }
        else
        {
            portnum = atoi(argv[2]);
        }

        StartServer(portnum);
    }
    else
    {
        // Call client
        string hostname;
        int portnum;
        if (argc < 3)
        {
            string tmp;
            cout << "Provide a hostname: ";
            cin >> hostname;
            cout << "Provide a port number: ";
            cin >> tmp;
            portnum = atoi(tmp.c_str());
        }
        else
        {
            hostname = argv[2];
            portnum = atoi(argv[3]);
        }

        StartClient(hostname, portnum);
    }

    return 0;
}


///////////////////////////////////////////////////////////////////
// Server Implementation
void StartServer(int portNumber)
{
    // Config variables
    int serverSocketFileDesc = -1;
    int clientSocketFileDesc = -1;
    socklen_t clientLen;

    // Socket Structure to use when making connections
    struct sockaddr_in serverAddress, clientAddress;

    // Create TCP socket over IP.
    if ((serverSocketFileDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "Error: serverSocketFileDesc failed to instantiate. Error Code: " << serverSocketFileDesc << endl;
        error("Error: socket() for server failed.");
    }

    // Set server address to zero
    bzero((char *) &serverAddress, sizeof(serverAddress));

    // Configure Server Address.
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNumber); // Convert to network byte order.
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind socket file desc to server address
    if (bind(serverSocketFileDesc, (sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
    {
        error("Error: Failed to bind socket to server address.");
    }

    // Start listen on socket
    listen(serverSocketFileDesc, 5);
    clientLen = sizeof(clientAddress);

    // Set up sigchld handler
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) < 0)
    {
        error("ERROR: Failed to setup SIGCHLD handler.");
    }

    // Start server loop
    while (true)
    {
        // Set up connection
        if ((clientSocketFileDesc = accept(serverSocketFileDesc, (sockaddr *) &clientAddress, &clientLen)) < 0) {
            error("ERROR: Failed to set up connection with client");
        }

        pid_t pid;
        if ((pid = fork()) < 0)
        {
            error("Failed to create process");
        }
        else if (pid == 0)
        {
            // Process the client connection
            close(serverSocketFileDesc);
            InfiniteRun(clientSocketFileDesc);
            
//            // Set up buffer and connection
//            int bufferLen = 1024;
//            char buffer[bufferLen];
//            bzero((char *) buffer, bufferLen);
//            int byteCount;
//            if ((byteCount = recv(clientSocketFileDesc, buffer, bufferLen, 0)) < 0) {
//                error("ERROR: Failed to read form buffer");
//            }
//
//            cout << "Received bytes: " << byteCount << endl;
//            cout << "Received message: " << buffer << endl;
//
//            // Send message back to client.
//            if ((byteCount = send(clientSocketFileDesc, "RECVED MESSAGE", 14, 0)) < 0) {
//                error("ERROR: Failed to send to client");
//            }

            // end process
            exit(0);
        }
        else
        {
            // Close the connection in the parent.
            close(clientSocketFileDesc);
        }


    }

    close(serverSocketFileDesc);
}

void InfiniteRun(int csfd)
{
    cout << "ENTERED PROCESS FOR CLIENT" << endl;
    // Set up buffer and connection
    int bufferLen = 1024;
    char buffer[bufferLen];
    bool exit = false;
    while (!exit)
    {
        bzero((char *) buffer, bufferLen);
        int byteCount;
        if ((byteCount = recv(csfd, buffer, bufferLen, 0)) < 0) {
            error("ERROR: Failed to read form buffer");
        }
        
        cout << "Received bytes: " << byteCount << endl;
        cout << "Received message: " << buffer << endl;
        
        // Send message back to client.
        if ((byteCount = send(csfd, "RECVED MESSAGE", 14, 0)) < 0) {
            error("ERROR: Failed to send to client");
        }
        
        exit = (strncmp("exit", buffer, 4) == 0);
    }
    
    cout << "EXIT PROCESS FOR CLIENT" << endl;

}

////////////////////////////////////////////////////////////////////////////////
// Client Implementation
void StartClient(string hostName, int portNumber)
{
    // Check to see if host is name or address
    cout << "Is host identified by name? (Y/n): ";
    string answer;
    cin >> answer;
    
    
    int clientSocketFileDesc = -1;
    sockaddr_in serverAddress;
    struct in_addr serverAddr;
    hostent *server;

    // Setup socket
    if ((clientSocketFileDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error("ERROR: Failed to create socket.");
    }

    // get server by hostname
    if (answer == "Y")
    {
        // Use the hostname
        if ((server = gethostbyname(hostName.c_str())) == NULL)
        {
            error("ERROR: Failed to get server hostname");
        }
    }
    else
    {
        // Use IP address
        inet_pton(AF_INET, hostName.c_str(), &serverAddr);
        if ((server = gethostbyaddr(&serverAddr, sizeof(serverAddr), AF_INET)) == NULL)
        {
            error("ERROR: Failed to get server address");
        }
    }
    // Zero out serverAddress and add configuration
    bzero((char *) &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serverAddress.sin_addr.s_addr, server->h_length);
    serverAddress.sin_port = htons(portNumber);


    // Connect client socket to server
    if (connect(clientSocketFileDesc, (sockaddr *) &serverAddress, sizeof(serverAddress)))
    {
        error("Error: Failed to connect to server.");
    }

    
    // Ask user for message to send server
    int buff_size = 1024;
    char buffer[buff_size];
    int byteCount;
    
    // Test One Caputure packet
    cout << "Conduct Test One (caputre TCP packet)? (Y,n): ";
    string testAccept;
    cin >> testAccept;
    if (testAccept == "Y")
    {
        // Capture a TCP packet and store in file.
        // 1. Make a file to store packet.
        system("touch A3Packet_1.txt");
        
        // 2. Set up tcpdump in background process to store packet.
        system("tcpdump -c 3 -s0 -Xvvvi eth0 tcp port 12345 > A3Packet_1.txt&");
        
        // Sleep so tcpdump get initialized
        cout << "\n\nWAIT FOR TCPDUMP: SHOULD PROCEED? (Y): ";
        string dummy;
        cin >> dummy;
        
        string testMsg = "This is a message to test the server.";
        
        bzero((char *)buffer, buff_size);
        bcopy((char *) testMsg.c_str(), buffer, testMsg.length());
        
        if ((byteCount = send(clientSocketFileDesc, buffer, buff_size, 0)) < 0)
        {
            error("ERROR: sending message to server.");
        }
        
        // Receive response from sever
        bzero(buffer, buff_size);
        if ((byteCount = recv(clientSocketFileDesc, buffer, buff_size, 0)) < 0)
        {
            error("ERROR: Could not read from sever.");
        }
        
        cout << "Message from server: " << buffer << endl;
    }
    
    
    
    bool exit = false;
    while (!exit){
        cout << "Enter Message to send to server: ";
        string tmp;
        cin.ignore();
        getline (cin,tmp);
        
        bzero((char *)buffer, buff_size);
        bcopy((char *) tmp.c_str(), buffer, tmp.length());
        
        if ((byteCount = send(clientSocketFileDesc, buffer, buff_size, 0)) < 0)
        {
            error("ERROR: sending message to server.");
        }
        
        // Receive response from sever
        bzero(buffer, buff_size);
        if ((byteCount = recv(clientSocketFileDesc, buffer, buff_size, 0)) < 0)
        {
            error("ERROR: Could not read from sever.");
        }
        
        cout << "Message from server: " << buffer << endl;
        exit = (tmp == "exit");
    }
    // Close connections
    close(clientSocketFileDesc);

}