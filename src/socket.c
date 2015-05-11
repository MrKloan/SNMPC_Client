#include "includes.h"

unsigned short SocketStart(void)
{
#ifdef WIN32
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2), &wsa) == 0)
        return 1;
    else
        return 0;
#elif defined (linux)
    return 1;
#endif
}

unsigned short SocketEnd(void)
{
#ifdef WIN32
    if(WSACleanup() == 0)
        return 1;
    else
        return 0;
#elif defined (linux)
    return 1;
#endif
}

int SocketError(void)
{
#ifdef WIN32
    return WSAGetLastError();
#elif defined (linux)
    return errno;
#endif
}

char *SocketStrError(void)
{
#ifdef WIN32
    LPTSTR buff;
    //char *msg;

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &buff, SOCKET_BUFFER, NULL);
    //strcpy(msg, buff);
    //LocalFree(buff);
    return buff;
#elif defined (linux)
    return strerror(errno);
#endif
}

unsigned short SocketClear(SOCKET socket)
{
    if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, "1", sizeof(char)) == SOCKET_ERROR)
        return 0;
    else
        return 1;
}

unsigned short initSocket(SOCKET *sock)
{
    /*timeval timeout;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;*/

    if((*sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        return 0;
    else
    {
        /*if(setsockopt(*sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0
        && setsockopt(*sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
        {
            printf("%d : %s", SocketError(), SocketStrError());
            return 0;
        }
        else*/
            return 1;
    }
}

unsigned short socketConnect(Server *server, const char *hostname, unsigned short port)
{
    HOSTENT *hostinfo = gethostbyname(hostname);

    if(hostinfo == NULL)
        return 0;
    else
    {
        server->infos.sin_addr      = *((IN_ADDR *)hostinfo->h_addr);
        server->infos.sin_port      = htons(port);
        server->infos.sin_family    = AF_INET;

        if(connect(server->socket, (SOCKADDR *)&server->infos, sizeof(server->infos)) == SOCKET_ERROR)
            return 0;
        else
            return 1;
    }
}

unsigned short socketSend(Server *server)
{
    if(send(server->socket, server->buffer, strlen(server->buffer), 0) < 0)
        return 0;
    else
    {
        g_print("SENT: %s\n", server->buffer);
        return 1;
    }
}

unsigned short socketReceive(Server *server)
{
    int n;

    if((n = recv(server->socket, server->buffer, SOCKET_BUFFER-1, 0)) <= 0)
        return 0;
    else
    {
        server->buffer[n] = '\0';
        g_print("RECEIVED: %s\n", server->buffer);
        return 1;
    }
}

void socketFree(Server *server)
{
    closesocket(server->socket);
}
