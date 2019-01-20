#include "leaderboard.h"

extern unsigned int bits_per_pixel;
extern void* second_buffer;
extern u32_t scan_code;
extern bool status_error;
extern int counter;
extern int hook_keyboard;
static int h_res = 800, v_res = 600;

/** Initializes every variable that will be needed for the development of the game
 */
void initializing_variables()
{
  //creates the sprites to display the names of the players
  player_letter1= create_sprite(empty_xpm, 0.04*(double)h_res, 0.34*(double)v_res,0,0);
  player_letter2= create_sprite(empty_xpm, 0.075*(double)h_res, 0.34*(double)v_res,0,0);
  player_letter3= create_sprite(empty_xpm, 0.11*(double)h_res, 0.34*(double)v_res,0,0);
  player_letter4= create_sprite(empty_xpm, 0.145*(double)h_res, 0.34*(double)v_res,0,0);
  player_letter5= create_sprite(empty_xpm, 0.18*(double)h_res, 0.34*(double)v_res,0,0);
  player_letter6= create_sprite(empty_xpm, 0.215*(double)h_res, 0.34*(double)v_res,0,0);
  player_letter7= create_sprite(empty_xpm, 0.25*(double)h_res, 0.34*(double)v_res,0,0);
  player_letter8= create_sprite(empty_xpm, 0.285*(double)h_res, 0.34*(double)v_res,0,0);
  player_letter9= create_sprite(empty_xpm, 0.32*(double)h_res, 0.34*(double)v_res,0,0);
  player_letter10= create_sprite(empty_xpm, 0.355*(double)h_res, 0.34*(double)v_res,0,0);
  Sprite* player_name_aux[10]={player_letter1, player_letter2, player_letter3, player_letter4, player_letter5, player_letter6, player_letter7, player_letter8, player_letter9, player_letter10};
  for(int i=0; i<10; i++)
    player_name[i]=player_name_aux[i];

  //creates the sprites to display the scores of the players
  player_uni=create_sprite(empty_xpm, 0.52*(double)h_res,0.34*(double)v_res,0,0);
	player_dez=create_sprite(empty_xpm, 0.50*(double)h_res,0.34*(double)v_res,0,0);
	player_cen=create_sprite(empty_xpm, 0.48*(double)h_res,0.34*(double)v_res,0,0);
	player_mil=create_sprite(empty_xpm, 0.46*(double)h_res,0.34*(double)v_res,0,0);
	Sprite* player_scores_aux[4]={player_uni, player_dez, player_cen, player_mil};
	for (int i = 0; i < 4; i++)
    player_scores[i]=player_scores_aux[i];
  
  //creates the sprites to display the dates of the players
  player_ponto1=create_sprite(empty_xpm, 0.65*(double)h_res, 0.34*(double)v_res,0,0);
  player_ponto2=create_sprite(empty_xpm, 0.71*(double)h_res, 0.34*(double)v_res,0,0);
  player_doispontos1=create_sprite(empty_xpm, 0.835*(double)h_res, 0.34*(double)v_res,0,0);
  player_doispontos2=create_sprite(empty_xpm, 0.895*(double)h_res, 0.34*(double)v_res,0,0);
  player_year_dez=create_sprite(empty_xpm, 0.61*(double)h_res, 0.34*(double)v_res, 0,0);
  player_year_uni=create_sprite(empty_xpm, 0.63*(double)h_res, 0.34*(double)v_res, 0,0);
  player_month_dez=create_sprite(empty_xpm, 0.67*(double)h_res, 0.34*(double)v_res, 0,0);
  player_month_uni=create_sprite(empty_xpm, 0.69*(double)h_res, 0.34*(double)v_res, 0,0);
  player_day_dez=create_sprite(empty_xpm, 0.73*(double)h_res, 0.34*(double)v_res, 0,0);
  player_day_uni=create_sprite(empty_xpm, 0.75*(double)h_res, 0.34*(double)v_res, 0,0);
  player_hour_dez=create_sprite(empty_xpm, 0.795*(double)h_res, 0.34*(double)v_res, 0,0);
  player_hour_uni=create_sprite(empty_xpm, 0.815*(double)h_res, 0.34*(double)v_res, 0,0);
  player_minute_dez=create_sprite(empty_xpm, 0.855*(double)h_res, 0.34*(double)v_res, 0,0);
  player_minute_uni=create_sprite(empty_xpm, 0.875*(double)h_res, 0.34*(double)v_res, 0,0);
  player_second_dez=create_sprite(empty_xpm, 0.915*(double)h_res, 0.34*(double)v_res, 0,0);
  player_second_uni=create_sprite(empty_xpm, 0.935*(double)h_res, 0.34*(double)v_res, 0,0);
  Sprite* player_date_aux[16]={player_year_dez, player_year_uni, player_ponto1, player_month_dez, player_month_uni, player_ponto2, player_day_dez, player_day_uni, player_hour_dez, player_hour_uni, player_doispontos1, player_minute_dez, player_minute_uni, player_doispontos2, player_second_dez, player_second_uni};
  for(int i=0;i<16;i++)
    player_date[i]=player_date_aux[i];
}

