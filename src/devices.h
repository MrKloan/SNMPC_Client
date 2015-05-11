#ifndef DEVICES_H_INCLUDED
#define DEVICES_H_INCLUDED

void addDevice(Devices **devices, const gchar *, const gchar *, const gchar *, const gchar *);

Devices *getLastDevice(Devices **);
Devices *getDeviceByIndex(Devices *, gushort);
Devices *getDeviceByName(Devices *, const gchar *, gshort *);
gushort countDevices(Devices *);
void delDeviceByIndex(Devices **, gushort);
void freeDevices(Devices **);


#endif // DEVICES_H_INCLUDED
