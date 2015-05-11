#include "includes.h"

/**
* Fonction de création de la fenêtre de planification pour un relais précis
*/
void create_schedule(Project *project, Device *device, gushort relay)
{
    GtkWidget           *content;
    GtkWidget           *button;

    gushort             i;
    gboolean            jump = FALSE;

    const gchar         *labelsbtn[4] = {
                            "Add Task",
                            "Modify",
                            "Delete Task",
                            "Close"
                        };

    project->schedule.flag = 1;
    project->schedule.dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(project->schedule.dialog), "Schedule");
    gtk_widget_set_size_request(project->schedule.dialog, 500, 300);
    gtk_window_set_resizable(GTK_WINDOW(project->schedule.dialog), FALSE);

    content = gtk_dialog_get_content_area(GTK_DIALOG(project->schedule.dialog));


    /* Table Layout */
    project->schedule.table = gtk_table_new(6, 4, TRUE);

    create_listview(project, device, relay);

    /* Buttons */
    for(i=0; i<4; i++)
    {
        button = gtk_button_new_with_label(labelsbtn[i]);
        g_object_set_data(G_OBJECT(button), "device", (gpointer) device);
        g_object_set_data(G_OBJECT(button), "relay", GUINT_TO_POINTER(relay));
        gtk_table_attach(GTK_TABLE(project->schedule.table),
                         button,
                         i, i+1, 5, 6,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);
        if(i == 0)
            g_signal_connect(GTK_OBJECT(button),
                             "clicked",
                             G_CALLBACK(add_task_cb),
                             project);
        else if(i == 1)
            g_signal_connect(GTK_OBJECT(button),
                             "clicked",
                             G_CALLBACK(modify_selected_task_cb),
                             project);
        else if(i == 2)
            g_signal_connect(GTK_OBJECT(button),
                             "clicked",
                             G_CALLBACK(delete_selected_task_cb),
                             project);
        else if(i == 3)
        {
            g_object_set_data(G_OBJECT(button), "jump", (gpointer) &jump);
            g_signal_connect(GTK_OBJECT(button),
                             "clicked",
                             G_CALLBACK(close_schedule_cb),
                             project->schedule.dialog);
        }
    }

    gtk_container_add(GTK_CONTAINER(content), project->schedule.table);
    gtk_widget_show_all(project->schedule.dialog);

    gtk_dialog_run(GTK_DIALOG(project->schedule.dialog));

    if(!jump)
        gtk_widget_destroy(project->schedule.dialog);

    project->schedule.flag = 0;
}

/**
* Fonction de création de la liste des tâches existantes présente dan la fenêtre de planification
*/
void create_listview(Project *project, Device *device, gushort relay)
{
    GtkWidget           *scrollbar;
    GtkTreeViewColumn   *column;
    GtkCellRenderer     *cell_renderer;

    /* Model Creation */
    project->schedule.liststore = gtk_tree_store_new(7,
                                                     G_TYPE_STRING,
                                                     G_TYPE_STRING,
                                                     G_TYPE_STRING,
                                                     G_TYPE_STRING,
                                                     G_TYPE_STRING,
                                                     G_TYPE_STRING,
                                                     G_TYPE_STRING
                                                     );

    //MAJ Informations avec la liststore
    liststore_maj(project, device, relay);

    /* TreeView Creation */
    project->schedule.listview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(project->schedule.liststore));

    /* Columns Creation */
    cell_renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("State", cell_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(project->schedule.listview), column);

    cell_renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Name", cell_renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(project->schedule.listview), column);

    cell_renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Date", cell_renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(project->schedule.listview), column);

    cell_renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Repeat", cell_renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(project->schedule.listview), column);

    cell_renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Type", cell_renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(project->schedule.listview), column);

    cell_renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Start", cell_renderer, "text", 5, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(project->schedule.listview), column);

    cell_renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("End", cell_renderer, "text", 6, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(project->schedule.listview), column);

    /* ScrollBar Creation & Layout Integration */
    scrollbar = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrollbar), project->schedule.listview);
    gtk_table_attach(GTK_TABLE(project->schedule.table),
                     scrollbar,
                     0, 4, 0, 5,
                     GTK_EXPAND | GTK_FILL,
                     GTK_EXPAND | GTK_FILL,
                     0, 0);
}

