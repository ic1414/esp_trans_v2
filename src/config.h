#ifndef config_h
#define config_h



// joystick---------------------------------------------------------------------
#define pin_joystick1X 36
#define invert_joystick1X false

#define pin_joystick1Y 39
#define invert_joystick1Y false

#define pin_joystick2X 34
#define invert_joystick2X true

#define pin_joystick2Y 35
#define invert_joystick2Y true

#define serial_print_joy false // serial print values of joysticks 

// switch-------------------------------------------------------------------------
#define pin_switch1 2
#define pin_switch2 4

// bluetooth gamepad---------------------------------------------------------------
#define gamepad_name "gtwhhh gamepad"
#define gamepad_manufactor "gtwhhh"

// rotary encoder------------------------------------------------------------------
#define pin_rotary_a 27 // any intput pin with interrupt
#define pin_rotary_b 13 // any intput pin with interrupt
#define pin_rotary_button 14 // any intput pin with interrupt
#define pin_rotary_vcc -1 // 不使用gpio供电，用电源（3v3）供电。
#define rotary_steps 8 // 旋转灵敏度

// read battery voltage-------------------------------------------------------------
#define pin_battery 32
#define vDivider_lowR 3.0 // voltage devider low side resistor
#define vDivider_highR 4.7 // voltage devider high side resistor
#define serial_print_batt 1 // serial print bettery voltage

// nrf24l001-----------------------------------------------------------------------
// use default spi pins of esp32 wroom, no need to define
#define pin_nrf_ce 25
#define pin_nrf_csn 26

// oled--------------------------------------------------------------------
// use default iic pins of esp32 wroom, no need to define
#define oled_width 128
#define oled_hight 64
#define oled_reset -1
#define oled_address1 0x3C // iic address
#define oled_address2 0x3D // iic address

#endif