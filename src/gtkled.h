#ifndef GTKLED_H_INCLUDED
#define GTKLED_H_INCLUDED

GtkWidget *gtk_led_new(gboolean);
void gtk_led_set_state(GtkWidget *, gboolean);
void gtk_led_set_state_thread(GtkWidget *, gboolean);
gpointer led_pulse_thread(GtkWidget *);

#endif // GTKLED_H_INCLUDED
