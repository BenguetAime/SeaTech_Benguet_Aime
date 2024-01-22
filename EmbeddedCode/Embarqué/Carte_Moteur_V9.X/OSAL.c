/**************************************************************************************************
  Filename:       OSAL.c
  Revised:        $Date: 2011-05-27 09:03:48 -0700 (Fri, 27 May 2011) $
  Revision:       $Revision: 26134 $

  Description:    This API allows the software components in the Z-stack to be written
                  independently of the specifics of the operating system, kernel or tasking
                  environment (including control loops or connect-to-interrupt systems).


  Copyright 2004-2011 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License"). You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product. Other than for
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

#include <string.h>

#include "OSAL.h"
#include "OSAL_Memory.h"

#ifdef USE_OSAL
#define TASK_NO_TASK      0xFF
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Message Pool Definitions
OSAL_msg_q_t OSAL_qHead;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Index of active task
static uint8 activeTaskID = TASK_NO_TASK;

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */


/*********************************************************************
 * @fn      OSAL_strlen
 *
 * @brief
 *
 *   Calculates the length of a string.  The string must be null
 *   terminated.
 *
 * @param   char *pString - pointer to text string
 *
 * @return  int - number of characters
 */
int OSAL_strlen( char *pString )
{
  return (int)( strlen( pString ) );
}

/*********************************************************************
 * @fn      OSAL_memcpy
 *
 * @brief
 *
 *    memory copy.
 *
 *   Note: This function differs from the standard memcpy(), since
 *         it returns the pointer to the next destination uint8. The
 *         standard memcpy() returns the original destination address.
 *
 * @param   dst - destination address
 * @param   src - source address
 * @param   len - number of bytes to copy
 *
 * @return  pointer to end of destination buffer
 */
void *OSAL_memcpy( void *dst, const void *src, unsigned int len )
{
  uint8 *pDst;
  const uint8 *pSrc;

  pSrc = src;
  pDst = dst;

  while ( len-- )
    *pDst++ = *pSrc++;

  return ( pDst );
}

/*********************************************************************
 * @fn      OSAL_revmemcpy
 *
 * @brief    reverse memory copy.  Starts at the end of the
 *   source buffer, by taking the source address pointer and moving
 *   pointer ahead "len" bytes, then decrementing the pointer.
 *
 *   Note: This function differs from the standard memcpy(), since
 *         it returns the pointer to the next destination uint8. The
 *         standard memcpy() returns the original destination address.
 *
 * @param   dst - destination address
 * @param   src - source address
 * @param   len - number of bytes to copy
 *
 * @return  pointer to end of destination buffer
 */
void *OSAL_revmemcpy( void *dst, const void  *src, unsigned int len )
{
  uint8 *pDst;
  const uint8  *pSrc;

  pSrc = src;
  pSrc += (len-1);
  pDst = dst;

  while ( len-- )
    *pDst++ = *pSrc--;

  return ( pDst );
}

/*********************************************************************
 * @fn      OSAL_memdup
 *
 * @brief   Allocates a buffer [with OSAL_mem_alloc()] and copies
 *          the src buffer into the newly allocated space.
 *
 * @param   src - source address
 * @param   len - number of bytes to copy
 *
 * @return  pointer to the new allocated buffer, or NULL if
 *          allocation problem.
 */
void *OSAL_memdup( const void  *src, unsigned int len )
{
  uint8 *pDst;

  pDst = OSAL_mem_alloc( len );
  if ( pDst )
  {
    OSAL_memcpy( pDst, src, len );
  }

  return ( (void *)pDst );
}

/*********************************************************************
 * @fn      OSAL_memcmp
 *
 * @brief
 *
 *    memory compare.
 *
 * @param   src1 - source 1 addrexx
 * @param   src2 - source 2 address
 * @param   len - number of bytes to compare
 *
 * @return  TRUE - same, FALSE - different
 */
