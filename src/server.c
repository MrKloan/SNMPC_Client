#include "includes.h"

void serverInit(Server *server)
{
    if(server->running == FALSE)
    {
        SocketStart();

        server->lock = g_mutex_new();
        server->sender_lock = g_mutex_new();
        server->dispatch_lock = g_mutex_new();
        server->notifier = g_cond_new();
        server->dispatch_notifier = g_cond_new();

        server->sender_queue = NULL;
        server->dispatch_queue = NULL;

        server->running = TRUE;
    }
}

void serverFree(Server *server)
{
    if(server->running == TRUE)
    {
        server->running = FALSE;

        g_mutex_free(server->lock);
        g_mutex_free(server->sender_lock);
        g_mutex_free(server->dispatch_lock);
        g_cond_broadcast(server->notifier);
        g_cond_free(server->notifier);
        g_cond_broadcast(server->dispatch_notifier);
        g_cond_free(server->dispatch_notifier);
        freeQueue(&server->dispatch_queue);
        freeQueue(&server->sender_queue);

        socketFree(server);
        SocketEnd();
    }
}

gpointer listener(gpointer data)
{
    Project *project = (Project *) data;

    while(project->server.running)
    {
        if(socketReceive(&project->server))
        {
            pushQueue(&project->server.dispatch_queue, project->server.buffer, project->server.dispatch_lock);
            g_cond_broadcast(project->server.dispatch_notifier);
        }
        else
        {
            gdk_threads_enter();
            warning_dialog(project->home.window, "\nAn error occured while listening to server packet!");
            gtk_main_quit();
            gdk_threads_leave();
            //gtk_widget_destroy(project->home.window);
            //gtk_widget_show_all(project->login.window);

            break;
        }

    }

    return NULL;
}

gpointer dispatcher(gpointer data)
{
    Project *project = (Project *) data;

    while(project->server.running)
    {
        char *packet;

        g_mutex_lock(project->server.lock);

        if(countQueue(project->server.dispatch_queue) == 0)
            g_cond_wait(project->server.dispatch_notifier, project->server.lock);

        if(!project->server.running)
            break;

        packet = popQueue(&project->server.dispatch_queue, project->server.dispatch_lock);

        if(packet != NULL)
        {
            char **parts;
            unsigned short sizeparts, i;

            parts = explode(packet, PACKET_END, &sizeparts);

            if(parts != NULL && sizeparts > 0)
            {
                for(i = 0; i < sizeparts; i++)
                {
                    uncipherPacket(project, &parts[i]);

                    if(parts[i] != NULL)
                    {
                        char **packets;
                        unsigned short j, size;

                        packets = explode(parts[i], PACKET_END, &size);

                        for(j = 0; j < size ; j++)
                        {
                            handlePacket((gpointer)project, packets[j]);
                            free(packets[j]);
                        }
                        free(packets);
                        free(parts[i]);
                    }
                }
                free(parts);
            }
            free(packet);
        }
        g_mutex_unlock(project->server.lock);
    }

    return NULL;
}

gpointer sender(gpointer data)
{
    Project *project = (Project *) data;

    while(project->server.running)
    {
        char *packet;
        g_mutex_lock(project->server.lock);

        if(countQueue(project->server.sender_queue) == 0)
            g_cond_wait(project->server.notifier, project->server.lock);

        if(!project->server.running)
            break;

        packet = popQueue(&project->server.sender_queue, project->server.sender_lock);

        if(packet != NULL)
        {
            cipherPacket(project, &packet);
            strncpy(project->server.buffer, packet, 1488/8);

            if(socketSend(&project->server))
            {

            }
            else
            {
                gdk_threads_enter();
                warning_dialog(project->home.window, "\nAn error occured while sending packet to the server!");
                g_mutex_unlock(project->server.lock);
                gtk_main_quit();
                gdk_threads_leave();

                break;
            }
            free(packet);
        }
        g_mutex_unlock(project->server.lock);
    }

    return NULL;
}