/**
* Fonction de chargement des tâches dans un arbre pour être ensuite exploité et affiché par listview
*/
void liststore_maj(Project *project, Device *device, gushort relay)
{
    GtkTreeIter iter;
    Tasks *tasks = device->tasks[relay-1];

    //On efface le contenu de l'arbre précédement stocké
	gtk_tree_store_clear(project->schedule.liststore);

    while(tasks != NULL)
    {
        char buffer[50];
        //Creation de la nouvelle ligne
        gtk_tree_store_append(project->schedule.liststore, &iter, NULL);

        // on ajoute l'id et son texte à l'arbre
        gtk_tree_store_set(project->schedule.liststore, &iter, 0, (tasks->element.enabled_val) ? "Enabled" : "Disabled", -1);
        gtk_tree_store_set(project->schedule.liststore, &iter, 1, tasks->element.name_val, -1);
        gtk_tree_store_set(project->schedule.liststore, &iter, 2, tasks->element.date_val, -1);
        gtk_tree_store_set(project->schedule.liststore, &iter, 3, (tasks->element.repeat_val == 0) ? "Once" : "Repeat", -1);
        gtk_tree_store_set(project->schedule.liststore, &iter, 4, (tasks->element.type_val == 0) ? "On/OFF" : "Pulse", -1);

        sprintf(buffer, "%huh%hu", tasks->element.hours_val[0], tasks->element.minutes_val[0]);
        gtk_tree_store_set(project->schedule.liststore, &iter, 5, buffer, -1);

        if(tasks->element.type_val == 0)
            sprintf(buffer, "%huh%hu", tasks->element.hours_val[1], tasks->element.minutes_val[1]);
        else
            strcpy(buffer, "Pulse timing");
        gtk_tree_store_set(project->schedule.liststore, &iter, 6, buffer, -1);
        tasks = tasks->next;
    }
}

/**
* Fonction d'ajout d'une tâche dans la planification
*/
void add_task_cb(GtkButton *button, Project *project)
{
    GtkWidget   *content;
    GtkWidget   *frame;
    GtkWidget   *label;

    const gchar *labels[6] = {
                  "Name :",
                  "Task :",
                  "Repeat :",
                  "Type :",
                  "Beginning :",
                  "End :"
                };

    gushort     i;
    guint       result;
    gchar       buffer[SOCKET_BUFFER];
    gushort     relay = (gushort) GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "relay"));
    Device      *device = (Device *)g_object_get_data(G_OBJECT(button), "device");

    project->newtask.repeat_flag = 0;
    project->newtask.type_flag = 0;

    project->newtask.dialog = gtk_dialog_new_with_buttons("Add Task",
                                       GTK_WINDOW(project->schedule.dialog),
                                       GTK_DIALOG_MODAL,
                                       "Add",
                                       GTK_RESPONSE_ACCEPT,
                                       "Cancel",
                                       GTK_RESPONSE_CANCEL,
                                       NULL);
    gtk_widget_set_size_request(project->newtask.dialog, 450, 280);
    gtk_window_set_resizable(GTK_WINDOW(project->newtask.dialog), FALSE);

    content = gtk_dialog_get_content_area(GTK_DIALOG(project->newtask.dialog));

    /* Frame Layout */
    sprintf(buffer, "Relay %hu Schedule", relay);
    frame = gtk_frame_new(buffer);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(content), frame);

    project->newtask.table = gtk_table_new(4, 4, TRUE);
    gtk_container_add(GTK_CONTAINER(frame), project->newtask.table);

    label = gtk_label_new(labels[0]);
    gtk_table_attach(GTK_TABLE(project->newtask.table),
                    label,
                    0, 1, 0, 1,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);

    project->newtask.name = gtk_entry_new();
    gtk_table_attach(GTK_TABLE(project->newtask.table),
                    project->newtask.name,
                    1, 2, 0, 1,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);

    label = gtk_label_new(labels[1]);
    gtk_table_attach(GTK_TABLE(project->newtask.table),
                    label,
                    0, 1, 1, 2,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);

    project->newtask.enabled = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(project->newtask.enabled), "Enable");
    gtk_combo_box_append_text(GTK_COMBO_BOX(project->newtask.enabled), "Disable");
    gtk_table_attach(GTK_TABLE(project->newtask.table),
                    project->newtask.enabled,
                    1, 2, 1, 2,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(project->newtask.enabled), 0);

    label = gtk_label_new(labels[2]);
    gtk_table_attach(GTK_TABLE(project->newtask.table),
                    label,
                    0, 1, 2, 3,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);

    project->newtask.repeat = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(project->newtask.repeat), "Once");
    gtk_combo_box_append_text(GTK_COMBO_BOX(project->newtask.repeat), "Repeat");
    g_signal_connect(G_OBJECT(project->newtask.repeat),
                    "changed",
                    G_CALLBACK(repeat_change),
                    project);
    gtk_table_attach(GTK_TABLE(project->newtask.table),
                    project->newtask.repeat,
                    1, 2, 2, 3,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(project->newtask.repeat), 0);

    label = gtk_label_new(labels[3]);
    gtk_table_attach(GTK_TABLE(project->newtask.table),
                    label,
                    0, 1, 3, 4,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);

    project->newtask.type = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(project->newtask.type), "ON/OFF");
    gtk_combo_box_append_text(GTK_COMBO_BOX(project->newtask.type), "Pulse");
    g_signal_connect(G_OBJECT(project->newtask.type),
                    "changed",
                    G_CALLBACK(type_change),
                    project);
    gtk_table_attach(GTK_TABLE(project->newtask.table),
                    project->newtask.type,
                    1, 2, 3, 4,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(project->newtask.type), 0);


    for(i = 4; i < 6; i++)
    {
        GtkWidget *hbox;

        label = gtk_label_new(labels[i]);
        gtk_table_attach(GTK_TABLE(project->newtask.table),
                     label,
                     0, 1, i, i+1,
                     GTK_EXPAND | GTK_FILL,
                     GTK_EXPAND | GTK_FILL,
                     0, 0);

        hbox = gtk_hbox_new(TRUE, 0);

        if(i == 4)
        {
            project->newtask.hours[i-4] = gtk_spin_button_new_with_range(0, 23, 1);
            gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(project->newtask.hours[i-4]), FALSE, FALSE, 0);

            project->newtask.minutes[i-4] = gtk_spin_button_new_with_range(0, 59, 1);
            gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(project->newtask.minutes[i-4]), FALSE, FALSE, 0);
            gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(project->newtask.minutes[i-4]), TRUE);
        }

        gtk_table_attach(GTK_TABLE(project->newtask.table),
                         hbox,
                         1, 2, i, i+1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);
    }

    gtk_widget_show_all(project->newtask.dialog);

    result = gtk_dialog_run(GTK_DIALOG(project->newtask.dialog));

    switch(result)
    {
        case GTK_RESPONSE_ACCEPT :
            g_object_set_data(G_OBJECT(project->newtask.dialog), "device", (gpointer) device);
            g_object_set_data(G_OBJECT(project->newtask.dialog), "relay", GUINT_TO_POINTER(relay));
            submit_task_cb(GTK_WIDGET(project->newtask.dialog), project);
            break;

        case GTK_RESPONSE_CANCEL :
            gtk_widget_destroy(project->newtask.dialog);
            break;
    }
}

