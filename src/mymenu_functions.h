#ifndef mymenu_functions
#define mymenu_functions

#include <Arduino.h>
#include "joystick.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BleGamepad.h>
#include "mynrf.h"
#include "config.h"



extern Adafruit_SSD1306 display1; // 引用src.ino中的实例
extern Adafruit_SSD1306 display2; // 引用src.ino中的实例
extern JOYSTICK joysticks; // 引用src.ino中定义的实例



// 菜单的功能函数。功能函数有固定的格式。----------------------------------------------

void func_menu_bl(int16_t);
void func_menu_nrf(int16_t);
void func_menu_nrf_device1(int16_t);
void func_menu_nrf_device2(int16_t);
void func_menu_cali(int16_t);
void func_menu_exit(int16_t);
void func_menu_home(int16_t);
void func_menu_black(int16_t);


// 普通函数-------------------------------------------------------

void func_read_batt_v();
void func_show_batt_d1();
void func_show_batt_d2();




/*
Example.h

void func_menu_example(int16_t);

void func_menu_example(int16_t n)
{
    switch (n)
    {
    case 1:
        // show something on display1 when current menu is menu_example
        break;
    case 2:
        // show something on display2 when menu_example is a submenu of current menu
        break;
    case 3:
        // do something once when menu_example is selected
        break;
    case 4:
        // do something continuously when menu_example is selected
        break;
    case 5:
        // do something once when exit menu_example
        break;
    }
}

*/







#endif