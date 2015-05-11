#include "includes.h"

/**
* Fonction de création de la fenêtre de login
*/
void create_login(Project *project)
{
    GtkWidget   *table;
    GtkWidget   *label;
    GtkWidget   *button;

    gushort     i;

    const gchar *labels[3] = {
        "IP Address :",
        "Username :",
        "Password :"
    };


    /* Window Config */
    project->login.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(project->login.window), TITLE);
    gtk_window_set_default_size(GTK_WINDOW(project->login.window), 400, 150);
    gtk_window_set_position(GTK_WINDOW(project->login.window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(project->login.window), FALSE);

    /* Table Layout */
    table = gtk_table_new(6, 2, TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(table), 400, 150);
    gtk_container_add(GTK_CONTAINER(project->login.window), GTK_WIDGET(table));

    /* Widgets Creation */
    for(i=0; i<3; i++)
    {
        label  = gtk_label_new(labels[i]);
        gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT); // Not Work
        gtk_table_attach_defaults(GTK_TABLE(table),
                                  label,
                                  0, 1, i+1, i+2);

        project->login.entry[i] = gtk_entry_new();
        gtk_entry_set_max_length(GTK_ENTRY(project->login.entry[i]),
                                 (i == 0) ? 21 : MAX_LENGTH);
        g_signal_connect(GTK_OBJECT(project->login.entry[i]),
                         "activate",
                         G_CALLBACK(submit_login_cb),
                         project);
        gtk_table_attach_defaults(GTK_TABLE(table),
                                  project->login.entry[i],
                                  1, 2, i+1, i+2);
    }

    /* Password Visibility */
    gtk_entry_set_visibility(GTK_ENTRY(project->login.entry[2]), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(project->login.entry[2]), '*');

    /* Submit Button */
    button = gtk_button_new_with_label("\tSend\t");
    g_signal_connect(GTK_OBJECT(button),
                     "clicked",
                     G_CALLBACK(submit_login_cb),
                     project);
    gtk_table_attach(GTK_TABLE(table),
                     button,
                     0, 2, 5, 6,
                     GTK_EXPAND,
                     GTK_EXPAND,
                     0, 0);

    /* Callbacks */
    g_signal_connect(GTK_OBJECT(project->login.window),
                         "destroy",
                         G_CALLBACK(destroy_login),
                         project);
    g_signal_connect(GTK_OBJECT(project->login.window),
                         "delete_event",
                         G_CALLBACK(delete_login),
                         project);
}

/**
* Fonction de vérification et traitement des données avant l'envoi au serveur
*/
void submit_login_cb(GtkButton *submit, Project *project)
{
    char            **args;
    unsigned short  size, i;

    /* Not Empty Verification & IP RegEx */
    if(g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->login.entry[0])), "") != 0 &&
       g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->login.entry[1])), "") != 0 &&
       g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->login.entry[2])), "") != 0)
    {
        args = explode((char *)gtk_entry_get_text(GTK_ENTRY(project->login.entry[0])), ":", &size);

        if(regex_verification(gtk_entry_get_text(GTK_ENTRY(project->login.entry[0])), IPPORT_ADDR) && size == 2)
            send_login(project, args);
        else
            warning_dialog(project->login.window, "\nWrong IP Address ! (Format : x.x.x.x:port)");

        for(i = 0; i < size; i++)
            free(args[i]);
        free(args);
    }
    else
        warning_dialog(project->login.window, "\nAt least one field is empty.");
}

/**
* Fonction de première connexion avec le serveur et envoi des données entrées
* Passage à l'écran principal si le serveur autorise l'accès
*/
void send_login(Project *project, char **args)
{
    serverInit(&project->server);

    if(initSocket(&project->server.socket))
    {
        if(socketConnect(&project->server, args[0], (unsigned short)atoi(args[1])))
        {
            char *buffer = NULL;
            buffer = malloc(SOCKET_BUFFER * sizeof(char));

            SHA1Reset(&project->sha);

            sprintf(buffer, "%s%s%s", SALT, gtk_entry_get_text(GTK_ENTRY(project->login.entry[1])), PEPPER);
            strcpy(project->login.name, gtk_entry_get_text(GTK_ENTRY(project->login.entry[1])));

            SHA1Input(&project->sha,
                (const unsigned char *)buffer,
                strlen(buffer)
            );

            if(SHA1Result(&project->sha))
                strcpy(project->login.login, project->sha.sha_message);

            SHA1Reset(&project->sha);

            sprintf(buffer, "%s%s%s", SALT, gtk_entry_get_text(GTK_ENTRY(project->login.entry[2])), PEPPER);

            SHA1Input(&project->sha,
                (const unsigned char *)buffer,
                strlen(buffer)
            );

            if(SHA1Result(&project->sha))
                strcpy(project->login.password, project->sha.sha_message);

            char *packet =  formatPackets(3,
                            "loginServer",
                            project->login.login,
                            project->login.password);

            cipherPacket(project, &packet);
            strcpy(project->server.buffer, packet);

            //snmpc_sleep(1);
            if(socketSend(&project->server))
            {
                if(socketReceive(&project->server))
                {
                    char **parts;
                    unsigned short i, sizeparts;

                    strcpy(packet, project->server.buffer);

                    if((parts = explode(packet, PACKET_END, &sizeparts)) != NULL)
                    {
                        for(i = 0; i < sizeparts; i++)
                        {
                            char **packets;
                            unsigned short j, size;

                            uncipherPacket(project, &parts[i]);
                            if((packets = explode(parts[i], PACKET_END, &size)) != NULL)
                            {
                                handlePacket(project, packets[0]);

                                for(j=0; j < size; j++)
                                    free(packets[j]);
                                free(packets);
                            }
                            free(parts[i]);
                        }
                        free(parts);
                    }
                }
                else
                    serverFree(&project->server);
            }
            else
                serverFree(&project->server);

            free(packet);
            free(buffer);
        }
        else
        {
            socketFree(&project->server);
            warning_dialog(project->login.window, "\nUnable to connect to remote host.");
        }
    }
    else
        warning_dialog(project->login.window, "\nAn Error occured while initializing the socket.");
}

void delete_login(GtkWidget *widget, GdkEvent  *event, Project *project)
{
    if(!project->home.flag)
        gtk_main_quit();
}

void destroy_login(GtkWidget *widget, Project *project)
{
    if(!project->home.flag)
        gtk_main_quit();
}
