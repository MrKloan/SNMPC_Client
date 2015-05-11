#include "includes.h"

/**
* Fonction de création de la fenêtre de modification du profil de l'utilisateur courant
*/
void create_profile(GtkButton *button, Project *project)
{
    GtkWidget   *content;
    GtkWidget   *table;
    GtkWidget   *label;
    GtkWidget   *delaccount;

    gushort     i;
    guint       result;
    gchar       *labels[5] = {
                    "Username :",
                    "Old Password :",
                    "New Password :",
                    "Confirm :",
                    "Delete Account :"
                };

    project->profile.dialog = gtk_dialog_new_with_buttons("Profile",
                              GTK_WINDOW(project->home.window),
                              GTK_DIALOG_MODAL,
                              "Save",
                              GTK_RESPONSE_ACCEPT,
                              "Cancel",
                              GTK_RESPONSE_CANCEL,
                              NULL);
    gtk_widget_set_size_request(project->profile.dialog, 300, 250);
    gtk_window_set_resizable(GTK_WINDOW(project->profile.dialog), FALSE);

    content = gtk_dialog_get_content_area(GTK_DIALOG(project->profile.dialog));

    /* Table Layout */
    table = gtk_table_new(8, 2, TRUE);

    for(i = 1; i < 5; i++)
    {
        label = gtk_label_new(labels[i-1]);
        gtk_table_attach(GTK_TABLE(table),
                         label,
                         0, 1, i, i+1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);
        if(i==1)
        {
            label = gtk_label_new(project->login.name);
            gtk_table_attach(GTK_TABLE(table),
                             label,
                             1, 2, i, i+1,
                             GTK_EXPAND | GTK_FILL,
                             GTK_EXPAND | GTK_FILL,
                             0, 0);
        }
        else
        {
            project->profile.entry[i-2] = gtk_entry_new();
            g_signal_connect(GTK_OBJECT(project->profile.entry[i-2]),
                             "activate",
                             G_CALLBACK(submit_profile_cb),
                             project);
            gtk_table_attach(GTK_TABLE(table),
                             project->profile.entry[i-2],
                             1, 2, i, i+1,
                             GTK_EXPAND | GTK_FILL,
                             GTK_EXPAND | GTK_FILL,
                             0, 0);

                /* Password Visibility */
            gtk_entry_set_visibility(GTK_ENTRY(project->profile.entry[i-2]), FALSE);
            gtk_entry_set_invisible_char(GTK_ENTRY(project->profile.entry[i-2]), '*');
        }
    }

    label = gtk_label_new(labels[4]);
    gtk_table_attach(GTK_TABLE(table),
                 label,
                 0, 1, 6, 7,
                 GTK_EXPAND | GTK_FILL,
                 GTK_EXPAND | GTK_FILL,
                 0, 0);

    delaccount = gtk_button_new_with_label("Delete");
    g_signal_connect(GTK_OBJECT(delaccount),
                     "clicked",
                     G_CALLBACK(delete_account_cb),
                     project);
    gtk_table_attach(GTK_TABLE(table),
                     delaccount,
                     1, 2, 6, 7,
                     GTK_EXPAND | GTK_FILL,
                     GTK_EXPAND | GTK_FILL,
                     0, 0);

    gtk_container_add(GTK_CONTAINER(content), table);
    gtk_widget_show_all(project->profile.dialog);

    result = gtk_dialog_run(GTK_DIALOG(project->profile.dialog));

    switch(result)
    {
        case GTK_RESPONSE_ACCEPT :
            submit_profile_cb(GTK_WIDGET(project->profile.dialog), project);
            break;

        case GTK_RESPONSE_CANCEL :
            gtk_widget_destroy(project->profile.dialog);
            break;
    }
}

/**
* Fonction de vérification et traitement des données avant l'envoi au serveur
*/
void submit_profile_cb(GtkWidget *submit, Project *project)
{
    if(g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->profile.entry[0])), "") != 0
       && g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->profile.entry[1])), "") != 0
       && g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->profile.entry[2])), "") != 0)
    {
        if(g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->profile.entry[1])),
           gtk_entry_get_text(GTK_ENTRY(project->profile.entry[2]))) == 0)
        {
            save_profile(project);
            gtk_widget_destroy(project->profile.dialog);
        }
        else
            warning_dialog(project->profile.dialog,"\nThe new password and confirmation do not match.");
    }
    else
        warning_dialog(project->profile.dialog, "\nAt least one field is empty.");
}

/**
* Fonction de sauvegarde et d'envoi des données au serveur
*/
void save_profile(Project *project)
{
    char *buffer = NULL, *packet = NULL;
    char tmp[2][SOCKET_BUFFER];

    buffer = malloc(SOCKET_BUFFER * sizeof(char));

    SHA1Reset(&project->sha);

    sprintf(buffer, "%s%s%s", SALT, gtk_entry_get_text(GTK_ENTRY(project->profile.entry[0])), PEPPER);

    SHA1Input(&project->sha,
        (const unsigned char *)buffer,
        strlen(buffer)
    );

    if(SHA1Result(&project->sha))
        strcpy(tmp[0], project->sha.sha_message);

    SHA1Reset(&project->sha);

    sprintf(buffer, "%s%s%s", SALT, gtk_entry_get_text(GTK_ENTRY(project->profile.entry[2])), PEPPER);

    SHA1Input(&project->sha,
        (const unsigned char *)buffer,
        strlen(buffer)
    );

    if(SHA1Result(&project->sha))
        strcpy(tmp[1], project->sha.sha_message);


    packet = formatPackets(4,
             "changeProfile",
             project->login.login,
             tmp[0],
             tmp[1]);

    pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);
    g_cond_broadcast(project->server.notifier);

    free(packet);
    free(buffer);
}

/**
* Fonction de suppression du compte utilisateur courant
*/
void delete_account_cb(GtkButton *button, Project *project)
{
    GtkWidget   *dialog;
    char        *packet;

    guint       result;


    dialog = gtk_message_dialog_new(GTK_WINDOW(project->profile.dialog),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_WARNING,
                                    GTK_BUTTONS_YES_NO,
                                    "\nAre you sure to delete your account ?\n"
                                    "Yes : Close your session and delete your account\n"
                                    "No : Cancel");

    gtk_window_set_title(GTK_WINDOW(dialog), "Delete Account");
    result = gtk_dialog_run(GTK_DIALOG(dialog));

    switch(result)
    {
        case GTK_RESPONSE_YES :
        {
            gtk_widget_destroy(project->profile.dialog);
            packet = formatPackets(3,
                                   "delAccount",
                                   project->login.login,
                                   project->login.password);

            pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);
            g_cond_broadcast(project->server.notifier);

            free(packet);
            break;
        }

        case GTK_RESPONSE_NO :
            break;
    }

    gtk_widget_destroy(dialog);
}
