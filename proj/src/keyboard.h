#pragma once
#include <lcom/lcf.h>
#include <stdint.h>

/**
 *  @author Leonor Sousa
 *  @author Sílvia Rocha
 */

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

/** @defgroup Keyboard
 * @{
 *
 * Keyboard handling related functions
 */

#define DELAY_US 20000
#define OUT_BUF 0x60
#define STAT_REG 0x64
#define ESC_BREAK 0X81
#define OBF 0x01
#define IBF 0x02
#define PAR_ERR 0x80
#define TO_ERR 0x40
#define READ_CMD 0x20

int (keyboard_subscribe_int)(uint8_t *bit_no);

int (keyboard_unsubscribe_int)();

bool (input_buffer_full)();

void (kbc_ih)();

/** @} end of Keyboard */
#endif
