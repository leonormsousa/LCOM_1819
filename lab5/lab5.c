#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "i8254.h"
#include "graphics.h"
#include "keyboard.h"
#include "sprite.h"

extern unsigned h_res;
extern unsigned v_res;
extern unsigned bits_per_pixel;
extern u32_t scan_code;
extern bool status_error;
extern int counter;

#define PackedPixel 0x04
#define DirectColor 0x06

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");
  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");
  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");
  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;
  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();
  return 0;
}

int (video_test_init)(uint16_t mode, uint8_t delay) {
  if(vg_init(mode)==NULL)
    return 1;
  sleep(delay);
  if(vg_exit()!= 0)
    return 1;
  return 0;
}

int (video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                       uint16_t width, uint16_t height, uint32_t color) {
  void* init_adress= vg_init(mode);
  if(init_adress==NULL)
    return 1;
  if(x<=h_res && y<=v_res)
    {
      if (x+width >h_res)
        width = h_res -x;
      if (y+height >v_res)
        height = v_res -y;
      vg_draw_rectangle(x,y,width, height, color);
    }
         int ipc_status, irq_set;
  u8_t r;
  message msg;
  //subscribing the notifications
  if (keyboard_subscribe_int(&r) != 0)
   {
     vg_exit();
      return 1;
   }
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
                  kbc_ih();
                if(status_error)
                  break;
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
  if(vg_exit()!=0)
    return 1;
  if (keyboard_unsubscribe_int() != 0)
    return 1;
  return 0;
}

uint32_t process_direct_color(int row, int col, uint32_t first, uint8_t step, vbe_mode_info_t m)
{
  //red
  uint32_t redMask=((1 << m.RedMaskSize)-1);
  uint32_t red=first;
  red= red>>(m.RedFieldPosition);
  red = (red + col * step) & redMask;
  red= red<<m.RedFieldPosition;
  //green  
  uint32_t greenMask=((1 << m.GreenMaskSize)-1);
  uint32_t green=first;
  green= green>>m.GreenFieldPosition;
  green = (green + row * step) & greenMask;
  green=green<<m.GreenFieldPosition;
  //blue
  uint32_t blueMask=((1 << m.BlueMaskSize)-1);
  uint32_t blue = first;
  blue=blue>>m.BlueFieldPosition;
  blue = (blue + (col + row) * step) & blueMask; 
  blue= blue<<m.BlueFieldPosition;
  return (red | green |  blue );
}

int (video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step){
  vg_init(mode);
  vbe_mode_info_t m;
  vbe_get_mode_inf(mode, &m);
  uint16_t width;
  uint16_t height;
  if (no_rectangles!=0)
  {
    width=m.XResolution/no_rectangles;
    height=m.YResolution/no_rectangles;
  }
  //printing the rectangles
  for(int i=0; i< no_rectangles;i++)
  {
    for(int j=0; j< no_rectangles; j++)
    {
      uint32_t color;
      if(m.MemoryModel == PackedPixel)
        color = (first + (i * no_rectangles + j) * step) % (1 << m.BitsPerPixel);
      else if (m.MemoryModel == DirectColor)
        color=process_direct_color(i, j, first, step, m);
      else
      {
        vg_exit();
        return 1;
      }
      vg_draw_rectangle(j* width,i * height, width, height, color);
    }
  }
  //preparing the cycle
  int ipc_status, irq_set;
  u8_t r;
  message msg;
  //subscribing the notifications
  if (keyboard_subscribe_int(&r) != 0)
  {
    if(vg_exit()!=0)
     return 1;
  } 
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
                  kbc_ih();
                if(status_error)
                  break;
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
  if(vg_exit()!=0)
    return 1;
  if (keyboard_unsubscribe_int() != 0)
    return 1;
  return 0;
}

