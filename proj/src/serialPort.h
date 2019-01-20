#include <lcom/lcf.h>
#include <stdint.h>
#include <minix/drivers.h>
#include "i8254.h"

/**
 *  @author Leonor Sousa
 *  @author Sílvia Rocha
 */

#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_

/** @defgroup SerialPort
 * @{
 *
 * UART Protocol related functions
 */

#define SERIALPORT_COM1_IRQ 4
#define COM1_INIT_ADDRESS    0x3F8

int serialPort_subscribe_int(uint8_t *bit_no);

int serialPort_unsubscribe_int();

int write_character(uint8_t character);

int write_message(uint8_t message);

void serialPort_ih();

int configure_serialPort();

uint8_t get_acknowledgement_byte();

/** @} end of SerialPort */
#endif
