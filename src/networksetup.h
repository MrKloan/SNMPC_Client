#ifndef NETWORKSETUP_H_INCLUDED
#define NETWORKSETUP_H_INCLUDED

void create_network_setup(GtkButton *, Project *);
void submit_network_cb(GtkWidget *, Project *);
void save_network(Project *, Device *);
void restart_device_cb(GtkButton *, Project *);

#endif // NETWORKSETUP_H_INCLUDED
