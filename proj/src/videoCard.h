#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

/**
 *  @author Leonor Sousa
 *  @author Sílvia Rocha
 */

#ifndef _VIDEOCARD_H_
#define _VIDEOCARD_H_

/** @defgroup videoCard
 * @{
 *
 * Videocard related functions
 */

void *(vg_init)(uint16_t mode);

int (vg_enter)(uint16_t mode);

void *(map_adress)(uint16_t mode);

int vbe_get_mode_inf(u_int16_t mode, vbe_mode_info_t *info);

/** @} end of videoCard */
#endif
