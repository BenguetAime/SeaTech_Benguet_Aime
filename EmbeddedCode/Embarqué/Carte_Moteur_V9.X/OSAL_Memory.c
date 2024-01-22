/**************************************************************************************************
  Filename:       OSAL_Memory.c
  Revised:        $Date: 2010-09-20 14:59:43 -0700 (Mon, 20 Sep 2010) $
  Revision:       $Revision: 23848 $

  Description:    OSAL Heap Memory management functions. There is an Application Note that
                  should be read before studying and/or modifying this module:
                  SWRA204 "Heap Memory Management"

  Copyright 2004-2010 Texas Instruments Incorporated. All rights reserved.

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

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "define.h"
#include "OSAL_Memory.h"

#ifdef USE_OSAL
#define OSALMEM_PROFILER TRUE
/* ------------------------------------------------------------------------------------------------
 *                                           Constants
 * ------------------------------------------------------------------------------------------------
 */

#define OSALMEM_IN_USE             0x8000               //Macro for bit Mask, this set the bit "in use"
#if (MAXMEMHEAP & OSALMEM_IN_USE)
#error MAXMEMHEAP is too big to manage!
#endif

#define OSALMEM_HDRSZ              sizeof(OSALMemHdr_t)

// Round a value up to the ceiling of OSALMEM_HDRSZ for critical dependencies on even multiples.
#define OSALMEM_ROUND(X)       ((((X) + OSALMEM_HDRSZ - 1) / OSALMEM_HDRSZ) * OSALMEM_HDRSZ)

/* Minimum wasted bytes to justify splitting a block before allocation.
 * Adjust accordingly to attempt to balance the tradeoff of wasted space and runtime throughput
 * spent splitting blocks into sizes that may not be practically usable when sandwiched between
 * two blocks in use (and thereby not able to be coalesced.)
 * Ensure that this size is an even multiple of OSALMEM_HDRSZ.
 */
#if !defined OSALMEM_MIN_BLKSZ
#define OSALMEM_MIN_BLKSZ         (OSALMEM_ROUND((OSALMEM_HDRSZ * 2)))
#endif

#if !defined OSALMEM_LL_BLKSZ
#if !defined NONWK
#define OSALMEM_LL_BLKSZ          (OSALMEM_ROUND(6) + (1 * OSALMEM_HDRSZ))
#else
/*
 * Profiling the sample apps with default settings shows the following long-lived allocations
 * which should live at the bottom of the small-block bucket so that they are never iterated over
 * by OSAL_mem_alloc/free(), nor ever considered for coalescing, etc. This saves significant
 * run-time throughput (on 8051 SOC if not also MSP). This is dynamic "dead space" and is not
 * available to the small-block bucket heap.
 *
 * Adjust this size accordingly to accomodate application-specific changes including changing the
 * size of long-lived objects profiled by sample apps and long-lived objects added by application.
 */
#if defined ZCL_KEY_ESTABLISH     // Attempt to capture worst-case for SE sample apps.
#define OSALMEM_LL_BLKSZ          (OSALMEM_ROUND(526) + (32 * OSALMEM_HDRSZ))
#elif defined TC_LINKKEY_JOIN
#define OSALMEM_LL_BLKSZ          (OSALMEM_ROUND(454) + (21 * OSALMEM_HDRSZ))
#elif ((defined SECURE) && (SECURE != 0))
#define OSALMEM_LL_BLKSZ          (OSALMEM_ROUND(418) + (19 * OSALMEM_HDRSZ))
#else
#define OSALMEM_LL_BLKSZ          (OSALMEM_ROUND(417) + (19 * OSALMEM_HDRSZ))
#endif
#endif
#endif

/* Adjust accordingly to attempt to accomodate the block sizes of the vast majority of
 * very high frequency allocations/frees by profiling the system runtime.
 * This default of 16 accomodates the OSAL timers block, OSALTimerRec_t, and many others.
 * Ensure that this size is an even multiple of OSALMEM_MIN_BLKSZ for run-time efficiency.
 */
