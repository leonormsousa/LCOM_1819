#include <lcom/lcf.h>
#include "sprite.h"

#define H_RES 1024
#define V_RES 768
#define BLACK 0xFF


/** Creates a new sprite with pixmap "pic", with specified
* position (within the screen limits) and speed;
* Does not draw the sprite on the screen
* Returns NULL on invalid pixmap.
*/
Sprite * create_sprite(const char *pic[], int x, int y,
	int xspeed, int yspeed) {
	//allocate space for the "object"
	Sprite *sp = (Sprite *)malloc(sizeof(Sprite));
	if (sp == NULL)
		return NULL;
	// read the sprite pixmap
	sp->map = read_xpm(pic, &(sp->width), &(sp->height));
	if (sp->map == NULL) {
		free(sp);
		return NULL;
	}
	sp->x=x;
	sp->y=y;
	sp->xspeed=xspeed;
	sp->yspeed=yspeed;
	return sp;
}

void destroy_sprite(Sprite *sp) {
	if (sp == NULL)
		return;
	if (sp->map)
		free(sp->map);
	free(sp);
	sp = NULL; // XXX: pointer is passed by value
			   // should do this @ the caller
}

int animate_sprite(Sprite *sp)
{
	if (sp->xspeed !=0)
		sp->x+=sp->xspeed;
	if (sp->yspeed !=0)
		sp->y+=sp->yspeed;
return 0;
}

int draw_sprite(Sprite *sp, char *base)
{
	char *pixel = sp->map;
	for(int i=0;i < sp->height; i++)
	{
		if ((sp->y + i) >= V_RES)
			break;
		for(int j=0; j < sp->width; j++)
		{
			if((sp->x + j) >= H_RES)
			{
				pixel += (sp->x +j)-H_RES;
				break;
			}	
			else
			{
				char *ptr = base + (((sp->y+i)*H_RES)+(sp->x+j));
     		*ptr = *pixel;
	  		pixel++;
			}
		}
	}
	return 0;
}

int erase_sprite_screen(Sprite *sp, char *base)
{
	for(int i=0;i < sp->height; i++)
	{
		if ((sp->y + i) >= V_RES)
			break;
		for(int j=0; j < sp->width; j++)
		{
			if((sp->x + j) >= H_RES)
				break;

			else
			{
				char *ptr = base + (((sp->y+i)*H_RES)+(sp->x+j));
     		*ptr = BLACK;
			}
		}
	}
	return 0;
}