uint8 OSAL_memcmp( const void  *src1, const void  *src2, unsigned int len )
{
  const uint8  *pSrc1;
  const uint8  *pSrc2;

  pSrc1 = src1;
  pSrc2 = src2;

  while ( len-- )
  {
    if( *pSrc1++ != *pSrc2++ )
      return FALSE;
  }
  return TRUE;
}


/*********************************************************************
 * @fn      OSAL_memset
 *
 * @brief
 *
 *   Set memory buffer to value.
 *
 * @param   dest - pointer to buffer
 * @param   value - what to set each uint8 of the message
 * @param   size - how big
 *
 * @return  pointer to destination buffer
 */
void *OSAL_memset( void *dest, uint8 value, int len )
{
  return memset( dest, value, len );
}

/*********************************************************************
 * @fn      OSAL_build_uint16
 *
 * @brief
 *
 *   Build a uint16 out of 2 bytes (0 then 1).
 *
 * @param   swapped - 0 then 1
 *
 * @return  uint16
 */
uint16 OSAL_build_uint16( uint8 *swapped )
{
  return ( BUILD_UINT16( swapped[0], swapped[1] ) );
}

/*********************************************************************
 * @fn      OSAL_build_uint32
 *
 * @brief
 *
 *   Build a uint32 out of sequential bytes.
 *
 * @param   swapped - sequential bytes
 * @param   len - number of bytes in the uint8 array
 *
 * @return  uint32
 */
uint32 OSAL_build_uint32( uint8 *swapped, uint8 len )
{
  if ( len == 2 )
    return ( BUILD_UINT32( swapped[0], swapped[1], 0L, 0L ) );
  else if ( len == 3 )
    return ( BUILD_UINT32( swapped[0], swapped[1], swapped[2], 0L ) );
  else if ( len == 4 )
    return ( BUILD_UINT32( swapped[0], swapped[1], swapped[2], swapped[3] ) );
  else
    return ( (uint32)swapped[0] );
}

/*********************************************************************
 * API FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      OSAL_msg_allocate
 *
 * @brief
 *
 *    This function is called by a task to allocate a message buffer
 *    into which the task will encode the particular message it wishes
 *    to send.  This common buffer scheme is used to strictly limit the
 *    creation of message buffers within the system due to RAM size
 *    limitations on the microprocessor.   Note that all message buffers
 *    are a fixed size (at least initially).  The parameter len is kept
 *    in case a message pool with varying fixed message sizes is later
 *    created (for example, a pool of message buffers of size LARGE,
 *    MEDIUM and SMALL could be maintained and allocated based on request
 *    from the tasks).
 *
 *
 * @param   uint8 len  - wanted buffer length
 *
 *
 * @return  pointer to allocated buffer or NULL if allocation failed.
 */
uint8 * OSAL_msg_allocate( uint16 len )
{
  OSAL_msg_hdr_t *hdr;

  if ( len == 0 )
    return ( NULL );
  //On alloue un espace memoire de taille len, et on y ajoute la taille d'un header memoire(5 octets)
  hdr = (OSAL_msg_hdr_t *) OSAL_mem_alloc( (unsigned short)(len + sizeof( OSAL_msg_hdr_t )) );
  if ( hdr )
  {
    hdr->next = NULL;                       //On pointe le prochain header a NULL ce qui signifie que c'est le dernier element de la queue
    hdr->len = len;                         //La taille du buffer alloué
    hdr->dest_id = TASK_NO_TASK;            //La tache par defaut
    return ( (uint8 *) (hdr + 1) );         //On retourne le pointeur du buffer qui se situe a hdr +1 (header + taille d'un champ header).
  }
  else
    return ( NULL );                        //Si l'allocation a echoué on retourne un pointeur NULL
}

/*********************************************************************
 * @fn      OSAL_msg_deallocate
 *
 * @brief
 *
 *    This function is used to deallocate a message buffer. This function
 *    is called by a task (or processing element) after it has finished
 *    processing a received message.
 *
 *
 * @param   uint8 *msg_ptr - pointer to new message buffer
 *
 * @return  SUCCESS, INVALID_MSG_POINTER
 */
