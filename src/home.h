#ifndef HOME_H_INCLUDED
#define HOME_H_INCLUDED

void create_home(Project *);
void create_menu(Project *);
void create_page(Project *, Device *);

void led_switch_cb(GtkWidget *, gpointer);
void on_off_cb(GtkButton *, Project *);
void pulse_cb(GtkButton *, Project *);
void schedule_cb(GtkButton *, Project *);

void all_on_off_cb(GtkButton *, Project *);
void all_pulse_cb(GtkButton *, Project *);

void visualization_cb(GtkButton *, Project *);

void disconnect_cb(GtkWidget *, Project *);
void about_cb(GtkButton *);

void delete_home(GtkWidget *, GdkEvent  *, Project *);
void destroy_home(GtkWidget *, Project *);

#endif // HOME_H_INCLUDED
