
#if !defined(_BCM_LED_H_)
#define _BCM_LED_H_

#define BCM_LED_OFF 0
#define BCM_LED_ON  1

void bcm_common_led_init(void);
void bcm_led_driver_set(unsigned short num, unsigned short state);
void bcm_led_driver_toggle(unsigned short num);
void bcm_common_led_setAllSoftLedsOff(void);
void bcm_common_led_setInitial(void);
short * bcm_led_driver_get_optled_map(void);
void bcm_ethsw_led_init(void);
void bcm_led_zero_flash_rate(int channel);
void bcm_led_set_source(unsigned int serial_sel, unsigned int hwled_sel);
void bcm_led_update_source(unsigned int serial_sel, unsigned int serial_mask,
        unsigned int hwled_sel, unsigned int hwled_mask);
#if defined(CONFIG_BCM96855) || defined(_BCM96855_)
void bcm_cled_mux_leds(unsigned int gpio, unsigned int output_led, unsigned int input_led_mask, unsigned int is_hw);
#endif
#endif  /* _BCM_LED_H_ */


