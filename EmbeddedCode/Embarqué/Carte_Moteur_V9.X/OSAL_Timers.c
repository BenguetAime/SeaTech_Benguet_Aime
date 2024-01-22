/**************************************************************************************************
  Filename:       OSAL_Timers.c
  Revised:        $Date: 2010-09-17 16:25:30 -0700 (Fri, 17 Sep 2010) $
  Revision:       $Revision: 23835 $

  Description:    OSAL Timer definition and manipulation functions.


  Copyright 2004-2009 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "define.h"
#include "OSAL_Timers.h"
#include "OSAL.h"
#ifdef USE_OSAL
#include "UTLN_OSAL.h"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
  void   *next;
  unsigned short timeout;
  unsigned short event_flag;
  unsigned char  task_id;
  unsigned short reloadTimeout;
} OSALTimerRec_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */
OSALTimerRec_t *timerHead;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
// Milliseconds since last reboot
static unsigned long OSAL_systemClock;

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
OSALTimerRec_t  *OSALAddTimer( unsigned char task_id, unsigned short event_flag, unsigned short timeout );
OSALTimerRec_t *OSALFindTimer( unsigned char task_id, unsigned short event_flag );
void OSALDeleteTimer( OSALTimerRec_t *rmTimer );

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      OSALTimerInit
 *
 * @brief   Initialization for the OSAL Timer System.
 *
 * @param   none
 *
 * @return
 */
void OSALTimerInit( void )
{
  OSAL_systemClock = 0;
}

/*********************************************************************
 * @fn      OSALAddTimer
 *
 * @brief   Add a timer to the timer list.
 *          Ints must be disabled.
 *
 * @param   task_id
 * @param   event_flag
 * @param   timeout
 *
 * @return  OSALTimerRec_t * - pointer to newly created timer
 */
OSALTimerRec_t * OSALAddTimer( unsigned char task_id, unsigned short event_flag, unsigned short timeout )
{
  OSALTimerRec_t *newTimer;
  OSALTimerRec_t *srchTimer;

  // Look for an existing timer first
  newTimer = OSALFindTimer( task_id, event_flag );
  if ( newTimer )
  {
    // Timer is found - update it.
    newTimer->timeout = timeout;

    return ( newTimer );
  }
  else
  {
    // New Timer
    newTimer = OSAL_mem_alloc( sizeof( OSALTimerRec_t ) );

    if ( newTimer )
    {
      // Fill in new timer
      newTimer->task_id = task_id;
      newTimer->event_flag = event_flag;
      newTimer->timeout = timeout;
      newTimer->next = (void *)NULL;
      newTimer->reloadTimeout = 0;

      // Does the timer list already exist
      if ( timerHead == NULL )
      {
        // Start task list
        timerHead = newTimer;
      }
      else
      {
        // Add it to the end of the timer list
        srchTimer = timerHead;

        // Stop at the last record
        while ( srchTimer->next )
          srchTimer = srchTimer->next;

        // Add to the list
        srchTimer->next = newTimer;
      }

      return ( newTimer );
    }
    else
      return ( (OSALTimerRec_t *)NULL );
  }
}

/*********************************************************************
 * @fn      OSALFindTimer
 *
 * @brief   Find a timer in a timer list.
 *          Ints must be disabled.
 *
 * @param   task_id
 * @param   event_flag
 *
 * @return  OSALTimerRec_t *
 */
OSALTimerRec_t *OSALFindTimer( unsigned char task_id, unsigned short event_flag )
{
  OSALTimerRec_t *srchTimer;

  // Head of the timer list
  srchTimer = timerHead;

  // Stop when found or at the end
  while ( srchTimer )
  {
    if ( srchTimer->event_flag == event_flag &&
         srchTimer->task_id == task_id )
      break;

    // Not this one, check another
    srchTimer = srchTimer->next;
  }

  return ( srchTimer );
}

/*********************************************************************
 * @fn      OSALDeleteTimer
 *
 * @brief   Delete a timer from a timer list.
 *
 * @param   table
 * @param   rmTimer
 *
 * @return  none
 */
void OSALDeleteTimer( OSALTimerRec_t *rmTimer )
{
  // Does the timer list really exist
  if ( rmTimer )
  {
    // Clear the event flag and OSALTimerUpdate() will delete
    // the timer from the list.
    rmTimer->event_flag = 0;
  }
}