uint8 OSAL_msg_deallocate( uint8 *msg_ptr )
{
  uint8 *x;
  //Si le message n'est pas null
  if ( msg_ptr == NULL )
    return ( INVALID_MSG_POINTER );

  // On ne dealloue pas si le message fait encore parti d'une queue
  if ( OSAL_MSG_ID( msg_ptr ) != TASK_NO_TASK )
    return ( MSG_BUFFER_NOT_AVAIL );
  //On decale le pointeur afin de pointer sur le header et non sur le buffer
  x = (uint8 *)((uint8 *)msg_ptr - sizeof( OSAL_msg_hdr_t ));
  //On libere l'espace memoire
  OSAL_mem_free( (void *)x );           

  return ( SUCCESS );
}

/*********************************************************************
 * @fn      OSAL_msg_send
 *
 * @brief
 *
 *    This function is called by a task to send a command message to
 *    another task or processing element.  The sending_task field must
 *    refer to a valid task, since the task ID will be used
 *    for the response message.  This function will also set a message
 *    ready event in the destination tasks event list.
 *
 *
 * @param   uint8 destination task - Send msg to?  Task ID
 * @param   uint8 *msg_ptr - pointer to new message buffer
 * @param   uint8 len - length of data in message
 *
 * @return  SUCCESS, INVALID_TASK, INVALID_MSG_POINTER
 */
uint8 OSAL_msg_send( uint8 destination_task, uint8 *msg_ptr )
{
  if ( msg_ptr == NULL )
    return ( INVALID_MSG_POINTER );

  //On verifie que le message soit bien destiné a une tache existante
  if ( destination_task >= tasksCnt )
  {
    OSAL_msg_deallocate( msg_ptr );             //Si ce n'est pas le cas, on dealloue la memoire
    return ( INVALID_TASK );
  }

  // Check the message header
  if ( OSAL_MSG_NEXT( msg_ptr ) != NULL ||
       OSAL_MSG_ID( msg_ptr ) != TASK_NO_TASK )
  {
    OSAL_msg_deallocate( msg_ptr );     //On de-alloue le message
    return ( INVALID_MSG_POINTER );
  }

  OSAL_MSG_ID( msg_ptr ) = destination_task;

  // On ajoute le message a la queue (enqueue)
  OSAL_msg_enqueue( &OSAL_qHead, msg_ptr );

  // On signal a la tache qu'un message est en attente
  OSAL_set_event( destination_task, SYS_EVENT_MSG );

  return ( SUCCESS );
}

/*********************************************************************
 * @fn      OSAL_msg_receive
 *
 * @brief
 *
 *    This function is called by a task to retrieve a received command
 *    message. The calling task must deallocate the message buffer after
 *    processing the message using the OSAL_msg_deallocate() call.
 *
 * @param   uint8 task_id - receiving tasks ID
 *
 * @return  *uint8 - message information or NULL if no message
 */
uint8 *OSAL_msg_receive( uint8 task_id )
{
  OSAL_msg_hdr_t *listHdr;
  OSAL_msg_hdr_t *prevHdr = NULL;
  OSAL_msg_hdr_t *foundHdr = NULL;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION();

  // Point to the top of the queue
  listHdr = OSAL_qHead;

  // Look through the queue for a message that belongs to the asking task
  while ( listHdr != NULL )
  {
    if ( (listHdr - 1)->dest_id == task_id )
    {
      if ( foundHdr == NULL )
      {
        // Save the first one
        foundHdr = listHdr;
      }
      else
      {
        // Second msg found, stop looking
        break;
      }
    }
    if ( foundHdr == NULL )
    {
      prevHdr = listHdr;
    }
    listHdr = OSAL_MSG_NEXT( listHdr );
  }

  // Is there more than one?
  if ( listHdr != NULL )
  {
    // Yes, Signal the task that a message is waiting
    OSAL_set_event( task_id, SYS_EVENT_MSG );
  }
  else
  {
    // No more
    OSAL_clear_event( task_id, SYS_EVENT_MSG );
  }

  // Did we find a message?
  if ( foundHdr != NULL )
  {
    // Take out of the link list
    OSAL_msg_extract( &OSAL_qHead, foundHdr, prevHdr );
  }

  // Release interrupts
  HAL_EXIT_CRITICAL_SECTION();

  return ( (uint8*) foundHdr );
}

