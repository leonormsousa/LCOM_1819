#include "menu.h"

extern u32_t scan_code;
extern bool status_error;
static int h_res = 800, v_res = 600;
static Sprite* menu_arrows[4];

void initialize_variable()
{
	arrow_option1=create_sprite(arrow_xpm, 0.03125*(double)h_res, 0.325*(double)v_res, 0,-1);
	arrow_option2=create_sprite(arrow_xpm, 0.03125*(double)h_res, 0.4583*(double)v_res, 0,-1);
	arrow_option3=create_sprite(arrow_xpm, 0.03125*(double)h_res, 0.5917*(double)v_res, 0,-1);
	arrow_option4=create_sprite(arrow_xpm, 0.03125*(double)h_res, 0.725*(double)v_res, 0,-1);
	Sprite* menu_arrows_aux[4]={arrow_option1, arrow_option2, arrow_option3, arrow_option4};
	for(int i=0; i<4;i++)
	{
		menu_arrows[i]=menu_arrows_aux[i];
	}
}

/** Returns the image that corresponds to the menu passed as argument
 */
unsigned char* choose_menu(char menu)
{
	switch(menu)
	{
		case 'm':
			return image_load(choose_mode_xpm);
		case 's':
			return image_load(choose_if_local_xpm);
		case 'p':
			return image_load(choose_numberOfPlayers_xpm);
		case 'l':
			return image_load(choose_leaderboard_xpm);;
		default:
			break;
	}
	return NULL;
}

/** Draws the choosen menu background and allows the user to select 
 * one out of two available options returning it 
 */
int choose_option(char* address, char menu)
{
	scan_code=0;
	unsigned char* img= choose_menu(menu);
	if(img==NULL)
		return 1;
	background(address, img);
  initialize_variable();
  int ipc_status;
	u8_t r;
	message msg;
	if (keyboard_subscribe_int(&r) != 0)
		return 1;
  int option=0, option_choosen=-1, irq_set_keyboard = BIT(r);
  while (option_choosen==-1 && scan_code!= 0x81)
	{
		//receiving the notification
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
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
				if (msg.m_notify.interrupts & irq_set_keyboard)
				{
					kbc_ih();
					if (status_error)
						break;
          switch  (scan_code)
          {
						//ENTER KEY
            case 0x1c:
            {  option_choosen=option;
              break;
						}
						//DOWN ARROW
            case 0x50:
            {
							background(address, img);
							if(option<2)
								option++;
							else
								option=1;
							draw_sprite(menu_arrows[option],address);
              break;
						}
						//UP ARROW
            case 0x48:
            {
							background(address, img);
							if(option>1)
								option--;
							else
								option=2;
							draw_sprite(menu_arrows[option],address);
              break;
						}
            default:
              break;
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
	if (keyboard_unsubscribe_int() != 0)
   return -1;
  return option_choosen;
}

/** Draws the main menu allowing the user to select one out of 
 * the four available options returning it 
 */
int menu(char *address)
{
	scan_code=0;
  unsigned char* img=image_load(menu_xpm);
  background(address, img);
  initialize_variable();
  int ipc_status;
	u8_t r;
	message msg;
	if (keyboard_subscribe_int(&r) != 0)
		return 1;
  int irq_set_keyboard = BIT(r);
  int option=-1, option_choosen=-1;
  while (option_choosen==-1 && scan_code!= 0x81)
	{
		//receiving the notification
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
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
				if (msg.m_notify.interrupts & irq_set_keyboard)
				{
					kbc_ih();
					if (status_error)
						break;
          switch  (scan_code)
          {
						//ENTER KEY
            case 0x1c:
            {  option_choosen=option;
              break;
						}
						//DOWN ARROW
            case 0x50:
            {
							background(address, img);
							if(option<3)
								option++;
							else
								option=0;
							draw_sprite(menu_arrows[option],address);
              break;
						}
						//UP ARROW
            case 0x48:
            {
							background(address, img);
							if(option>0)
								option--;
							else
								option=3;
							draw_sprite(menu_arrows[option],address);
              break;
						}
            default:
              break;
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
  if (keyboard_unsubscribe_int() != 0)
   return -1;
  return option_choosen;
}

