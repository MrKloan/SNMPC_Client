#include "includes.h"

/**
* Fonction de création de la fenêtre Network Setup pour un device précis
*/
void create_network_setup(GtkButton *button, Project *project)
{
    GtkWidget   *content;
    GtkWidget   *table;
    GtkWidget   *label;
    GtkWidget   *widget;

    Device      *device;

    gushort     i;
    guint       result;

    gchar       *labels[7] = {
                    "Static/DHCP :",
                    "IP Address :",
                    "Subnet Mask :",
                    "Gateway :",
                    "MAC Address :",
                    "Change Configuration :",
                    "Restart Device :"
                };

    device = (Device *) g_object_get_data(G_OBJECT(button), "device");

    project->networksetup.dialog = gtk_dialog_new_with_buttons("Network Setup",
                                   GTK_WINDOW(project->home.window),
                                   GTK_DIALOG_MODAL,
                                   "Save",
                                   GTK_RESPONSE_ACCEPT,
                                   "Cancel",
                                   GTK_RESPONSE_CANCEL,
                                   NULL);
    gtk_widget_set_size_request(project->networksetup.dialog, 300, 270);
    gtk_window_set_resizable(GTK_WINDOW(project->networksetup.dialog), FALSE);

    content = gtk_dialog_get_content_area(GTK_DIALOG(project->networksetup.dialog));

    /* Table Layout */
    table = gtk_table_new(9, 2, TRUE);

    for(i=0; i<5; i++)
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
            project->networksetup.entry[i] = gtk_combo_box_new_text();
            gtk_table_attach(GTK_TABLE(table),
                             project->networksetup.entry[i],
                             1, 2, i, i+1,
                             GTK_EXPAND | GTK_FILL,
                             GTK_EXPAND | GTK_FILL,
                             0, 0);

            gtk_combo_box_append_text(GTK_COMBO_BOX(project->networksetup.entry[i]), "Static");
            gtk_combo_box_append_text(GTK_COMBO_BOX(project->networksetup.entry[i]), "DHCP");

            gtk_combo_box_set_active(GTK_COMBO_BOX(project->networksetup.entry[i]), device->dhcp_config);
        }
        else
        {
            if(i != 4)
            {
                project->networksetup.entry[i] = gtk_entry_new();
                gtk_entry_set_max_length(GTK_ENTRY(project->networksetup.entry[i]), 15);
                g_object_set_data(G_OBJECT(project->networksetup.entry[i]), "device", (gpointer) device);
                g_signal_connect(GTK_OBJECT(project->networksetup.entry[i]),
                                 "activate",
                                 G_CALLBACK(submit_network_cb),
                                 project);
            }
            else
                project->networksetup.entry[i] = gtk_label_new(device->mac_addr);
            gtk_table_attach(GTK_TABLE(table),
                             project->networksetup.entry[i],
                             1, 2, i, i+1,
                             GTK_EXPAND | GTK_FILL,
                             GTK_EXPAND | GTK_FILL,
                             0, 0);
            if(i==1)
                gtk_entry_set_text(GTK_ENTRY(project->networksetup.entry[i]), device->ip);
            else if(i==2)
                gtk_entry_set_text(GTK_ENTRY(project->networksetup.entry[i]), device->subnet_mask);
            else if(i==3)
                gtk_entry_set_text(GTK_ENTRY(project->networksetup.entry[i]), device->gateway);
        }
    }
    label = gtk_label_new(labels[5]);
    gtk_table_attach(GTK_TABLE(table),
                     label,
                     0, 1, 6, 7,
                     GTK_EXPAND | GTK_FILL,
                     GTK_EXPAND | GTK_FILL,
                     0, 0);

    widget = gtk_button_new_with_label("Configuration");
    g_object_set_data(G_OBJECT(widget), "device", (gpointer) device);
    g_signal_connect(GTK_OBJECT(widget),
                     "clicked",
                     G_CALLBACK(create_configuration),
                     project);
    gtk_table_attach(GTK_TABLE(table),
                     widget,
                     1, 2, 6, 7,
                     GTK_EXPAND | GTK_FILL,
                     GTK_EXPAND | GTK_FILL,
                     0, 0);

    label = gtk_label_new(labels[6]);
    gtk_table_attach(GTK_TABLE(table),
                     label,
                     0, 1, 7, 8,
                     GTK_EXPAND | GTK_FILL,
                     GTK_EXPAND | GTK_FILL,
                     0, 0);

    widget = gtk_button_new_with_label("Restart");
    g_object_set_data(G_OBJECT(widget), "device", (gpointer) device);
    g_signal_connect(GTK_OBJECT(widget),
                     "clicked",
                     G_CALLBACK(restart_device_cb),
                     project);
    gtk_table_attach(GTK_TABLE(table),
                     widget,
                     1, 2, 7, 8,
                     GTK_EXPAND | GTK_FILL,
                     GTK_EXPAND | GTK_FILL,
                     0, 0);

    gtk_container_add(GTK_CONTAINER(content), table);
    gtk_widget_show_all(project->networksetup.dialog);

    result = gtk_dialog_run(GTK_DIALOG(project->networksetup.dialog));

    switch(result)
    {
        case GTK_RESPONSE_ACCEPT :
            g_object_set_data(G_OBJECT(project->networksetup.dialog), "device", (gpointer) device);
            submit_network_cb(GTK_WIDGET(project->networksetup.dialog), project);
            break;


        case GTK_RESPONSE_CANCEL :
            gtk_widget_destroy(project->networksetup.dialog);
            break;
    }

}