/**
* Fonction de modification de la fenêtre add task lors de l'action sur la combobox Repeat
*/
void repeat_change(GtkComboBox *button, Project *project)
{
    if(gtk_combo_box_get_active(GTK_COMBO_BOX(project->newtask.repeat)) == 0)
    {
        gushort i;

        if(project->newtask.repeat_flag)
        {
            gtk_widget_destroy(project->newtask.week);
            for(i = 0; i < 7; i++)
                gtk_widget_destroy(project->newtask.days[i]);
            for(i = 0; i < 8; i++)
                gtk_widget_destroy(project->newtask.day_label[i]);
        }

        project->newtask.date = gtk_calendar_new();
        gtk_table_attach(GTK_TABLE(project->newtask.table),
                        project->newtask.date,
                        2, 4, 0, 5,
                        GTK_EXPAND | GTK_FILL,
                        GTK_EXPAND | GTK_FILL,
                        10, 0);
        gtk_widget_show_all(project->newtask.dialog);

        project->newtask.repeat_flag = 1;
    }
    else
    {
        GtkWidget   *dtable;
        gushort     i;

        const gchar *days_labels[7] = {"M", "T", "W", "T", "F", "S", "S"};

        gtk_widget_destroy(project->newtask.date);

        project->newtask.day_label[0] = gtk_label_new("Interval (Weeks) :");
        gtk_table_attach(GTK_TABLE(project->newtask.table),
                        project->newtask.day_label[0],
                        2, 4, 1, 2,
                        GTK_EXPAND | GTK_FILL,
                        GTK_EXPAND | GTK_FILL,
                        0, 0);
        project->newtask.week = gtk_spin_button_new_with_range(1, 52, 1);
        gtk_table_attach(GTK_TABLE(project->newtask.table),
                        project->newtask.week,
                        2, 4, 2, 3,
                        GTK_EXPAND | GTK_FILL,
                        GTK_EXPAND | GTK_FILL,
                        60, 0);

        dtable = gtk_table_new(2, 7, FALSE);
        for(i = 0; i < 7; i++)
        {
            project->newtask.day_label[i+1] = gtk_label_new(days_labels[i]);
            gtk_table_attach(GTK_TABLE(dtable),
                        project->newtask.day_label[i+1],
                        i, i+1, 0, 1,
                        GTK_EXPAND | GTK_FILL,
                        GTK_EXPAND | GTK_FILL,
                        0, 0);
            project->newtask.days[i] = gtk_check_button_new();
            gtk_table_attach(GTK_TABLE(dtable),
                        project->newtask.days[i],
                        i, i+1, 1, 2,
                        GTK_EXPAND | GTK_FILL,
                        GTK_EXPAND | GTK_FILL,
                        3, 0);
        }
        gtk_table_attach(GTK_TABLE(project->newtask.table),
                        dtable,
                        2, 4, 3, 5,
                        GTK_EXPAND | GTK_FILL,
                        GTK_EXPAND | GTK_FILL,
                        10, 0);
        gtk_widget_show_all(project->newtask.dialog);
    }
}

