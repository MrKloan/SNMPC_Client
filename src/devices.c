#include "includes.h"

void addDevice(Devices **devices, const gchar *name, const gchar *ip, const gchar *cm_public, const gchar *cm_private)
{
    Devices *last = getLastDevice(devices);
    Devices *temp;
    gushort i;

    temp = malloc(sizeof(Devices));

    strcpy(temp->element.name, name);
    strcpy(temp->element.ip, ip);
    strcpy(temp->element.communities[0], cm_public);
    strcpy(temp->element.communities[1], cm_private);

    for(i=0 ; i < 8 ; i++)
        temp->element.tasks[i] = NULL;

    temp->next = NULL;

    if(last == NULL)
        *devices = temp;
    else
        last->next = temp;
}

/**
 * Renvoie un pointeur sur le dernier élément alloué de la liste chaînée.
 */
Devices *getLastDevice(Devices **devices)
{
    if(*devices != NULL)
    {
        if((*devices)->next != NULL)
            return getLastDevice(&(*devices)->next);
        else
            return *devices;
    }
    else
        return NULL;
}

/**
 * Renvoie un pointeur sur l'élément de liste chaînée situé à l'indice donné.
 */
Devices *getDeviceByIndex(Devices *devices, gushort index)
{
    gushort i;

    for(i=0 ; i < index ; i++)
        devices = devices->next;

    return devices;
}

Devices *getDeviceByName(Devices *devices, const gchar *name, gshort *index)
{
    gushort i = 0;

    while(devices != NULL)
    {
        if(strcmp(devices->element.name, name) == 0)
        {
            *index = i;
            return devices;
        }
        devices = devices->next;
        i++;
    }

    *index = -1;
    return NULL;
}

gushort countDevices(Devices *devices)
{
    gushort i = 0;

    while(devices != NULL)
    {
        i++;
        devices = devices->next;
    }

    return i;
}

void delDeviceByIndex(Devices **devices, gushort index)
{
    if(*devices != NULL)
    {
        if(index != 0)
        {
            Devices *prev = getDeviceByIndex(*devices, index-1);
            Devices *current = getDeviceByIndex(*devices, index);

            prev->next = current->next;
            free(current);
        }
        else
        {
            Devices *current = getDeviceByIndex(*devices, index);
            *devices = current->next;
            free(current);
        }
    }

}

/**
 * Libère la mémoire allouée pour la liste chaînée d'utilisateurs.
 */
void freeDevices(Devices **devices)
{
    while(*devices != NULL)
    {
        Devices *temp = (*devices)->next;
        free(*devices);
        *devices = temp;
    }
}
