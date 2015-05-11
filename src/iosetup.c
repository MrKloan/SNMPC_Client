#include "includes.h"

/**
* Fonction de création de la fenêtre IO Setup pour un device précis
*/
void create_io_setup(GtkButton *button, Project *project)
{
    GtkWidget   *content;
    GtkWidget   *table;
    GtkWidget   *frame;
    GtkWidget   *tframe;
    GtkWidget   *hbox;
    GtkWidget   *label;

    guint       result;
    gushort     i, row, col = 0;
    gchar       text[20];
    Device      *device;

    device = (Device *)g_object_get_data(G_OBJECT(button), "device");
    project->iosetup.dialog = gtk_dialog_new_with_buttons("I/O Setup",
                              GTK_WINDOW(project->home.window),
                              GTK_DIALOG_MODAL,
                              "Save",
                              GTK_RESPONSE_ACCEPT,
                              "Cancel",
                              GTK_RESPONSE_CANCEL,
                              NULL);
    gtk_widget_set_size_request(project->iosetup.dialog, 500, 400);
    gtk_window_set_resizable(GTK_WINDOW(project->iosetup.dialog), FALSE);

    content = gtk_dialog_get_content_area(GTK_DIALOG(project->iosetup.dialog));

    /* Table Layout */
    table = gtk_table_new(7, 2, TRUE);


    frame = gtk_frame_new("Relay Description");
    shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_table_attach(GTK_TABLE(table),
                     frame,
                     0, 2, 0, 3,
                     GTK_EXPAND | GTK_FILL,
                     GTK_EXPAND | GTK_FILL,
                     0, 10);

    tframe = gtk_table_new(4, 2, TRUE);
    gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(tframe));

    for(i=0, row=0; i<8; i++, row++)
    {
        if(i==4)
        {
            row = 0;
            col = 1;
        }

		hbox = gtk_hbox_new(TRUE, 0);

        sprintf(text, "Relay %hu :", i+1);
        label = gtk_label_new(text);
        gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);

        project->iosetup.name[i] = gtk_entry_new();
        g_object_set_data(G_OBJECT(project->iosetup.name[i]),
                          "device",
                          (gpointer) device);
        g_signal_connect(GTK_OBJECT(project->iosetup.name[i]),
                         "activate",
                         G_CALLBACK(submit_io_cb),
                         project);
        gtk_entry_set_max_length(GTK_ENTRY(project->iosetup.name[i]), 11);
        gtk_box_pack_start(GTK_BOX(hbox), project->iosetup.name[i], TRUE, TRUE, 0);
        if(device->relays_name[i])
            gtk_entry_set_text(GTK_ENTRY(project->iosetup.name[i]), device->relays_name[i]);
        gtk_table_attach(GTK_TABLE(tframe),
                         hbox,
                         col, col+1, row, row+1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);
    }

    frame = gtk_frame_new("Pulse Duration (seconds)");
    shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_table_attach(GTK_TABLE(table),
                         frame,
                         0, 2, 3, 6,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

    tframe = gtk_table_new(4, 2, TRUE);
    gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(tframe));

    col = 0;

    for(i=0, row=0; i<8; i++, row++)
    {
        if(i==4)
        {
            row = 0;
            col = 1;
        }

		hbox = gtk_hbox_new(TRUE, 0);

        sprintf(text, "Relay %hu :", i+1);
        label = gtk_label_new(text);
        gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);

        project->iosetup.pulse_duration[i] = gtk_spin_button_new_with_range(1, 253, 1);
        gtk_box_pack_start(GTK_BOX(hbox), project->iosetup.pulse_duration[i], TRUE, TRUE, 0);
        g_object_set_data(G_OBJECT(project->iosetup.pulse_duration[i]), "device", (gpointer) device);
        g_signal_connect(GTK_OBJECT(project->iosetup.pulse_duration[i]),
                         "activate",
                         G_CALLBACK(submit_io_cb),
                         project);
        if(device->pulse_duration[i])
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->iosetup.pulse_duration[i]), device->pulse_duration[i]);
        gtk_table_attach(GTK_TABLE(tframe),
                         hbox,
                         col, col+1, row, row+1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);
    }

    //Visualisation
    frame = gtk_frame_new("Visualization (seconds)");
    shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_table_attach(GTK_TABLE(table),
                         frame,
                         0, 2, 6, 7,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

    tframe = gtk_table_new(1, 4, TRUE);

    strcpy(text, "Auto refresh rate");
    label = gtk_label_new(text);
    gtk_table_attach(GTK_TABLE(tframe),
                         label,
                         0, 2, 0, 1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

    project->iosetup.visualization = gtk_spin_button_new_with_range(1, 253, 1);
    if(project->iosetup.visualization_rate)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->iosetup.visualization), project->iosetup.visualization_rate);
    gtk_table_attach(GTK_TABLE(tframe),
                         project->iosetup.visualization,
                         2, 3, 0, 1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

    gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(tframe));

    //Attach table
    gtk_container_add(GTK_CONTAINER(content), table);
    gtk_widget_show_all(project->iosetup.dialog);

    result = gtk_dialog_run(GTK_DIALOG(project->iosetup.dialog));

    switch(result)
    {
        case GTK_RESPONSE_ACCEPT :
            g_object_set_data(G_OBJECT(project->iosetup.dialog), "device", (gpointer) device);
            submit_io_cb(GTK_WIDGET(project->iosetup.dialog), project);
            break;

        case GTK_RESPONSE_CANCEL :
            gtk_widget_destroy(project->iosetup.dialog);
            break;
    }
}

