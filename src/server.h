#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

void serverInit(Server *);
void serverFree(Server *);
gpointer listener(gpointer);
gpointer dispatcher(gpointer);
gpointer sender(gpointer);
void handlePacket(gpointer, char *);

#endif // SERVER_H_INCLUDED