/**
* Fonction de modification de la fenêtre add task lors de l'action sur la combobox Type
*/
void type_change(GtkComboBox *button, Project *project)
{
    if(gtk_combo_box_get_active(GTK_COMBO_BOX(project->newtask.type)) == 0)
    {
        GtkWidget *hbox;

        if(project->newtask.type_flag)
            gtk_widget_destroy(project->newtask.type_label);

        hbox = gtk_hbox_new(TRUE, 0);

        project->newtask.hours[1] = gtk_spin_button_new_with_range(0, 23, 1);
        gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(project->newtask.hours[1]), FALSE, FALSE, 0);

        project->newtask.minutes[1] = gtk_spin_button_new_with_range(0, 59, 1);
        gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(project->newtask.minutes[1]), FALSE, FALSE, 0);
        gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(project->newtask.minutes[1]), TRUE);

        gtk_table_attach(GTK_TABLE(project->newtask.table),
                         hbox,
                         1, 2, 5, 6,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

        project->newtask.type_flag = 1;
        gtk_widget_show_all(project->newtask.dialog);
    }
    else
    {
        gtk_widget_destroy(project->newtask.hours[1]);
        gtk_widget_destroy(project->newtask.minutes[1]);

        project->newtask.type_label = gtk_label_new("Pulse timing");
        gtk_table_attach(GTK_TABLE(project->newtask.table),
                         project->newtask.type_label,
                         1, 2, 5, 6,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

        gtk_widget_show_all(project->newtask.dialog);
    }
}

/**
* Fonction de vérification et traitement des données avant l'envoi au serveur
*/
void submit_task_cb(GtkWidget *submit, Project *project)
{
    gushort relay = (gushort) GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(submit), "relay"));
    Device *device = (Device *)g_object_get_data(G_OBJECT(submit), "device");

    if(g_strcmp0(gtk_entry_get_text(GTK_ENTRY(project->newtask.name)), "") != 0)
    {
        Tasks *task;
        if((task = getTaskByName(device->tasks[relay-1], gtk_entry_get_text(GTK_ENTRY(project->newtask.name)))) == NULL)
        {
            if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.hours[0])) < 0)
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->newtask.hours[0]), 0);
            if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.hours[0])) > 23)
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->newtask.hours[0]), 23);

            if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.minutes[0])) < 0)
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->newtask.minutes[0]), 0);
            if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.minutes[0])) > 59)
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->newtask.minutes[0]), 59);

            if(gtk_combo_box_get_active(GTK_COMBO_BOX(project->newtask.type)) == 0)
            {
                if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.hours[1])) < 0)
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->newtask.hours[1]), 0);
                if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.hours[1])) > 23)
                        gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->newtask.hours[1]), 23);

                if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.minutes[1])) < 0)
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->newtask.minutes[1]), 0);
                if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.minutes[1])) > 59)
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(project->newtask.minutes[1]), 59);
            }

            save_task(project, device->name, relay);
            gtk_widget_destroy(project->newtask.dialog);
        }
        else
            warning_dialog(project->newtask.dialog, "\nThis task name already exist.");
    }
    else
        warning_dialog(project->newtask.dialog, "\nAt least one field is empty or error with spin buttons.");
}

