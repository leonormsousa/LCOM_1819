#include <stdio.h>
#include <stdlib.h>
#include "sprite.h"
#include "keyboard.h"
#include "pixmaps.h"
#include "i8254.h"

/**
 *  @author Leonor Sousa
 *  @author Sílvia Rocha
 */

#ifndef _MENU_H_
#define _MENU_H_

/** @defgroup Menu
 * @{
 *
 * Main menu and submenus display related functions
 */

Sprite* arrow_option1;
Sprite* arrow_option2;
Sprite* arrow_option3;
Sprite* arrow_option4;

int menu(char *address);

int choose_option(char* address, char menu);

/** @} end of Menu */
#endif
