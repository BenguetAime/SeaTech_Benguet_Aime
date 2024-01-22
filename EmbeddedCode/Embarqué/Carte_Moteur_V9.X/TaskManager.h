#ifndef TASKMANAGER_H
#define	TASKMANAGER_H


typedef struct {
    union {
        struct {
            unsigned char stateNumber;
            unsigned char initialStateActionDone;
            unsigned char finalStateActionDone;
        };
    };
} TaskState;

//#define TASK_DEBUT_MATCH 0
#define TASK_MODE_AUTOMATIQUE 1
#define TASK_EVITEMENT 4
#define TASK_DEPLACEMENT_CHAINE 5
#define TASK_AVALAGE 6
#define TASK_DEMO 7
#define TASK_COLLISION 8
#define TASK_GOTO 10
#define TASK_ATTENTE 11
#define TASK_EVENT 12
#define TASK_ANTI_COLLISION 13
#define TASK_RECALAGE 14
#define TASK_ARRET_ASSERVI 15
#define TASK_EVITEMENT_VITESSE 16
#define TASK_COLLISION_AVANT 17
#define TASK_COLLISION_ARRIERE 18



#endif	/* TASKMANAGER_H */