/**
* Fonction d'envoi des données au serveur
*/
void save_task(Project *project, const gchar *name, gushort relay)
{
    char *packet;
    char buffer[6][50];

    sprintf(buffer[0], "%hu", relay);
    sprintf(buffer[1], "%dh%d", (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.hours[0])),
                                (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.minutes[0])));

    if(gtk_combo_box_get_active(GTK_COMBO_BOX(project->newtask.type)) == 0)
        sprintf(buffer[2], "%dh%d", (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.hours[1])),
                                    (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.minutes[1])));
    else
        strcpy(buffer[2], "0h0");

    if((gtk_combo_box_get_active(GTK_COMBO_BOX(project->newtask.repeat)) == 1))
    {
        time_t timestamp = time(NULL);
        struct tm date = *localtime(&timestamp);
        date.tm_year += 1900;
        date.tm_mon += 1;

        sprintf(buffer[3], "%d", (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(project->newtask.week)));
        sprintf(buffer[4], "%d,%d,%d,%d,%d,%d,%d",
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(project->newtask.days[0])),
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(project->newtask.days[1])),
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(project->newtask.days[2])),
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(project->newtask.days[3])),
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(project->newtask.days[4])),
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(project->newtask.days[5])),
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(project->newtask.days[6])));

        sprintf(buffer[5], "%d/%d/%d", date.tm_mday, date.tm_mon, date.tm_year);
    }
    else
    {
        guint d, m, y;
        gtk_calendar_get_date (GTK_CALENDAR(project->newtask.date), &y, &m, &d);

        strcpy(buffer[3], "0");
        strcpy(buffer[4], "0,0,0,0,0,0,0");
        sprintf(buffer[5], "%u/%u/%u", d, m+1, y);
    }

    packet = formatPackets(12, "addTask",
                                name,
                                buffer[0],
                                gtk_entry_get_text(GTK_ENTRY(project->newtask.name)),
                                (gtk_combo_box_get_active(GTK_COMBO_BOX(project->newtask.enabled)) == 0) ? "1" : "0",
                                (gtk_combo_box_get_active(GTK_COMBO_BOX(project->newtask.repeat)) == 0) ? "0" : "1",
                                buffer[5],
                                buffer[3],
                                buffer[4],
                                (gtk_combo_box_get_active(GTK_COMBO_BOX(project->newtask.type)) == 0) ? "0" : "1",
                                buffer[1],
                                buffer[2]);

    pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);

    g_cond_broadcast(project->server.notifier);

    free(packet);
}

/**
* Fonction de modification d'une tâche précise
*/
void modify_selected_task_cb(GtkButton *button, Project *project)
{
    GtkTreeIter         iter;
    GtkTreeSelection    *selection;
    GtkTreeModel        *model;
    GtkTreePath         *path;

    int                 *index;
    Tasks               *task;
    Device              *device;
    gushort             relay;

    device = (Device *) g_object_get_data(G_OBJECT(button), "device");
    relay = (gushort) GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "relay"));

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(project->schedule.listview));

    if(gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        path = gtk_tree_model_get_path(model, &iter);
        index = gtk_tree_path_get_indices(path);

        task = getTask(device->tasks[relay-1], index[0]);

        if(task != NULL)
            modify_task_cb(project, device, relay, task);
    }
    else
        warning_dialog(project->schedule.dialog, "Please select a task.");
}

