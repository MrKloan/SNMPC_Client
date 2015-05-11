#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#if defined (linux)
    typedef int SOCKET;
    typedef struct sockaddr_in  SOCKADDR_IN;
    typedef struct sockaddr     SOCKADDR;
    typedef struct in_addr      IN_ADDR;
    typedef struct hostent      HOSTENT;
    typedef struct timeval      timeval;
#endif

typedef struct SHA1Context
{
    unsigned        Message_Digest[5];
    char            sha_message[41];

    unsigned        Length_Low;
    unsigned        Length_High;

    unsigned char   Message_Block[64];
    int             Message_Block_Index;

    int             Computed;
    int             Corrupted;

} SHA1Context;

typedef struct Matrix
{
    gushort         matrix[4][8];
    gushort         columns;
    gushort         lines;

} Matrix;

typedef struct Queue
{
    char            *value;
    struct Queue    *next;

} Queue;

typedef struct Server
{
    gboolean        running;

    SOCKET          socket;
    SOCKADDR_IN     infos;
    GThread         *listener;
    GThread         *sender;
    GThread         *dispatcher;
    GMutex          *lock;
    GMutex          *sender_lock;
    GMutex          *dispatch_lock;
    GCond           *notifier;
    GCond           *dispatch_notifier;

    Queue           *sender_queue;
    Queue           *dispatch_queue;
    char            buffer[SOCKET_BUFFER];

} Server;

typedef struct Login
{
    GtkWidget       *window;
    GtkWidget       *entry[3];
    gchar           name[20];
    gchar           login[41];
    gchar           password[41];

} Login;

typedef struct Home
{
    GtkWidget       *window;
    GtkWidget       *menu_bar;
    GtkWidget       *notebook;
    gushort         flag;

} Home;

typedef struct AddDevice
{
    GtkWidget       *dialog;
    GtkWidget       *entry[4];
    gushort         nodevice;

} AddDevice;

typedef struct DelDevice
{
    GtkWidget       *dialog;
    GtkWidget       *combobox;

} DelDevice;

typedef struct Profile
{
    GtkWidget       *dialog;
    GtkWidget       *entry[3];

} Profile;

typedef struct Visualization
{
    gboolean        visualizing;

    GtkWidget       *window;
    GtkWidget       **table;
    GtkWidget       **frame;
    GtkWidget       ***led;
    GtkWidget       ***label;

} Visualization;

typedef struct Schedule
{
    GtkWidget       *dialog;
    GtkWidget       *table;

    GtkTreeStore    *liststore;
    GtkWidget       *listview;

    gushort         flag;

} Schedule;

typedef struct NewTask
{
    GtkWidget       *dialog;
    GtkWidget       *table;

    GtkWidget       *name;
    GtkWidget       *enabled;
    GtkWidget       *repeat;
    GtkWidget       *type;
    GtkWidget       *date;
    GtkWidget       *week;
    GtkWidget       *days[7];
    GtkWidget       *hours[2];
    GtkWidget       *minutes[2];

    GtkWidget       *day_label[8];
    gushort         repeat_flag;

    GtkWidget       *type_label;
    gushort         type_flag;

} NewTask;

typedef struct NetworkSetup
{
    GtkWidget       *dialog;
    GtkWidget       *entry[5];

} NetworkSetup;

typedef struct IOSetup
{
    GtkWidget       *dialog;
    GtkWidget       *name[9];
    GtkWidget       *pulse_duration[8];
    GtkWidget       *visualization;
    gushort         visualization_rate;

} IOSetup;

typedef struct SNMPSetup
{
    GtkWidget       *dialog;
    GtkWidget       *entry[3];

} SNMPSetup;

typedef struct Configuration
{
    GtkWidget       *dialog;
    GtkWidget       *entry[4];

} Configuration;

typedef struct Task
{
    GtkWidget       *dialog;
    GtkWidget       *table;

    GtkWidget       *name;
    GtkWidget       *enabled;
    GtkWidget       *repeat;
    GtkWidget       *type;
    GtkWidget       *date;
    GtkWidget       *week;
    GtkWidget       *days[7];
    GtkWidget       *hours[2];
    GtkWidget       *minutes[2];

    GtkWidget       *day_label[8];
    gushort         repeat_flag;

    GtkWidget       *type_label;
    gushort         type_flag;

    char            name_val[20];
    gushort         enabled_val;
    gushort         repeat_val;
    gushort         type_val;
    char            date_val[11]; //jj/mm/aaaa
    gushort         weeks_val;
    gushort         days_val[7];
    gushort         hours_val[2];
    gushort         minutes_val[2];

} Task;

typedef struct Tasks {
    Task         element;
    struct Tasks *next;
} Tasks;

typedef struct Device
{
    gchar           name[20];
    gchar           device_name[50];
    gchar           version[50];
    gchar           date[50];

    gchar           communities[2][14];

    gchar           ip[16];
    gboolean        dhcp_config;
    gchar           subnet_mask[16];
    gchar           gateway[16];
    gchar           dns[16];
    gchar           mac_addr[18];

    gboolean        trap_enabled;
    gchar           trap_ip[16];
    gchar           trap_community[14];

    gchar           relays_name[8][12];
    gboolean        relays_state[8];
    gboolean        pulse_state[8];
    gushort         pulse_duration[8];
    gboolean        digital_input;

    gboolean        restart;
    gboolean        disconnected;

    GtkWidget       *relays_led[8];
    GtkWidget       *frame[10];
    GtkWidget       *table;

    Tasks           *tasks[8];

} Device;

typedef struct Devices
{
    Device          element;
    struct Devices  *next;

} Devices;

typedef struct Project
{
    Server          server;

    Login           login;
    Home            home;

    AddDevice       adddevice;
    DelDevice       deldevice;
    Profile         profile;

    Visualization   visualization;

    Schedule        schedule;
    NewTask         newtask;

    NetworkSetup    networksetup;
    IOSetup         iosetup;
    SNMPSetup       snmpsetup;
    Configuration   configuration;

    Devices         *devices;

    SHA1Context     sha;
    Matrix          matrix;

} Project;


#endif // STRUCTURES_H_INCLUDED
