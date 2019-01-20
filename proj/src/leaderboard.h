#include "menu.h"
#include "rtc.h"

/**
 *  @author Leonor Sousa
 *  @author Sílvia Rocha
 */

#ifndef _LEADERBOARD_H_
#define _LEADERBOARD_H_

/** @defgroup leaderboard Leaderboard
 * @{
 *
 * Instructions and leaderboard display related functions
 */

typedef struct {
  unsigned char* name[10];
  uint32_t nameC[10];
  int score;
  unsigned char* scoreSP[4];
  Date dateI;
  unsigned char* date[16];
} Player;

void order_array(Player leaderboard[], int n);

void draw_instructions();

void draw_leaderboard(bool mode);

void draw_leaderboard_content(Player leaderboard[]);

int choose_info_menu(bool menu, char *init_adress, bool mode);



Player leaderboard_classic[10];
Player leaderboard_extreme[10];

Sprite* insert_name;
unsigned char* empty;

Sprite* player_letter1;
Sprite* player_letter2;
Sprite* player_letter3;
Sprite* player_letter4;
Sprite* player_letter5;
Sprite* player_letter6;
Sprite* player_letter7;
Sprite* player_letter8;
Sprite* player_letter9;
Sprite* player_letter10;
Sprite* player_name[10];

Sprite* player_uni;
Sprite* player_dez;
Sprite* player_cen;
Sprite* player_mil;
Sprite* player_scores[4];

Sprite* player_year_dez;
Sprite* player_year_uni;
Sprite* player_ponto1;
Sprite* player_month_dez;
Sprite* player_month_uni;
Sprite* player_ponto2;
Sprite* player_day_dez;
Sprite* player_day_uni;
Sprite* player_hour_dez;
Sprite* player_hour_uni;
Sprite* player_doispontos1;
Sprite* player_minute_dez;
Sprite* player_minute_uni;
Sprite* player_doispontos2;
Sprite* player_second_dez;
Sprite* player_second_uni;
Sprite* player_date[16];

/** @} end of Leaderboard */
#endif
