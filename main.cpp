#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
    char buffer[1024];
    int serverSocketFileDesc = -1;
    int clientSocketFileDesc = -1;
    int clientLen = 0;

    // Socket Structure to use when making connections
    struct sockaddr_in serverAddress, clientAddress;

    // Create TCP socket over IP.
    if ((serverSocketFileDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "Error: serverSocketFileDesc failed to instantiate" << endl;
        error("Error: socket() for server failed.");
    }
    cout << "File descriptor: " << serverSocketFileDesc;


}