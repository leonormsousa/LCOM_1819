#include "game.h"

extern unsigned int bits_per_pixel;
extern void* second_buffer;
extern u32_t scan_code;
extern bool status_error;
extern int counter;
extern bool status_error_mouse;
extern u32_t scan_code_mouse;
extern int hook_mouse;
extern uint32_t scan_code_received;
static bool pause_game = false;
static int h_res = 800, v_res = 600;
static uint8_t frequency_appear = 70;
static uint8_t frequency_desappear = 180;
static uint8_t frequencyObj_appear = 90;
static int8_t bad_score = 20;
static int8_t good_score = -15;
static int player1_score = 0;
static int player2_score = 0;
static int n_figures = 8;
static int active_figures[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; /// the index represents the corresponding figure, the value represents how much time the figure will take to desappear
static int active_objects[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };/// the index represents the corresponding object (newspapper), the value represents if the object is active or not

/** Initializes every variable that will be needed for the development of the game
 */
void initialize_variables()
{
	distance_y_tip_hammer = 0.067*(double)v_res;
	figure_heigth_to_aim = 0.179*(double)v_res;
	paused_menu=create_sprite(pause_xpm,0.25*(double)h_res, 0.3125*(double)v_res,0,0);
	img = image_load(background_xpm);
	good_figure = image_load(figuraboa_xpm);
	bad_figure = image_load(figurama_xpm);
	//creates a sprite for every possible position for a figure to appear
	figure1 = create_sprite(figurama_xpm, 0.107*(double)h_res, 0.5556*(double)v_res, 0, -3);
	figure2 = create_sprite(figurama_xpm, 0.326*(double)h_res, 0.5556*(double)v_res, 0, -3);
	figure3 = create_sprite(figurama_xpm, 0.544*(double)h_res, 0.5556*(double)v_res, 0, -3);
	figure4 = create_sprite(figurama_xpm, 0.763*(double)h_res, 0.5556*(double)v_res, 0, -3);
	figure5 = create_sprite(figurama_xpm, 0.107*(double)h_res, 0.9028*(double)v_res, 0, -3);
	figure6 = create_sprite(figurama_xpm, 0.326*(double)h_res, 0.9028*(double)v_res, 0, -3);
	figure7 = create_sprite(figurama_xpm, 0.544*(double)h_res, 0.9028*(double)v_res, 0, -3);
	figure8 = create_sprite(figurama_xpm, 0.763*(double)h_res, 0.9028*(double)v_res, 0, -3);
	Sprite* figures_aux[8]={ figure1, figure2, figure3, figure4, figure5, figure6, figure7, figure8 };
	for (int i = 0; i < n_figures; i++)
		figures[i] = figures_aux[i];
	//creates a sprite for each cursor
	cursor = create_sprite(hammer_xpm, h_res / 2 + 100, v_res / 2, 0, 0);
	cursor_keyboard = create_sprite(hammer2_xpm, 123, 200, 0, 0);

	zero=image_load(zero_xpm);
	menos=image_load(menos_xpm);
	ponto=image_load(ponto_xpm);
	doispontos=image_load(doispontos_xpm);
	empty=image_load(empty_xpm);

	//creates the sprites to display the score
	score1_display_uni=create_sprite(zero_xpm, 0.685*(double)h_res+90,0.22*(double)v_res-25,0,0);
	score1_display_dez=create_sprite(empty_xpm, 0.685*(double)h_res+60,0.22*(double)v_res-25,0,0);
	score1_display_cen=create_sprite(empty_xpm, 0.685*(double)h_res+30,0.22*(double)v_res-25,0,0);
	score1_display_mil=create_sprite(empty_xpm, 0.685*(double)h_res,0.22*(double)v_res-25,0,0);
	score2_display_uni=create_sprite(zero_xpm, 0.175*(double)h_res+90,0.22*(double)v_res-25,0,0);
	score2_display_dez=create_sprite(empty_xpm, 0.175*(double)h_res+60,0.22*(double)v_res-25,0,0);
	score2_display_cen=create_sprite(empty_xpm, 0.175*(double)h_res+30,0.22*(double)v_res-25,0,0);
	score2_display_mil=create_sprite(empty_xpm, 0.175*(double)h_res,0.22*(double)v_res-25,0,0);
	Sprite* score1_aux[4] ={score1_display_uni, score1_display_dez, score1_display_cen, score1_display_mil};
	Sprite* score2_aux[4]={score2_display_uni, score2_display_dez, score2_display_cen, score2_display_mil};
	for (int i = 0; i < 4; i++)
	{
		score_player1[i] = score1_aux[i];
		score_player2[i] = score2_aux[i];
	}

	//creates the sprites to display the time
	seconds=create_sprite(um_xpm, 0.45*(double)h_res, 0.17*(double)v_res-25,0,0);
	dec_seconds=create_sprite(zero_xpm, 0.45*(double)h_res+60, 0.17*(double)v_res-25,0,0);
	cen_seconds=create_sprite(zero_xpm, 0.45*(double)h_res+90, 0.17*(double)v_res-25,0,0);
	dois_pontos=create_sprite(doispontos_xpm, 0.45*(double)h_res+30, 0.17*(double)v_res-25,0,0);
	Sprite* time_aux[4]={cen_seconds, dec_seconds, dois_pontos, seconds};
	for (int i = 0; i < 4; i++)
	{
		tempo_de_jogo[i]=time_aux[i];
	}

	//creates the sprites used to make objects (newspapper) appear
	pos1 = create_sprite(fake_news_xpm, 0, 0, 0, 3);
	pos2 = create_sprite(fake_news_xpm, 0.125*(double)h_res, 0, 0, 3);
	pos3 = create_sprite(fake_news_xpm, 0.250*(double)h_res, 0, 0, 3);
	pos4 = create_sprite(fake_news_xpm, 0.375*(double)h_res, 0, 0, 3);
	pos5 = create_sprite(fake_news_xpm, 0.5*(double)h_res, 0, 0, 3);
	pos6 = create_sprite(fake_news_xpm, 0.625*(double)h_res, 0, 0, 3);	
	pos7 = create_sprite(fake_news_xpm, 0.750*(double)h_res, 0, 0, 3);
	pos8 = create_sprite(fake_news_xpm, 0.875*(double)h_res, 0, 0, 3);
	Sprite* positions_aux[8] = { pos1, pos2, pos3, pos4, pos5, pos6, pos7, pos8 };
	for (int i = 0; i < 8; i++)
	{
		fake_news[i] = positions_aux[i];
	}
 	
	//creates the sprites used for the input of the player's name
	insert_name=create_sprite(insert_name_player1_xpm, 0.25*(double)h_res, 0.3125*(double)v_res,0,0);
	name_letter1= create_sprite(empty_xpm, 0.32*(double)h_res, 0.53*(double)v_res, 0,0);
	name_letter2= create_sprite(empty_xpm, 0.32*(double)h_res+30, 0.53*(double)v_res, 0,0);
	name_letter3= create_sprite(empty_xpm, 0.32*(double)h_res+60, 0.53*(double)v_res, 0,0);
	name_letter4= create_sprite(empty_xpm, 0.32*(double)h_res+90, 0.53*(double)v_res, 0,0);
	name_letter5= create_sprite(empty_xpm, 0.32*(double)h_res+120, 0.53*(double)v_res, 0,0);
	name_letter6= create_sprite(empty_xpm, 0.32*(double)h_res+150, 0.53*(double)v_res, 0,0);
	name_letter7= create_sprite(empty_xpm, 0.32*(double)h_res+180, 0.53*(double)v_res, 0,0);
	name_letter8= create_sprite(empty_xpm, 0.32*(double)h_res+210, 0.53*(double)v_res, 0,0);
	name_letter9= create_sprite(empty_xpm, 0.32*(double)h_res+240, 0.53*(double)v_res, 0,0);
	name_letter10= create_sprite(empty_xpm, 0.32*(double)h_res+270, 0.53*(double)v_res, 0,0);
	Sprite* name_aux[10]={name_letter1, name_letter2, name_letter3, name_letter4, name_letter5, name_letter6, name_letter7, name_letter8,name_letter9, name_letter10};
	for(int i =0;  i<10; i++)
	{
		name[i]=name_aux[i];
		nameChar[i]=0x0;
	}
}

/** Resets some variables to their original value to assure the right execution
 *  of the program when it is played multiple times in a row
 */
void reset_variables()
{
	for (int i = 0; i < n_figures; i++)
		active_figures[i]=0;
	player1_score=0;
	player2_score=0;
	counter=0;
}

/**Decreases the amount of time that a figure will remain active 
 */
void decrease_active_figures_time()
{
	for (int i = 0; i < n_figures; i++)
	{
		if (active_figures[i] > 0)
			active_figures[i]--;
	}
}

/**Changes the frequency with which a figure appears and desappears
 */ 
void change_frequency()
{
	if (counter == (30 * 60))
	{
		frequency_appear = 50;
		frequency_desappear = 130;
	}
	if (counter == (50 * 60))
	{
		frequency_appear = 35;
		frequency_desappear = 90;
	}
}
 
void change_score_aux(Sprite* score_player[], int player_score)
{
	int score=abs(player_score);
		for(int i=1; i<4;i++)
			score_player[i]->map=empty;
		score_player[0]->map=zero;
		if(player_score<0)
			score_player[3]->map=menos;
		for(int i=0; i<4;i++)
		{
			if(score==0)
				break;
			score_player[i]->map=int_to_img(score);
			score /= 10;
		}
}

/**Changes the display of a player's score
 */
void change_score(int player)
{
	if(player==2)
		change_score_aux(score_player2, player2_score);
	if(player==1)	
		change_score_aux(score_player1, player1_score);
}

/**Changes the display of the time remaining
 */
void change_time()
{
	if(counter%60!=0)
		return;
	int time_remaining=60-(counter/60);
	tempo_de_jogo[3]->map=zero;
	for(int i=0; i<2;i++)
		tempo_de_jogo[i]->map=zero;
	for(int i=0; i<2;i++)
	{
		if(time_remaining==0)
			break;
		tempo_de_jogo[i]->map=int_to_img(time_remaining);
		time_remaining /= 10;	
	}
}

void change_keyboard_pointer_aux(Sprite* kbc_cursor)
{
	int aux;
		// 'D' key pressed
		if(scan_code==0x20)
		{
			aux=kbc_cursor->x;
			kbc_cursor->x += 175;
			if(kbc_cursor->x>h_res)
				kbc_cursor->x=123;
			if(test_collision(kbc_cursor, cursor))
				kbc_cursor->x=aux;
		} 
		// 'A' key pressed
		else if(scan_code==0x1e)
		{
			aux=kbc_cursor->x;
			kbc_cursor->x-=175;
			if(kbc_cursor->x<0)
				kbc_cursor->x=648; 
			if(test_collision(kbc_cursor, cursor))
				kbc_cursor->x=aux;
		}
		// 'S' key pressed 
		else if(scan_code==0x1f)
		{
			aux=kbc_cursor->y;
			kbc_cursor->y += 200;
			if(kbc_cursor->y>=v_res)
				kbc_cursor->y=200;
			if(test_collision(kbc_cursor, cursor))
				kbc_cursor->y=aux;
		}
		// 'W' key pressed
		else if(scan_code==0x11)
		{
			aux=kbc_cursor->y;
			kbc_cursor->y -= 200;
			if(kbc_cursor->y<200)
				kbc_cursor->y=400;
			if(test_collision(kbc_cursor, cursor))
				kbc_cursor->y=aux;
		}
}

/**Changes the keyboard controlled hammer position accordingly 
 * to the scan code received and checking if the movement will 
 * originate a collision
 */
void change_keyboard_pointer(bool place)
{
	//if the scan code comes from the local player
	if(place)
		change_keyboard_pointer_aux(cursor_keyboard);
	//if the scan code comes from a player on another computer
	else
		change_keyboard_pointer_aux(cursor);
}

/**Tests if the keyboard controlled hammer is in position to hit a figure and 
 * if the hit key (spacebar) is pressed
 */
void test_for_keyboard_hit(bool place)
{
	bool cursor_keyboard_ready = false;
	for (int i = 0; i < 8; i++)
	{
		if (active_figures[i] == 0)
			continue;
		//checks if the hammer is on the right position
		if ((cursor_keyboard->x >= figures[i]->x) && (cursor_keyboard->x <= figures[i]->x + figures[i]->width) && (cursor_keyboard->y + distance_y_tip_hammer >= figures[i]->y) && (cursor_keyboard->y + distance_y_tip_hammer <= figures[i]->y + figure_heigth_to_aim))
			cursor_keyboard_ready = true;
		//checks if the previous condition is fulfilled and the spacebar key is pressed
		if((scan_code==0x39) && cursor_keyboard_ready)
		{
			//if the scan code comes from the local player
			if(place)
			{
				if (figures[i]->xspeed > 0)
					player2_score += bad_score;
				else
					player2_score += good_score;
				change_score(2);
			}
			//if the scan code comes from a player on another computer
			else
			{
				if (figures[i]->xspeed > 0)
					player1_score += bad_score;
				else
					player1_score += good_score;
				change_score(1);
			}
			//makes the figure that was hit desappear
			active_figures[i] = 0;
			break;
		}
		else
			cursor_keyboard_ready = false;
	}
}

/**Changes the mouse controlled hammer position accordingly 
 * to the displacements received on the packet and checking if 
 * the displacement will originate a collision
 */
void change_mouse_pointer(struct packet *pack, int num_players)
{
	int aux;
	//verifies if the delta_x value has signal
	if (pack->delta_x > 0) 
	{
		aux=cursor->x;
		cursor->x = min(cursor->x + pack->delta_x,h_res-91);
		if(test_collision(cursor, cursor_keyboard)&& num_players==2)
			cursor->x=aux;
	}	
	else
	{
		aux=cursor->x;
		cursor->x = max(cursor->x + pack->delta_x, 0);
		if(test_collision(cursor, cursor_keyboard) && num_players==2)
			cursor->x=aux;
	}
	//verifies if the delta_y value has signal
	if (pack->delta_y > 0)
	{
		aux=cursor->y;
		cursor->y = max(cursor->y - pack->delta_y, 0);
		if(test_collision(cursor, cursor_keyboard)&& num_players==2)
			cursor->y=aux;
	}
	else
	{
		aux=cursor->y;
		cursor->y = min(cursor->y - pack->delta_y, v_res-116);	
		if(test_collision(cursor, cursor_keyboard)&& num_players==2)
			cursor->y=aux;
	}
}

/**Tests if the mouse controlled hammer is in position to hit a figure and 
 * if the hit button (left button) is pressed
 */
void test_for_hit(struct packet *pack)
{
	bool cursor_ready = false;
	for (int i = 0; i < 8; i++)
	{
		if (active_figures[i] == 0)
			continue;
		//checks if the hammer is on the right position
		if ((cursor->x >= figures[i]->x) && (cursor->x <= figures[i]->x + figures[i]->width) && (cursor->y + distance_y_tip_hammer >= figures[i]->y) && (cursor->y + distance_y_tip_hammer <= figures[i]->y + figure_heigth_to_aim))
			cursor_ready = true;
		//checks if the previous condition is fulfilled and the left button is pressed
		if((pack->lb == 1) && cursor_ready)
		{
			if (figures[i]->xspeed > 0)
				player1_score += bad_score;
			else
				player1_score += good_score;
			//makes the figure that was hit desappear	
			active_figures[i] = 0;
			change_score(1);
			break;
		}
		else
			cursor_ready = false;
	}
}

/**Draws the score of one or both players depending on the argument received
 */
void draw_score(int num_players)
{
	if(num_players==2)
	{
		for(int i=0; i<4;i++)
			draw_sprite(score_player2[i], second_buffer);
	}
	for(int i=0; i<4;i++)
		draw_sprite(score_player1[i], second_buffer);
}

/**Draws the remaining time 
 */
void draw_time()
{
	for(int i=0; i<4;i++)
		draw_sprite(tempo_de_jogo[i], second_buffer);
}

/**Draws every aspect of the game i.e. the background, figures, hammers, newspappers, scores and time remaining
 */
void draw_game(char *address, unsigned char* img, bool mode, int num_players)
{
	background(second_buffer, img);
	//draws figures
	for (int i = 0; i < n_figures; i++)
	{
		if (active_figures[i] != 0)
			draw_sprite(figures[i],second_buffer);
	}
	//draws the objects (newspappers)
	if(!mode)
	{
		for (int j = 0; j < 8; j++)
		{
			if (active_objects[j] != 0)
				draw_sprite(fake_news[j],second_buffer);
		}
	}
	if(num_players==2)
		draw_sprite(cursor_keyboard, second_buffer);
	draw_sprite(cursor, second_buffer);
	draw_score(num_players);
	draw_time();
	if(pause_game)
		draw_sprite(paused_menu, second_buffer);
	memcpy(address,second_buffer,h_res*v_res*((bits_per_pixel+7)/8));
}

/**Makes a new figure appear in specific frequencys
 */
void make_figure_appear(char* address)
{
	//if it is time to draw a new figure
	if (counter%frequency_appear == 0)
	{
		//randomly decides where the figure will appear
		unsigned int n_figure = rand() % 8;
		while (active_figures[n_figure] != 0)
			n_figure = rand() % n_figures;
		
		//resets the sprite selected to its original position
		if(figures[n_figure]->y!=0.5556*(double)v_res && figures[n_figure]->y!=0.9028*(double)v_res)
		{
			if(n_figure<4)
				figures[n_figure]->y=0.5556*(double)v_res;
			else
				figures[n_figure]->y=0.9028*(double)v_res;
		}
		//defines how much time the figure will remain active
		active_figures[n_figure] = frequency_desappear;
		//randomly decides if the figure that is going to appear is a good or bad figure
		bool bad = rand() % 2;
		if (bad)
		{
			figures[n_figure]->map = bad_figure;
			figures[n_figure]->xspeed = 1;
		}
		else
		{
			figures[n_figure]->map = good_figure;
			figures[n_figure]->xspeed = -1;
		}
		//draws the figure with some delay to give the idea of movement instead of just appearing
		for(int j=0; j<5; j++)
		{
			tickdelay(1);
			figures[n_figure]->y = figures[n_figure]->y - 30;
			if(n_figure<4)
				draw_fig(figures[n_figure], address,0.5556*(double)v_res);
			else
			 	draw_fig(figures[n_figure], address,0.9028*(double)v_res);
		}
	}
}

/**Makes a new object(newspapper) appear in specific frequencys
 */
void make_fallingObj_appear()
{
	if (counter%frequencyObj_appear == 0)
	{
		//definew where the figure should appear
		unsigned int position = rand() % 8;
		while (active_objects[position] != 0)
			position = rand() % n_figures;
		//sets the figure as active
		active_objects[position] = 1;
	}
}

/**Changes the current position of an object (newspapper) checking for collisions
 *  or if the object as reached the end of the screen
 */
void move_fallingObj()
{
	for(int i=0; i<8;i++)
	{
		if(active_objects[i]!=0)
		{
			fake_news[i]->y ++;
			//if a collision with on of the hammers occurs the object desappears and the respective player loses points
			if(test_collision(fake_news[i], cursor_keyboard)|| test_collision(cursor_keyboard,fake_news[i]))
			{
				fake_news[i]->y=0;
				active_objects[i]=0;
				player2_score= player2_score - 10;
				change_score(2);
			}
			if(test_collision(fake_news[i],cursor) || test_collision(cursor,fake_news[i])) 
			{
				fake_news[i]->y=0;
				active_objects[i]=0;
				player1_score= player1_score - 10;
				change_score(1);
			}
			//if the end of the screen is reached, the object's position is reset to the beggining of the screen and the object is marked as inactive
			if((fake_news[i]->y >= v_res))
			{
				fake_news[i]->y=0;
				active_objects[i]=0;
			}
		}
	}	
}

/**Draws on the screen the name(text input) of the player as the player is writing it 
 */
void draw_name()
{
  for(int i=0; i<10;i++)
		draw_sprite(name[i], second_buffer);
}

/**Creates a new player with the data received i.e. the text input, the score obtained and the date and time at which the player entered the leaderboard 
 */
Player create_leaderboard_player(Date date, char nameChar[], int score, Sprite** scoreSP)
{
	Player new_player;
	//initializes the names of the player
	size_t size = sizeof(name)/sizeof(name[0]);
	for(size_t i=0; i< size;i++)
	{
		new_player.name[i]=name[i]->map;
		new_player.nameC[i]=nameChar[i];
	}
	//initializes the scores of the player
 	new_player.score=score;
	for(int j=0; j<4;j++)
		new_player.scoreSP[j]=scoreSP[j]->map;
	//initializes the dates of the player
	new_player.date[0]=int_to_img(date.year/10);
	new_player.date[1]=int_to_img(date.year);
	new_player.date[2]=ponto;
	new_player.date[3]=int_to_img(date.month/10);
	new_player.date[4]=int_to_img(date.month);
	new_player.date[5]=ponto;
	new_player.date[6]=int_to_img(date.day/10);
	new_player.date[7]=int_to_img(date.day);
	new_player.date[8]=int_to_img(date.hour/10);
	new_player.date[9]=int_to_img(date.hour);
	new_player.date[10]=doispontos;
	new_player.date[11]=int_to_img(date.minute/10);
	new_player.date[12]=int_to_img(date.minute);
	new_player.date[13]=doispontos;
	new_player.date[14]=int_to_img(date.second/10);
	new_player.date[15]=int_to_img(date.second);
	new_player.dateI.year=date.year;
	new_player.dateI.month=date.month;
	new_player.dateI.day=date.day;
	new_player.dateI.hour=date.hour;
	new_player.dateI.minute=date.minute;
	new_player.dateI.minute=date.minute;
	new_player.dateI.second=date.second;
	new_player.dateI.second=date.second;
	for(int i=0; i<10;i++)
	 	name[i]->map=empty;
	return new_player;
}

/**Checks if the player made a high enough score to enter the leaderboard. If so, displays a screen and asks for the player to enter his name
 */
bool update_leaderboard(int player, Date date, char* address, bool mode)
{
	int score;
	Sprite** scoreSP;
	scan_code=0;
	//collects the score data of the player
	if(player==1)
	{
		score=player1_score;
		scoreSP=score_player1;
		insert_name->map=image_load(insert_name_player1_xpm);
	}
	if(player==2)
	{
		score=player2_score;
		scoreSP=score_player2;
		insert_name->map=image_load(insert_name_player2_xpm);
	}
	//checks if the score is high enough
	if(!mode)
	{
		if(score<=leaderboard_extreme[9].score)
    	return false;
	}
	else
	{
		if(score<=leaderboard_classic[9].score)
    	return false;
	}
	u8_t r;
	message msg;
	if (keyboard_subscribe_int(&r) != 0)
	return 1;
	int irq_set_keyboard = BIT(r);
	int ipc_status;
	uint8_t counter_letters=0;
	while (scan_code != 0x1c) 
	{
    	draw_sprite(insert_name, second_buffer);
    	draw_name();
		memcpy(address,second_buffer,h_res*v_res*((bits_per_pixel+7)/8));
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
					//if the player wants to delete a letter (the backspace key was pressed)
					if (scan_code==0x0e && counter_letters>0) 
					{
						name[counter_letters-1]->map=empty;
						counter_letters--;
					}
					//if the maximum number of letters was reached 
					else if (counter_letters==10)
						break;
					else
					{
						//adds the new letter to the player's name
						unsigned char* letter= scancode_to_img(scan_code);
						if (letter != NULL)
						{
							nameChar[counter_letters]=scan_code;
							name[counter_letters]->map=letter;
							counter_letters++;
						}
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
    return false;
  //adds the player to the corresponding leaderboard
  if(!mode)
	{
		leaderboard_extreme[9]=create_leaderboard_player(date, nameChar, score, scoreSP);
		order_array(leaderboard_extreme, 10);
	}
	else 
	{
		leaderboard_classic[9]=create_leaderboard_player(date, nameChar, score, scoreSP);
	  order_array(leaderboard_classic, 10);
	}
  return true;
}

/**Subscribe of the interruptions of the various devices and enable of data reporting
 */
int subscribe_and_enable(int *irq_set_timer, int *irq_set_keyboard, int *irq_set_mouse, int *irq_set_serialPort, bool place)
{
	u8_t r=0;
	if (keyboard_subscribe_int(&r) != 0)
		return 1;
	*irq_set_keyboard = BIT(r);
	if (timer_subscribe_int(&r) != 0)
		return 1;
	*irq_set_timer = BIT(r);
	if (mouse_subscribe_int(&r) !=0)
    	return 1;
  *irq_set_mouse= BIT(r);
  sys_irqdisable(&hook_mouse);
  if(mouse_enable_data_report()!=0)
   		return 1;
  sys_irqenable(&hook_mouse);    
  if (!place)
  {           
	if(serialPort_subscribe_int(&r)!=0)
    return 1;
  if (configure_serialPort() != 0)
	  return 1;
	*irq_set_serialPort= BIT(r);
  }
	return 0;
}

/**Unsubscribe of the interruptions of the various devices and disable of data reporting
 */
int unsubscribe_and_disable(bool place)
{
	if (keyboard_unsubscribe_int() != 0)
    return 1;
	if(timer_unsubscribe_int() != 0)
    return 1;
	sys_irqdisable(&hook_mouse);
 	if (mouse_disable_data_report()!=0)
    	return 1;
 	if(mouse_unsubscribe_int() != 0)
    	return 1;
  	if(out_buf_read()!=0)
    	return 1;
	if(!place)
	{
		if(serialPort_unsubscribe_int()!=0)
			return 1;
	}
  return 0;   
}

/**Game's main function. Handles every interruption calling the corresponding functions including the draw functions. Saves the new leaderboard information 
 */
int game(char *address, bool mode, int num_players, bool place)
{
	printf("game beggining \n");
	initialize_variables();
	draw_game(address, img, mode, num_players);
	printf("after first draw \n");
	int ipc_status;
	u8_t r;
	message msg;
	int irq_set_timer, irq_set_keyboard, irq_set_mouse, irq_set_serialPort;
	if (subscribe_and_enable(&irq_set_timer, &irq_set_keyboard, &irq_set_mouse, &irq_set_serialPort, place) != 0)
		return 1;
	while (scan_code != 0x81 && counter<=60*60)
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
			draw_game(address, img, mode, num_players);
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE:
				if (msg.m_notify.interrupts & irq_set_timer)
				{
					if (!pause_game)
					{
						timer_int_handler();
						change_time();
						decrease_active_figures_time();
						make_figure_appear(address);
						change_frequency();
						if(!mode)
						{
							make_fallingObj_appear();
							move_fallingObj();
						}
					}
				}
				if (msg.m_notify.interrupts & irq_set_keyboard)
				{
					kbc_ih();
					if (status_error)
						break;
					if (scan_code == P_KEY)
						pause_game = !pause_game;
					else
					{
						if(!pause_game)
						{
							change_keyboard_pointer(true);
							test_for_keyboard_hit(true);
						}
						if(!place)
						{
							write_message(scan_code);
							get_acknowledgement_byte();

						}
					}
				}
				if (msg.m_notify.interrupts & irq_set_mouse)
				{
				 	mouse_ih();
				 	if (status_error_mouse)
				 		break;
				 	struct packet pack;
				 	parse_packet(&pack);
				 	if (!pause_game && place)
					{
				 		change_mouse_pointer(&pack, num_players);
				 		test_for_hit(&pack);
				 	}
				}
				if (msg.m_notify.interrupts & irq_set_serialPort)
				{
					serialPort_ih();
					if(scan_code_received!=0 && !place)
					{
						if (scan_code == P_KEY)
							pause_game = !pause_game;
						else if(!pause_game)
						{
							change_keyboard_pointer(false);
							test_for_keyboard_hit(false);
						}
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
	tickdelay(20);
	//calls the update_leaderboard() function to test if the players made it to the leaderboard
	Date date=read_date();
	unsubscribe_and_disable(place);
	update_leaderboard(1, date, address,mode);
	if(num_players==2)
	{
		date=read_date();
		update_leaderboard(2, date, address,mode);
	}
	//saves the new leaderboard data on a file
	write_highscores_in_files(leaderboard_classic, leaderboard_extreme);
	reset_variables();
 	return 0;
}
