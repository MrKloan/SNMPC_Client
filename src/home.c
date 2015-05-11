#include "includes.h"

/**
* Fonction de création de la fenêtre d'accueil
*/
void create_home(Project *project)
{
    GtkWidget *vbox;

    project->home.flag = 1;
    /* Window Config */
    project->home.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(project->home.window), TITLE);
    gtk_window_set_default_size(GTK_WINDOW(project->home.window), 850, 400);
    gtk_window_set_position(GTK_WINDOW(project->home.window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(project->home.window), FALSE);

    /* Vertical Layout */
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(project->home.window), GTK_WIDGET(vbox));

    /* Menu */
    create_menu(project);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(project->home.menu_bar), FALSE, FALSE, 0);

    /* Notebook */
    project->home.notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(project->home.notebook), GTK_POS_TOP);
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(project->home.notebook), TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(project->home.notebook), 850, 400);
    gtk_box_pack_start(GTK_BOX(vbox), project->home.notebook, TRUE, TRUE, 0);

    /* Callbacks */

    g_signal_connect(GTK_OBJECT(project->home.window),
                     "destroy",
                     G_CALLBACK(destroy_home),
                     project);
    g_signal_connect(GTK_OBJECT(project->home.window),
                     "delete_event",
                     G_CALLBACK(delete_home),
                     project);
}

/**
* Menu de la fenêtre d'accueil
*/
void create_menu(Project *project)
{
    GtkWidget   *menu;
    GtkWidget   *menu_item;

    /* Create menu bar */
    project->home.menu_bar = gtk_menu_bar_new();

    /* Menu items & submenus */
    menu = gtk_menu_new();
        menu_item = gtk_menu_item_new_with_label("File");
            gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
            gtk_menu_shell_append(GTK_MENU_SHELL(project->home.menu_bar), menu_item);

        menu_item = gtk_menu_item_new_with_label("Disconnect");
            g_signal_connect(GTK_OBJECT(menu_item), "activate", G_CALLBACK(disconnect_cb), project);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

        menu_item = gtk_menu_item_new_with_label("Quit");
            g_signal_connect(GTK_OBJECT(menu_item),
                             "activate",
                             G_CALLBACK(gtk_main_quit),
                             NULL);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    menu = gtk_menu_new();
        menu_item = gtk_menu_item_new_with_label("Settings");
            gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
            gtk_menu_shell_append(GTK_MENU_SHELL(project->home.menu_bar), menu_item);

        menu_item = gtk_menu_item_new_with_label("Add Device");
            g_signal_connect(GTK_OBJECT(menu_item), "activate", G_CALLBACK(create_add_device), project);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

        menu_item = gtk_menu_item_new_with_label("Delete Device");
            g_signal_connect(GTK_OBJECT(menu_item), "activate", G_CALLBACK(create_del_device), project);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

        menu_item = gtk_menu_item_new_with_label("Profile");
            g_signal_connect(GTK_OBJECT(menu_item), "activate", G_CALLBACK(create_profile), project);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    menu = gtk_menu_new();
        menu_item = gtk_menu_item_new_with_label("Help");
            gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
            gtk_menu_shell_append(GTK_MENU_SHELL(project->home.menu_bar), menu_item);

        menu_item = gtk_menu_item_new_with_label("About");
            g_signal_connect(GTK_OBJECT(menu_item), "activate", G_CALLBACK(about_cb), NULL);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
}

