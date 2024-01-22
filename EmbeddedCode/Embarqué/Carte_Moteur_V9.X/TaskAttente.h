/* 
 * File:   TaskAttente.h
 * Author: E105-POSTE4
 *
 * Created on 8 mars 2017, 15:42
 */

#ifndef TASKATTENTE_H
#define	TASKATTENTE_H

#define STATE_TASK_ATTENTE_INIT 0
#define STATE_TASK_ATTENTE_FINISHED 1

void TaskAttenteLoop(void);
unsigned char TaskAttenteIsFinished();
void TaskAttenteInit();

#endif	/* TASKATTENTE_H */

