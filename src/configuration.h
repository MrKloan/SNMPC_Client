#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

void create_configuration(GtkButton *, Project *);
void submit_configuration_cb(GtkWidget *, Project *);
void save_configuration(Project *, Device *);
void refresh_configuration(GtkButton *, Project *);

#endif // CONFIGURATION_H_INCLUDED