/**
* Corps de la fenêtre d'accueil
*/
void create_page(Project *project, Device *device)
{
    GtkWidget   *label;
    GtkWidget   *tframe;
    GtkWidget   *button;

    gushort     i, row, col = 0;

    const gchar *options[4] = {
        "All ON",
        "All OFF",
        "Pulse All",
        "Visualization"
    };
    const gchar *configs[4] = {
        "Network Setup",
        "I/O Setup",
        "SNMP Setup",
        "Informations"
    };

    /* Page Name */
    label = gtk_label_new(device->name);

    /* Table Layout */
    device->table = gtk_table_new(5, 2, TRUE);

    /* Relays */
    for(i=0, row=0; i<8; i++, row++)
    {
        if(i==4)
        {
            row = 0;
            col = 1;
        }
        device->frame[i] = gtk_frame_new("");
        shadow_type(GTK_FRAME(device->frame[i]), GTK_SHADOW_IN);
        gtk_table_attach(GTK_TABLE(device->table),
                         device->frame[i],
                         col, col+1, row, row+1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         10, 5);

        tframe = gtk_table_new(2, 4, TRUE);
        gtk_container_add(GTK_CONTAINER(device->frame[i]), GTK_WIDGET(tframe));

        device->relays_led[i] = gtk_led_new(FALSE);
        gtk_table_attach(GTK_TABLE(tframe),
                         device->relays_led[i],
                         0, 1, 0, 2,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

        button = gtk_button_new_with_label("ON/OFF");
        g_object_set_data(G_OBJECT(button), "device", (gpointer) device);
        g_object_set_data(G_OBJECT(button), "relay", GINT_TO_POINTER(i+1));
        g_signal_connect(G_OBJECT(button),
                         "clicked",
                         G_CALLBACK(on_off_cb),
                         project);
        gtk_table_attach(GTK_TABLE(tframe),
                         button,
                         1, 2, 0, 2,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

        button = gtk_button_new_with_label("Pulse");
        g_object_set_data(G_OBJECT(button), "device", (gpointer) device);
        g_object_set_data(G_OBJECT(button), "relay", GINT_TO_POINTER(i+1));
        g_signal_connect(G_OBJECT(button),
                         "clicked",
                         G_CALLBACK(pulse_cb),
                         project);
        gtk_table_attach(GTK_TABLE(tframe),
                         button,
                         2, 3, 0, 2,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

        button = gtk_button_new_with_label("Schedule");
        g_object_set_data(G_OBJECT(button), "device", (gpointer) device);
        g_object_set_data(G_OBJECT(button), "relay", GUINT_TO_POINTER(i+1));
        g_signal_connect(G_OBJECT(button),
                         "clicked",
                         G_CALLBACK(schedule_cb),
                         project);
        gtk_table_attach(GTK_TABLE(tframe),
                         button,
                         3, 4, 0, 2,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);
    }

    /* Options */
    device->frame[8] = gtk_frame_new("Options");
    shadow_type(GTK_FRAME(device->frame[8]), GTK_SHADOW_IN);
    gtk_table_attach(GTK_TABLE(device->table),
                     device->frame[8],
                     0, 1, 4, 5,
                     GTK_EXPAND | GTK_FILL,
                     GTK_EXPAND | GTK_FILL,
                     10, 5);
    tframe = gtk_table_new(2, 4, TRUE);
    gtk_container_add(GTK_CONTAINER(device->frame[8]), GTK_WIDGET(tframe));

    for(i=0; i<4; i++)
    {
        button = gtk_button_new_with_label(options[i]);
        g_object_set_data(G_OBJECT(button), "device", (gpointer) device);
        gtk_table_attach(GTK_TABLE(tframe),
                         button,
                         i, i+1, 0, 2,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

        if(i == 0 || i == 1)
        {
            g_object_set_data(G_OBJECT(button), "state", GUINT_TO_POINTER((i == 0) ? 1 : 0));
            g_signal_connect(G_OBJECT(button),
                         "clicked",
                         G_CALLBACK(all_on_off_cb),
                         project);
        }
        else if(i == 2)
            g_signal_connect(G_OBJECT(button),
                         "clicked",
                         G_CALLBACK(all_pulse_cb),
                         project);
        else if(i == 3)
            g_signal_connect(G_OBJECT(button),
                         "clicked",
                         G_CALLBACK(visualization_cb),
                         project);
    }

    /* Configurations */
    device->frame[9] = gtk_frame_new("Configurations");
    shadow_type(GTK_FRAME(device->frame[9]), GTK_SHADOW_IN);
    gtk_table_attach(GTK_TABLE(device->table),
                     device->frame[9],
                     1, 2, 4, 5,
                     GTK_EXPAND | GTK_FILL,
                     GTK_EXPAND | GTK_FILL,
                     10, 5);
    tframe = gtk_table_new(2, 3, TRUE);
    gtk_container_add(GTK_CONTAINER(device->frame[9]), GTK_WIDGET(tframe));

    for(i=0; i<4; i++)
    {
        button = gtk_button_new_with_label(configs[i]);
        g_object_set_data(G_OBJECT(button), "device", (gpointer) device);
        gtk_table_attach(GTK_TABLE(tframe),
                         button,
                         i, i+1, 0, 2,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

        if(i==0)
            g_signal_connect(G_OBJECT(button),
                         "clicked",
                         G_CALLBACK(create_network_setup),
                         project);
        else if(i==1)
            g_signal_connect(G_OBJECT(button),
                         "clicked",
                         G_CALLBACK(create_io_setup),
                         project);
        else if(i==2)
            g_signal_connect(G_OBJECT(button),
                         "clicked",
                         G_CALLBACK(create_snmp_setup),
                         project);
        else if(i==3)
            g_signal_connect(G_OBJECT(button),
                         "clicked",
                         G_CALLBACK(create_informations),
                         project);
    }

    /* Page Integration */
    gtk_notebook_append_page(GTK_NOTEBOOK(project->home.notebook), device->table, label);
}

/*****
* Fonctions de configuration d'un seul relais sur un device
******/

/**
* Changement d'image pour la LED Green/Red
*//*
void led_switch_cb(GtkWidget *widget, gpointer data)
{
   GtkWidget *led = (GtkWidget *) data;

   gtk_led_set_state(led, !gtk_led_get_state(GTK_LED(led)));
}*/

/**
* Fonction de changement d'état pour un relais précis sur un device précis
*/
void on_off_cb(GtkButton *button, Project *project)
{
    Device      *device;
    guint       relay;
    char        buffer[10];

    device = (Device *)g_object_get_data(G_OBJECT(button), "device");
    relay  = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "relay"));

    sprintf(buffer, "relay%u", relay);

    char *packet =  formatPackets(3,
                    device->name,
                    buffer,
                    (device->relays_state[relay-1] == FALSE) ? "on" : "off");

    pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);
    g_cond_broadcast(project->server.notifier);

    free(packet);
}

/**
* Fonction d'activation du Pulse d'un relais
*/
void pulse_cb(GtkButton *button, Project *project)
{
    Device      *device;
    guint       relay;
    char        buffer[10];

    device = (Device *)g_object_get_data(G_OBJECT(button), "device");
    relay  = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "relay"));

    sprintf(buffer, "pulse%u", relay);

    char *packet =  formatPackets(3,
                    device->name,
                    buffer,
                    "on");

    pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);
    g_cond_broadcast(project->server.notifier);

    free(packet);
}

