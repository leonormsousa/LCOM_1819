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

#ifndef _RTC_H_
#define _RTC_H_

/** @defgroup RTC
 * @{
 *
 * Real Time Clock handling functions
 */

#define RTC_IRQ 8
#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define YEAR_REG 9
#define MONTH_REG 8
#define DAY_REG 7
#define HOUR_REG 4
#define MINUTE_REG 2
#define SECOND_REG 0
#define REG_A 10
#define REG_B 11
#define BCD_MASK 0x04
#define UIP_MASK 0x80
#define TIME_TO_UPDATE 244 

typedef struct {
  uint32_t year;
  uint32_t month;
  uint32_t day;
  uint32_t hour;
  uint32_t minute;
  uint32_t second;
} Date;

int rtc_subscribe_int(uint8_t *bit_no);

int rtc_unsubscribe_int();

bool is_updating();

uint32_t convert_to_decimal(uint32_t date);

Date read_date();

/** @} end of RTC */
#endif