/**************************************************************************************************
 * @fn          OSAL_msg_find
 *
 * @brief       This function finds in place an OSAL message matching the task_id and event
 *              parameters.
 *
 * input parameters
 *
 * @param       task_id - The OSAL task id that the enqueued OSAL message must match.
 * @param       event - The OSAL event id that the enqueued OSAL message must match.
 *
 * output parameters
 *
 * None.
 *
 * @return      NULL if no match, otherwise an in place pointer to the matching OSAL message.
 **************************************************************************************************
 */
OSAL_event_hdr_t *OSAL_msg_find(uint8 task_id, uint8 event)
{
  OSAL_msg_hdr_t *pHdr;             //Pointeur de message OSAL

  HAL_ENTER_CRITICAL_SECTION();  // Hold off interrupts.

  pHdr = OSAL_qHead;  // Point to the top of the queue.

  // Look through the queue for a message that matches the task_id and event parameters.
  while (pHdr != NULL)
  {
    if (((pHdr-1)->dest_id == task_id) && (((OSAL_event_hdr_t *)pHdr)->event == event))
    {
      break;
    }

    pHdr = OSAL_MSG_NEXT(pHdr);
  }

  HAL_EXIT_CRITICAL_SECTION();  // Release interrupts.

  return (OSAL_event_hdr_t *)pHdr;
}

/*********************************************************************
 * @fn      OSAL_msg_enqueue
 *
 * @brief
 *
 *    This function enqueues an OSAL message into an OSAL queue.
 *
 * @param   OSAL_msg_q_t *q_ptr - OSAL queue
 * @param   void *msg_ptr  - OSAL message
 *
 * @return  none
 */
void OSAL_msg_enqueue( OSAL_msg_q_t *q_ptr, void *msg_ptr )
{
  void *list;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION();

  OSAL_MSG_NEXT( msg_ptr ) = NULL;
  // If first message in queue
  if ( *q_ptr == NULL )
  {
    *q_ptr = msg_ptr;
  }
  else
  {
    // Find end of queue
    for ( list = *q_ptr; OSAL_MSG_NEXT( list ) != NULL; list = OSAL_MSG_NEXT( list ) );

    // Add message to end of queue
    OSAL_MSG_NEXT( list ) = msg_ptr;
  }

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION();
}

/*********************************************************************
 * @fn      OSAL_msg_dequeue
 *
 * @brief
 *
 *    This function dequeues an OSAL message from an OSAL queue.
 *
 * @param   OSAL_msg_q_t *q_ptr - OSAL queue
 *
 * @return  void * - pointer to OSAL message or NULL of queue is empty.
 */
void *OSAL_msg_dequeue( OSAL_msg_q_t *q_ptr )
{
  void *msg_ptr = NULL;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION();

  //Si la queue n'est pas vide
  if ( *q_ptr != NULL )
  {
    // Dequeue message
    msg_ptr = *q_ptr;                           //Le message pointe vers le premier element de la queue
    *q_ptr = OSAL_MSG_NEXT( msg_ptr );          //La queue pointe vers le prochain message(pn decale vers le prochain msg)
    OSAL_MSG_NEXT( msg_ptr ) = NULL;            //Le prochain message  est NULL
    OSAL_MSG_ID( msg_ptr ) = TASK_NO_TASK;      //Le message est destiné a personnev (pour effacement memoire)
  }

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION();

  return msg_ptr;
}

