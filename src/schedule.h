#ifndef SCHEDULE_H_INCLUDED
#define SCHEDULE_H_INCLUDED

void create_schedule(Project *, Device *, gushort);
void create_listview(Project *, Device *, gushort);
void liststore_maj(Project *, Device *, gushort);

void add_task_cb(GtkButton *, Project *);

void repeat_change(GtkComboBox *, Project *);
void type_change(GtkComboBox *, Project *);

void submit_task_cb(GtkWidget *, Project *);
void save_task(Project *, const gchar *, gushort);

void modify_selected_task_cb(GtkButton *, Project *);
void modify_task_cb(Project *, Device *, gushort, Tasks *);
void modify_repeat_change(GtkComboBox *, Tasks *);
void modify_type_change(GtkComboBox *, Tasks *);
void submit_modify_task_cb(GtkWidget *, Project *);
void save_modify_task(Project *, Tasks *, const char *, gushort);

void delete_selected_task_cb(GtkButton *, Project *);

void close_schedule_cb(GtkButton *, GtkWidget *);

#endif // SCHEDULE_H_INCLUDED
