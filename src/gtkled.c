#include "includes.h"

GtkWidget *gtk_led_new(gboolean state)
{
    GtkWidget *led;

    if(state)
        led = gtk_image_new_from_file("img/led_on.png");
    else
        led = gtk_image_new_from_file("img/led_off.png");

    g_object_set_data(G_OBJECT(led), "state", GUINT_TO_POINTER(state));

    return led;
}

void gtk_led_set_state(GtkWidget *led, gboolean new_state)
{
    //gdk_threads_enter();
    if(new_state)
        gtk_image_set_from_file(GTK_IMAGE(led), "img/led_on.png");
    else
        gtk_image_set_from_file(GTK_IMAGE(led), "img/led_off.png");

    g_object_set_data(G_OBJECT(led), "state", GUINT_TO_POINTER(new_state));
    //gdk_threads_leave();
}

void gtk_led_set_state_thread(GtkWidget *led, gboolean new_state)
{
    gdk_threads_enter();
    if(new_state)
        gtk_image_set_from_file(GTK_IMAGE(led), "img/led_on.png");
    else
        gtk_image_set_from_file(GTK_IMAGE(led), "img/led_off.png");

    g_object_set_data(G_OBJECT(led), "state", GUINT_TO_POINTER(new_state));
    gdk_threads_leave();
}

gpointer led_pulse_thread(GtkWidget *led)
{
    gushort duration = (gushort) GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(led), "duration"));
    gboolean state =  (gboolean) GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(led), "state"));

    gtk_led_set_state_thread(led, !state);

    snmpc_sleep(duration);

    gtk_led_set_state(led, state);

    return NULL;
}
