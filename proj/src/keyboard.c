#include "keyboard.h"

int hook_keyboard=1;
u32_t scan_code=0;
bool status_error=false;

/** Subscribes the keyboard interruptions
 * Returns the hook_id used
 */
int (keyboard_subscribe_int)(uint8_t *bit_no)
{
  *bit_no=hook_keyboard;
  if(sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_keyboard)!= OK)
    return 1;
  return 0;
}
/** Unsubscribes the RTC interruptions
 */
int (keyboard_unsubscribe_int)() 
{
  if(sys_irqrmpolicy(&hook_keyboard)!= OK)
    return 1;
  return 0;
}

/** Checks if the input buffer is full
 */
bool (input_buffer_full)()
{
  u32_t stat;
  if (sys_inb(STAT_REG, &stat) != OK)
    return true;
  if ((stat & IBF) != 0)
    return true;
  return false;
}

/** Handles every interruption from the keyboard reading the corresponding scan code and checking for errors
 */
void(kbc_ih)()
{ 
  u32_t stat;
  sys_inb(STAT_REG, &stat);
  if(stat & OBF)
    sys_inb(OUT_BUF, &scan_code);
  else
    status_error = true;
  if ( (stat &(PAR_ERR | TO_ERR)) != 0 )
  status_error=true;
}

