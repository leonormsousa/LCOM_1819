#include <lcom/lcf.h>
/**
 *  @author Joao Cardoso (jcard@fe.up.pt)
 *  Added by pfs@fe.up.pt
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

/** @defgroup sprite Sprite
 * @{
 *
 * Sprite related functions
 */

/** A Sprite is an "object" that contains all needed information to
 * create, animate, and destroy a pixmap.  The functions assume that
 * the background is BLACK and they take into account collision with
 * other graphical objects or the screen limits. 
 */
typedef struct {
  int x,y;             /**< current sprite position */
  int width, height;   /**< sprite dimensions */
  int xspeed, yspeed;  /**< current speeds in the x and y direction */
  unsigned char *map;           /**< the sprite pixmap (use read_xpm()) */
} Sprite;


Sprite * create_sprite(xpm_string_t pic[], int x, int y,	int xspeed, int yspeed);

int draw_fig(Sprite *sp,char *base, int v_res_lim);

unsigned char* image_load(xpm_string_t pic[]);

void destroy_sprite(Sprite *sp);

int draw_sprite(Sprite *sp, char *base);


void background( char*init_adress, unsigned char *imagem);

bool test_collision_pixel(Sprite *sp1, Sprite *sp2, int x1, int y1, int x2, int y2, int width, int height);

bool test_collision(Sprite *sp1, Sprite *sp2);

/** @} end of sprite */
#endif
