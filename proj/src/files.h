#include <lcom/lcf.h>
#include <stdint.h>
#include "leaderboard.h"
#include "utility.h"

/**
 *  @author Leonor Sousa
 *  @author Sílvia Rocha
 */

#ifndef _FILES_H_
#define _FILES_H_

/** @defgroup Files
 * @{
 *
 * Reading and writing files related functions
 */

void write_in_file(FILE* highscores, Player leaderboard[]);

int write_highscores_in_files(Player leaderboard_classic[], Player leaderboard_extreme[]);

void read_file(FILE* highscores, Player leaderboard[]); 

int read_highscores_from_files(Player leaderboard_classic[], Player leaderboard_extreme[]);

/** @} end of Files */
#endif