void modify_task_cb(Project *project, Device *device, gushort relay, Tasks *task)
{
    GtkWidget   *content;
    GtkWidget   *frame;
    GtkWidget   *label;

    const gchar *labels[6] = {
                  "Name :",
                  "Task :",
                  "Repeat :",
                  "Type :",
                  "Beginning :",
                  "End :"
                };

    gushort     i;
    guint       result;
    gchar       buffer[SOCKET_BUFFER];

    task->element.repeat_flag = 0;
    task->element.type_flag = 0;

    task->element.dialog = gtk_dialog_new_with_buttons("Modify Task",
                            GTK_WINDOW(project->schedule.dialog),
                            GTK_DIALOG_MODAL,
                            "Modify",
                            GTK_RESPONSE_ACCEPT,
                            "Cancel",
                            GTK_RESPONSE_CANCEL,
                            NULL);
    gtk_widget_set_size_request(task->element.dialog, 450, 280);
    gtk_window_set_resizable(GTK_WINDOW(task->element.dialog), FALSE);

    content = gtk_dialog_get_content_area(GTK_DIALOG(task->element.dialog));

    /* Frame Layout */
    sprintf(buffer, "Relay %hu Schedule", relay);
    frame = gtk_frame_new(buffer);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(content), frame);

    task->element.table = gtk_table_new(4, 4, TRUE);
    gtk_container_add(GTK_CONTAINER(frame), task->element.table);

    label = gtk_label_new(labels[0]);
    gtk_table_attach(GTK_TABLE(task->element.table),
                    label,
                    0, 1, 0, 1,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);

    task->element.name = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(task->element.name), task->element.name_val);
    gtk_table_attach(GTK_TABLE(task->element.table),
                    task->element.name,
                    1, 2, 0, 1,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);

    label = gtk_label_new(labels[1]);
    gtk_table_attach(GTK_TABLE(task->element.table),
                    label,
                    0, 1, 1, 2,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);

    task->element.enabled = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(task->element.enabled), "Enable");
    gtk_combo_box_append_text(GTK_COMBO_BOX(task->element.enabled), "Disable");
    gtk_table_attach(GTK_TABLE(task->element.table),
                    task->element.enabled,
                    1, 2, 1, 2,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(task->element.enabled), !task->element.enabled_val);

    label = gtk_label_new(labels[2]);
    gtk_table_attach(GTK_TABLE(task->element.table),
                    label,
                    0, 1, 2, 3,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);

    task->element.repeat = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(task->element.repeat), "Once");
    gtk_combo_box_append_text(GTK_COMBO_BOX(task->element.repeat), "Repeat");
    g_object_set_data(G_OBJECT(task->element.repeat), "task", (gpointer) task);
    g_signal_connect(G_OBJECT(task->element.repeat),
                    "changed",
                    G_CALLBACK(modify_repeat_change),
                    task);
    gtk_table_attach(GTK_TABLE(task->element.table),
                    task->element.repeat,
                    1, 2, 2, 3,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(task->element.repeat), task->element.repeat_val);

    label = gtk_label_new(labels[3]);
    gtk_table_attach(GTK_TABLE(task->element.table),
                    label,
                    0, 1, 3, 4,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);

    task->element.type = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(task->element.type), "ON/OFF");
    gtk_combo_box_append_text(GTK_COMBO_BOX(task->element.type), "Pulse");
    g_object_set_data(G_OBJECT(task->element.type), "task", (gpointer) task);
    g_signal_connect(G_OBJECT(task->element.type),
                    "changed",
                    G_CALLBACK(modify_type_change),
                    task);
    gtk_table_attach(GTK_TABLE(task->element.table),
                    task->element.type,
                    1, 2, 3, 4,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(task->element.type), task->element.type_val);


    for(i = 4; i < 6; i++)
    {
        GtkWidget *hbox;

        label = gtk_label_new(labels[i]);
        gtk_table_attach(GTK_TABLE(task->element.table),
                     label,
                     0, 1, i, i+1,
                     GTK_EXPAND | GTK_FILL,
                     GTK_EXPAND | GTK_FILL,
                     0, 0);

        hbox = gtk_hbox_new(TRUE, 0);

        if(i == 4)
        {
            task->element.hours[0] = gtk_spin_button_new_with_range(0, 23, 1);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.hours[0]), task->element.hours_val[0]);
            gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(task->element.hours[0]), FALSE, FALSE, 0);

            task->element.minutes[0] = gtk_spin_button_new_with_range(0, 59, 1);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.minutes[0]), task->element.minutes_val[0]);
            gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(task->element.minutes[0]), FALSE, FALSE, 0);
        }

        gtk_table_attach(GTK_TABLE(task->element.table),
                         hbox,
                         1, 2, i, i+1,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);
    }

    gtk_widget_show_all(task->element.dialog);

    result = gtk_dialog_run(GTK_DIALOG(task->element.dialog));

    switch(result)
    {
        case GTK_RESPONSE_ACCEPT :
            g_object_set_data(G_OBJECT(task->element.dialog), "task", (gpointer) task);
            g_object_set_data(G_OBJECT(task->element.dialog), "device", (gpointer) device);
            g_object_set_data(G_OBJECT(task->element.dialog), "relay", GUINT_TO_POINTER(relay));
            submit_modify_task_cb(GTK_WIDGET(task->element.dialog), project);
            break;

        case GTK_RESPONSE_CANCEL :
            gtk_widget_destroy(task->element.dialog);
            break;
    }
}

