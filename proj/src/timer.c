#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

/**
 *  @author Leonor Sousa
 *  @author Sílvia Rocha
 */

/** @defgroup Timer
 * @{
 *
 * Timer handling related functions
 */

int counter=0;
int hook_timer;

/** Subscribes the timer interruptions
 * Returns the hook_id used
 */
int (timer_subscribe_int)(uint8_t *bit_no)
{
  *bit_no=hook_timer;
  if(sys_irqsetpolicy(CLOCK_IRQ,IRQ_REENABLE, &hook_timer)!= OK)
    return 1;
  return 0;
}

/** Unsubscribes the timer interruptions
 */
int (timer_unsubscribe_int)() 
{
  if(sys_irqrmpolicy(&hook_timer)!= OK)
    return 1;
  return 0;
}

/** Handles any timer interrupt incrementing the counter variable
 */
void (timer_int_handler)() 
{
  counter++;
}

/** @} end of Timer */