#if !defined OSALMEM_SMALL_BLKSZ
#define OSALMEM_SMALL_BLKSZ       (OSALMEM_ROUND(16))
#endif
#if !defined OSALMEM_SMALL_BLKCNT
#define OSALMEM_SMALL_BLKCNT       8
#endif

/*
 * These numbers setup the size of the small-block bucket which is reserved at the front of the
 * heap for allocations of OSALMEM_SMALL_BLKSZ or smaller.
 */

// Size of the heap bucket reserved for small block-sized allocations.
// Adjust accordingly to attempt to accomodate the vast majority of very high frequency operations.
#define OSALMEM_SMALLBLK_BUCKET  ((OSALMEM_SMALL_BLKSZ * OSALMEM_SMALL_BLKCNT) + OSALMEM_LL_BLKSZ)
// Index of the first available OSALMemHdr_t after the small-block heap which will be set in-use in
// order to prevent the small-block bucket from being coalesced with the wilderness.
#define OSALMEM_SMALLBLK_HDRCNT   (OSALMEM_SMALLBLK_BUCKET / OSALMEM_HDRSZ)
// Index of the first available OSALMemHdr_t after the small-block heap which will be set in-use in
#define OSALMEM_BIGBLK_IDX        (OSALMEM_SMALLBLK_HDRCNT + 1)
// The size of the wilderness after losing the small-block heap, the wasted header to block the
// small-block heap from being coalesced, and the wasted header to mark the end of the heap.
#define OSALMEM_BIGBLK_SZ         (MAXMEMHEAP - OSALMEM_SMALLBLK_BUCKET - OSALMEM_HDRSZ*2)
// Index of the last available OSALMemHdr_t at the end of the heap which will be set to zero for
// fast comparisons with zero to determine the end of the heap.
#define OSALMEM_LASTBLK_IDX      ((MAXMEMHEAP / OSALMEM_HDRSZ) - 1)

// For information about memory profiling, refer to SWRA204 "Heap Memory Management", section 1.5.
#if !defined OSALMEM_PROFILER
#define OSALMEM_PROFILER           FALSE  // Enable/disable the memory usage profiling buckets.
#endif
#if !defined OSALMEM_PROFILER_LL
#define OSALMEM_PROFILER_LL        FALSE  // Special profiling of the Long-Lived bucket.
#endif

#if OSALMEM_PROFILER
#define OSALMEM_INIT              'X'
#define OSALMEM_ALOC              'A'
#define OSALMEM_REIN              'F'
#endif

/* ------------------------------------------------------------------------------------------------
 *                                           Typedefs
 * ------------------------------------------------------------------------------------------------
 */

typedef struct {
  // The 15 LSB's of 'val' indicate the total item size, including the header, in 8-bit bytes.
  unsigned len : 15;
  // The 1 MSB of 'val' is used as a boolean to indicate in-use or freed.
  unsigned inUse : 1;
} OSALMemHdrHdr_t;

typedef union {
  /* Dummy variable so compiler forces structure to alignment of largest element while not wasting
   * space on targets when the halDataAlign_t is smaller than a unsigned short.
   */
  halDataAlign_t alignDummy;
  unsigned short val;
  OSALMemHdrHdr_t hdr;
} OSALMemHdr_t;

/* ------------------------------------------------------------------------------------------------
 *                                           Local Variables
 * ------------------------------------------------------------------------------------------------
 */

//__eds__ OSALMemHdr_t __attribute__((section("eds1"),space(eds))) theHeap[MAXMEMHEAP / OSALMEM_HDRSZ];
//__eds__ OSALMemHdr_t __attribute__((section("eds1"),space(eds))) *ff1;  // First free block in the small-block bucket.
//On declare notre "Heap" (block d'octets permetant l'allocation dynamic)
// Avec un attribut de type (aligned) et non packed!!!!!!! 
OSALMemHdr_t __attribute__((aligned)) theHeap[MAXMEMHEAP / OSALMEM_HDRSZ];
OSALMemHdr_t *ff1;  // First free block in the small-block bucket.

static unsigned char OSALMemStat;            // Discrete status flags: 0x01 = kicked.

