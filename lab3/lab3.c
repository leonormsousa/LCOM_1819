#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdbool.h>
#include <stdint.h>
#include <minix/sysutil.h>
#include "keyboard.h"
#include "i8254.h"

extern int counter;
extern u32_t scan_code;
extern bool status_error;
extern uint number_sys_inb_calls;
extern int sys_inb_cnt(port_t port, u32_t *byte);
void (kbc_asm_ih)();
int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (kbd_test_scan)(bool assembly)
{
	int ipc_status, irq_set;
  u8_t r;
  message msg;
  uint8_t scan_code_array[2];
  uint8_t scan_code_size =0;
  //subscribing the notifications
  if (keyboard_subscribe_int(&r) != 0)
    return 1;
  irq_set= BIT(r);
  //Loop that receives notifications and treats them
  while(scan_code != ESC_BREAK) 
  { 
    //receiving the notification
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) 
      {
        printf("driver_receive failed with: %d", r); 
        continue; 
      } 
    //verifying the type of the notification
    if (is_ipc_notify(ipc_status)) 
      { 
        switch (_ENDPOINT_P(msg.m_source)) 
          {
            case HARDWARE: 
            if (msg.m_notify.interrupts & irq_set)
              { 
                if(assembly)
                  kbc_asm_ih();
                else
                  kbc_ih();
                if(status_error)
                  break;
                scan_code_array[scan_code_size] = scan_code;
                scan_code_size++;
                if (scan_code != 0xe0)
                {
                  bool break_code = scan_code >> 7;
                  if(kbd_print_scancode(!break_code, scan_code_size, scan_code_array)!=OK)
                    return 1;
                  scan_code_size =0;
                }
              }
              break; 
            default:
               break; // no other notifications expected: do nothing
          } 
      } 
    else 
    { 
      // received a standard message, not a notification, no standard messages expected: do nothing 
    }
  }
  //unsubscribing the notifications
  if (keyboard_unsubscribe_int() != 0)
    return 1;
  if(!assembly)
    if(kbd_print_no_sysinb(number_sys_inb_calls)!=OK)
      return 1;
  return 0;
}

int (kbd_test_poll)()
{
  u32_t scan_code =0;
  u32_t stat;
  while(scan_code != ESC_BREAK)
  {
    if (sys_inb_cnt(STAT_REG, &stat) != OK)
      return 1;
    /* loop while 8042 output buffer is empty */
    if(stat & OBF)
    {
      if (sys_inb_cnt(OUT_BUF, &scan_code) != OK)
        return 1;
      if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
         {
            bool break_code = scan_code >> 7;
            u8_t scan_byte = (u8_t)scan_code;
            kbd_print_scancode(!break_code, 1, &scan_byte);
         }
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  kbd_print_no_sysinb(number_sys_inb_calls);
  if ( input_buffer_full() )
    return 1;
  if (keyboard_enable_int() != 0)
    return 1;
  return 0;
}


int (kbd_test_timed_scan)(uint8_t n) 
{
int ipc_status, irq_set_kbd,irq_set_timer;
  u8_t r, seconds=0;
  message msg;
  uint8_t scan_code_array[2];
  uint8_t scan_code_size =0;
  //subscribing the notifications
  if(timer_subscribe_int(&r)!=0)
    return 1;
    irq_set_timer= BIT(r);
  if (keyboard_subscribe_int(&r) != 0)
    return 1;
  irq_set_kbd= BIT(r);
  //Loop that receives notifications and treats them
  while((scan_code != ESC_BREAK) && seconds<n) 
  { 
    //receiving the notification
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) 
      {
        printf("driver_receive failed with: %d", r); 
        continue; 
      } 
    //verifying the type of the notification
    if (is_ipc_notify(ipc_status)) 
      { 
        switch (_ENDPOINT_P(msg.m_source)) 
          {
            case HARDWARE: 
            if (msg.m_notify.interrupts & irq_set_kbd)
              { 
                kbc_ih();
                if(status_error)
                  break;
                scan_code_array[scan_code_size] = scan_code;
                scan_code_size++;
                seconds=0;
                counter=0;
                if (scan_code != 0xe0)
                {
                  bool break_code = scan_code >> 7;
                  if(kbd_print_scancode(!break_code, scan_code_size, scan_code_array)!=OK)
                    return 1;
                  scan_code_size =0;
                }
              }
              if (msg.m_notify.interrupts & irq_set_timer)
              { 
                timer_int_handler();
                //verifying if a minute as passed
                if(counter%60==0)
                  {
                  counter=0;  
                  seconds++;
                  }
              }
              break;

            default:
               break; // no other notifications expected: do nothing
          } 
      } 
    else 
    { 
      // received a standard message, not a notification, no standard messages expected: do nothing 
    }
  }
  //unsubscribing the notifications
    if(kbd_print_no_sysinb(number_sys_inb_calls)!=OK)
      return 1;
  if (keyboard_unsubscribe_int() != 0)
    return 1;
  if(timer_unsubscribe_int()!=0)
    return 1;
  return 0;    
}
