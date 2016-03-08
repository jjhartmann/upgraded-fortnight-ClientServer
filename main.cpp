#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

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

    if (type)
    {
        // Call client
        StartServer(atoi(argv[2]));
    }
    else
    {
        // Call server

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


}