#if OSALMEM_METRICS
static unsigned short blkMax;  // Max cnt of all blocks ever seen at once.
static unsigned short blkCnt;  // Current cnt of all blocks.
static unsigned short blkFree; // Current cnt of free blocks.
static unsigned short memAlo;  // Current total memory allocated.
static unsigned short memMax;  // Max total memory ever allocated at once.
#endif

#if OSALMEM_PROFILER
extern void *OSAL_memset( void *dest, uint8 value, int len );
#define OSALMEM_PROMAX  8
/* The profiling buckets must differ by at least OSALMEM_MIN_BLKSZ; the
 * last bucket must equal the max alloc size. Set the bucket sizes to
 * whatever sizes necessary to show how your application is using memory.
 */
static unsigned short proCnt[OSALMEM_PROMAX] = {
OSALMEM_SMALL_BLKSZ, 48, 112, 176, 192, 224, 256, 65535 };
static unsigned short proCur[OSALMEM_PROMAX] = { 0 };
static unsigned short proMax[OSALMEM_PROMAX] = { 0 };
static unsigned short proTot[OSALMEM_PROMAX] = { 0 };
static unsigned short proSmallBlkMiss;
#endif


/**************************************************************************************************
 * @fn          OSAL_mem_init
 *
 * @brief       This function is the OSAL heap memory management initialization callback.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
void OSAL_mem_init(void)
{
#if OSALMEM_PROFILER
  (void)OSAL_memset(theHeap, OSALMEM_INIT, MAXMEMHEAP);
#endif

  // Setup a NULL block at the end of the heap for fast comparisons with zero.
  theHeap[OSALMEM_LASTBLK_IDX].val = 0;

  // Setup the small-block bucket.
  ff1 = theHeap;
  ff1->val = OSALMEM_SMALLBLK_BUCKET;                   // Set 'len' & clear 'inUse' field.
  // Set 'len' & 'inUse' fields - this is a 'zero data bytes' lifetime allocation to block the
  // small-block bucket from ever being coalesced with the wilderness.
  theHeap[OSALMEM_SMALLBLK_HDRCNT].val = (OSALMEM_HDRSZ | OSALMEM_IN_USE);

  // Setup the wilderness.
  theHeap[OSALMEM_BIGBLK_IDX].val = OSALMEM_BIGBLK_SZ;  // Set 'len' & clear 'inUse' field.

#if ( OSALMEM_METRICS )
  /* Start with the small-block bucket and the wilderness - don't count the
   * end-of-heap NULL block nor the end-of-small-block NULL block.
   */
  blkCnt = blkFree = 2;
#endif
}

/**************************************************************************************************
 * @fn          OSAL_mem_kick
 *
 * @brief       This function is the OSAL task initialization callback.
 * @brief       Kick the ff1 pointer out past the long-lived OSAL Task blocks.
 *              Invoke this once after all long-lived blocks have been allocated -
 *              presently at the end of OSAL_init_system().
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
void OSAL_mem_kick(void)
{
  OSALMemHdr_t *tmp = OSAL_mem_alloc(1);

  HAL_ASSERT((tmp != NULL));
  HAL_ENTER_CRITICAL_SECTION();  // Hold off interrupts.

  /* All long-lived allocations have filled the LL block reserved in the small-block bucket.
   * Set 'OSALMemStat' so searching for memory in this bucket from here onward will only be done
   * for sizes meeting the OSALMEM_SMALL_BLKSZ criteria.
   */
  ff1 = tmp - 1;       // Set 'ff1' to point to the first available memory after the LL block.
  OSAL_mem_free(tmp);
  OSALMemStat = 0x01;  // Set 'OSALMemStat' after the free because it enables memory profiling.

  HAL_EXIT_CRITICAL_SECTION();  // Re-enable interrupts.
}

/**************************************************************************************************
 * @fn          OSAL_mem_alloc
 *
 * @brief       This function implements the OSAL dynamic memory allocation functionality.
 *
 * input parameters
 *
 * @param size - the number of bytes to allocate from the HEAP.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */

