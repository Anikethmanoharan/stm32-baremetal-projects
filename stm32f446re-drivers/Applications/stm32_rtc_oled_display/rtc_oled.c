#include <string.h>
#include "ds1307.h"
#include "ssd1306_oled.h"

/*---------------------------------------------------------------------------
 * Simple software delay
 *--------------------------------------------------------------------------*/
void delay(void)
{
    for(volatile uint32_t i = 0; i < 500000; i++);
}

/*---------------------------------------------------------------------------
 * Helper: return day-of-week string from ds1307 day number (1=Sunday…7=Saturday)
 *--------------------------------------------------------------------------*/
static char* get_day_of_week(uint8_t i)
{
    char* days[] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
    if(i < 1 || i > 7) return "Unknown";
    return days[i - 1];
}

/*---------------------------------------------------------------------------
 * Helper: write a 2-digit decimal into buf[0] and buf[1] (no null terminator)
 *--------------------------------------------------------------------------*/
static void number_to_string(uint8_t num, char *buf)
{
    if(num < 10)
    {
        buf[0] = '0';
        buf[1] = (char)(num + 48);
    }
    else
    {
        buf[0] = (char)((num / 10) + 48);
        buf[1] = (char)((num % 10) + 48);
    }
}

/*---------------------------------------------------------------------------
 * Helper: build "HH:MM:SS" string (8 chars + null) from RTC_time_t
 *--------------------------------------------------------------------------*/
static char* time_to_string(RTC_time_t *rtc_time)
{
    static char buf[9];  /* "HH:MM:SS\0" */
    buf[2] = ':';
    buf[5] = ':';
    number_to_string(rtc_time->hours,   buf);
    number_to_string(rtc_time->minutes, &buf[3]);
    number_to_string(rtc_time->seconds, &buf[6]);
    buf[8] = '\0';
    return buf;
}

/*---------------------------------------------------------------------------
 * Helper: build "DD/MM/YY" string (8 chars + null) from RTC_date_t
 *--------------------------------------------------------------------------*/
static char* date_to_string(RTC_date_t *rtc_date)
{
    static char buf[9];  /* "DD/MM/YY\0" */
    buf[2] = '/';
    buf[5] = '/';
    number_to_string(rtc_date->date,  buf);
    number_to_string(rtc_date->month, &buf[3]);
    number_to_string(rtc_date->year,  &buf[6]);
    buf[8] = '\0';
    return buf;
}

/*---------------------------------------------------------------------------
 * Helper: build "HH:MM:SS AM" or "HH:MM:SS PM" (11 chars + null)
 *--------------------------------------------------------------------------*/
static char* time_to_string_with_ampm(RTC_time_t *rtc_time)
{
    static char buf[12]; /* "HH:MM:SS AM\0" */
    char *time_str = time_to_string(rtc_time);

    buf[0]  = time_str[0];
    buf[1]  = time_str[1];
    buf[2]  = ':';
    buf[3]  = time_str[3];
    buf[4]  = time_str[4];
    buf[5]  = ':';
    buf[6]  = time_str[6];
    buf[7]  = time_str[7];
    buf[8]  = ' ';
    buf[9]  = (rtc_time->time_format == TIME_FORMAT_12HRS_PM) ? 'P' : 'A';
    buf[10] = 'M';
    buf[11] = '\0';
    return buf;
}

/*===========================================================================
 * main
 *==========================================================================*/
int main(void)
{
    RTC_time_t current_time;
    RTC_date_t current_date;

    /*-----------------------------------------------------------------------
     * 1. Initialise DS1307 RTC
     *---------------------------------------------------------------------*/
    if(ds1307_init())
    {
        /* Init failed: display error on OLED and halt */
        SSD1306_Init();
        SSD1306_Clear();
        SSD1306_SetCursor(0, 0);
        SSD1306_PrintString("RTC INIT FAIL");
        while(1);
    }

    /*-----------------------------------------------------------------------
     * 2. Set initial date and time (modify once, then comment out to keep time)
     *---------------------------------------------------------------------*/
    current_date.day   = TUESDAY;
    current_date.date  = 9;
    current_date.month = 6;
    current_date.year  = 26;

    current_time.hours       = 11;
    current_time.minutes     = 46;
    current_time.seconds     = 50;
    current_time.time_format = TIME_FORMAT_12HRS_AM;

    ds1307_set_current_date(&current_date);
    ds1307_set_current_time(&current_time);

    /*-----------------------------------------------------------------------
     * 3. Initialise SSD1306 OLED
     *---------------------------------------------------------------------*/
    SSD1306_Init();

    /*-----------------------------------------------------------------------
     * 4. Main loop: read RTC every ~1 s and refresh the display
     *---------------------------------------------------------------------*/
    while(1)
    {
        /* Read current time and date from DS1307 */
        ds1307_get_current_time(&current_time);
        ds1307_get_current_date(&current_date);

        /* --- Line 0: Label "Time:" --- */
        SSD1306_SetCursor(0, 0);
        SSD1306_PrintString("Time:           ");   /* trailing spaces erase leftover pixels */

        /* --- Line 1: HH:MM:SS [AM/PM] --- */
        SSD1306_SetCursor(1, 0);
        if(current_time.time_format != TIME_FORMAT_24HRS)
        {
            SSD1306_PrintString(time_to_string_with_ampm(&current_time));
        }
        else
        {
            SSD1306_PrintString(time_to_string(&current_time));
        }
        SSD1306_PrintString("   "); /* clear any leftover chars */

        /* --- Line 3: Label "Date:" --- */
        SSD1306_SetCursor(3, 0);
        SSD1306_PrintString("Date:           ");

        /* --- Line 4: DD/MM/YY --- */
        SSD1306_SetCursor(4, 0);
        SSD1306_PrintString(date_to_string(&current_date));
        SSD1306_PrintString("   ");

        /* --- Line 6: Day of week --- */
        SSD1306_SetCursor(6, 0);
        SSD1306_PrintString(get_day_of_week(current_date.day));
        SSD1306_PrintString("          ");  /* clear leftover from longer day names */

        delay();
    }

    return 0;
}