/** Orders the leaderboard accordingly to the players scores
 */
void order_array(Player leaderboard[], int n)
{
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)             
    {
      if (leaderboard[j].score < leaderboard[i].score)
      {
        Player tmp = leaderboard[i];
        leaderboard[i] = leaderboard[j];
        leaderboard[j] = tmp;
      }
    }
  }
}

/** Draws on the screen the instructions of the game
 */
void draw_instructions()
{
  unsigned char* img = image_load(instructions_xpm);
  background(second_buffer, img);
}

/** Draws the content of the leaderboard passed as argument i.e. the name, 
 * score and date of each player
 */
void draw_leaderboard_content(Player leaderboard[])
{
    for(int i=0; i<10;i++)
    {
      for(int j=0;j<10;j++)//draws the names
      {
  
        if(leaderboard[i].score!=-2000)
          player_name[j]->map=leaderboard[i].name[j];
        else
          player_name[j]->map=empty;
        draw_sprite(player_name[j], second_buffer);
        player_name[j]->y += 0.0585*(double)v_res;
      }
      for(int j=0; j<4;j++)//draws the scores
      {
        if(leaderboard[i].score!=-2000)
          player_scores[j]->map=leaderboard[i].scoreSP[j];
        else
          player_scores[j]->map=empty;
        draw_sprite(player_scores[j], second_buffer);
        player_scores[j]->y += 0.0585*(double)v_res;
      }
      for(int j=0; j<16;j++)//draws the dates
      {
        if(leaderboard[i].score!=-2000)
          player_date[j]->map=leaderboard[i].date[j];
        else
          player_date[j]->map=empty;
        draw_sprite(player_date[j], second_buffer);
        player_date[j]->y += 0.0585*(double)v_res; 
      }
    }
}

/** Draws the corresponding leaderboard background
 */
void draw_leaderboard(bool mode)
{
  //extreme mode
  if (!mode)
  {
    unsigned char* img = image_load(leaderboard_extreme_xpm);
    background(second_buffer, img);
    draw_leaderboard_content(leaderboard_extreme);
  }
  //classic mode
  else
  {
    unsigned char* img = image_load(leaderboard_classic_xpm);
    background(second_buffer, img);
    draw_leaderboard_content(leaderboard_classic);
  }
}

/** Depending on the argument menu this functions draws the instructions(menu=true) or the leaderboard(menu=false) and awaits for the user to press the ESC key to leave the choosen menu
 */
int choose_info_menu(bool menu, char *address, bool mode)
{
  //decides which menu to draw
  if (menu)
    draw_instructions();
  else 
    {
      initializing_variables();
      draw_leaderboard(mode);
    }
  memcpy(address, second_buffer, h_res*v_res*((bits_per_pixel+7)/8));
	int ipc_status;
	u8_t r;
	message msg;
	if (keyboard_subscribe_int(&r) != 0)
		return 1;
  int irq_set_keyboard = BIT(r);
  while (scan_code != 0x81)
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
   return 1;
  return 0;
}
