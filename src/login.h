#ifndef LOGIN_H_INCLUDED
#define LOGIN_H_INCLUDED

void create_login(Project *);

void submit_login_cb(GtkButton *, Project *);
void send_login(Project *, char **);
void delete_login(GtkWidget *, GdkEvent  *, Project*);
void destroy_login(GtkWidget *, Project*);

#endif // LOGIN_H_INCLUDED