/**
* Fonction de modification de la fenêtre add task lors de l'action sur la combobox Repeat
*/
void modify_repeat_change(GtkComboBox *button, Tasks *task)
{
    if(gtk_combo_box_get_active(GTK_COMBO_BOX(task->element.repeat)) == 0)
    {
        gushort i, size;
        char **buffer;

        buffer = explode(task->element.date_val, "/", &size);

        if(task->element.repeat_flag)
        {
            gtk_widget_destroy(task->element.week);
            for(i = 0; i < 7; i++)
                gtk_widget_destroy(task->element.days[i]);
            for(i = 0; i < 8; i++)
                gtk_widget_destroy(task->element.day_label[i]);
        }

        task->element.date = gtk_calendar_new();
        gtk_calendar_select_day(GTK_CALENDAR(task->element.date), (guint)atoi(buffer[0]));
        gtk_calendar_select_month(GTK_CALENDAR(task->element.date), (guint)atoi(buffer[1])-1, (guint)atoi(buffer[2]));
        gtk_table_attach(GTK_TABLE(task->element.table),
                        task->element.date,
                        2, 4, 0, 5,
                        GTK_EXPAND | GTK_FILL,
                        GTK_EXPAND | GTK_FILL,
                        10, 0);
        gtk_widget_show_all(task->element.dialog);

        task->element.repeat_flag = 1;

        for(i = 0; i < size; i++)
            free(buffer[i]);
        free(buffer);
    }
    else
    {
        GtkWidget   *dtable;
        gushort     i;

        const gchar *days_labels[7] = {"M", "T", "W", "T", "F", "S", "S"};

        if(task->element.repeat_flag)
            gtk_widget_destroy(task->element.date);

        task->element.day_label[0] = gtk_label_new("Interval (Weeks) :");
        gtk_table_attach(GTK_TABLE(task->element.table),
                        task->element.day_label[0],
                        2, 4, 1, 2,
                        GTK_EXPAND | GTK_FILL,
                        GTK_EXPAND | GTK_FILL,
                        0, 0);
        task->element.week = gtk_spin_button_new_with_range(1, 52, 1);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.week), task->element.weeks_val);
        gtk_table_attach(GTK_TABLE(task->element.table),
                        task->element.week,
                        2, 4, 2, 3,
                        GTK_EXPAND | GTK_FILL,
                        GTK_EXPAND | GTK_FILL,
                        60, 0);

        dtable = gtk_table_new(2, 7, FALSE);
        for(i = 0; i < 7; i++)
        {
            task->element.day_label[i+1] = gtk_label_new(days_labels[i]);
            gtk_table_attach(GTK_TABLE(dtable),
                        task->element.day_label[i+1],
                        i, i+1, 0, 1,
                        GTK_EXPAND | GTK_FILL,
                        GTK_EXPAND | GTK_FILL,
                        0, 0);
            task->element.days[i] = gtk_check_button_new();
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(task->element.days[i]), task->element.days_val[i]);
            gtk_table_attach(GTK_TABLE(dtable),
                        task->element.days[i],
                        i, i+1, 1, 2,
                        GTK_EXPAND | GTK_FILL,
                        GTK_EXPAND | GTK_FILL,
                        3, 0);
        }
        gtk_table_attach(GTK_TABLE(task->element.table),
                        dtable,
                        2, 4, 3, 5,
                        GTK_EXPAND | GTK_FILL,
                        GTK_EXPAND | GTK_FILL,
                        10, 0);
        task->element.repeat_flag = 1;
        gtk_widget_show_all(task->element.dialog);
    }
}

/**
* Fonction de modification de la fenêtre modify task lors de l'action sur la combobox Type
*/
void modify_type_change(GtkComboBox *button, Tasks *task)
{
    if(gtk_combo_box_get_active(GTK_COMBO_BOX(task->element.type)) == 0)
    {
        GtkWidget *hbox;

        if(task->element.type_flag)
            gtk_widget_destroy(task->element.type_label);

        hbox = gtk_hbox_new(TRUE, 0);

        task->element.hours[1] = gtk_spin_button_new_with_range(0, 23, 1);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.hours[1]), task->element.hours_val[1]);
        gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(task->element.hours[1]), FALSE, FALSE, 0);

        task->element.minutes[1] = gtk_spin_button_new_with_range(0, 59, 1);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.minutes[1]), task->element.minutes_val[1]);
        gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(task->element.minutes[1]), FALSE, FALSE, 0);

        gtk_table_attach(GTK_TABLE(task->element.table),
                         hbox,
                         1, 2, 5, 6,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

        task->element.type_flag = 1;
        gtk_widget_show_all(task->element.dialog);
    }
    else
    {
        if(task->element.type_flag)
        {
            gtk_widget_destroy(task->element.hours[1]);
            gtk_widget_destroy(task->element.minutes[1]);
        }

        task->element.type_label = gtk_label_new("Pulse timing");
        gtk_table_attach(GTK_TABLE(task->element.table),
                         task->element.type_label,
                         1, 2, 5, 6,
                         GTK_EXPAND | GTK_FILL,
                         GTK_EXPAND | GTK_FILL,
                         0, 0);

        task->element.type_flag = 1;
        gtk_widget_show_all(task->element.dialog);
    }
}

/**
* Fonction de vérification et traitement des données avant l'envoi au serveur
*/
void submit_modify_task_cb(GtkWidget *submit, Project *project)
{
    Tasks *task = (Tasks *) g_object_get_data(G_OBJECT(submit), "task");
    gushort relay = (gushort) GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(submit), "relay"));
    Device *device = (Device *)g_object_get_data(G_OBJECT(submit), "device");

    if(g_strcmp0(gtk_entry_get_text(GTK_ENTRY(task->element.name)), "") != 0)
    {
        if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.hours[0])) < 0)
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.hours[0]), 0);
        if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.hours[0])) > 23)
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.hours[0]), 23);

        if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.minutes[0])) < 0)
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.minutes[0]), 0);
        if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.minutes[0])) > 59)
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.minutes[0]), 59);

        if(gtk_combo_box_get_active(GTK_COMBO_BOX(task->element.type)) == 0)
        {
            if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.hours[1])) < 0)
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.hours[1]), 0);
            if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.hours[1])) > 23)
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.hours[1]), 23);

            if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.minutes[1])) < 0)
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.minutes[1]), 0);
            if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.minutes[1])) > 59)
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(task->element.minutes[1]), 59);
        }

        save_modify_task(project, task, device->name, relay);
        gtk_widget_destroy(task->element.dialog);
    }
    else
        warning_dialog(task->element.dialog, "\nAt least one field is empty or error with spin buttons.");
}

