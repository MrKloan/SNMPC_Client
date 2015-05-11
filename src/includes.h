#ifndef INCLUDES_H_INCLUDED
#define INCLUDES_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

#ifdef WIN32
    #include <windows.h>
    #include <winsock2.h>

#elif defined (linux)
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
#endif

#include "defines.h"
#include "structures.h"
#include "server.h"
#include "socket.h"
#include "packets.h"
#include "sha1.h"

#include "tasks.h"
#include "devices.h"
#include "queue.h"

#include "login.h"
#include "home.h"
#include "adddevice.h"
#include "deldevice.h"
#include "profile.h"
#include "visualization.h"
#include "schedule.h"

#include "iosetup.h"
#include "networksetup.h"
#include "snmpsetup.h"
#include "informations.h"
#include "configuration.h"

#include "utilities.h"
#include "gtkled.h"

#endif // INCLUDES_H_INCLUDED
