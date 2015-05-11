#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

#define TITLE               "SNMPC Client"
#define VERSION             "v1.0"
#define LOGO                "applications-development"
#define COPYRIGHT           "© 2014 Mathieu BOISNARD & Valentin FRIES"
#define COMMENTS            "Projet C"

#define SALT                ""
#define PEPPER              ""

#define MAX_LENGTH          20
#define SOCKET_BUFFER       5120
#define ENTRY_BUFFER        50
#define PACKET_DELIMITER    ";"
#define PACKET_END          "|"

#if defined (linux)
    #define INVALID_SOCKET  -1
    #define SOCKET_ERROR    -1
    #define closesocket(s)  close(s)
    #define snmpc_sleep(s)  usleep(s*1000000)
    #define shadow_type(frame, mode)   gtk_frame_set_shadow_type(frame, mode)
#elif defined WIN32
    #define snmpc_sleep(s)  Sleep(s)
    #define shadow_type(frame, mode)
#endif

enum
{
    STATIC,
    DHCP
};

enum
{
    IPPORT_ADDR,
    IP_ADDR,
    MAC_ADDR
};

#endif // DEFINES_H_INCLUDED
