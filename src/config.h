#ifndef config_h
#define config_h


#define invert_joystick1X 0
#define invert_joystick1Y 0
#define invert_joystick2X 1
#define invert_joystick2Y 1
#define pin_joystick1X 36
#define pin_joystick1Y 39
#define pin_joystick2X 34
#define pin_joystick2Y 35
#define serial_print_joy 0

#define pin_switch1 2
#define pin_switch2 4

#define gamepad_name "gtwhhh gamepad"
#define gamepad_manufactor "gtwhhh"

#define pin_rotary_a 27
#define pin_rotary_b 13
#define pin_rotary_button 14
#define pin_rotary_vcc -1
#define rotary_steps 8


#define pin_battery 32
#define vDivider_lowR 3.0
#define vDivider_highR 4.7
#define serial_print_batt 1

#define pin_nrf_ce 25
#define pin_nrf_csn 26
#define nrf_sending_addr1 "00001"
#define nrf_sending_addr2 "00002"
#define nrf_sending_addr3 "00003"
#define nrf_sending_addr4 "00004"
#define nrf_sending_addr5 "00005"
#define nrf_sending_addr6 "00006" //dont use this one

#define nrf_receiving_addr1 "00007"
#define nrf_receiving_addr2 "00008"
#define nrf_receiving_addr3 "00009"
#define nrf_receiving_addr4 "00010"
#define nrf_receiving_addr5 "00011"
#define nrf_receiving_addr6 "00012" //dont use this one


#define oled_width 128
#define oled_hight 64
#define oled_reset -1
#define oled_address1 0x3C
#define oled_address2 0x3D

#endif