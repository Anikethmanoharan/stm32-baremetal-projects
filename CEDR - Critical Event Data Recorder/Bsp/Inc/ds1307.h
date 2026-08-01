#ifndef DS1307_H_
#define DS1307_H_

#include "stm32f446xx.h"


/*Application Configurable Items*/
//                  sharing with oled
#define DS1307_I2C          I2C1          /* was I2C2 */
#define DS1307_I2C_GPIO_PORT GPIOB
#define DS1307_I2C_SDA_PIN  GPIO_PIN_NO_9 /* PB9, AF4 — shared with OLED */
#define DS1307_I2C_SCL_PIN  GPIO_PIN_NO_8 /* PB8, AF4 — shared with OLED */

#define DS1307_I2C_SPEED		I2C_SCL_SPEED_SM //tiny RTC supports only STANDARD MODE
//#define DS1307_I2C_PUPD			GPIO_NO_PUPD//GPIO_PIN_PU //if external use external 3.3kohm and here no pupd

#define DS1307_I2C_PUPD    GPIO_PIN_PU    /* was GPIO_NO_PUPD */


/*Register Addresses*/              //from Timekeeper Registers
#define DS1307_ADDR_SEC		0x00
#define DS1307_ADDR_MIN		0x01
#define DS1307_ADDR_HRS		0x02
#define DS1307_ADDR_DAY		0x03
#define DS1307_ADDR_DATE	0x04
#define DS1307_ADDR_MONTH	0x05
#define DS1307_ADDR_YEAR	0x06

#define TIME_FORMAT_12HRS_AM	0
#define TIME_FORMAT_12HRS_PM	1
#define TIME_FORMAT_24HRS		2

#define DS1307_I2C_ADDRESS		0x68  //from datasheet - 1101000 = 0x68

#define	SUNDAY		1
#define	MONDAY		2
#define TUESDAY		3
#define WEDNESDAY	4
#define THURSDAY	5
#define FRIDAY		6
#define SATURDAY	7

typedef struct
{
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t day;

}RTC_date_t;


typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t time_format;

}RTC_time_t;


//Function prototypes for user level applications

uint8_t ds1307_init(void);
void ds1307_set_current_time(RTC_time_t *);
void ds1307_get_current_time(RTC_time_t *);

void ds1307_set_current_date(RTC_date_t *);
void ds1307_get_current_date(RTC_date_t *);

#endif /* DS1307_H_ */
