// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "i8254.h"
#include "keyboard.h"
#include "mouse.h"

extern bool status_error_mouse;
extern u32_t scan_code_mouse;
extern int counter;
extern uint32_t num_packets;
extern int hook_mouse;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  int ipc_status, irq_set;
  u8_t r;
  message msg;
  //subscribing the notifications
  if (mouse_subscribe_int(&r) !=0)
    return 1;
  irq_set=BIT(r);
  sys_irqdisable(&hook_mouse);
  if(mouse_enable_data_report()!=0)
    return 1;
  sys_irqenable(&hook_mouse);
  //Loop that receives notifications and treats them
  while((uint32_t)num_packets<cnt) 
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
                mouse_ih();
                if(status_error_mouse)
                  break;
                struct packet pack;
                if (parse_packet(&pack))
                  mouse_print_packet(&pack);
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
  sys_irqdisable(&hook_mouse);
  if (mouse_disable_data_report()!=0)
    return 1;
  if(mouse_unsubscribe_int() != 0)
    return 1;
  if(out_buf_read()!=0)
    return 1;
  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  u32_t stat=0;
 /* if (mouse_disable_data_report()!=0)
    return 1;
  if (mouse_set_remote()!=0)
    return 1;*/
  while(num_packets<cnt)
  {
    send_command_mouse(READ_DATA);
    //para os 3 bytes
    for (int i=0; i<3; i++)
    {
      tickdelay(micros_to_ticks(period));
      if (sys_inb(STAT_REG, &stat) != OK)
        return 1;
      if (!(stat & OBF) || !(stat & AUX))
        break;
      if (sys_inb(OUT_BUF, &scan_code_mouse) != OK)
        return 1;
      if ( (stat &(PAR_ERR | TO_ERR)) == 0)
      {
        struct packet pack;
        if (parse_packet(&pack))
          mouse_print_packet(&pack);
      }
      else
        break;
    }
  }
  mouse_set_stream();
  if (mouse_disable_data_report()!=0)
    return 1;
  uint32_t command_byte = minix_get_dflt_kbc_cmd_byte();
  if(input_buffer_full())
    return 1;
  if (sys_outb(STAT_REG, WRITE_CMD) != OK)
    return 1;
  if(input_buffer_full())
    return 1;
  if (sys_outb(IN_BUF, command_byte) != OK)
    return 1;
  if(out_buf_read()!=0)
    return 1;
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) 
{
  int ipc_status, irq_set_timer, irq_set;
  u8_t r, seconds=0;
  message msg;
  //subscribing the notifications
  if(timer_subscribe_int(&r)!=0)
    return 1;
  irq_set_timer= BIT(r);
  if (mouse_subscribe_int(&r) !=0)
    return 1;
  irq_set= BIT(r);
  sys_irqdisable(&hook_mouse);
  if(mouse_enable_data_report()!=0)
    return 1;
  sys_irqenable(&hook_mouse);
  //Loop that receives notifications and treats them
  while(seconds<idle_time) 
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
            {
              if (msg.m_notify.interrupts & irq_set)
                { 
                  mouse_ih();
                  if(status_error_mouse)
                    break;
                  struct packet pack;
                  if (parse_packet(&pack))
                    mouse_print_packet(&pack);
                  counter=0;
                }
              if (msg.m_notify.interrupts & irq_set_timer)
                { 
                  timer_int_handler();
                  //verifying if a second as passed
                  if(counter%60==0)
                    {
                    counter=0;  
                    seconds++;
                    }
                }
                break;
            }
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
  if(timer_unsubscribe_int() != 0)
    return 1;
  sys_irqdisable(&hook_mouse);
  if (mouse_disable_data_report()!=0)
    return 1;
  if(mouse_unsubscribe_int() != 0)
    return 1;
  if(out_buf_read()!=0)
    return 1;
  return 0;   
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  bool first_fase_concluded=false, second_fase_concluded=false, third_fase_concluded=false, fourth_fase_concluded=false;
  int16_t delta_x=0, delta_y=0;
  int ipc_status, irq_set;
  u8_t r;
  message msg;
  //subscribing the notifications
  if (mouse_subscribe_int(&r) !=0)
    return 1;
  irq_set=BIT(r);
  sys_irqdisable(&hook_mouse);
  if(mouse_enable_data_report()!=0)
    return 1;
  sys_irqenable(&hook_mouse);
  //Loop that receives notifications and treats them
  while(!fourth_fase_concluded) 
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
                mouse_ih();
                if(status_error_mouse)
                  break;
                struct packet pack;
                if (parse_packet(&pack))
                {
                  mouse_print_packet(&pack);
                  //FASE 1 - botão esq. e deslocamento positivo em x e y;
                  if (pack.lb==1 && pack.mb==0 && pack.rb==0)
                  {
                    delta_x+=pack.delta_x;
                    delta_y+=pack.delta_y;
                    if (pack.delta_x < 0)
                      if (!(abs(pack.delta_x)<tolerance))
                      {
                        first_fase_concluded=false;
                        delta_x=0;
                        delta_y=0;
                        break;
                      }
                    if (pack.delta_y < 0)
                      if (!(abs(pack.delta_y)<tolerance))
                      {
                        first_fase_concluded=false;
                        delta_x=0;
                        delta_y=0;
                        break;
                      }
                    if (delta_x>= x_len && abs(delta_y/delta_x)>=1)
                      first_fase_concluded = true;
                  }
                  //FASE 2 - nenhum botão pressionado
                  if (first_fase_concluded && pack.lb==0 && pack.mb==0 && pack.rb==0)
                    {
                      second_fase_concluded = true;
                      delta_x=0;
                      delta_y=0;
                    }
                  //FASE 3 - botão dir. e deslocamento positivo em x,  negativo em y;
                  if (second_fase_concluded && pack.lb==0 && pack.mb==0 && pack.rb==1)
                  {
                    delta_x+=pack.delta_x;
                    delta_y+=pack.delta_y;
                    if (pack.delta_x < 0)
                      if (!(abs(pack.delta_x)<tolerance))
                      {
                        first_fase_concluded=false;
                        delta_x=0;
                        delta_y=0;
                        break;
                      }
                    if (pack.delta_y > 0)
                      if (!(abs(pack.delta_y)<tolerance))
                      {
                        first_fase_concluded=false;
                        delta_x=0;
                        delta_y=0;
                        break;
                      }
                    if (delta_x>= x_len && abs(delta_y/delta_x)>=1)
                      third_fase_concluded = true;
                  }
                  //FASE 4 - nenhum botão pressionado
                  if (third_fase_concluded && pack.lb==0 && pack.mb==0 && pack.rb==0)
                      fourth_fase_concluded = true;
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
  sys_irqdisable(&hook_mouse);
  if (mouse_disable_data_report()!=0)
    return 1;
  if(mouse_unsubscribe_int() != 0)
    return 1;
  if(out_buf_read()!=0)
    return 1;
  return 0;
}
