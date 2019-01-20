#include "rtc.h"
int hook_rtc=3;

/** Subscribes the RTC interruptions
 * Returns the hook_id used
 */
int rtc_subscribe_int(uint8_t *bit_no)
{
	*bit_no = hook_rtc;   
	if(sys_irqsetpolicy(RTC_IRQ , IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_rtc) != OK)
		return 1;   
	return 0; 
}

/** Unsubscribes the RTC interruptions
 */
int rtc_unsubscribe_int()
{
	if(sys_irqdisable(&hook_rtc) != OK)
		return 1;
	if(sys_irqrmpolicy(&hook_rtc)!=OK)
		return 1;
	return 0;
}

/** Reads the RTC register passed in the argument reg placing the information on the argument data
 */
int read_register(uint32_t reg, uint32_t * data)
{
	if(sys_outb(RTC_ADDR_REG, reg) != OK) 
		return 1;  
	else if(sys_inb(RTC_DATA_REG, data) != OK)
		return 1;
	return 0;
}

/** Reads the year from the corresponding RTC register
 */
int read_year()
{
  uint32_t year;
  if(read_register(YEAR_REG, &year) != 0)
		return -1;
  else
    return year;
}

/** Reads the month from the corresponding RTC register
 */
int read_month()
{
  uint32_t month;
  if(read_register(MONTH_REG, &month) != 0)
    return -1;
  else
    return month;
}

/** Reads the day from the corresponding RTC register
 */
int read_day()
{
	uint32_t day;
  if(read_register(DAY_REG, &day) != 0)
    return -1;
	else
    return day; 
}

/** Reads the hour from the corresponding RTC register
 */
int read_hour()
{
	uint32_t hour;
  if(read_register(HOUR_REG, &hour) != 0)
    return -1;
  else
    return hour;
}

/** Reads the minute from the corresponding RTC register
 */
int read_minute()
{
	uint32_t minute;
  if(read_register(MINUTE_REG, &minute) != 0)
		return -1;
  else
    return minute;
}

/** Reads the second from the corresponding RTC register
 */
int read_second()
{
	uint32_t second;
  if(read_register(SECOND_REG, &second) != 0)
    return -1;
  else
    return second;  
}

/** Checks it the data on the registers is in BCD or binary
 * Returns 0 if bcd
 * Returns 0x07 if binary
 * Returns -1 upon failure
 */
int is_BCD_mode()
{
	uint32_t b_reg;
	if (read_register(REG_B, &b_reg) !=0)
		return -1;
	return (b_reg & BCD_MASK);
}

/** Checks if an update is occuring on the A register
 * Returns true if it is
 */
bool is_updating()
{
	uint32_t a_reg;
	if (read_register(REG_A, &a_reg) !=0)
		return false;
	return (a_reg & UIP_MASK);
}

/** Converts the data passed as argument to an human friendly format (decimal) and returns it
 */
uint32_t convert_to_decimal(uint32_t date)
{
  return (date - 6 * (date>> 4));
}

/** Reads every parameter of the data struct Date from the corresponding register and places it on the right parameter 
 * Returns a Date struct with the information read
 */
Date read_date()
{
	if(is_updating())
		usleep(TIME_TO_UPDATE);
	Date date;
	if (is_BCD_mode() == 0)
	{
	date.year=convert_to_decimal(read_year());
	date.month=convert_to_decimal(read_month());
	date.day=convert_to_decimal(read_day());
	date.hour=convert_to_decimal(read_hour());
	date.minute=convert_to_decimal(read_minute());
	date.second=convert_to_decimal(read_second());
	}
	else
	{
		date.year=read_year();
		date.month=read_month();
		date.day=read_day();
		date.hour=read_hour();
		date.minute=read_minute();
		date.second=read_second();
	}
	return date;

}
