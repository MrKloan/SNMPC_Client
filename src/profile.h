#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED

void create_profile(GtkButton *, Project *);
void submit_profile_cb(GtkWidget *, Project *);
void save_profile(Project *);
void delete_account_cb(GtkButton *, Project *);

#endif // SETTINGS_H_INCLUDED
