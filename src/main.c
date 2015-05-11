#include "includes.h"

int main(int argc, char *argv[])
{
    Project project;

    /* Devices linked list Initialization */
    project.devices                     = NULL;
    project.server.running              = FALSE;
    project.visualization.window        = NULL;
    project.visualization.visualizing   = FALSE;
    project.schedule.flag               = 0;
    project.adddevice.nodevice          = 0;
    project.home.flag                   = 0;

    /* GTK Threads Initialization */
    if(!g_thread_supported())
        g_thread_init(NULL);
    gdk_threads_init();

    /* GTK Initialization */
    gtk_init(&argc, &argv);

    /* Chargement de la matrice de chiffrement */
    load_matrix(&project);

    create_login(&project);

    /* Display all widgets */
    gtk_widget_show_all(project.login.window);

    /* Main Loop */
    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();

    serverFree(&project.server);
    freeDevices(&project.devices);

    return EXIT_SUCCESS;
}
