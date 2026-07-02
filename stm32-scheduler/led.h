
#ifndef LED_H_
#define LED_H_

#define LED_RED    0
#define LED_GREEN  1
#define LED_BLUE   2

#define DELAY_COUNT_1MS 		 1250U
#define DELAY_COUNT_1S  		(1000U * DELAY_COUNT_1MS)
#define DELAY_COUNT_800MS 		(800U  * DELAY_COUNT_1MS)
#define DELAY_COUNT_400MS 		(400U  * DELAY_COUNT_1MS)

void led_init_all(void);
void led_on(uint8_t led_no);
void led_off(uint8_t led_no);
void delay(uint32_t count);

#endif /* LED_H_ */
