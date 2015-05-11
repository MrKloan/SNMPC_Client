#include "includes.h"

/**
* Fonction de création de la fenêtre de suppression d'un device
*/
void create_del_device(GtkWidget *window, Project *project)
{
    GtkWidget   *content;
    GtkWidget   *table;
    GtkWidget   *label;

    guint       result;
    Devices     *temp;

    project->deldevice.dialog = gtk_dialog_new_with_buttons("Delete Device",
                                         GTK_WINDOW(project->home.window),
                                         GTK_DIALOG_MODAL,
                                         "Delete",
                                         GTK_RESPONSE_ACCEPT,
                                         "Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         NULL);
    gtk_widget_set_size_request(project->deldevice.dialog, 300, 150);
    gtk_window_set_resizable(GTK_WINDOW(project->deldevice.dialog), FALSE);

    content = gtk_dialog_get_content_area(GTK_DIALOG(project->deldevice.dialog));

    /* Table Layout */
    table = gtk_table_new(3, 2, TRUE);

    label = gtk_label_new("Choose a device :");
    gtk_table_attach(GTK_TABLE(table),
                         label,
                         0, 1, 1, 2,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

    project->deldevice.combobox = gtk_combo_box_new_text();
    gtk_table_attach(GTK_TABLE(table),
                         project->deldevice.combobox,
                         1, 2, 1, 2,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

    /* Add devices to combobox */
    temp = project->devices;
    while(temp != NULL)
    {
        gtk_combo_box_append_text(GTK_COMBO_BOX(project->deldevice.combobox), temp->element.name);
        temp = temp->next;
    }

    /* First selected device */
    gtk_combo_box_set_active(GTK_COMBO_BOX(project->deldevice.combobox), 0);

    gtk_container_add(GTK_CONTAINER(content), table);
    gtk_widget_show_all(project->deldevice.dialog);

    result = gtk_dialog_run(GTK_DIALOG(project->deldevice.dialog));

    switch(result)
    {
        case GTK_RESPONSE_ACCEPT :
            submit_del_device_cb(GTK_WIDGET(project->deldevice.dialog), project);
            break;

        case GTK_RESPONSE_CANCEL :
            gtk_widget_destroy(project->deldevice.dialog);
            break;
    }

}

/**
* Fonction de vérification et traitement des données avant l'envoi au serveur
*/
void submit_del_device_cb(GtkWidget *submit, Project *project)
{
    save_del_device(project);
    gtk_widget_destroy(project->deldevice.dialog);
}

/**
* Fonction de sauvegarde et d'envoi des données au serveur
*/
void save_del_device(Project *project)
{
    char *packet;

    packet = formatPackets(4,
             "delDevice",
             gtk_combo_box_get_active_text(GTK_COMBO_BOX(project->deldevice.combobox)),
             project->login.login,
             project->login.password);

    pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);
    g_cond_broadcast(project->server.notifier);

    free(packet);

    gtk_widget_show_all(project->home.window);
}