/**
* Fonction d'envoi des données au serveur
*/
void save_modify_task(Project *project, Tasks *task, const gchar *name, gushort relay)
{
    char *packet;
    char buffer[6][50];

    sprintf(buffer[0], "%hu", relay);
    sprintf(buffer[1], "%dh%d", (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.hours[0])),
                                (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.minutes[0])));
    if(gtk_combo_box_get_active(GTK_COMBO_BOX(task->element.type)) == 0)
        sprintf(buffer[2], "%dh%d", (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.hours[1])),
                                    (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.minutes[1])));
    else
        strcpy(buffer[2], "0h0");

    if((gtk_combo_box_get_active(GTK_COMBO_BOX(task->element.repeat)) == 1))
    {
        time_t timestamp = time(NULL);
        struct tm date = *localtime(&timestamp);
        date.tm_year += 1900;
        date.tm_mon += 1;

        sprintf(buffer[3], "%d", (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(task->element.week)));
        sprintf(buffer[4], "%d,%d,%d,%d,%d,%d,%d",
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(task->element.days[0])),
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(task->element.days[1])),
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(task->element.days[2])),
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(task->element.days[3])),
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(task->element.days[4])),
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(task->element.days[5])),
                            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(task->element.days[6])));

        sprintf(buffer[5], "%d/%d/%d", date.tm_mday, date.tm_mon, date.tm_year);
    }
    else
    {
        guint d, m, y;
        gtk_calendar_get_date(GTK_CALENDAR(task->element.date), &y, &m, &d);

        strcpy(buffer[3], "0");
        strcpy(buffer[4], "0,0,0,0,0,0,0");
        sprintf(buffer[5], "%u/%u/%u", d, m+1, y);
    }

    packet = formatPackets(13, "modifyTask",
                                name,
                                buffer[0],
                                task->element.name_val,
                                gtk_entry_get_text(GTK_ENTRY(task->element.name)),
                                (gtk_combo_box_get_active(GTK_COMBO_BOX(task->element.enabled)) == 0) ? "1" : "0",
                                (gtk_combo_box_get_active(GTK_COMBO_BOX(task->element.repeat)) == 0) ? "0" : "1",
                                buffer[5],
                                buffer[3],
                                buffer[4],
                                (gtk_combo_box_get_active(GTK_COMBO_BOX(task->element.type)) == 0) ? "0" : "1",
                                buffer[1],
                                buffer[2]);

    pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);

    g_cond_broadcast(project->server.notifier);

    free(packet);
}


/**
* Fonction de suppression d'une tâche précise
*/
void delete_selected_task_cb(GtkButton *button, Project *project)
{
    GtkTreeIter         iter;
    GtkTreeSelection    *selection;
    GtkTreeModel        *model;
    GtkTreePath         *path;

    int                 *index;
    Tasks               *task;
    Device              *device;
    gushort             relay;

    device = (Device *) g_object_get_data(G_OBJECT(button), "device");
    relay = (gushort) GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "relay"));

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(project->schedule.listview));

    if(gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        path = gtk_tree_model_get_path(model, &iter);
        index = gtk_tree_path_get_indices(path);

        task = getTask(device->tasks[relay-1], index[0]);

        if(task != NULL)
        {
            char *packet;
            char buffer[5];

            sprintf(buffer, "%hu", relay);

            packet = formatPackets(6, "deleteTask",
                                      device->name,
                                      buffer,
                                      task->element.name_val,
                                      project->login.login,
                                      project->login.password);

            pushQueue(&project->server.sender_queue, packet, project->server.sender_lock);

            g_cond_broadcast(project->server.notifier);

            free(packet);
        }
    }
    else
        warning_dialog(project->schedule.dialog, "Please select a task.");
}

/**
* Fonction de fermeture de la fenêtre de planification
* (Astuce pour éviter un Warning à la compilation)
*/
void close_schedule_cb(GtkButton *button, GtkWidget *widget)
{
    gboolean *jump = (gboolean*) g_object_get_data(G_OBJECT(button), "jump");

    *jump = TRUE;
    gtk_widget_destroy(widget);
}
