#include "includes.h"

/**
* Fonction de création de la fenêtre SNMP Setup pour un device précis
*/
void create_snmp_setup(GtkButton *button, Project *project)
{
    GtkWidget   *content;
    GtkWidget   *table;
    GtkWidget   *label;

    Device      *device;

    gushort     i;
    guint       result;

    gchar       *labels[3] = {
                    "SNMP Traps :",
                    "IP Address :",
                    "Community String :"
                };

    device = (Device *)g_object_get_data(G_OBJECT(button), "device");

    project->snmpsetup.dialog = gtk_dialog_new_with_buttons("SNMP Setup",
                                GTK_WINDOW(project->home.window),
                                GTK_DIALOG_MODAL,
                                "Save",
                                GTK_RESPONSE_ACCEPT,
                                "Cancel",
                                GTK_RESPONSE_CANCEL,
                                NULL);
    gtk_widget_set_size_request(project->snmpsetup.dialog, 300, 150);
    gtk_window_set_resizable(GTK_WINDOW(project->snmpsetup.dialog), FALSE);

    content = gtk_dialog_get_content_area(GTK_DIALOG(project->snmpsetup.dialog));

    /* Table Layout */
    table = gtk_table_new(3, 2, TRUE);

    for(i=0; i<3; i++)
    {
        label = gtk_label_new(labels[i]);
        gtk_table_attach(GTK_TABLE(table),
                     label,
                     0, 1, i, i+1,
                     GTK_EXPAND | GTK_FILL,
                     GTK_EXPAND | GTK_FILL,
                     0, 0);

        if(i == 0)
        {
            project->snmpsetup.entry[i] = gtk_combo_box_new_text();
            gtk_table_attach(GTK_TABLE(table),
                             project->snmpsetup.entry[i],
                             1, 2, i, i+1,
                             GTK_EXPAND | GTK_FILL,
                             GTK_EXPAND | GTK_FILL,
                             0, 0);

            gtk_combo_box_append_text(GTK_COMBO_BOX(project->snmpsetup.entry[i]), "Disable");
            gtk_combo_box_append_text(GTK_COMBO_BOX(project->snmpsetup.entry[i]), "Enable");

            gtk_combo_box_set_active(GTK_COMBO_BOX(project->snmpsetup.entry[i]), device->trap_enabled);
        }
        else
        {
            project->snmpsetup.entry[i] = gtk_entry_new();
            g_object_set_data(G_OBJECT(project->snmpsetup.entry[i]),
                          "device",
                          (gpointer) device);
            g_signal_connect(GTK_OBJECT(project->snmpsetup.entry[i]),
                         "activate",
                         G_CALLBACK(submit_snmp_cb),
                         project);
            gtk_entry_set_max_length(GTK_ENTRY(project->snmpsetup.entry[i]), (i==1) ? 15 : 13);
            gtk_table_attach(GTK_TABLE(table),
                         project->snmpsetup.entry[i],
                         1, 2, i, i+1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

            if(i==1)
                gtk_entry_set_text(GTK_ENTRY(project->snmpsetup.entry[i]), device->trap_ip);
            else if(i==2)
                gtk_entry_set_text(GTK_ENTRY(project->snmpsetup.entry[i]), device->trap_community);
        }
    }


    gtk_container_add(GTK_CONTAINER(content), table);
    gtk_widget_show_all(project->snmpsetup.dialog);

    result = gtk_dialog_run(GTK_DIALOG(project->snmpsetup.dialog));

    switch(result)
    {
        case GTK_RESPONSE_ACCEPT :
            g_object_set_data(G_OBJECT(project->snmpsetup.dialog), "device", (gpointer) device);
            submit_snmp_cb(GTK_WIDGET(project->snmpsetup.dialog), project);
            break;

        case GTK_RESPONSE_CANCEL :
            gtk_widget_destroy(project->snmpsetup.dialog);
            break;
    }
}

/**
* Fonction de vérification et traitement des données avant l'envoi au serveur
*/
void submit_snmp_cb(GtkWidget *submit, Project *project)
{
    Device *device = (Device *)g_object_get_data(G_OBJECT(submit), "device");

    if(g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->snmpsetup.entry[1])), "") != 0
       && g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->snmpsetup.entry[2])), "") != 0)
    {
        if(regex_verification(gtk_entry_get_text(GTK_ENTRY(project->snmpsetup.entry[1])), IP_ADDR))
        {
            save_snmp(project, device);
            gtk_widget_destroy(project->snmpsetup.dialog);
        }
        else
            warning_dialog(project->snmpsetup.dialog,
                           "\nWrong IP Address (format : x.x.x.x)");
    }
    else
        warning_dialog(project->snmpsetup.dialog, "\nAt least one field is empty.");
}

/**
* Fonction de sauvegarde et d'envoi des données au serveur
*/
void save_snmp(Project *project, Device *device)
{
    gushort i;
    char    *packet;
    char    *commands[3] = {
                "trapEnabled",
                "trapReceiverIPAddress",
                "trapCommunity"
            };


    for(i=0; i<3; i++)
    {
        packet = formatPackets(3,
                 device->name,
                 commands[i],
                 (i == 0) ? ((gtk_combo_box_get_active(GTK_COMBO_BOX(project->snmpsetup.entry[i])) == 0) ? "off" : "on")
                          : gtk_entry_get_text(GTK_ENTRY(project->snmpsetup.entry[i])));

        pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);

        free(packet);
    }

    g_cond_broadcast(project->server.notifier);
}
