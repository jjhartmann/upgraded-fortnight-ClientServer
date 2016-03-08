#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

using namespace std;

// Error Function
void error(string errorMessage)
{
    perror(errorMessage.c_str());
    exit(1);
}

// Forward declarations
void StartServer(int portNumber);

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

    // TODO: Place in loop and use pthreads.
    // Set up connection
    if ((clientSocketFileDesc = accept(serverSocketFileDesc, (sockaddr *) &clientAddress, &clientLen)) < 0)
    {
        error("ERROR: Failed to set up connection with client");
    }


    // Set up buffer and connection
    int bufferLen = 1024;
    char buffer[bufferLen];
    bzero((char *)buffer, bufferLen);
    int byteCount;
    if ((byteCount = recv(clientSocketFileDesc, buffer, bufferLen, 0)) < 0)
    {
        error("ERROR: Failed to read form buffer");
    }

    cout << "Received bytes: " << byteCount << endl;
    cout << "Received message: " << buffer << endl;

    // Send message back to client.
    if ((byteCount = send(clientSocketFileDesc, "RECVED MESSAGE", 14, 0)) < 0)
    {
        error("ERROR: Failed to send to client");
    }

    // close connectinos
    close(clientSocketFileDesc);
    close(serverSocketFileDesc);
}


////////////////////////////////////////////////////////////////////////////////
// Client Implementation
void StartClient(string hostName, int portNumber)
{
    
}