int (video_test_xpm)(const char *xpm[], uint16_t x, uint16_t y) {
  char* init_adress=vg_init(0x105);
  int width, height;
  char* sprite;
  sprite= read_xpm(xpm,&width, &height);
  //prints the pixmap
  for(int i=0; i<height;i++)
  {
    if((unsigned int)(y+i)>=v_res)
      break;
    for(int j=0; j<width; j++)
    {
      if((unsigned int)(x+j)>=h_res)
        break;
      char *ptr = init_adress + (((y + i) * h_res) + x +j);
      *ptr = sprite[i * width + j];
    }
  }
  //preparing the cycle
  int ipc_status, irq_set;
  u8_t r;
  message msg;
  //subscribing the notifications
  if (keyboard_subscribe_int(&r) != 0)
   {
     vg_exit();
    return 1;
   } 
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
            kbc_ih();
            if(status_error)
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
  if(vg_exit()!=0)
    return 1;
  if (keyboard_unsubscribe_int() != 0)
    return 1;
  return 0;
}

int (video_test_move)(const char *xpm[], uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  int frame_counter=0;
  uint16_t interrupts_between_frames=60*1/(double)fr_rate;
  if (abs(xi-xf)!=0 && abs(yi-yf)!=0)
    return 1;
  bool speed_neg=false;
  //inicializar o modo gráfico
  char* init_adress=vg_init(0x105);
  Sprite * spr;
    if (speed<0)
    {
      speed_neg=true;
      speed=1;
    }
  speed=abs(speed);
  //creating the sprite
  if ((xf-xi)>0)
    spr = create_sprite(xpm, xi, yi, speed, 0);
  else if (xf-xi<0)
    spr = create_sprite(xpm, xi, yi,-speed, 0);
  else if ((yf-yi)>0)
    spr = create_sprite(xpm, xi, yi,0, speed);
  else if (yf-yi<0)
    spr = create_sprite(xpm, xi, yi, 0,-speed);
  else
    spr = create_sprite(xpm, xi, yi, 0, 0);
  draw_sprite(spr, init_adress);
  //preparing the cycle
  int ipc_status, irq_set, irq_set_timer;
  u8_t r, seconds=0;
  message msg;
  //subscribing the notifications
  if(timer_subscribe_int(&r)!=0)
  {
    vg_exit();
    return 1;
  }
  irq_set_timer= BIT(r);
  if (keyboard_subscribe_int(&r) != 0)
  {
    vg_exit();
    return 1;
  }
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
           {
              if (msg.m_notify.interrupts & irq_set)
              { 
                  kbc_ih();
                  if(status_error)
                    break;
              } 
            if (msg.m_notify.interrupts & irq_set_timer)
                { 
                  timer_int_handler();
                  //verifying if a second as passed
                  if(counter%60==0)
                    { 
                    seconds++;
                    }  
                  if (((spr->y<=yf && spr->yspeed>0) || (spr->y>=yf && spr->yspeed<0) || (spr->x<=xf && spr->xspeed>0) || (spr->x>=xf && spr->xspeed<0)))
                  {
                    if (counter%interrupts_between_frames ==0)
                    {
                      frame_counter++;
                      if ((spr->y+spr->yspeed>yf && spr->yspeed>0) || (spr->y+spr->yspeed<yf && spr->yspeed<0) || (spr->x+spr->xspeed>xf && spr->xspeed>0) || (spr->x+spr->xspeed<xf && spr->xspeed<0))
                      {
                        erase_sprite_screen(spr, init_adress);
                        spr->xspeed=xf-spr->x;
                        spr->yspeed=yf-spr->y;
                        animate_sprite(spr);
                        draw_sprite(spr, init_adress);
                      }
                      else if (!speed_neg)
                      {
                        erase_sprite_screen(spr, init_adress);
                        animate_sprite(spr);
                        draw_sprite(spr, init_adress);
                      }
                      else if (frame_counter == abs(speed))
                      {
                        frame_counter=0;
                        erase_sprite_screen(spr, init_adress);
                        animate_sprite(spr);
                        draw_sprite(spr, init_adress);
                      }
                    }
                  }
                }
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
  vg_exit();
  if (keyboard_unsubscribe_int() != 0)
    return 1;
  if(timer_unsubscribe_int() != 0)
    return 1;
  return 0;
}

int (video_test_controller)() {
  vg_vbe_contr_info_t info;
  vbe_config_contr_info(&info);
  vg_display_vbe_contr_info(&info);
  return 0;
} 
