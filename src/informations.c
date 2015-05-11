#include "includes.h"

/**
* Fonction de création de la fenêtre d'informations pour un device précis
*/
void create_informations(GtkButton *button, Project *project)
{
    GtkWidget   *dialog;
    GtkWidget   *content;
    GtkWidget   *table;
    GtkWidget   *label;

    Device      *device;

    gushort     i;
    guint       result;

    gchar       *labels[3] = {
                    "Name :",
                    "Version :",
                    "Date :"
                };

    device = (Device *)g_object_get_data(G_OBJECT(button), "device");

    dialog = gtk_dialog_new_with_buttons("Informations",
                                         GTK_WINDOW(project->home.window),
                                         GTK_DIALOG_MODAL,
                                         "Close",
                                         GTK_RESPONSE_CLOSE,
                                         NULL);
    gtk_widget_set_size_request(dialog, 300, 200);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    /* Table Layout */
    table = gtk_table_new(5, 2, TRUE);

    for(i=1; i<=3; i++)
    {
        label = gtk_label_new(labels[i-1]);
        gtk_table_attach(GTK_TABLE(table),
                         label,
                         0, 1, i, i+1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);
        if(i == 1)
            label = gtk_label_new(device->device_name);
        else if(i == 2)
            label = gtk_label_new(device->version);
        else if(i == 3)
            label = gtk_label_new(device->date);
        gtk_table_attach(GTK_TABLE(table),
                          label,
                          1, 2, i, i+1,
                          GTK_EXPAND | GTK_FILL,
                          GTK_EXPAND | GTK_FILL,
                          0, 0);
    }

    gtk_container_add(GTK_CONTAINER(content), table);
    gtk_widget_show_all(dialog);

    result = gtk_dialog_run(GTK_DIALOG(dialog));

    switch(result)
    {
        case GTK_RESPONSE_CLOSE :
            break;
    }

    gtk_widget_destroy(dialog);
}
