#include <lcom/lcf.h>
#include <keyboard.h>
#include <stdint.h>

int hook=1;
u32_t scan_code=0;
bool status_error=false;
uint number_sys_inb_calls = 0;
#ifdef LAB3
int sys_inb_cnt(port_t port, u32_t *byte)
{
  number_sys_inb_calls++;
  return sys_inb(port, byte);
}
#else
#define sys_inb_cnt(p,q) sys_inb(p,q)
#endif

int (keyboard_subscribe_int)(uint8_t *bit_no)
{
  *bit_no=hook;
  if(sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook)!= OK)
    return 1;
  return 0;
}

int (keyboard_unsubscribe_int)() 
{
  if(sys_irqrmpolicy(&hook)!= OK)
    return 1;
  return 0;
}

bool (input_buffer_full)()
{
  u32_t stat;
  if (sys_inb(STAT_REG, &stat) != OK)
    return true;
  if ((stat & IBF) != 0)
    return true;
  return false;
}

int (keyboard_enable_int)()
{
   if (sys_outb(STAT_REG, READ_CMD) != OK)
    return 1;
  uint32_t  command_byte;
  if (sys_inb(OUT_BUF, &command_byte) != OK)
    return 1;
  command_byte = command_byte | 0x01;
  if (sys_outb(STAT_REG, OUT_BUF) != OK)
    return 1;
  if (sys_outb(OUT_BUF, command_byte) != OK)
    return 1;
  return 0;
}

void(kbc_ih)()
{ 
  u32_t stat;
  sys_inb_cnt(STAT_REG, &stat);
  if(stat & OBF)
    sys_inb_cnt(OUT_BUF, &scan_code);
  else
    status_error = true;
  if ( (stat &(PAR_ERR | TO_ERR)) != 0 )
  status_error=true;
}

