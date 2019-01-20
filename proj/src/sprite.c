#include <lcom/lcf.h>
#include "sprite.h"
#include <lcom/read_xpm.h>

#define H_RES 800
#define V_RES 600
#define BLACK 0xFF


/** Creates a new sprite with pixmap pic, with specified
* position (within the screen limits) and speed;
* Returns NULL on invalid pixmap.
*/
Sprite * create_sprite(xpm_string_t pic[], int x, int y,
	int xspeed, int yspeed) {
	enum xpm_image_type type = XPM_8_8_8;
	xpm_image_t img;
	xpm_load(pic, type, &img);
	Sprite *sp = (Sprite *)malloc(img.width*img.height*3);
	if (sp == NULL)
		return NULL;
	sp->map =xpm_load(pic, type, &img);
	sp->width=img.width;
	sp->height=img.height;
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

/** Loads a xpm to a variable of type unsigned char*
 */
unsigned char* image_load(xpm_string_t pic[])
{
	enum xpm_image_type type = XPM_8_8_8;
	xpm_image_t img;
	return xpm_load(pic, type, &img);
}

/** The sp sprite is erased from memory and used resources are released.
 */
void destroy_sprite(Sprite *sp) {
	if (sp == NULL)
		return;
	if (sp->map)
		free(sp->map);
	free(sp);
	sp = NULL; 
}

/** Draws on the screen a given sprite sp given a base address
 */
int draw_sprite(Sprite *sp,char *base)
{
	draw_fig(sp,base, V_RES);
	return 0;
}

/** Draws on the screen a given sprite sp given a base address and a vertical limit that does not let the sprite be drawn past it
 */
int draw_fig(Sprite *sp,char *base, int v_res_lim)
{
	char *pixel = (char *)sp->map;
	uint32_t transparency=xpm_transparency_color(XPM_8_8_8);
	for(int i=0;i < sp->height; i++)
	{
		if ((sp->y + i) >= v_res_lim)
			break;
		for(int j=0; j < sp->width; j++)
		{
			if((sp->x + j) >= H_RES)
				continue;
			char *ptr = base + (((sp->y+i)*H_RES)+(sp->x+j))*3;
     	if((*pixel)!=(char)transparency)
			{
				*ptr = *pixel;
				ptr ++;
				pixel++;
     		*ptr = *pixel;
				ptr ++;
				pixel++;
     		*ptr = *pixel;
	  		pixel++;
			}
			else
				pixel+=3;
		}
	}
	return 0;
}

/** Copys the information of a given image directly to the base address given for the whole screen resolution
 */
void background( char*base, unsigned char *imagem)
{
  memcpy(base, imagem, 800*600*3);
}


/** Tests, pixel per pixel, for collisions between sp1 and sp2 i.e. if there is any pixel where the sprites overlap and both sprites have colors different than the transparent for that pixel
 */
bool test_collision_pixel(Sprite *sp1, Sprite *sp2, int x1, int y1, int x2, int y2, int width, int height)
{
	uint32_t transparency=xpm_transparency_color(XPM_8_8_8);
	for (int col=0; col<width; col++)
	{
		for (int lin=0; lin<height; lin++)
		{
			unsigned char *ptr1=sp1->map + 3*(sp1->width*(y1+lin) + x1 + col);
			unsigned char *ptr2=sp2->map + 3*(sp2->width*(y2+lin) + x2 + col);
			if (((*ptr1) !=(char)transparency) && ((*ptr2) !=(char)transparency))
				return true;
		}
	}
	return false;
}

/** Tests, using the rectangle technique, if there are any collisions between sp1 and sp2.
 */
bool test_collision(Sprite *sp1, Sprite *sp2)
{
 	if ((sp1->x > sp2->x + sp2->width) || (sp2->x > sp1->x + sp1->width) || (sp1->y > sp2->y + sp2->height) || (sp2->y > sp1->y + sp1->height))
	 	return false;
	int x1=0, y1=0, x2=0, y2=0, width=0, height=0;
	if ((sp1->x >= sp2->x) && (sp1->x < sp2->x + sp2->width))
	{
		x1 = 0;
		width = sp2->x + sp2->width - sp1->x;
		x2 = sp2->width - width;
	}
	else if ((sp2->x >= sp1->x) && (sp2->x < sp1->x + sp1->width))
	{
		x2 = 0;
		width = sp1->x + sp1->width - sp2->x;
		x1 = sp1->width - width;
	}
	if ((sp1->y >= sp2->y) && (sp1->y < sp2->y + sp2->height))
	{
		y1 = 0;
		height = sp2->y + sp2->height - sp1->y;
		y2 = sp2->height - height;
	}
	else if ((sp2->y >= sp1->y) && (sp2->y < sp1->y + sp1->height))
	{
		y2 = 0;
		height = sp1->y + sp1->height - sp2->y;
		y1 = sp1->height - height;
	}
	return test_collision_pixel(sp1, sp2, x1, y1, x2, y2, width, height);
}
