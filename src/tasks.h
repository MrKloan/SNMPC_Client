#ifndef TASKS_H_INCLUDED
#define TASKS_H_INCLUDED

void addTask(Tasks **, const char *, const char *, const char *, const char *, const char *, const char *, const char *, const char *, const char *);
void modifyTask(Tasks **, const char *, const char *, const char *, const char *, const char *, const char *, const char *, const char *, const char *);
void deleteTask(Tasks **, unsigned short);
void deleteTaskByName(Tasks **, const char *);

Tasks *getLastTask(Tasks **);
Tasks *getTaskByName(Tasks *, const char *);
Tasks *getTask(Tasks *, unsigned short);
gushort countTasks(Tasks *);
void freeTasks(Tasks **);

#endif // TASKS_H_INCLUDED
