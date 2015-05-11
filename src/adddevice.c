#include "includes.h"

/**
* Fonction de création de la fenêtre d'ajout d'un device
*/
void create_add_device(GtkWidget *window, Project *project)
{
    GtkWidget   *content;
    GtkWidget   *table;
    GtkWidget   *label;

    guint       result;
    gushort     i;

    const gchar *labels[4] = {
        "Name :",
        "IP Address :",
        "Read Community String :",
        "Write Community String :"
    };


    project->adddevice.dialog = gtk_dialog_new_with_buttons("New Device",
                                GTK_WINDOW(project->home.window),
                                GTK_DIALOG_MODAL,
                                "Add",
                                GTK_RESPONSE_ACCEPT,
                                "Cancel",
                                GTK_RESPONSE_CANCEL,
                                NULL);
    gtk_widget_set_size_request(project->adddevice.dialog, 350, 200);
    gtk_window_set_resizable(GTK_WINDOW(project->adddevice.dialog), FALSE);

    content = gtk_dialog_get_content_area(GTK_DIALOG(project->adddevice.dialog));

    /* Table Layout */
    table = gtk_table_new(6, 2, TRUE);

    /* Labels & Entries Creation */
    for(i=1; i<5; i++)
    {
        label = gtk_label_new(labels[i-1]);
        gtk_table_attach(GTK_TABLE(table),
                         label,
                         0, 1, i, i+1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

        project->adddevice.entry[i-1] = gtk_entry_new();
        gtk_entry_set_max_length(GTK_ENTRY(project->adddevice.entry[i-1]),
                                 (i == 2) ? 15 : MAX_LENGTH);
        g_signal_connect(GTK_OBJECT(project->adddevice.entry[i-1]),
                         "activate",
                         G_CALLBACK(submit_add_device_cb),
                         project);
        gtk_table_attach(GTK_TABLE(table),
                         project->adddevice.entry[i-1],
                         1, 2, i, i+1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

    }
    gtk_container_add(GTK_CONTAINER(content), table);
    gtk_widget_show_all(project->adddevice.dialog);

    result = gtk_dialog_run(GTK_DIALOG(project->adddevice.dialog));

    switch(result)
    {
        case GTK_RESPONSE_ACCEPT :
            submit_add_device_cb(GTK_WIDGET(project->adddevice.dialog), project);
            break;

        case GTK_RESPONSE_CANCEL :
            gtk_widget_destroy(project->adddevice.dialog);
            break;
    }
}

/**
* Fonction de vérification et traitement des données avant l'envoi au serveur
*/
void submit_add_device_cb(GtkWidget *submit, Project *project)
{
    if(g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->adddevice.entry[0])), "") != 0
       && g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->adddevice.entry[1])), "") != 0
       && g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->adddevice.entry[2])), "") != 0
       && g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->adddevice.entry[3])), "") != 0)
    {
        if(regex_verification(gtk_entry_get_text(GTK_ENTRY(project->adddevice.entry[1])), IP_ADDR))
        {
            if(authorize_ip(project->devices, gtk_entry_get_text(GTK_ENTRY(project->adddevice.entry[1]))))
            {
                save_add_device(project);
                gtk_widget_destroy(project->adddevice.dialog);
            }
            else
                warning_dialog(project->adddevice.dialog, "\nThe IP address is already used !");
        }
        else
            warning_dialog(project->adddevice.dialog, "\nWrong IP Address !");
    }
    else
        warning_dialog(project->adddevice.dialog, "\nAt least one field is empty.");
}

/**
* Fonction de sauvegarde et d'envoi des données au serveur
*/
void save_add_device(Project *project)
{
    char    *packet;

    packet = formatPackets(7,
             "addDevice",
             gtk_entry_get_text(GTK_ENTRY(project->adddevice.entry[0])),
             gtk_entry_get_text(GTK_ENTRY(project->adddevice.entry[1])),
             gtk_entry_get_text(GTK_ENTRY(project->adddevice.entry[2])),
             gtk_entry_get_text(GTK_ENTRY(project->adddevice.entry[3])),
             project->login.login,
             project->login.password);

    pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);
    g_cond_broadcast(project->server.notifier);

    free(packet);
}
