#include "includes.h"

void start_visualization(Project *project)
{
    project->visualization.led = NULL;
    project->visualization.table = NULL;
    project->visualization.frame = NULL;
    project->visualization.label = NULL;

    pushQueue(&project->server.sender_queue, "visualization;start|", project->server.sender_lock);
    g_cond_broadcast(project->server.notifier);
    project->visualization.visualizing = TRUE;
}

void stop_visualization(Project *project)
{
    int     i;
    Devices *temp = project->devices;

    project->visualization.visualizing = FALSE;
    pushQueue(&project->server.sender_queue, "visualization;stop|", project->server.sender_lock);
    g_cond_broadcast(project->server.notifier);

    if(project->visualization.led != NULL)
    {
        for(i=0 ; temp != NULL ; i++)
        {
            free(project->visualization.led[i]);
            free(project->visualization.label[i]);
            temp = temp->next;
        }
        free(project->visualization.led);
        free(project->visualization.table);
        free(project->visualization.frame);
        free(project->visualization.label);

        project->visualization.led = NULL;
        project->visualization.table = NULL;
        project->visualization.frame = NULL;
        project->visualization.label = NULL;
    }
}

/**
* Fonction de création de la fenêtre de visualization générale de l'état des différents relais pour tous les devices
*/
void create_visualization(Project *project)
{
    GtkWidget   *scrollbar;
    GtkWidget   *vbox;

    gushort     i, j;
    gchar       txt[20];
    Devices     *temp;

    /* Window Config */
    project->visualization.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(project->visualization.window), "Visualization");
    gtk_window_set_position(GTK_WINDOW(project->visualization.window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(project->visualization.window), FALSE);

    /* ScrollBar Creation & Layout Integration */
    scrollbar = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(GTK_WIDGET(scrollbar), 700, 85*5); // Taille max
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar),
        GTK_POLICY_AUTOMATIC,
        GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(project->visualization.window), scrollbar);

    vbox = gtk_vbox_new(FALSE, 10);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW (scrollbar), vbox);

    //Parcours des devices
    temp = project->devices;

    for(i=0 ; temp != NULL ; i++)
    {
        project->visualization.led = realloc(project->visualization.led, (i+1)*sizeof(GtkWidget**));
        project->visualization.led[i] = malloc(9*sizeof(GtkWidget*));

        project->visualization.label = realloc(project->visualization.label, (i+1)*sizeof(GtkWidget**));
        project->visualization.label[i] = malloc(9*sizeof(GtkWidget*));

        project->visualization.frame = realloc(project->visualization.frame, (i+1)*sizeof(GtkWidget*));
        project->visualization.frame[i] = gtk_frame_new(temp->element.name);

        project->visualization.table = realloc(project->visualization.table, (i+1)*sizeof(GtkWidget*));

        shadow_type(GTK_FRAME(project->visualization.frame[i]), GTK_SHADOW_IN);
        gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(project->visualization.frame[i]), FALSE, FALSE, 0);

        //Si le device n'est pas déconnecté, on affiche les LED et leurs labels
        if(!temp->element.disconnected)
        {
            project->visualization.table[i] = gtk_table_new(3, 9, TRUE);
            gtk_container_add(GTK_CONTAINER(project->visualization.frame[i]), GTK_WIDGET(project->visualization.table[i]));

            for(j=0; j<9; j++)
            {
                project->visualization.led[i][j] = gtk_led_new((j == 0) ? temp->element.digital_input : temp->element.relays_state[j-1]);
                gtk_table_attach(GTK_TABLE(project->visualization.table[i]),
                             project->visualization.led[i][j],
                             j, j+1, 1, 2,
                             GTK_EXPAND | GTK_FILL,
                             GTK_EXPAND | GTK_FILL,
                             0, 0);

                if(j==0)
                    sprintf(txt, "Digital Input");
                else
                    strcpy(txt, temp->element.relays_name[j-1]);

                project->visualization.label[i][j] = gtk_label_new(txt);
                gtk_table_attach(GTK_TABLE(project->visualization.table[i]),
                             project->visualization.label[i][j],
                             j, j+1, 2, 3,
                             GTK_EXPAND | GTK_FILL,
                             GTK_EXPAND | GTK_FILL,
                             0, 0);
            }
        }
        //Si le device est déconnecté, message d'erreur
        else
        {
            project->visualization.table[i] = gtk_table_new(1, 1, TRUE);
            gtk_container_add(GTK_CONTAINER(project->visualization.frame[i]), GTK_WIDGET(project->visualization.table[i]));

            strcpy(txt, "Device is unreachable.");
            project->visualization.label[i][0] = gtk_label_new(txt);
            gtk_table_attach(GTK_TABLE(project->visualization.table[i]),
                         project->visualization.label[i][0],
                         0, 1, 0, 1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);
        }

        temp = temp->next;
    }
    gtk_widget_show_all(project->visualization.window);

    g_object_set_data(G_OBJECT(project->visualization.window), "project", (gpointer)project);

    /* Callbacks */
    g_signal_connect(GTK_OBJECT(project->visualization.window),
                     "delete_event",
                     G_CALLBACK(destroy_visualization_cb),
                     NULL);
}

void destroy_visualization_cb(GtkWidget *widget, gpointer data)
{
    Project *project = (Project *) g_object_get_data(G_OBJECT(widget), "project");

    stop_visualization(project);
    gtk_widget_destroy(project->visualization.window);
    project->visualization.window = NULL;
}