/**
* Fonction de vérification et traitement des données avant l'envoi au serveur
*/
void submit_io_cb(GtkWidget *submit, Project *project)
{
    gushort i;
    char    buffer[10];

    Device  *device = (Device *)g_object_get_data(G_OBJECT(submit), "device");


    for(i = 0; i < 8; i++)
    {
        if(g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->iosetup.name[i])), "") == 0)
        {
            sprintf(buffer, "Relay %hu", i+1);
            gtk_entry_set_text(GTK_ENTRY(project->iosetup.name[i]), buffer);
        }
        if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->iosetup.pulse_duration[i])) < 1)
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->iosetup.pulse_duration[i]), 1);
        if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->iosetup.pulse_duration[i])) > 253)
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->iosetup.pulse_duration[i]), 253);
    }

    if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->iosetup.visualization)) < 1)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->iosetup.visualization), 1);
    if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->iosetup.visualization)) > 253)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->iosetup.visualization), 253);

    save_io(project, device);
    gtk_widget_destroy(project->iosetup.dialog);
}

/**
* Fonction de sauvegarde et d'envoi des données au serveur
*/
void save_io(Project *project, Device *device)
{
    gushort i;
    char    *packet;
    char    buffer[50];
    char    pulse[10];

    for(i=0; i<8; i++)
    {
        sprintf(buffer, "relay%hudescription", i+1);
        packet = formatPackets(3,
                device->name,
                buffer,
                gtk_entry_get_text(GTK_ENTRY(project->iosetup.name[i])));

        pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);
        free(packet);
    }

    for(i=0; i<8; i++)
    {
        sprintf(buffer, "relay%huPulseDuration", i+1);
        sprintf(pulse, "%d", (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->iosetup.pulse_duration[i])));

        packet = formatPackets(3,
                 device->name,
                 buffer,
                 pulse);

        pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);
        free(packet);
    }

    //Visualization Rate
    strcpy(buffer, "visualizationRate");
    sprintf(pulse, "%d", (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->iosetup.visualization)));

    packet = formatPackets(3,
             device->name,
             buffer,
             pulse);

    pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);
    free(packet);

    g_cond_broadcast(project->server.notifier);
}
