#ifndef PACKETS_H_INCLUDED
#define PACKETS_H_INCLUDED

char *formatPackets(unsigned short, ...);
void cipherPacket(Project *, char **);
void uncipherPacket(Project *, char **);

#endif // PACKETS_H_INCLUDED