/*********************************************************************
 * @fn      OSAL_start_timerEx
 *
 * @brief
 *
 *   This function is called to start a timer to expire in n mSecs.
 *   When the timer expires, the calling task will get the specified event.
 *
 * @param   unsigned char taskID - task id to set timer for
 * @param   unsigned short event_id - event to be notified with
 * @param   UNINT16 timeout_value - in milliseconds.
 *
 * @return  SUCCESS, or NO_TIMER_AVAIL.
 */
unsigned char OSAL_start_timerEx( unsigned char taskID, unsigned short event_id, unsigned short timeout_value )
{
  OSALTimerRec_t *newTimer;

  HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.

  // Add timer
  newTimer = OSALAddTimer( taskID, event_id, timeout_value );

  HAL_EXIT_CRITICAL_SECTION(  );   // Re-enable interrupts.

  return ( (newTimer != NULL) ? SUCCESS : NO_TIMER_AVAIL );
}

/*********************************************************************
 * @fn      OSAL_start_reload_timer
 *
 * @brief
 *
 *   This function is called to start a timer to expire in n mSecs.
 *   When the timer expires, the calling task will get the specified event
 *   and the timer will be reloaded with the timeout value.
 *
 * @param   unsigned char taskID - task id to set timer for
 * @param   unsigned short event_id - event to be notified with
 * @param   UNINT16 timeout_value - in milliseconds.
 *
 * @return  SUCCESS, or NO_TIMER_AVAIL.
 */
unsigned char OSAL_start_reload_timer( unsigned char taskID, unsigned short event_id, unsigned short timeout_value )
{
  OSALTimerRec_t *newTimer;

  HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.

  // Add timer
  newTimer = OSALAddTimer( taskID, event_id, timeout_value );
  if ( newTimer )
  {
    // Load the reload timeout value
    newTimer->reloadTimeout = timeout_value;
  }

  HAL_EXIT_CRITICAL_SECTION(  );   // Re-enable interrupts.

  return ( (newTimer != NULL) ? SUCCESS : NO_TIMER_AVAIL );
}

/*********************************************************************
 * @fn      OSAL_stop_timerEx
 *
 * @brief
 *
 *   This function is called to stop a timer that has already been started.
 *   If ZSUCCESS, the function will cancel the timer and prevent the event
 *   associated with the timer from being set for the calling task.
 *
 * @param   unsigned char task_id - task id of timer to stop
 * @param   unsigned short event_id - identifier of the timer that is to be stopped
 *
 * @return  SUCCESS or INVALID_EVENT_ID
 */
unsigned char OSAL_stop_timerEx( unsigned char task_id, unsigned short event_id )
{
  OSALTimerRec_t *foundTimer;

  HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.

  // Find the timer to stop
  foundTimer = OSALFindTimer( task_id, event_id );
  if ( foundTimer )
  {
    OSALDeleteTimer( foundTimer );
  }

  HAL_EXIT_CRITICAL_SECTION(  );   // Re-enable interrupts.

  return ( (foundTimer != NULL) ? SUCCESS : INVALID_EVENT_ID );
}

/*********************************************************************
 * @fn      OSAL_get_timeoutEx
 *
 * @brief
 *
 * @param   unsigned char task_id - task id of timer to check
 * @param   unsigned short event_id - identifier of timer to be checked
 *
 * @return  Return the timer's tick count if found, zero otherwise.
 */
unsigned short OSAL_get_timeoutEx( unsigned char task_id, unsigned short event_id )
{
  unsigned short rtrn = 0;
  OSALTimerRec_t *tmr;

  HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.

  tmr = OSALFindTimer( task_id, event_id );

  if ( tmr )
  {
    rtrn = tmr->timeout;
  }

  HAL_EXIT_CRITICAL_SECTION(  );   // Re-enable interrupts.

  return rtrn;
}

/*********************************************************************
 * @fn      OSAL_timer_num_active
 *
 * @brief
 *
 *   This function counts the number of active timers.
 *
 * @return  unsigned char - number of timers
 */