void *OSAL_mem_alloc( unsigned short size )
{
  OSALMemHdr_t *prev = NULL;
  OSALMemHdr_t *hdr;
  unsigned char coal = 0;

  size += OSALMEM_HDRSZ;

  // Calculate required bytes to add to 'size' to align to halDataAlign_t.
  if ( sizeof( halDataAlign_t ) == 2 )
  {
    size += (size & 0x01);
  }
  else if ( sizeof( halDataAlign_t ) != 1 )
  {
    const unsigned char mod = size % sizeof( halDataAlign_t );

    if ( mod != 0 )
    {
      size += (sizeof( halDataAlign_t ) - mod);
    }
  }

  HAL_ENTER_CRITICAL_SECTION( );  // Hold off interrupts.

  // Smaller allocations are first attempted in the small-block bucket, and all long-lived
  // allocations are channeled into the LL block reserved within this bucket.
  if ((OSALMemStat == 0) || (size <= OSALMEM_SMALL_BLKSZ))
  {
    hdr = ff1;
  }
  else
  {
    hdr = (theHeap + OSALMEM_BIGBLK_IDX);
  }

  do
  {
    if ( hdr->hdr.inUse )
    {
      coal = 0;
    }
    else
    {
      if ( coal != 0 )
      {
#if ( OSALMEM_METRICS )
        blkCnt--;
        blkFree--;
#endif

        prev->hdr.len += hdr->hdr.len;

        if ( prev->hdr.len >= size )
        {
          hdr = prev;
          break;
        }
      }
      else
      {
        if ( hdr->hdr.len >= size )
        {
          break;
        }

        coal = 1;
        prev = hdr;
      }
    }

    hdr = (OSALMemHdr_t *)((unsigned char *)hdr + hdr->hdr.len);

    if ( hdr->val == 0 )
    {
      hdr = NULL;
      break;
    }
  } while (1);

  if ( hdr != NULL )
  {
    unsigned short tmp = hdr->hdr.len - size;

    // Determine whether the threshold for splitting is met.
    if ( tmp >= OSALMEM_MIN_BLKSZ )
    {
      // Split the block before allocating it.
      OSALMemHdr_t *next = (OSALMemHdr_t *)((unsigned char *)hdr + size);
      next->val = tmp;                     // Set 'len' & clear 'inUse' field.
      hdr->val = (size | OSALMEM_IN_USE);  // Set 'len' & 'inUse' field.

#if ( OSALMEM_METRICS )
      blkCnt++;
      if ( blkMax < blkCnt )
      {
        blkMax = blkCnt;
      }
      memAlo += size;
#endif
    }
    else
    {
#if ( OSALMEM_METRICS )
      memAlo += hdr->hdr.len;
      blkFree--;
#endif

      hdr->hdr.inUse = TRUE;
    }

#if ( OSALMEM_METRICS )
    if ( memMax < memAlo )
    {
      memMax = memAlo;
    }
#endif

#if ( OSALMEM_PROFILER )
#if !OSALMEM_PROFILER_LL
    if (OSALMemStat != 0)  // Don't profile until after the LL block is filled.
#endif
    {
      unsigned char idx;

      for ( idx = 0; idx < OSALMEM_PROMAX; idx++ )
      {
        if ( hdr->hdr.len <= proCnt[idx] )
        {
          break;
        }
      }
      proCur[idx]++;
      if ( proMax[idx] < proCur[idx] )
      {
        proMax[idx] = proCur[idx];
      }
      proTot[idx]++;

      /* A small-block could not be allocated in the small-block bucket.
       * When this occurs significantly frequently, increase the size of the
       * bucket in order to restore better worst case run times. Set the first
       * profiling bucket size in proCnt[] to the small-block bucket size and
       * divide proSmallBlkMiss by the corresponding proTot[] size to get % miss.
       * Best worst case time on TrasmitApp was achieved at a 0-15% miss rate
       * during steady state Tx load, 0% during idle and steady state Rx load.
       */
      if ((hdr->hdr.len <= OSALMEM_SMALL_BLKSZ) && (hdr >= (theHeap + OSALMEM_BIGBLK_IDX)))
      {
        proSmallBlkMiss++;
      }
    }

    (void)OSAL_memset((unsigned char *)(hdr+1), OSALMEM_ALOC, (hdr->hdr.len - OSALMEM_HDRSZ));
#endif

    if ((OSALMemStat != 0) && (ff1 == hdr))
    {
      ff1 = (OSALMemHdr_t *)((unsigned char *)hdr + hdr->hdr.len);
    }

    hdr++;
  }

  HAL_EXIT_CRITICAL_SECTION( );  // Re-enable interrupts.
  return (void *)hdr;
}