/**
* Fonction de création de la fenêtre de planification pour un relais précis
*/
void schedule_cb(GtkButton *button, Project *project)
{
    Device *device  = (Device *) g_object_get_data(G_OBJECT(button), "device");
    gushort relay   = (gushort) GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "relay"));

    create_schedule(project, device, relay);
}

/*****
* Bloc de fonctions de gestion simultanée de tous les relais d'un device
******/

/**
* Fonction de changement d'état pour tous les relais sur un device précis
*/
void all_on_off_cb(GtkButton *button, Project *project)
{
    guint       state;
    Device      *device;

    device = (Device *)g_object_get_data(G_OBJECT(button), "device");
    state  = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "state"));

    char *packet =  formatPackets(3,
                    device->name,
                    (state == 1) ? "allOn" : "allOff",
                    "on");

    pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);
    g_cond_broadcast(project->server.notifier);

    free(packet);
}

/**
* Fonction d'activation du pulse de tous relais sur un device précis
*/
void all_pulse_cb(GtkButton *button, Project *project)
{
    Device      *device;

    device = (Device *)g_object_get_data(G_OBJECT(button), "device");

    char *packet =  formatPackets(3,
                    device->name,
                    "allPulse",
                    "on");

    pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);
    g_cond_broadcast(project->server.notifier);

    free(packet);
}

/**
* Fonction d'ouverture de le fenêtre de visualisation
*/
void visualization_cb(GtkButton *button, Project *project)
{
    if(project->visualization.window == NULL)
    {
        start_visualization(project);
        create_visualization(project);
    }
}

/**
* Fonction de déconnexion et retour au login
*/
void disconnect_cb(GtkWidget *window, Project *project)
{
    project->home.flag = 0;
    gtk_widget_destroy(project->home.window);

    create_login(project);
    gtk_widget_show_all(project->login.window);

    //serverFree(&project->server);
}

/**
* Fonction About
*/
void about_cb(GtkButton *button)
{
    GtkAboutDialog *dialog;

    const gchar* name                   = TITLE;
    const gchar* logo_icon_name         = LOGO;
    const gchar* program_name           = TITLE;
    const gchar* version                = VERSION;
    const gchar* comments               = COMMENTS;
    const gchar* copyright              = COPYRIGHT;

    dialog                              = GTK_ABOUT_DIALOG(gtk_about_dialog_new());

    /* Configuration Assignment */
    gtk_about_dialog_set_name           (dialog, name);
    gtk_about_dialog_set_logo_icon_name (dialog, logo_icon_name);
    gtk_about_dialog_set_program_name   (dialog, program_name);
    gtk_about_dialog_set_version        (dialog, version);
    gtk_about_dialog_set_comments       (dialog, comments);
    gtk_about_dialog_set_copyright      (dialog, copyright);


    /* Callback */
    gtk_signal_connect(GTK_OBJECT(dialog), "response", GTK_SIGNAL_FUNC(gtk_widget_destroy), NULL);

    /* Run Dialog Box */
    gtk_dialog_run(GTK_DIALOG(dialog));
}

void delete_home(GtkWidget *widget, GdkEvent  *event, Project *project)
{
    if(project->home.flag)
        gtk_main_quit();
}

void destroy_home(GtkWidget *widget, Project *project)
{
    if(project->home.flag)
        gtk_main_quit();
}