/*********************************************************************
 * @fn      OSAL_msg_push
 *
 * @brief
 *
 *    This function pushes an OSAL message to the head of an OSAL
 *    queue.
 *
 * @param   OSAL_msg_q_t *q_ptr - OSAL queue
 * @param   void *msg_ptr  - OSAL message
 *
 * @return  none
 */
void OSAL_msg_push( OSAL_msg_q_t *q_ptr, void *msg_ptr )
{

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION();

  // Push message to head of queue
  OSAL_MSG_NEXT( msg_ptr ) = *q_ptr;            //On fait pointer le prochain message sur la queue
  *q_ptr = msg_ptr;                             //La queue pointe maintenant vers notre message

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION();
}

/*********************************************************************
 * @fn      OSAL_msg_extract
 *
 * @brief
 *
 *    This function extracts and removes an OSAL message from the
 *    middle of an OSAL queue.
 *
 * @param   OSAL_msg_q_t *q_ptr - OSAL queue
 * @param   void *msg_ptr  - OSAL message to be extracted
 * @param   void *prev_ptr  - OSAL message before msg_ptr in queue
 *
 * @return  none
 */
void OSAL_msg_extract( OSAL_msg_q_t *q_ptr, void *msg_ptr, void *prev_ptr )
{

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION();

  if ( msg_ptr == *q_ptr )
  {
    // remove from first
    *q_ptr = OSAL_MSG_NEXT( msg_ptr );
  }
  else
  {
    // remove from middle
    OSAL_MSG_NEXT( prev_ptr ) = OSAL_MSG_NEXT( msg_ptr );
  }
  OSAL_MSG_NEXT( msg_ptr ) = NULL;
  OSAL_MSG_ID( msg_ptr ) = TASK_NO_TASK;

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION();
}

/*********************************************************************
 * @fn      OSAL_msg_enqueue_max
 *
 * @brief
 *
 *    This function enqueues an OSAL message into an OSAL queue if
 *    the length of the queue is less than max.
 *
 * @param   OSAL_msg_q_t *q_ptr - OSAL queue
 * @param   void *msg_ptr  - OSAL message
 * @param   uint8 max - maximum length of queue
 *
 * @return  TRUE if message was enqueued, FALSE otherwise
 */
uint8 OSAL_msg_enqueue_max( OSAL_msg_q_t *q_ptr, void *msg_ptr, uint8 max )
{
  void *list;
  uint8 ret = FALSE;


  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION();

  // If first message in queue
  if ( *q_ptr == NULL )
  {
    *q_ptr = msg_ptr;
    ret = TRUE;
  }
  else
  {
    // Find end of queue or max
    list = *q_ptr;
    max--;
    while ( (OSAL_MSG_NEXT( list ) != NULL) && (max > 0) )
    {
      list = OSAL_MSG_NEXT( list );
      max--;
    }

    // Add message to end of queue if max not reached
    if ( max != 0 )
    {
      OSAL_MSG_NEXT( list ) = msg_ptr;
      ret = TRUE;
    }
  }

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION();

  return ret;
}

/*********************************************************************
 * @fn      OSAL_set_event
 *
 * @brief
 *
 *    This function is called to set the event flags for a task. The
 *    event passed in is OR'd into the task's event variable.
 *
 * @param   uint8 task_id - receiving tasks ID
 * @param   uint8 event_flag - what event to set
 *
 * @return  SUCCESS, INVALID_TASK
 */
uint8 OSAL_set_event( uint8 task_id, uint16 event_flag )
{
  if ( task_id < tasksCnt )
  {
    HAL_ENTER_CRITICAL_SECTION();    // Hold off interrupts
    tasksEvents[task_id] |= event_flag;  // Stuff the event bit(s)
    HAL_EXIT_CRITICAL_SECTION();     // Release interrupts
    return ( SUCCESS );
  }
   else
  {
    return ( INVALID_TASK );
  }
}

