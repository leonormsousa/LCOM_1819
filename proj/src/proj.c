#include "game.h"

/**
 *  @author Leonor Sousa
 *  @author Sílvia Rocha
 */

#ifndef _PROJ_H_
#define _PROJ_H_

/** @defgroup Proj
 * @{
 *
 * Root of the project related functions
 */

extern uint8_t scan_code_received;
int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

/** Creates an empty player and initializes every position of the leaderboard with it
 */
void initialize_emptyLeaderboard()
{
	empty_player.score=-2000;
	for(int i=0; i<4;i++)
	 	empty_player.scoreSP[i]=empty;
	for(int i=0; i<10;i++)
	{
    empty_player.name[i]=empty;
    empty_player.nameC[i]=0x0;
  }
	for(int i=0; i<16;i++)
	 	empty_player.date[i]=empty;
  Player leaderboard_aux[10]={empty_player, empty_player, empty_player, empty_player, empty_player, empty_player, empty_player, empty_player, empty_player, empty_player};
  for(int i=0;i<10;i++)
  {
    leaderboard_extreme[i]=leaderboard_aux[i];
    leaderboard_classic[i]=leaderboard_aux[i];
  }
}

/** Root of the whole project. Contains a loop that calls the main menu at every iteration until the exit option is choosen. The argument argv[0][0] is used to decide (when the players are playing in diferent computers) who is player 1 and who is player 2
 */
int (proj_main_loop)(int argc, char* argv[])
{
  //if there is an invalid number of arguments
	if(argc>1)
  {
    printf("Invalid number of arguments \n");
    return 1;
  }
  if((argc!=0) && (argv[0][0]!= '1') && (argv[0][0]!='2'))
  {
    printf("Invalid argument \n");
    return 1;
  }
  char* init_address=vg_init(0x115);
  int option=-1;
  initialize_emptyLeaderboard();
  initialize_variables();
  read_highscores_from_files(leaderboard_classic, leaderboard_extreme);
  //if the user is player 1
  if(argv[0][0]!='2')
  {
    u8_t r;
    while(option!=3)
    { 
      option= menu(init_address);
      switch(option)
      {
        int num_players;
        bool mode= true;
        case -1:
          return 0;
        case 0:
          //choose the number of players menu
          option=choose_option(init_address,'p');
          if(option==-1)
            break;
          num_players=option;
          //choose the game mode menu
          option=choose_option(init_address, 'm');
          if(option==-1)
            break;
          switch(option)
          {
            case 1:
              mode=true;
              break;
            case 2:
              mode=false;
              break;
            default:
              break;
          }
          if(num_players==2 && argc!=0)
          {
            //choose if you want to play locally or not
            option=choose_option(init_address, 's');
            switch(option)
            {
              case 1:
                game(init_address, mode, 2, true);
                break;
              case 2:
                //sends player 2 a notification to start the game on the right mode
                if(serialPort_subscribe_int(&r)!=0)
                  return 1;
                if (configure_serialPort() != 0)
		              return 1;
                if(mode)
                  write_message(0x01);
                else
                   write_message(0x02);
                if(serialPort_unsubscribe_int()!=0)
                  return 1;
                game(init_address, mode, 2, false);
                break;
              default:
                break;
            }
          }
          else if(num_players==2)
            game(init_address, mode, 2, true);     
          else
            game(init_address, mode, 1, true);     
          break;
        case 1:
          //instructions display
          choose_info_menu(true, init_address, true);
          break;
        case 2:
        //leaderboard display
          option=choose_option(init_address, 'l');
          switch(option)
          {
            case 1:
              choose_info_menu(false, init_address,true);
              break;
            case 2:
              choose_info_menu(false, init_address,false);
              break;
            default:
            break;
          }
          break;
        default:
          break;
      }
    }
  }
  //if the user is player 2 the program awaits a UART interruption in order to synchronize the game 
  if(argv[0][0]=='2')
  {
    bool mode=false, exit=false;
    int ipc_status,byte=0;
	  u8_t r;
	  message msg;
	  if (serialPort_subscribe_int(&r) != 0)
		  return 1;
    if (configure_serialPort() != 0)
			return 1;
    int irq_set_serialPort = BIT(r);
    while (!exit)
    {
      if (configure_serialPort() != 0)
			  return 1;
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
          if (msg.m_notify.interrupts & irq_set_serialPort)
          {
            serialPort_ih();
            byte++;
            if(scan_code_received==0x01)
            {
              mode=true;
            }
            if(scan_code_received==0x02)
            {
              mode=false;
            }
            if(scan_code_received==0x03)
            {
              exit=true;
            }
            if(byte==3)
            {
              byte=0;
              if(serialPort_unsubscribe_int()!=0)
                return 1;
              tickdelay(100);
              game(init_address, mode,2, false);
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
    serialPort_unsubscribe_int();
  }
  background(init_address, image_load(credits_xpm));
  sleep(5);
  vg_exit();
  return 0;
}

/** @} end of Proj */
#endif
