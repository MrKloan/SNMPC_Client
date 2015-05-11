#include "includes.h"

void addTask(Tasks **tasks, const char *name, const char *enabled, const char *repeat, const char *date, const char *weeks, const char *days, const char *type, const char *start, const char *end)
{
    Tasks *last = getLastTask(tasks);
    Tasks *temp;
    char **args;
    unsigned short i, size = 0;

    temp = malloc(sizeof(Tasks));

    strcpy(temp->element.name_val, name);
    temp->element.enabled_val = (gushort)atoi(enabled);
    temp->element.repeat_val = (gushort)atoi(repeat);
    strcpy(temp->element.date_val, date);
    temp->element.weeks_val = (gushort)atoi(weeks);
    temp->element.type_val = (gushort)atoi(type);

    args = explode((char*)days, ",", &size);
    if(size == 7)
    {
        for(i=0 ; i < 7 ; i++)
        {
            temp->element.days_val[i] = (gushort)atoi(args[i]);
            free(args[i]);
        }
        free(args);
    }

    args = explode((char*)start, "h", &size);
    if(size == 2)
    {
        temp->element.hours_val[0] = (gushort)atoi(args[0]);
        temp->element.minutes_val[0] = (gushort)atoi(args[1]);

        free(args[0]);
        free(args[1]);
        free(args);
    }

    args = explode((char*)end, "h", &size);
    if(size == 2)
    {
        temp->element.hours_val[1] = (gushort)atoi(args[0]);
        temp->element.minutes_val[1] = (gushort)atoi(args[1]);

        free(args[0]);
        free(args[1]);
        free(args);
    }

    temp->next = NULL;

    if(last == NULL)
        *tasks = temp;
    else
        last->next = temp;
}

/**
 * Suppression de l'élément de liste chaînée situé à l'indice donné.
 */
void deleteTask(Tasks **tasks, unsigned short index)
{
    if(index > 0)
    {
        Tasks *prev = getTask(*tasks, index-1);
        Tasks *next = getTask(*tasks, index+1);

        free(getTask(*tasks, index));
        prev->next = next;
    }
    else
    {
        Tasks *next = (*tasks)->next;

        free(*tasks);
        *tasks = next;
    }
}

/**
 * Suppression de l'élément de liste chaînée situé à l'indice donné.
 */
void deleteTaskByName(Tasks **tasks, const char *name)
{
    Tasks *temp = *tasks;
    unsigned short index;

    for(index = 0; temp != NULL; index++, temp = temp->next)
    {
        if(strcmp(temp->element.name_val, name) == 0)
        {
            deleteTask(tasks, index);
            break;
        }
    }
}

void modifyTask(Tasks **task, const char *name, const char *enabled, const char *repeat, const char *date, const char *weeks, const char *days, const char *type, const char *start, const char *end)
{
    char **args;
    unsigned short i, size = 0;

    strcpy((*task)->element.name_val, name);
    (*task)->element.enabled_val = (gushort)atoi(enabled);
    (*task)->element.repeat_val = (gushort)atoi(repeat);
    strcpy((*task)->element.date_val, date);
    (*task)->element.weeks_val = (gushort)atoi(weeks);
    (*task)->element.type_val = (gushort)atoi(type);

    args = explode((char*)days, ",", &size);
    if(size == 7)
    {
        for(i=0 ; i < 7 ; i++)
        {
            (*task)->element.days_val[i] = (gushort)atoi(args[i]);
            free(args[i]);
        }
        free(args);
    }

    args = explode((char*)start, "h", &size);
    if(size == 2)
    {
        (*task)->element.hours_val[0] = (gushort)atoi(args[0]);
        (*task)->element.minutes_val[0] = (gushort)atoi(args[1]);

        free(args[0]);
        free(args[1]);
        free(args);
    }

    args = explode((char*)end, "h", &size);
    if(size == 2)
    {
        (*task)->element.hours_val[1] = (gushort)atoi(args[0]);
        (*task)->element.minutes_val[1] = (gushort)atoi(args[1]);

        free(args[0]);
        free(args[1]);
        free(args);
    }
}

/**
 * Renvoie un pointeur sur le dernier élément alloué de la liste chaînée.
 */
Tasks *getLastTask(Tasks **tasks)
{
    if(*tasks != NULL)
    {
        if((*tasks)->next != NULL)
            return getLastTask(&(*tasks)->next);
        else
            return *tasks;
    }
    else
        return NULL;
}

/**
 * Renvoie un pointeur sur l'élément de liste chaînée ayant le nom donné.
 */
Tasks *getTaskByName(Tasks *tasks, const char *name)
{
    while(tasks != NULL)
    {
        if(strcmp(tasks->element.name_val, name) == 0)
            return tasks;
        tasks = tasks->next;
    }

    return NULL;
}

/**
 * Renvoie un pointeur sur l'élément de liste chaînée situé à l'indice donné.
 */
Tasks *getTask(Tasks *tasks, unsigned short index)
{
    unsigned short i;

    for(i=0 ; i < index ; i++)
        tasks = tasks->next;

    return tasks;
}

gushort countTasks(Tasks *tasks)
{
    gushort i = 0;

    while(tasks != NULL)
    {
        i++;
        tasks = tasks->next;
    }

    return i;
}

/**
 * Libère la mémoire allouée pour la liste chaînée d'utilisateurs.
 */
void freeTasks(Tasks **tasks)
{
    while(*tasks != NULL)
    {
        Tasks *temp = (*tasks)->next;
        free(*tasks);
        *tasks = temp;
    }
}