/*********************************************************************
 * @fn      OSAL_clear_event
 *
 * @brief
 *
 *    This function is called to clear the event flags for a task. The
 *    event passed in is masked out of the task's event variable.
 *
 * @param   uint8 task_id - receiving tasks ID
 * @param   uint8 event_flag - what event to clear
 *
 * @return  SUCCESS, INVALID_TASK
 */
uint8 OSAL_clear_event( uint8 task_id, uint16 event_flag )
{
  if ( task_id < tasksCnt )
  {
    HAL_ENTER_CRITICAL_SECTION();    // Hold off interrupts
    tasksEvents[task_id] &= ~(event_flag);   // Clear the event bit(s)
    HAL_EXIT_CRITICAL_SECTION();     // Release interrupts
    return ( SUCCESS );
  }
   else
  {
    return ( INVALID_TASK );
  }
}

/*********************************************************************
 * @fn      OSAL_isr_register
 *
 * @brief
 *
 *   This function is called to register a service routine with an
 *   interrupt. When the interrupt occurs, this service routine is called.
 *
 * @param   uint8 interrupt_id - Interrupt number
 * @param   void (*isr_ptr)( uint8* ) - function pointer to ISR
 *
 * @return  SUCCESS, INVALID_INTERRUPT_ID,
 */
uint8 OSAL_isr_register( uint8 interrupt_id, void (*isr_ptr)( uint8* ) )
{
  // Remove these statements when functionality is complete
  (void)interrupt_id;
  (void)isr_ptr;
  return ( SUCCESS );
}



/*********************************************************************
 * @fn      OSAL_init_system
 *
 * @brief
 *
 *   This function initializes the "task" system by creating the
 *   tasks defined in the task table (OSAL_Tasks.h).
 *
 * @param   void
 *
 * @return  SUCCESS
 */
uint8 OSAL_init_system( void )
{
  // Initialize the Memory Allocation System
  OSAL_mem_init();

  // Initialize the message queue
  OSAL_qHead = NULL;

  // Initialize the timers
  OSALTimerInit();

  OSALInitTasks();

  // Setup efficient search for the first free block of heap.
  OSAL_mem_kick();

  return ( SUCCESS );
}

/*********************************************************************
 * @fn      OSAL_buffer_uint32
 *
 * @brief
 *
 *   Buffer an uint32 value - LSB first.
 *
 * @param   buf - buffer
 * @param   val - uint32 value
 *
 * @return  pointer to end of destination buffer
 */
uint8* OSAL_buffer_uint32( uint8 *buf, uint32 val )
{
  *buf++ = BREAK_UINT32( val, 0 );
  *buf++ = BREAK_UINT32( val, 1 );
  *buf++ = BREAK_UINT32( val, 2 );
  *buf++ = BREAK_UINT32( val, 3 );

  return buf;
}


/*********************************************************************
 * @fn      OSAL_isbufset
 *
 * @brief
 *
 *   Is all of the array elements set to a value?
 *
 * @param   buf - buffer to check
 * @param   val - value to check each array element for
 * @param   len - length to check
 *
 * @return  TRUE if all "val"
 *          FALSE otherwise
 */
uint8 OSAL_isbufset( uint8 *buf, uint8 val, uint8 len )
{
  uint8 x;

  if ( buf == NULL )
  {
    return ( FALSE );
  }

  for ( x = 0; x < len; x++ )
  {
    // Check for non-initialized value
    if ( buf[x] != val )
    {
      return ( FALSE );
    }
  }
  return ( TRUE );
}

/*********************************************************************
 * @fn      OSAL_self
 *
 * @brief
 *
 *   This function returns the task ID of the current (active) task.
 *
 * @param   void
 *
 * @return   active task ID or TASK_NO_TASK if no task is active
 */
uint8 OSAL_self( void )
{
  return ( activeTaskID );
}

#endif
/*********************************************************************
 */
