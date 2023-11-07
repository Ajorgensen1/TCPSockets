# TCPSockets
TCP Server and Client files to set up a connection using sockets
---
To use:
1. open two terminals on your machine
2. Compile server in one terminal using command < gcc -o TCPEchoServer -std=gnu99 TCPEchoServer4.c TCPServerUtility.c DieWithMessage.c AddressUtility.c >
3. Compile Client in other terminal using command < gcc -o TCPEchoClient -std=gnu99 TCPEchoClient4.c TCPClientUtility.c DieWithMessage.c AddressUtility.c >
4. Run the SERVER first using command < ./TCPEchoServer 'port_number goes here' >
5. Then run the CLIENT second using command < ./TCPEchoClient 'machine IP Address' 'Server port_number' >
6. On client side, select which file you would like to receive from server
