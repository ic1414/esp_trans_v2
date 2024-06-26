#include "mymenu.h"




// init
void MYMENU::init(void (*func_5in1)(int16_t))
{
    ptr_5in1 = func_5in1;
}


// 功能函数中的case1。
void MYMENU::showOnDisplay1(){(*ptr_5in1)(1);}

// 功能函数中的case2。
void MYMENU::showOnDisplay2(){ (*ptr_5in1)(2); }

// 功能函数中的case3。
void MYMENU::function_setup(){ (*ptr_5in1)(3); }

// 功能函数中的case4。
void MYMENU::function_loop(){ (*ptr_5in1)(4); }

// 功能函数中的case5。
void MYMENU::function_exit(){( *ptr_5in1)(5); }


// get number of valid submenus
int16_t MYMENU::get_num_valid_submenus(){
    return num_valid_submenus;
}


// 设置菜单的子菜单
void MYMENU::set_submenu(MYMENU * array[], uint8_t arr_size){
    // 在这里，array 是外部数组的起始内存地址。 sizeof(array) == 0，需要外部传进 arr_size。
    num_valid_submenus = arr_size;

    // 不能超过定义中最多子菜单的数量
    if(num_valid_submenus > mymenu_max_subs){
        for(;;){
            Serial.print("exceed max sub menu size !!!!!!");
            delay(100);
        }
    }

    for(uint8_t i=0; i<num_valid_submenus; i++){
        array[i]->parent_menu = this; // 将当前菜单设为数组里菜单的父级菜单
        submenu_list[i] = array[i]; // 将子菜单链接到父级菜单
    }

}


void NAVI::init(MYMENU *start_menu, MYMENU* exit_menu, AiEsp32RotaryEncoder *rotaryEncoder){
    current_menu = start_menu; // 开机的时候进入到 start_menu
    exitt = exit_menu; // 让 导航 知道什么是<退出>菜单
    encoder = rotaryEncoder; // 让 导航 能获得编码器的值
    encoder->setBoundaries(0, current_menu->get_num_valid_submenus()-1, 1); // 设置编码器的范围
    encoder->reset(0); // reset the encoder to prevent index out of range.
}


#define time_update_delay 50 // 每隔x毫秒刷新一次屏幕（除了刷新屏幕你也可以做其他的任务）
#define time_button_delay 400 // 连续两次按下编码器最少间隔x毫秒，用来防噪。

// 菜单导航更新。你的所有功能函数将在这里面被触发。
void NAVI::update(){
    static unsigned long timer_update = millis(); // 屏幕刷新 计时器
    static unsigned long timer_lastpress = millis(); // 上次按下编码器时间 计时器

    // 更新两个屏幕的画面
    if(millis() - timer_update >= time_update_delay){
        timer_update = millis();

        current_menu->showOnDisplay1();
        current_menu->submenu_list[encoder->readEncoder()]->showOnDisplay2();
    }

    if (encoder->isEncoderButtonClicked() and (millis() - timer_lastpress > time_button_delay)){
        timer_lastpress = millis();

        // 如果选择中的子菜单是<退出>菜单
        if(current_menu->submenu_list[encoder->readEncoder()] == exitt)
        {
            exitt->function_setup();
            current_menu->function_exit();
            current_menu = current_menu->parent_menu; // 导航到当前菜单的父级菜单
        }
        else{
            // 导航到选中的子菜单
            current_menu = current_menu->submenu_list[encoder->readEncoder()];
        }

        encoder->setBoundaries(0, current_menu->get_num_valid_submenus() - 1, 1); // prevent index out of range
        encoder->reset(0); // reset the encoder to prevent referring arr[4] (length(arr) == 2).
        current_menu->function_setup();
    }

    current_menu->function_loop();

}



