#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib, "WS2_32.lib")
#pragma warning(disable: 4996)

std::string GetErrorMsgText(int code)
{
    std::string msgText;
    switch (code)
    {
    case WSAEINTR:        msgText = "The function was interrupted"; break;
    case WSAEACCES:        msgText = "Permission denied"; break;
    case WSAEFAULT:        msgText = "Bad address"; break;
    case WSAEINVAL:        msgText = "Invalid argument"; break;
    case WSAEMFILE:        msgText = "Too many open files"; break;
    case WSAEWOULDBLOCK:    msgText = "Resource temporarily unavailable"; break;
    case WSAEINPROGRESS:    msgText = "Operation in progress"; break;
    case WSAEALREADY:      msgText = "Operation already in progress"; break;
    case WSAENOTSOCK:      msgText = "Socket is not a valid socket"; break;
    case WSAEDESTADDRREQ:    msgText = "Destination address required"; break;
    case WSAEMSGSIZE:      msgText = "Message too long"; break;
    case WSAEPROTOTYPE:      msgText = "Wrong protocol type for socket"; break;
    case WSAENOPROTOOPT:    msgText = "Protocol option error"; break;
    case WSAEPROTONOSUPPORT:  msgText = "Protocol not supported"; break;
    case WSAESOCKTNOSUPPORT:  msgText = "Socket type not supported"; break;
    case WSAEOPNOTSUPP:      msgText = "Operation not supported"; break;
    case WSAEPFNOSUPPORT:    msgText = "Protocol family not supported"; break;
    case WSAEAFNOSUPPORT:    msgText = "Address family not supported by protocol"; break;
    case WSAEADDRINUSE:      msgText = "Address already in use"; break;
    case WSAEADDRNOTAVAIL:    msgText = "Requested address not available"; break;
    case WSAENETDOWN:      msgText = "Network is down"; break;
    case WSAENETUNREACH:    msgText = "Network unreachable"; break;
    case WSAENETRESET:      msgText = "Network reset"; break;
    case WSAECONNABORTED:    msgText = "Software caused connection abort"; break;
    case WSAECONNRESET:      msgText = "Connection reset"; break;
    case WSAENOBUFS:      msgText = "Insufficient buffer space"; break;
    case WSAEISCONN:      msgText = "Socket is already connected"; break;
    case WSAENOTCONN:      msgText = "Socket is not connected"; break;
    case WSAESHUTDOWN:      msgText = "Cannot send on a socket after shutdown"; break;
    case WSAETIMEDOUT:      msgText = "Connection timed out"; break;
    case WSAECONNREFUSED:    msgText = "Connection refused"; break;
    case WSAEHOSTDOWN:      msgText = "Host is down"; break;
    case WSAEHOSTUNREACH:    msgText = "No route to host"; break;
    case WSAEPROCLIM:      msgText = "Too many processes"; break;
    case WSASYSNOTREADY:    msgText = "Network subsystem is unavailable"; break;
    case WSAVERNOTSUPPORTED:  msgText = "Winsock version not supported"; break;
    case WSANOTINITIALISED:    msgText = "WSAStartup not yet called"; break;
    case WSAEDISCON:      msgText = "Graceful shutdown in progress"; break;
    case WSATYPE_NOT_FOUND:    msgText = "Class type not found"; break;
    case WSAHOST_NOT_FOUND:    msgText = "Host not found"; break;
    case WSATRY_AGAIN:      msgText = "Non-authoritative host not found"; break;
    case WSANO_RECOVERY:    msgText = "Non-recoverable error"; break;
    case WSANO_DATA:      msgText = "No record of requested type"; break;
    case WSA_INVALID_HANDLE:  msgText = "Invalid event handle"; break;
    case WSA_INVALID_PARAMETER: msgText = "One or more invalid parameters"; break;
    case WSA_IO_INCOMPLETE:    msgText = "I/O object not signaled"; break;
    case WSA_IO_PENDING:    msgText = "I/O operation pending"; break;
    case WSA_NOT_ENOUGH_MEMORY: msgText = "Insufficient memory"; break;
    case WSA_OPERATION_ABORTED: msgText = "Operation aborted"; break;
        //case WSAINVALIDPROCTABLE:  msgText = "Invalid service"; break;
        //case WSAINVALIDPROVIDER:  msgText = "Invalid service provider"; break;
        //case WSAPROVIDERFAILEDINIT: msgText = "Unable to initialize service provider"; break;


    case WSASYSCALLFAILURE:    msgText = "System call failure"; break;
    default:                msgText = "***ERROR***";      break;
    };
    return msgText;
};

std::string SetErrorMsgText(std::string msgText, int code)
{
    return msgText + GetErrorMsgText(code);
};

int main() {
    WSADATA wsaData;
    try
    {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
            throw SetErrorMsgText("Startup:", WSAGetLastError());

        SOCKET server_socket;
        if ((server_socket = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
            throw SetErrorMsgText("socket:", WSAGetLastError());

        SOCKADDR_IN serv;
        serv.sin_family = AF_INET;
        serv.sin_port = htons(2000);
        serv.sin_addr.s_addr = inet_addr("192.168.43.207");
        if (bind(server_socket, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
            throw SetErrorMsgText("bind:", WSAGetLastError());

        if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR)
            throw SetErrorMsgText("listen:", WSAGetLastError());

        SOCKET sS;
        SOCKADDR_IN client;
        memset(&client, 0, sizeof(client));
        int lc = sizeof(client);

        while (true)
        {
            if ((sS = accept(server_socket, (sockaddr*)&client, &lc)) == INVALID_SOCKET)
                throw SetErrorMsgText("accept:", WSAGetLastError());

            std::cout << "New connection [" << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << "]" << std::endl;

            clock_t start = clock();
            while (true) {
                char ibuf[50];
                memset(ibuf, 0, sizeof(ibuf));
                int libuf = 0;

                if ((libuf = recv(sS, ibuf, sizeof(ibuf), NULL)) == SOCKET_ERROR) {
                    std::cout << "Client disconnected or error occurred." << std::endl;
                    break;
                }

                if (libuf == 0) {
                    std::cout << "Client disconnected (" << clock() - start << "ms)." << std::endl;
                    break;
                }

                std::cout << "recv: " << ibuf << std::endl;

                if (send(sS, ibuf, libuf, NULL) == SOCKET_ERROR) {
                    throw SetErrorMsgText("send:", WSAGetLastError());
                }
            }

            if (closesocket(sS) == SOCKET_ERROR)
                throw SetErrorMsgText("closesocket:", WSAGetLastError());
        }

        if (closesocket(server_socket) == SOCKET_ERROR)
            throw SetErrorMsgText("closesocket:", WSAGetLastError());


        if (WSACleanup() == SOCKET_ERROR)
            throw SetErrorMsgText("Cleanup:", WSAGetLastError());
    }
    catch (std::string errorMsgText)
    {
        std::cout << std::endl << errorMsgText;
    }

    return 0;
}
