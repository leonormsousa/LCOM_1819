#include <lcom/lcf.h>
#include <stdint.h>
#include "pixmaps.h"
#include "sprite.h"

/**
 *  @author Leonor Sousa
 *  @author Sílvia Rocha
 */

#ifndef _UTILITY_H_
#define _UTILITY_H_

/** @defgroup Utility
 * @{
 *
 * Some general useful functions
 */

int min(int valor1, int valor2);

int max(int valor1, int valor2);

unsigned char* scancode_to_img(uint32_t scan_code);

unsigned char* int_to_img(int value);

/** @} end of Utility */
#endif
