#include "includes.h"

/**
* Fonction de création de la fenêtre Configuration pour un device précis
*/
void create_configuration(GtkButton *button, Project *project)
{
    GtkWidget   *content;
    GtkWidget   *table;
    GtkWidget   *label;

    Device      *device;

    gushort     i;
    guint       result;

    gchar       *labels[4] = {
                    "Device Name :",
                    "IP Address :",
                    "Read Community String :",
                    "Write Community String :"
                };

    device = (Device *) g_object_get_data(G_OBJECT(button), "device");

    project->configuration.dialog = gtk_dialog_new_with_buttons("Configuration",
                                   GTK_WINDOW(project->home.window),
                                   GTK_DIALOG_MODAL,
                                   "Save",
                                   GTK_RESPONSE_ACCEPT,
                                   "Cancel",
                                   GTK_RESPONSE_CANCEL,
                                   NULL);
    gtk_widget_set_size_request(project->configuration.dialog, 320, 200);
    gtk_window_set_resizable(GTK_WINDOW(project->configuration.dialog), FALSE);

    content = gtk_dialog_get_content_area(GTK_DIALOG(project->configuration.dialog));

    /* Table Layout */
    table = gtk_table_new(5, 2, TRUE);

    for(i = 0; i < 4; i++)
    {
        label = gtk_label_new(labels[i]);
        gtk_table_attach(GTK_TABLE(table),
                         label,
                         0, 1, i, i+1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

        project->configuration.entry[i] = gtk_entry_new();
        if(i == 0)
        {
            gtk_entry_set_max_length(GTK_ENTRY(project->configuration.entry[i]), 20);
            gtk_entry_set_text(GTK_ENTRY(project->configuration.entry[i]), device->name);
        }
        else if(i == 1)
        {
            gtk_entry_set_max_length(GTK_ENTRY(project->configuration.entry[i]), 15);
            gtk_entry_set_text(GTK_ENTRY(project->configuration.entry[i]), device->ip);
        }
        else
        {
            gtk_entry_set_max_length(GTK_ENTRY(project->configuration.entry[i]), 13);
            gtk_entry_set_text(GTK_ENTRY(project->configuration.entry[i]), device->communities[i-2]);
        }
        g_object_set_data(G_OBJECT(project->configuration.entry[i]), "device", (gpointer) device);
        g_signal_connect(GTK_OBJECT(project->configuration.entry[i]),
                         "activate",
                         G_CALLBACK(submit_configuration_cb),
                         project);
        gtk_table_attach(GTK_TABLE(table),
                         project->configuration.entry[i],
                         1, 2, i, i+1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);
    }

    gtk_container_add(GTK_CONTAINER(content), table);
    gtk_widget_show_all(project->configuration.dialog);

    result = gtk_dialog_run(GTK_DIALOG(project->configuration.dialog));

    switch(result)
    {
        case GTK_RESPONSE_ACCEPT :
            g_object_set_data(G_OBJECT(project->configuration.dialog), "device", (gpointer) device);
            submit_configuration_cb(GTK_WIDGET(project->configuration.dialog), project);
            break;


        case GTK_RESPONSE_CANCEL :
            gtk_widget_destroy(project->configuration.dialog);
            break;
    }

}

/**
* Fonction de vérification et traitement des données avant l'envoi au serveur
*/
void submit_configuration_cb(GtkWidget *submit, Project *project)
{
    Device *device = (Device *)g_object_get_data(G_OBJECT(submit), "device");

    if(g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->configuration.entry[0])), "") != 0
       && g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->configuration.entry[1])), "") != 0
       && g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->configuration.entry[2])), "") != 0
       && g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->configuration.entry[3])), "") != 0)
    {
        if(regex_verification(gtk_entry_get_text(GTK_ENTRY(project->configuration.entry[1])), IP_ADDR))
        {
            save_configuration(project, device);
            gtk_widget_destroy(project->configuration.dialog);
        }
        else
            warning_dialog(project->configuration.dialog,
                           "\nWrong IP Address (format : x.x.x.x)");
    }
    else
        warning_dialog(project->configuration.dialog, "\nAt least one field is empty.");
}

/**
* Fonction de sauvegarde et d'envoi des données au serveur
*/
void save_configuration(Project *project, Device *device)
{
    char    *packet;

    packet = formatPackets(6,
                          "changeConfig",
                          device->name,
                          gtk_entry_get_text(GTK_ENTRY(project->configuration.entry[0])),
                          gtk_entry_get_text(GTK_ENTRY(project->configuration.entry[1])),
                          gtk_entry_get_text(GTK_ENTRY(project->configuration.entry[2])),
                          gtk_entry_get_text(GTK_ENTRY(project->configuration.entry[3])));

    pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);

    free(packet);

    g_cond_broadcast(project->server.notifier);
}

void refresh_configuration(GtkButton *button, Project *project)
{
    Device *device = (Device *) g_object_get_data(G_OBJECT(button), "device");

    char    *packet;

    packet = formatPackets(6,
                          "changeConfig",
                          device->name,
                          device->name,
                          device->ip,
                          device->communities[0],
                          device->communities[1]);

    pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);

    free(packet);

    g_cond_broadcast(project->server.notifier);
}
