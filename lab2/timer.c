#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

int counter=0;
int hook;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) 
{
  //verifying if the timer input is valid
  if ((timer != 0) && (timer != 1) && (timer != 2))
    return 1;
  //reading the status of the counter
  uint32_t read_back_command = TIMER_RB_SEL(timer) | TIMER_RB_CMD | TIMER_RB_COUNT_;
  uint8_t timer_port = TIMER_0 + timer;
  if (sys_outb(TIMER_CTRL, read_back_command) != OK)
    return 1;
  uint32_t output_read_back;
  if (sys_inb(timer_port, &output_read_back) != OK)
    return 1;
  //building the control word
  uint8_t control_word = (uint8_t)output_read_back;
  control_word = control_word & 0x0F;
  control_word = control_word | TIMER_LSB_MSB;
  if (timer == 0)
    control_word = control_word | TIMER_SEL0;
  if (timer == 1)
    control_word = control_word | TIMER_SEL1;
  if (timer == 2)
    control_word = control_word | TIMER_SEL2;
  if (sys_outb(TIMER_CTRL, control_word) != OK)
    return 1;
  //changing the frequency
  if(freq <= 18) // to prevent the divisor from having more than 16 bits (2^16=65536 TIMER_FREQ/18=66287;)
    return 1;
uint16_t initial_timer_value = (uint16_t)(TIMER_FREQ /freq);
u8_t initial_timer_value_msb;
util_get_MSB(initial_timer_value, &initial_timer_value_msb);
u8_t initial_timer_value_lsb;
util_get_LSB(initial_timer_value, &initial_timer_value_lsb);
if (sys_outb(timer_port, initial_timer_value_lsb) != OK)
  return 1;
if (sys_outb(timer_port, initial_timer_value_msb) != OK)
  return 1;  
return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no)
{
  if(sys_irqsetpolicy(CLOCK_IRQ,IRQ_REENABLE, &hook)!= OK)
    return 1;
  *bit_no=hook;
  return 0;
}

int (timer_unsubscribe_int)() 
{
  if(sys_irqrmpolicy(&hook)!= OK)
    return 1;
  return 0;
}

void (timer_int_handler)() 
{
counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st)
{
  //verifying if the timer input is valid
  if ((timer != 0) && (timer != 1) && (timer != 2))
    return 1;
  uint32_t read_back_command = TIMER_RB_SEL(timer) | TIMER_RB_CMD | TIMER_RB_COUNT_;
  uint32_t status;
  uint8_t timer_port = TIMER_0 + timer;
  if (sys_outb(TIMER_CTRL, read_back_command) != OK)
    return 1;
  if (sys_inb(timer_port, &status) != OK)
    return 1;
  *st = (uint8_t)status;
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field)
{
  //verifying if the timer input is valid
  if ((timer != 0) && (timer != 1) && (timer != 2))
    return 1;
  union timer_status_field_val status;
  if (field == all)
    status.byte = st;
  else if (field == initial)
    status.in_mode = ((st & 0x30)>>4);
  else if (field == mode)
    status.count_mode = ((st & 0x0E) >> 1);
  else if (field == base)
    status.bcd = st & 0x01;
  else
    return 1;
  int to_print = timer_print_config(timer, field, status);
  if ((to_print < 0) || (to_print > 5))
    return 1;
  printf("The status is: %d.\n", to_print);
  return 0;
}