/**
* Fonction de vérification et traitement des données avant l'envoi au serveur
*/
void submit_network_cb(GtkWidget *submit, Project *project)
{
    Device *device = (Device *)g_object_get_data(G_OBJECT(submit), "device");

    if(g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->networksetup.entry[1])), "") != 0
       && g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->networksetup.entry[2])), "") != 0
       && g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->networksetup.entry[3])), "") != 0
       && g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->networksetup.entry[4])), "") != 0)
    {
        if(regex_verification(gtk_entry_get_text(GTK_ENTRY(project->networksetup.entry[1])), IP_ADDR)
           && regex_verification(gtk_entry_get_text(GTK_ENTRY(project->networksetup.entry[2])), IP_ADDR)
           && regex_verification(gtk_entry_get_text(GTK_ENTRY(project->networksetup.entry[3])), IP_ADDR))
           /*&& regex_verification(gtk_entry_get_text(GTK_ENTRY(project->networksetup.entry[4])), MAC_ADDR))*/
        {
            save_network(project, device);
            gtk_widget_destroy(project->networksetup.dialog);
        }
        else
            warning_dialog(project->networksetup.dialog,
                           "\nWrong IP Address (format : x.x.x.x) "
                           "or MAC address (format : xx:xx:xx:xx:xx:xx or xx-xx-xx-xx-xx-xx)");
    }
    else
        warning_dialog(project->networksetup.dialog, "\nAt least one field is empty.");
}

/**
* Fonction de sauvegarde et d'envoi des données au serveur
*/
void save_network(Project *project, Device *device)
{
    gushort i;
    char    *packet;
    char    *commands[4] = {
                "dhcpConfig",
                "deviceIPAddress",
                "subnetMask",
                "gateway"/*,
                "deviceMACAddress"*/
            };


    for(i=0; i<4; i++)
    {
        packet = formatPackets(3,
                 device->name,
                 commands[i],
                 (i == 0) ? ((gtk_combo_box_get_active(GTK_COMBO_BOX(project->networksetup.entry[i])) == 0) ? "off" : "on")
                         : gtk_entry_get_text(GTK_ENTRY(project->networksetup.entry[i])));

        pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);

        free(packet);
    }

    g_cond_broadcast(project->server.notifier);
}

/**
* Fonction de redémarrage d'un device
*/
void restart_device_cb(GtkButton *button, Project *project)
{
    GtkWidget   *dialog;
    Device      *device;
    char        *packet;

    guint       result;

    device = (Device *)g_object_get_data(G_OBJECT(button), "device");

    dialog = gtk_message_dialog_new(GTK_WINDOW(project->networksetup.dialog),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_YES_NO,
                                    "\nAre you sure to restart the device ?");

    gtk_window_set_title(GTK_WINDOW(dialog), "Restart the device");
    result = gtk_dialog_run(GTK_DIALOG(dialog));

    switch(result)
    {
        case GTK_RESPONSE_YES :
        {
            packet = formatPackets(3, device->name, "restartDevice", "on");

            pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);
            g_cond_broadcast(project->server.notifier);

            free(packet);

            gtk_widget_destroy(dialog);
            gtk_widget_destroy(project->networksetup.dialog);
            break;
        }

        case GTK_RESPONSE_NO :
            gtk_widget_destroy(dialog);
            break;
    }
}
