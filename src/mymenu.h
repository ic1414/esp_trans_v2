#ifndef mymenu_h
#define mymenu_h

#include <Arduino.h>
#include <AiEsp32RotaryEncoder.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



// 每个目录最多的子菜单数量
#define mymenu_max_subs 7

// 菜单
class MYMENU
{
private:
    uint8_t num_valid_submenus = 0; //当前菜单的可用子菜单数量
    void (*ptr_5in1)(int16_t) = NULL; // 功能函数。

public:
    void init(void (*)(int16_t));

    void showOnDisplay1();
    void showOnDisplay2();
    void function_setup();
    void function_loop();
    void function_exit();

    MYMENU *parent_menu = NULL; // 菜单的父级菜单
    MYMENU *submenu_list[mymenu_max_subs] = {NULL}; // 子菜单数组
    
    int16_t get_num_valid_submenus();
    void set_submenu(MYMENU * array[], uint8_t);
};


// 菜单导航。（navi - navigation）
class NAVI
{
private:
    MYMENU *exitt = NULL; // <退出>菜单。当导航到<退出>的时候则回到当前菜单的父级菜单
    MYMENU *current_menu = NULL; // 最新被选中的菜单，当一个菜单是 current_menu 时它的功能函数将被执行
    AiEsp32RotaryEncoder *encoder; // 旋转编码器

public:
    void init(MYMENU *, MYMENU *, AiEsp32RotaryEncoder *);
    void update();
};




#endif
