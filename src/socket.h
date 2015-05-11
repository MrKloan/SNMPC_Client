#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

unsigned short SocketStart(void);
unsigned short SocketEnd(void);
int SocketError(void);
char *SocketStrError(void);
unsigned short SocketClear(SOCKET);
unsigned short initSocket(SOCKET *);
unsigned short socketConnect(Server *, const char *, unsigned short);
unsigned short socketSend(Server *);
unsigned short socketReceive(Server *);
void socketFree(Server *);

#endif // SOCKET_H_INCLUDED
