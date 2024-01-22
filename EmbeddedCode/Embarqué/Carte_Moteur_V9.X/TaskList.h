#ifndef TASKLIST_H
#define	TASKLIST_H

typedef struct TaskStruct {

    struct {
        unsigned char payload[30];
        int payloadLength;
    };
} Task;

#define MAX_TASK_LIST_LENGTH 30
typedef struct TaskListStruct {
    struct {
        volatile Task tasks[MAX_TASK_LIST_LENGTH];
        volatile int writeIndex;
        volatile int readIndex;
        volatile int taskListSize;
        volatile Task currentTask;
    };
} TaskList;

typedef enum TaskOptions TaskOptions;
enum TaskOptions {
    TaskOptions_None = 0,
    SetAsNextMove = 1,
    ReplaceAndClearAll = 2,
    SetAsNextAction = 3,
};

typedef enum TaskType TaskType;
enum TaskType
{
    TaskType_GOTO = 0,
    TaskType_AVANCE = 1,
    TaskType_RECULE = 2,
    TaskType_TOURNE = 3,
    TaskType_ARRET_ASSERVI = 4,
    TaskType_CIRCLE_ARC = 5,
    TaskType_SET_ORIENTATION = 6,
    TaskType_ARRET_NON_ASSERVI = 7,
    TaskType_EVITEMENT_VITESSE = 8,
    TaskType_COLLISION_AVANT = 9,
    TaskType_COLLISION_ARRIERE = 10
};

void InitTaskList(TaskList* tasklist);
void Put(TaskList* tasklist, Task task);
void PutInFirst(TaskList* tasklist, Task task);
void PutCurrentTaskInFirst(TaskList* tasklist);
Task Get(TaskList* tasklist);
int GetNumberOfRemainingTasks(TaskList* taskList);
int GetRemainingPlacesForTasks(TaskList* taskList);

#endif	/* TASKLIST_H */

