
#include "TaskList.h"

void InitTaskList(TaskList* tasklist)
{
    tasklist->taskListSize = MAX_TASK_LIST_LENGTH;
    tasklist->writeIndex = 0;
    tasklist->readIndex = 0;
}


void Put(TaskList* tasklist, Task task) {
    tasklist->tasks[tasklist->writeIndex++] = task;
    if (tasklist->writeIndex >= tasklist->taskListSize)
        tasklist->writeIndex = 0;
}

void PutInFirst(TaskList* tasklist, Task task) {
    if(tasklist->readIndex>0)
        tasklist->readIndex--;
    else
        tasklist->readIndex = tasklist->taskListSize - 1;        
    tasklist->tasks[tasklist->readIndex] = task;
}

void PutCurrentTaskInFirst(TaskList* tasklist)
{
    PutInFirst(tasklist, tasklist->currentTask);
}

Task Get(TaskList* tasklist) {
    tasklist->currentTask = tasklist->tasks[tasklist->readIndex++];
    if (tasklist->readIndex >= tasklist->taskListSize)
        tasklist->readIndex = 0;
    return tasklist->currentTask;
}

int GetNumberOfRemainingTasks(TaskList* taskList) {
    if (taskList->writeIndex >= taskList->readIndex)
        return taskList->writeIndex - taskList->readIndex;
    else
        return taskList->taskListSize - (taskList->readIndex - taskList->writeIndex);
}

int GetRemainingPlacesForTasks(TaskList* taskList) {
    return taskList->taskListSize - GetNumberOfRemainingTasks(taskList);
}

