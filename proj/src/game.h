#include "mouse.h"
#include "serialPort.h"
#include "utility.h"
#include "files.h"

#define P_KEY 0x19

/**
 *  @author Leonor Sousa
 *  @author Sílvia Rocha
 */

#ifndef _GAME_H_
#define _GAME_H_

/** @defgroup Game
 * @{
 *
 * Functions related to game execution and update
 */

void decrease_active_figures_time();

void change_frequency();

void change_score(int player);

void change_time();

void change_keyboard_pointer(bool place);

void test_for_keyboard_hit(bool place);

void change_mouse_pointer(struct packet *pack, int num_players);

void test_for_hit(struct packet *pack);

void draw_score(int num_players);

void draw_time();

void draw_game(char *address, unsigned char* img, bool mode, int num_players);

void make_figure_appear(char* address);

void make_fallingObj_appear();

void move_fallingObj();

void draw_name();

Player create_leaderboard_player(Date date, char nameChar[], int score, Sprite** scoreSP);

bool update_leaderboard(int player, Date date, char* address, bool mode);

int subscribe_and_enable(int *irq_set_timer, int *irq_set_keyboard, int *irq_set_mouse, int *irq_set_serialPort, bool place);

int unsubscribe_and_disable(bool place);

int game(char *address, bool mode, int num_players, bool place);

void initialize_variables();

unsigned char* img;
unsigned char* good_figure;
unsigned char* bad_figure;

unsigned char* empty;
unsigned char* zero;
unsigned char* menos;
unsigned char* ponto;
unsigned char* doispontos;

Sprite* paused_menu;

Sprite* figure1;
Sprite* figure2;
Sprite* figure3;
Sprite* figure4;
Sprite* figure5;
Sprite* figure6;
Sprite* figure7;
Sprite* figure8;
Sprite* cursor;
Sprite* cursor_keyboard;
Sprite* figures[8];

Sprite* score1_display_uni;
Sprite* score1_display_dez;
Sprite* score1_display_cen;
Sprite* score1_display_mil;
Sprite* score_player1[4];

Sprite* score2_display_uni;
Sprite* score2_display_dez;
Sprite* score2_display_cen;
Sprite* score2_display_mil;
Sprite* score_player2[4];

Sprite* seconds;
Sprite* dec_seconds;
Sprite* cen_seconds;
Sprite* dois_pontos;
Sprite* tempo_de_jogo[4];

Sprite* pos1;
Sprite* pos2;
Sprite* pos3;
Sprite* pos4;
Sprite* pos5;
Sprite* pos6;
Sprite* pos7;
Sprite* pos8;
Sprite* fake_news[8];

Sprite* insert_name;
Sprite* name_letter1;
Sprite* name_letter2;
Sprite* name_letter3;
Sprite* name_letter4;
Sprite* name_letter5;
Sprite* name_letter6;
Sprite* name_letter7;
Sprite* name_letter8;
Sprite* name_letter9;
Sprite* name_letter10;
Sprite* name[10];

char nameChar[10];
int distance_y_tip_hammer;
int figure_heigth_to_aim;

Player empty_player;

/** @} end of Game */
#endif
