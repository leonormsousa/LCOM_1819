#include"mouse.h"

int hook_mouse=2;
u32_t scan_code_mouse=0;
bool status_error_mouse=false;
uint32_t num_packets=0;

int (mouse_enable_data_report)()
{
  return send_command_mouse(DATA_REPORT_ENABLE);
}

int(mouse_disable_data_report)()
{
    return send_command_mouse(DATA_REPORT_DISABLE);
}

int (mouse_subscribe_int)(uint8_t *bit_no)
{
  *bit_no=hook_mouse;
  if(sys_irqsetpolicy(KBD_AUX_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_mouse)!=OK)
      return 1;
  return 0;
}

int (mouse_unsubscribe_int)()
{
  if(sys_irqrmpolicy(&hook_mouse)!=OK)
    return 1;
  return 0;
}

int(mouse_set_remote)()
{
    return send_command_mouse(SET_REMOTE_MODE);

}

int(mouse_set_stream)()
{
    return send_command_mouse(SET_STREAM_MODE);
}

void (mouse_ih)()
{
  u32_t stat;
  sys_inb(STAT_REG, &stat);
  if(stat & OBF)
      sys_inb(OUT_BUF, &scan_code_mouse);
  else
    status_error_mouse = true;
  if ( (stat &(PAR_ERR | TO_ERR)) != 0 )
    status_error_mouse=true;
}

int (send_command_mouse)(u32_t command)
{
  for (int i=0; i<5;i++)
  {
    u32_t stat;
    sys_inb(STAT_REG, &stat);
    if ((stat & (AUX | IBF)) !=0)
      return 1;
    sys_outb(STAT_REG, WRITE_TO_MOUSE);
    sys_inb(STAT_REG, &stat);
    if ((stat & (AUX | IBF)) !=0)
      return 1;
    sys_outb(IN_BUF, command);
    sys_inb(OUT_BUF, &stat);
    if (stat == ERROR)
      return 1;
    if (stat == ACK)
      return 0;
  }
  return 1;
}

bool parse_packet(struct packet* pack)
{
  static uint8_t packets[3];
  static int counter_packets=0;
  if (counter_packets==0)
  {
      if ((scan_code_mouse & 0x08) == 0)
        return false;
  }
  packets[counter_packets++]=scan_code_mouse;
  if(counter_packets%3==0)
  {
      pack->bytes[0] = packets[0];
      pack->bytes[1] = packets[1];
      pack->bytes[2] = packets[2];
      pack->rb = (packets[0] & 0x02) >>1;
      pack->mb = (packets[0] & 0x04) >>2;
      pack->lb = (packets[0] & 0x01);
      pack->x_ov = (packets[0] & 0x40) >>6;
      pack->y_ov = (packets[0] & 0x80) >>7;
      int16_t del_x = (uint16_t)packets[1];
      int16_t del_y = (uint16_t)packets[2];
      if ((packets[0] & 0x10) == 0x10)
        del_x |=  0xFF00;
      if ((packets[0] & 0x20) == 0x20)
        del_y |=  0xFF00;
      pack->delta_x = del_x;
      pack->delta_y = del_y;
      num_packets++;
      counter_packets=0;
      return true;
  }
  return false;
}

int (out_buf_read)()
{
  u32_t stat;
  if(sys_inb(STAT_REG, &stat)!= OK)
    return 1;
  if(stat & OBF)
    if(sys_inb(OUT_BUF, &stat)!=OK)
      return 1;
  return 0;
}