void handlePacket(gpointer func_data, char *packet)
{
    char            **args;
    gushort         size, i;
    gshort          index;
    Devices         *device;

    Project *project = (Project *) func_data;

    char *commands[11] = {
        "loginServer",
        "addDevice",
        "delDevice",
        "closeAccount",
        "visualization",
        "visualizationRate",
        "addTask",
        "deleteTask",
        "modifyTask",
        "error",
        "noDevice"
    };

    char *snmpCommands[15] = {
        //Network setup
        "dhcpConfig",
        "deviceIPAddress",
        "subnetMask",
        "gateway",
        "deviceMACAddress",
        //SNMP setup
        "trapEnabled",
        "trapReceiverIPAddress",
        "trapCommunity",
        //All relays
        "allOn",
        "allOff",
        "allPulse",
        //Digital input
        "digitalInput",
        //Product Infos
        "name",
        "version",
        "date"
    };

    g_print("%s\n", packet);
    args = explode(packet, PACKET_DELIMITER, &size);

    //login
    if(strcmp(args[0], commands[0]) == 0)
    {
        if(strcmp(args[1], "valid") == 0)
        {
            project->server.listener = g_thread_create((GThreadFunc)listener, project, FALSE, NULL);
            project->server.dispatcher = g_thread_create((GThreadFunc)dispatcher, project, FALSE, NULL);
            project->server.sender = g_thread_create((GThreadFunc)sender, project, FALSE, NULL);

            project->home.flag = 1;
            gtk_widget_destroy(project->login.window);
            create_home(project);
            gtk_widget_show_all(project->home.window);
        }
        else
            warning_dialog(project->login.window, "\nInvalid login informations.");
    }
    //add device
    else if(!strcmp(args[0], commands[1]))
    {
        if(size == 5)
        {
            addDevice(&project->devices, args[1], args[2], args[3], args[4]);

            device = getLastDevice(&project->devices);
            index = countDevices(project->devices)-1;

            gdk_threads_enter();

            if(project->adddevice.nodevice)
            {
                gtk_notebook_remove_page(GTK_NOTEBOOK(project->home.notebook), 0);
                project->adddevice.nodevice = 0;
            }

            //Update Home
            create_page(project, &device->element);
            gtk_widget_show_all(project->home.window);

            //Update Visualization
            if(project->visualization.window != NULL)
            {
                char buffer[128];

                gtk_widget_destroy(project->visualization.table[index]);

                project->visualization.table[index] = gtk_table_new(3, 9, TRUE);
                gtk_container_add(GTK_CONTAINER(project->visualization.frame[index]), GTK_WIDGET(project->visualization.table[index]));

                for(i=0; i < 9; i++)
                {
                    project->visualization.led[index][i] = gtk_led_new((i == 0) ? device->element.digital_input : device->element.relays_state[i-1]);
                    gtk_table_attach(GTK_TABLE(project->visualization.table[index]),
                                 project->visualization.led[index][i],
                                 i, i+1, 1, 2,
                                 GTK_EXPAND | GTK_FILL,
                                 GTK_EXPAND | GTK_FILL,
                                 0, 0);

                    if(i==0)
                        sprintf(buffer, "Digital Input");
                    else
                        strcpy(buffer, device->element.relays_name[i-1]);

                    project->visualization.label[index][i] = gtk_label_new(buffer);
                    gtk_table_attach(GTK_TABLE(project->visualization.table[index]),
                                 project->visualization.label[index][i],
                                 i, i+1, 2, 3,
                                 GTK_EXPAND | GTK_FILL,
                                 GTK_EXPAND | GTK_FILL,
                                 0, 0);
                }

                gtk_widget_show_all(project->visualization.frame[index]);
            }

            gdk_threads_leave();

            g_print("Device %s added!\n", device->element.name);
        }
        else
        {
            gdk_threads_enter();
            warning_dialog(project->home.window, "\nAn invalid device packet has been trashed.");
            gdk_threads_leave();
        }
    }
    //del device
    else if(!strcmp(args[0], commands[2]))
    {
        if(size == 2)
        {
            getDeviceByName(project->devices, args[1], &index);

            gdk_threads_enter();
            gtk_notebook_remove_page(GTK_NOTEBOOK(project->home.notebook), index);
            gdk_threads_leave();

            delDeviceByIndex(&project->devices, index);
        }
        else
        {
            gdk_threads_enter();
            warning_dialog(project->home.window, "\nAn invalid device packet has been trashed.");
            gdk_threads_leave();
        }
    }
    //close account
    else if(!strcmp(args[0], commands[3]))
    {
        if(size == 2)
        {
            if(!strcmp(args[1], project->login.login))
            {
                gdk_threads_enter();
                disconnect_cb(project->home.window, project);
                gdk_threads_leave();
            }
            else
            {
                gdk_threads_enter();
                warning_dialog(project->home.window, "\nAn invalid close account packet has been trashed.");
                gdk_threads_leave();
            }
        }
        else
        {
            gdk_threads_enter();
            warning_dialog(project->home.window, "\nAn invalid close account packet has been trashed.");
            gdk_threads_leave();
        }
    }
    //visualization
    else if(!strcmp(args[0], commands[4]))
    {
        if(size == 4)
        {
            if((device = getDeviceByName(project->devices, args[1], &index)) != NULL)
            {
                if(project->visualization.window != NULL)
                {
                    //digitalInput
                    if(!strcmp(args[2], snmpCommands[11]))
                    {
                        gdk_threads_enter();
                        gtk_led_set_state(project->visualization.led[index][0], (atoi(args[3]) ? TRUE : FALSE));
                        gtk_widget_queue_draw(GTK_WIDGET(project->visualization.led[index][0]));
                        gdk_threads_leave();
                    }
                    else
                    {
                        char buffer[SOCKET_BUFFER];

                        for(i=0 ; i < 8 ; i++)
                        {
                            sprintf(buffer, "relay%hu", i+1);

                            if(!strcmp(args[2], buffer))
                            {
                                gdk_threads_enter();
                                gtk_led_set_state(project->visualization.led[index][i+1], (atoi(args[3]) ? TRUE : FALSE));
                                gtk_widget_queue_draw(GTK_WIDGET(project->visualization.led[index][i+1]));
                                gdk_threads_leave();
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                gdk_threads_enter();
                warning_dialog(project->home.window, "\nUnknown device in visualization packet. Aborting.");
                gdk_threads_leave();
            }
        }
        else
        {
            gdk_threads_enter();
            warning_dialog(project->home.window, "\nAn invalid visualization packet has been trashed.");
            gdk_threads_leave();
        }
    }
    //visualization rate
    else if(!strcmp(args[0], commands[5]))
    {
        if(size == 2)
        {
            project->iosetup.visualization_rate = atoi(args[1]);

            if(project->iosetup.visualization_rate < 1)
                project->iosetup.visualization_rate = 1;
            else if(project->iosetup.visualization_rate > 253)
                project->iosetup.visualization_rate = 253;
        }
        else
        {
            gdk_threads_enter();
            warning_dialog(project->home.window, "\nAn invalid visualization rate packet has been trashed.");
            gdk_threads_leave();
        }
    }
    //add task
    else if(!strcmp(args[0], commands[6]))
    {
        if(size == 12)
        {
            if((device = getDeviceByName(project->devices, args[1], &index)) != NULL)
            {
                gushort relay = (gushort)atoi(args[2]);
                if(relay > 0 && relay < 9)
                {
                    addTask(&device->element.tasks[relay-1], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11]);

                    if(project->schedule.flag)
                    {
                        gdk_threads_enter();
                        gtk_widget_destroy(project->schedule.listview);
                        create_listview(project, &device->element, relay);
                        gtk_widget_show_all(project->schedule.dialog);
                        gdk_threads_leave();
                    }
                }
            }
        }
        else
        {
            gdk_threads_enter();
            warning_dialog(project->home.window, "\nAn invalid add task packet has been trashed.");
            gdk_threads_leave();
        }
    }
    //del task
    else if(!strcmp(args[0], commands[7]))
    {
        if(size == 4)
        {
            if((device = getDeviceByName(project->devices, args[1], &index)) != NULL)
            {
                gushort relay = (gushort)atoi(args[2]);

                if(relay > 0 && relay < 9)
                {
                    deleteTaskByName(&device->element.tasks[relay-1], args[3]);

                    if(project->schedule.flag)
                    {
                        gdk_threads_enter();
                        gtk_widget_destroy(project->schedule.listview);
                        create_listview(project, &device->element, relay);
                        gtk_widget_show_all(project->schedule.dialog);
                        gdk_threads_leave();
                    }
                }
            }
        }
        else
        {
            gdk_threads_enter();
            warning_dialog(project->home.window, "\nAn invalid del task packet has been trashed.");
            gdk_threads_leave();
        }
    }
    //modify task
    else if(!strcmp(args[0], commands[8]))
    {
        if(size == 13)
        {
            if((device = getDeviceByName(project->devices, args[1], &index)) != NULL)
            {
                gushort relay = (gushort)atoi(args[2]);

                if(relay > 0 && relay < 9)
                {
                    Tasks *task = getTaskByName(device->element.tasks[relay-1], args[3]);
                    if(task != NULL)
                    {
                        modifyTask(&task, args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12]);

                        if(project->schedule.flag)
                        {
                            gdk_threads_enter();
                            gtk_widget_destroy(project->schedule.listview);
                            create_listview(project, &device->element, relay);
                            gtk_widget_show_all(project->schedule.dialog);
                            gdk_threads_leave();
                        }
                    }
                }
            }
        }
        else
        {
            gdk_threads_enter();
            warning_dialog(project->home.window, "\nAn invalid modify task packet has been trashed.");
            gdk_threads_leave();
        }
    }
    //error
    else if(!strcmp(args[0], commands[9]))
    {
        char buffer[SOCKET_BUFFER];

        if(size > 1)
        {
            buffer[0] = '\0';

            for(i=1 ; i < size ; i++)
                strcat(buffer, args[i]);
        }
        else
            strcpy(buffer, "\nInvalid packet error.");

        gdk_threads_enter();
        warning_dialog(project->home.window, buffer);
        gdk_threads_leave();
    }
    //no device
    else if(!strcmp(args[0], commands[10]))
    {
        if(size == 1)
        {
            GtkWidget   *table;
            GtkWidget   *label, *label2;

            gdk_threads_enter();

            label = gtk_label_new("No Device");
            table = gtk_table_new(5, 2, TRUE);
            label2 = gtk_label_new("No device has been found, please create a new one in Settings > Add Device.");
            gtk_table_attach(GTK_TABLE(table),
                            label2,
                            0, 4, 1, 3,
                            GTK_EXPAND | GTK_FILL,
                            GTK_EXPAND | GTK_FILL,
                            0, 0);

            /* Page Integration */
            gtk_notebook_append_page(GTK_NOTEBOOK(project->home.notebook), table, label);
            gtk_widget_show_all(project->home.window);
            project->adddevice.nodevice = 1;

            gdk_threads_leave();
        }
    }
    //Devices
    else if((device = getDeviceByName(project->devices, args[0], &index)) != NULL)
    {
        if(size == 2)
        {
            if(!strcmp(args[1], "disconnected"))
            {
                gushort i;
                GtkWidget *widget;
                gchar buffer[128];

                device->element.disconnected = TRUE;

                gdk_threads_enter();

                //Update Home
                for(i = 0; i < 10; i++)
                    gtk_widget_destroy(device->element.frame[i]);
                widget = gtk_label_new("Your device is unreachable, it may be due to a power failure or misconfiguration of the device.");
                gtk_table_attach(GTK_TABLE(device->element.table),
                                 widget,
                                 0, 4, 1, 2,
                                 GTK_EXPAND | GTK_FILL,
                                 GTK_EXPAND | GTK_FILL,
                                 0, 0);
                widget = gtk_button_new_with_label("Refresh");
                g_object_set_data(G_OBJECT(widget), "device", (gpointer) device);
                g_signal_connect(G_OBJECT(widget),
                                 "clicked",
                                 G_CALLBACK(refresh_configuration),
                                 project);
                gtk_table_attach(GTK_TABLE(device->element.table),
                                 widget,
                                 1, 2, 2, 3,
                                 GTK_EXPAND | GTK_FILL,
                                 GTK_EXPAND | GTK_FILL,
                                 0, 0);
                widget = gtk_button_new_with_label("Change Configuration");
                g_object_set_data(G_OBJECT(widget), "device", (gpointer) device);
                g_signal_connect(G_OBJECT(widget),
                                 "clicked",
                                 G_CALLBACK(create_configuration),
                                 project);
                gtk_table_attach(GTK_TABLE(device->element.table),
                                 widget,
                                 2, 3, 2, 3,
                                 GTK_EXPAND | GTK_FILL,
                                 GTK_EXPAND | GTK_FILL,
                                 0, 0);
                gtk_widget_show_all(device->element.table);

                //Update Visualization
                if(project->visualization.window != NULL)
                {
                    gtk_widget_destroy(project->visualization.table[index]);

                    project->visualization.table[index] = gtk_table_new(1, 1, TRUE);
                    gtk_container_add(GTK_CONTAINER(project->visualization.frame[index]), GTK_WIDGET(project->visualization.table[index]));

                    strcpy(buffer, "Device is unreachable.");
                    project->visualization.label[index][0] = gtk_label_new(buffer);
                    gtk_table_attach(GTK_TABLE(project->visualization.table[index]),
                                 project->visualization.label[index][0],
                                 0, 1, 0, 1,
                                 GTK_EXPAND | GTK_FILL,
                                 GTK_EXPAND | GTK_FILL,
                                 0, 0);

                    gtk_widget_show_all(project->visualization.table[index]);
                }

                sprintf(buffer, "\nDevice %s disconnected.", args[0]);
                warning_dialog(project->home.window, buffer);
                gdk_threads_leave();
            }
        }
        else if(size == 3)
        {
            device->element.disconnected = FALSE;

            if(!strcmp(args[2], "failed"))
            {
                gchar *buffer = malloc(128*sizeof(gchar));
                sprintf(buffer, "\nUnable to execute command %s on device %s.", args[1], args[0]);

                gdk_threads_enter();
                warning_dialog(project->home.window, buffer);
                gdk_threads_leave();

                free(buffer);
            }
            else
            {
                for(i=0 ; i < 8 ; i++)
                {
                    char buffer[ENTRY_BUFFER];

                    ///Relays
                    sprintf(buffer, "relay%hu", i+1);
                    if(!strcmp(args[1], buffer))
                    {
                        gdk_threads_enter();
                        device->element.relays_state[i] = (atoi(args[2]) == 1) ? TRUE : FALSE;
                        gtk_led_set_state(device->element.relays_led[i], device->element.relays_state[i]);
                        gtk_widget_queue_draw(GTK_WIDGET(device->element.relays_led[i]));
                        gdk_threads_leave();
                        break;
                    }

                    ///Pulse
                    sprintf(buffer, "pulse%hu", i+1);
                    if(!strcmp(args[1], buffer))
                    {
                        g_object_set_data(G_OBJECT(device->element.relays_led[i]), "duration", GUINT_TO_POINTER(device->element.pulse_duration[i]));
                        g_object_set_data(G_OBJECT(device->element.relays_led[i]), "state", GUINT_TO_POINTER(device->element.relays_state[i]));
                        gdk_threads_enter();
                        g_thread_create((GThreadFunc)led_pulse_thread, device->element.relays_led[i], FALSE, NULL);
                        gdk_threads_leave();
                        break;
                    }

                    ///IO setup
                    //relay description
                    sprintf(buffer, "relay%hudescription", i+1);
                    if(!strcmp(args[1], buffer))
                    {
                        gdk_threads_enter();
                        strcpy(device->element.relays_name[i], args[2]);
                        gtk_frame_set_label(GTK_FRAME(device->element.frame[i]), device->element.relays_name[i]);
                        gdk_threads_leave();
                        break;
                    }

                    //relay pulse Duration
                    sprintf(buffer, "relay%huPulseDuration", i+1);
                    if(!strcmp(args[1], buffer))
                    {
                        gdk_threads_enter();
                        device->element.pulse_duration[i] = atoi(args[2]);
                        gdk_threads_leave();
                        break;
                    }
                }

                ///Network setup
                //DHCP Config
                if(!strcmp(args[1], snmpCommands[0]))
                    device->element.dhcp_config = atoi(args[2]);
                //Device IP Address
                else if(!strcmp(args[1], snmpCommands[1]))
                    strcpy(device->element.ip, args[2]);
                //Subnet Mask
                else if(!strcmp(args[1], snmpCommands[2]))
                    strcpy(device->element.subnet_mask, args[2]);
                //Gateway
                else if(!strcmp(args[1], snmpCommands[3]))
                    strcpy(device->element.gateway, args[2]);
                //MAC Address
                else if(!strcmp(args[1], snmpCommands[4]))
                    strcpy(device->element.mac_addr, args[2]);

                ///SNMP setup
                //SNMP trap enable
                else if(!strcmp(args[1], snmpCommands[5]))
                    device->element.trap_enabled = atoi(args[2]);
                //trap IP
                else if(!strcmp(args[1], snmpCommands[6]))
                    strcpy(device->element.trap_ip, args[2]);
                //trap community
                else if(!strcmp(args[1], snmpCommands[7]))
                    strcpy(device->element.trap_community, args[2]);

                ///All relays
                //all on
                else if(!strcmp(args[1], snmpCommands[8]))
                {
                    for(i = 0 ; i < 8 ; i++)
                    {
                        gdk_threads_enter();
                        device->element.relays_state[i] = TRUE;
                        gtk_led_set_state(device->element.relays_led[i], device->element.relays_state[i]);
                        gtk_widget_queue_draw(GTK_WIDGET(device->element.relays_led[i]));
                        gdk_threads_leave();
                    }
                }
                //all off
                else if(!strcmp(args[1], snmpCommands[9]))
                {
                    for(i = 0 ; i < 8 ; i++)
                    {
                        gdk_threads_enter();
                        device->element.relays_state[i] = FALSE;
                        gtk_led_set_state(device->element.relays_led[i], device->element.relays_state[i]);
                        gtk_widget_queue_draw(GTK_WIDGET(device->element.relays_led[i]));
                        gdk_threads_leave();
                    }
                }
                //all pulse
                else if(!strcmp(args[1], snmpCommands[10]))
                {
                    for(i = 0 ; i < 8 ; i++)
                    {
                        g_object_set_data(G_OBJECT(device->element.relays_led[i]), "duration", GINT_TO_POINTER(device->element.pulse_duration[i]));
                        g_object_set_data(G_OBJECT(device->element.relays_led[i]), "state", GUINT_TO_POINTER(device->element.relays_state[i]));
                        gdk_threads_enter();
                        g_thread_create((GThreadFunc)led_pulse_thread, device->element.relays_led[i], FALSE, NULL);
                        gdk_threads_leave();
                    }
                }
                ///Digital input
                else if(!strcmp(args[1], snmpCommands[11]))
                    device->element.digital_input = (!strcmp(args[2], "1")) ? TRUE : FALSE;
                ///Product Infos
                //name
                else if(!strcmp(args[1], snmpCommands[12]))
                    strcpy(device->element.device_name, args[2]);
                //version
                else if(!strcmp(args[1], snmpCommands[13]))
                    strcpy(device->element.version, args[2]);
                //date
                else if(!strcmp(args[1], snmpCommands[14]))
                    strcpy(device->element.date, args[2]);
            }
        }
        else
        {
            char buffer[SOCKET_BUFFER];
            sprintf(buffer, "\nError in device packet: %s", packet);

            gdk_threads_enter();
            warning_dialog(project->home.window, buffer);
            gdk_threads_leave();
        }
    }
    else
    {
        char buffer[SOCKET_BUFFER];
        sprintf(buffer, "\nUnknown packet: %s", packet);

        gdk_threads_enter();
        warning_dialog(project->home.window, buffer);
        gdk_threads_leave();
    }

    for(i = 0; i < size; i++)
        free(args[i]);
    free(args);
}
