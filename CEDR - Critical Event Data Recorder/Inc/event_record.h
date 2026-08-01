#ifndef EVENT_RECORD_H_
#define EVENT_RECORD_H_


/*
 * event_record.h
 * Defines the event types and record format used by
 * the Critical Event Data Recorder (CEDR).
 */
typedef enum
{
    EVENT_NONE = 0,
    EVENT_BUTTON_PRESS,
	EVENT_PERIODIC_LOG,
    EVENT_ADC_THRESHOLD,
    EVENT_SENSOR_FAILURE,
    EVENT_FLASH_ERROR,

} EventType_t;


/*
 * Stores a single event record with its timestamp
 * and event type.
 */
typedef struct
{
    RTC_date_t date;
    RTC_time_t time;

    EventType_t eventType;

} EventRecord_t;


#endif /* EVENT_RECORD_H_ */