/**************************************************************************************************
 * @fn          OSAL_mem_free
 *
 * @brief       This function implements the OSAL dynamic memory de-allocation functionality.
 *
 * input parameters
 *
 * @param ptr - A valid pointer (i.e. a pointer returned by OSAL_mem_alloc()) to the memory to free.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */

void OSAL_mem_free(void *ptr)
{
  OSALMemHdr_t *hdr = (OSALMemHdr_t *)ptr - 1;


  HAL_ASSERT(((unsigned char *)ptr >= (unsigned char *)theHeap) && ((unsigned char *)ptr < (unsigned char *)theHeap+MAXMEMHEAP));
  HAL_ASSERT(hdr->hdr.inUse);

  HAL_ENTER_CRITICAL_SECTION( );  // Hold off interrupts.
  hdr->hdr.inUse = FALSE;

  if (ff1 > hdr)
  {
    ff1 = hdr;
  }

#if OSALMEM_PROFILER
#if !OSALMEM_PROFILER_LL
  if (OSALMemStat != 0)  // Don't profile until after the LL block is filled.
#endif
  {
    unsigned char idx;

    for (idx = 0; idx < OSALMEM_PROMAX; idx++)
    {
      if (hdr->hdr.len <= proCnt[idx])
      {
        break;
      }
    }

    proCur[idx]--;
  }

  (void)OSAL_memset((unsigned char *)(hdr+1), OSALMEM_REIN, (hdr->hdr.len - OSALMEM_HDRSZ) );
#endif
#if OSALMEM_METRICS
  memAlo -= hdr->hdr.len;
  blkFree++;
#endif

  HAL_EXIT_CRITICAL_SECTION( );  // Re-enable interrupts.
}

#if OSALMEM_METRICS
/*********************************************************************
 * @fn      OSAL_heap_block_max
 *
 * @brief   Return the maximum number of blocks ever allocated at once.
 *
 * @param   none
 *
 * @return  Maximum number of blocks ever allocated at once.
 */
unsigned short OSAL_heap_block_max( void )
{
  return blkMax;
}

/*********************************************************************
 * @fn      OSAL_heap_block_cnt
 *
 * @brief   Return the current number of blocks now allocated.
 *
 * @param   none
 *
 * @return  Current number of blocks now allocated.
 */
unsigned short OSAL_heap_block_cnt( void )
{
  return blkCnt;
}

/*********************************************************************
 * @fn      OSAL_heap_block_free
 *
 * @brief   Return the current number of free blocks.
 *
 * @param   none
 *
 * @return  Current number of free blocks.
 */
unsigned short OSAL_heap_block_free( void )
{
  return blkFree;
}

/*********************************************************************
 * @fn      OSAL_heap_mem_used
 *
 * @brief   Return the current number of bytes allocated.
 *
 * @param   none
 *
 * @return  Current number of bytes allocated.
 */
unsigned short OSAL_heap_mem_used( void )
{
  return memAlo;
}
#endif

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
/*********************************************************************
 * @fn      OSAL_heap_high_water
 *
 * @brief   Return the highest byte ever allocated in the heap.
 *
 * @param   none
 *
 * @return  Highest number of bytes ever used by the stack.
 */
unsigned short OSAL_heap_high_water( void )
{
#if ( OSALMEM_METRICS )
  return memMax;
#else
  return MAXMEMHEAP;
#endif
}
#endif

#endif //USE_OSAL_DYNAMIC_MEMORY_ALLOCATION
/**************************************************************************************************
*/