unsigned char OSAL_timer_num_active( void )
{
  unsigned char num_timers = 0;
  OSALTimerRec_t *srchTimer;

  HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.

  // Head of the timer list
  srchTimer = timerHead;

  // Count timers in the list
  while ( srchTimer != NULL )
  {
    num_timers++;
    srchTimer = srchTimer->next;
  }

  HAL_EXIT_CRITICAL_SECTION(  );   // Re-enable interrupts.

  return num_timers;
}

/*********************************************************************
 * @fn      OSALTimerUpdate
 *
 * @brief   Update the timer structures for a timer tick.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
void OSALTimerUpdate( unsigned short updateTime )
{
  OSALTimerRec_t *srchTimer;
  OSALTimerRec_t *prevTimer;

  HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.
  // Update the system time
  OSAL_systemClock += updateTime;
  HAL_EXIT_CRITICAL_SECTION(  );   // Re-enable interrupts.

  // Look for open timer slot
  if ( timerHead != NULL )
  {
    // Add it to the end of the timer list
    srchTimer = timerHead;
    prevTimer = (void *)NULL;

    // Look for open timer slot
    while ( srchTimer )
    {
      OSALTimerRec_t *freeTimer = NULL;
     
      HAL_ENTER_CRITICAL_SECTION(  );  // Hold off interrupts.
      
      if (srchTimer->timeout <= updateTime)
      {
        srchTimer->timeout = 0;
      }
      else
      {
        srchTimer->timeout = srchTimer->timeout - updateTime;
      }
      
      // Check for reloading
      if ( (srchTimer->timeout == 0) && (srchTimer->reloadTimeout) && (srchTimer->event_flag) )
      {
        // Notify the task of a timeout
        OSAL_set_event( srchTimer->task_id, srchTimer->event_flag );
        // Reload the timer timeout value
        srchTimer->timeout = srchTimer->reloadTimeout;
      }
      
      // When timeout or delete (event_flag == 0)
      if ( srchTimer->timeout == 0 || srchTimer->event_flag == 0 )
      {
        // Take out of list
        if ( prevTimer == NULL )
          timerHead = srchTimer->next;
        else
          prevTimer->next = srchTimer->next;

        // Setup to free memory
        freeTimer = srchTimer;

        // Next
        srchTimer = srchTimer->next;
      }
      else
      {
        // Get next
        prevTimer = srchTimer;
        srchTimer = srchTimer->next;
      }
      
      HAL_EXIT_CRITICAL_SECTION(  );   // Re-enable interrupts.
      
      if ( freeTimer )
      {
        if ( freeTimer->timeout == 0 )
        {
          OSAL_set_event( freeTimer->task_id, freeTimer->event_flag );
        }
        OSAL_mem_free( freeTimer );
      }
    }
  }
}

#ifdef POWER_SAVING
/*********************************************************************
 * @fn      OSAL_adjust_timers
 *
 * @brief   Update the timer structures for elapsed ticks.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
void OSAL_adjust_timers( void )
{
  unsigned short eTime;

  if ( timerHead != NULL )
  {
    // Compute elapsed time (msec)
    eTime = TimerElapsed() /  TICK_COUNT;

    if ( eTime )
      OSALTimerUpdate( eTime );
  }
}

/*********************************************************************
 * @fn      OSAL_next_timeout
 *
 * @brief
 *
 *   Search timer table to return the lowest timeout value. If the
 *   timer list is empty, then the returned timeout will be zero.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
unsigned short OSAL_next_timeout( void )
{
  unsigned short nextTimeout;
  OSALTimerRec_t *srchTimer;

  if ( timerHead != NULL )
  {
    // Head of the timer list
    srchTimer = timerHead;
    nextTimeout = OSAL_TIMERS_MAX_TIMEOUT;

    // Look for the next timeout timer
    while ( srchTimer != NULL )
    {
      if (srchTimer->timeout < nextTimeout)
      {
        nextTimeout = srchTimer->timeout;
      }
      // Check next timer
      srchTimer = srchTimer->next;
    }
  }
  else
  {
    // No timers
    nextTimeout = 0;
  }

  return ( nextTimeout );
}
#endif // POWER_SAVING

/*********************************************************************
 * @fn      OSAL_GetSystemClock()
 *
 * @brief   Read the local system clock.
 *
 * @param   none
 *
 * @return  local clock in milliseconds
 */
unsigned long OSAL_GetSystemClock( void )
{
  return ( OSAL_systemClock );
}

#endif
/*********************************************************************
*********************************************************************/
