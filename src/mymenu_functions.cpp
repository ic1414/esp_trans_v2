#include "mymenu_functions.h"



// gamepad ------------------------------------------------------
#define blegamepad_update_rate 5 //蓝牙遥控每隔x毫秒向电脑发送摇杆数据
BleGamepad bleGamepad(gamepad_name, gamepad_manufactor, 100);

// 蓝牙菜单的功能函数
void func_menu_bl(int16_t n)
{
   
    switch (n)
    {
    // 选中 menu_bl 的时候，在oled屏幕1显示下面case1中的画面。每隔50ms更新一次。
    case 1:
        display1.clearDisplay(); // 清空画布

        // 蓝牙连接状态
        display1.setTextSize(1); display1.setCursor(1, 1);
        if (bleGamepad.isConnected()) display1.println(F("bl yes"));
        else display1.println(F("bl no"));

        // 油门 摇杆数值
        display1.setTextSize(2); display1.setCursor(2, 25);
        display1.print(F("t")); display1.println(joysticks.get_lx());

        // yaw 摇杆数值
        display1.setCursor(2, 45);
        display1.print(F("y")); display1.println(joysticks.get_ly());
        
        // pitch 摇杆数值
        display1.setCursor(70, 25);
        display1.print(F("p")); display1.print(joysticks.get_rx());

        // roll 摇杆数值
        display1.setCursor(70, 45);
        display1.print(F("r")); display1.print(joysticks.get_ry());

        display1.display(); // 将画布内容显示在屏幕上
        break;

    // 如果 menu_bl 是当前菜单的子菜单，在oled屏幕2显示下面case2中的画面。每隔50ms更新一次。
    case 2:
        display2.clearDisplay(); // 清空画布

        func_show_batt_d2(); // 电池电压

        display2.setTextSize(3); display2.setCursor(40, 20);
        display2.print(F("bl"));

        display2.display(); // 将画布内容显示在屏幕上
        break;

    // 当选中 menu_bl 的时候，执行case3中的代码一次
    case 3:
        static bool gamepad_begined = 0;
        if (!(gamepad_begined)) bleGamepad.begin();
        if (!(gamepad_begined)) gamepad_begined = 1;
        break;

    // 当选中 menu_bl 的时候，每隔 blegamepad_update_rate 秒执行一次case4中的代码
    case 4:
        static unsigned long timer_blegamepad = millis();
        if(millis() - timer_blegamepad >= blegamepad_update_rate)
        {
            timer_blegamepad = millis();
            if (bleGamepad.isConnected())
            {
                bleGamepad.setLeftThumb(joysticks.get_lx() * 5, joysticks.get_ly() * 5); //向电脑发送数据
                bleGamepad.setRightThumb(joysticks.get_rx() * 5, joysticks.get_ry() * 5); //向电脑发送数据
            }
        }
        break;

    // 当离开 menu_bl 的时候，执行case5中的代码一次
    case 5:
        bleGamepad.end(); // 不过这个函数似乎没有效果。
        break;
    }

}




// ------------------------------------------一些细节将不会在下面的代码中注释----------------------------------------------




// ！！！！！！！！！！！！！！！！！！！！！！！！！！ nrf代码没有经过测试 ！！！！！！！！！！！！！！！！！！！！！！！！！！！！
// nrf--------------------------------------------------------
#define mynrff_update_rate 5 //遥控每隔x毫秒发送一次 nrf 信号
MYNRF mynrff;

// nrf主菜单的功能函数
void func_menu_nrf(int16_t n)
{   
    switch (n)
    {
    // 选中 menu_nrf 的时候，oled屏幕1不显示画面
    case 1:
        display1.clearDisplay();
        display1.display();
        break;

    // 如果 menu_nrf 是当前菜单的子菜单，将在oled屏幕2显示下面case2中的画面
    case 2:
        display2.clearDisplay();

        func_show_batt_d2();

        display2.setTextSize(3); display2.setCursor(40, 20);
        display2.print(F("nrf"));

        display2.display();
        break;
    
    // 当选中 menu_nrf 的时候，执行case3中的代码一次
    case 3:
        mynrff.init();
        mynrff.lock(); // nrf上锁
        break;

    // 不执行任何循环代码
    case 4:
    break;
    
    // 离开 menu_nrf 的时候，执行case5中的代码一次
    case 5:
        mynrff.lock(); // nrf上锁
        break;
    }
}




// nrf_d1（nrf device1）菜单的功能函数
void func_menu_nrf_device1(int16_t n){
    static bool report;
    static unsigned long timer_mynrff = millis();
    switch (n)
    {
    case 1:
        display1.clearDisplay();

        // lock for safety
        if (mynrff.is_locked() == 1)
        {
            display1.setTextSize(2); display1.setCursor(1, 15);
            display1.println(F("<--scroll\n<-down to\n<--unlock"));
        }
        else
        {
            display1.setTextSize(1); display1.setCursor(1, 1);
            display1.print(F("a")); display1.print(mynrff.data_droneData.motorOut[0]);
            
            display1.setCursor(1, 18);
            display1.print(F("b")); display1.print(mynrff.data_droneData.motorOut[1]);
            
            display1.setCursor(1, 36);
            display1.print(F("c")); display1.print(mynrff.data_droneData.motorOut[2]);
            
            display1.setCursor(1, 54);
            display1.print(F("d")); display1.print(mynrff.data_droneData.motorOut[3]);

            display1.setCursor(38, 1);
            display1.print("v"); display1.print(mynrff.data_droneData.voltage);

            display1.setCursor(38, 18);
            display1.print(F("x")); display1.print(mynrff.data_droneData.cur_gyro[0]);
            
            display1.setCursor(38, 36);
            display1.print(F("y")); display1.print(mynrff.data_droneData.cur_gyro[1]);
            
            display1.setCursor(38, 54);
            display1.print(F("z")); display1.print(mynrff.data_droneData.cur_gyro[2]);

            display1.setCursor(85, 18);
            display1.print(F("x")); display1.print(mynrff.data_droneData.cur_angle[0]);
            
            display1.setCursor(85, 36);
            display1.print(F("y")); display1.print(mynrff.data_droneData.cur_angle[1]);
            
            display1.setCursor(85, 54);
            display1.print(F("z")); display1.print(mynrff.data_droneData.cur_angle[2]);
        }

        display1.display();
        break;

    case 2:
        display2.clearDisplay();

        func_show_batt_d2();

        display2.setTextSize(3); display2.setCursor(40, 20);
        display2.print(F("nrf1"));

        display2.display();
        break;

    case 3:
        mynrff.change_commu_pipe(0, 0); //设置通信 地址 和 通道。
        mynrff.lock(); // 上锁
        break;

    case 4:
        if(millis() - timer_mynrff >= mynrff_update_rate){
            timer_mynrff = millis();

            if (joysticks.get_lx() <= 3 and not(digitalRead(pin_switch2)) ? 1 : 0) mynrff.unlock(); // 降低油门和关闭“发动机”解锁遥控
            report = mynrff.send_control(not(digitalRead(pin_switch1)) ? 1 : 0, not(digitalRead(pin_switch2)) ? 1 : 0, joysticks.get_rx(), joysticks.get_ry(), joysticks.get_ly(), joysticks.get_lx());
            mynrff.receive(report);
            mynrff.is_lost();
        }
    break;
    
    case 5:
        mynrff.lock(); // 上锁
        break;
    }
}




// nrf_d2（nrf device2）菜单的功能函数。这里直接调用 nrf device1 的一些case，因为两者的区别只有通信地址和屏幕显示画面。
void func_menu_nrf_device2(int16_t n){
    switch (n)
    {
    case 1:
        func_menu_nrf_device1(1); // 调用 nrf device1 的case
        break;

    case 2:
        display2.clearDisplay();

        func_show_batt_d2();

        display2.setTextSize(3); display2.setCursor(40, 20);
        display2.print(F("nrf2"));

        display2.display();
        break;

    case 3:
        mynrff.change_commu_pipe(1, 0); // 改下地址, 不改通道
        mynrff.lock();
        break;

    case 4:
        func_menu_nrf_device1(4); // 调用 nrf device1 的case
        break;

    case 5:
        func_menu_nrf_device1(5); // 调用 nrf device1 的case
        break;
    }
}




// 校准菜单的功能函数
void func_menu_cali(int16_t n)
{

    switch (n)
    {
    case 1:
        display1.clearDisplay();
        display1.display();
        break;

    case 2:
        display2.clearDisplay();

        func_show_batt_d2();

        display2.setTextSize(3); display2.setCursor(40, 20);
        display2.print(F("cali"));
        
        display2.display();
        break;

    case 3:
        // 如果校准过了就不校准。设置为每次开机只能校准一次，没校准好的话就要重启。
        if (joysticks.is_calibrated()) break;

        // 屏幕显示 ！move。 不要动
        display1.clearDisplay();
        display1.setTextSize(2); display1.setCursor(30, 20);
        display1.print(F("!move"));
        display1.display();

        joysticks.calibrate_mid();

        // 屏幕显示 move。 最大程度去动摇杆。
        display1.clearDisplay();
        display1.setCursor(30, 20);
        display1.print(F("move"));
        display1.display();
        
        joysticks.calibrate_min_max();

        // 屏幕显示 done。
        display1.clearDisplay();
        display1.setCursor(30, 20);
        display1.print(F("done"));
        display1.display();
        delay(1000); 
        break;

    case 4:
        break;
    case 5:
        break;
    }
}



// <退出>菜单的功能函数
void func_menu_exit(int16_t n)
{
    switch (n)
    {
    case 1:
        break;

    // 作为子菜单的时候显示以下画面
    case 2:
        display2.clearDisplay();

        func_show_batt_d2();

        display2.setTextSize(3);
        display2.setCursor(40, 20); display2.print(F("exit"));
        
        display2.display();
        break;
    
    // 选中的时候清空屏幕画面
    case 3:
        display1.clearDisplay();
        display2.clearDisplay();
        display1.display();
        display2.display();
        break;

    case 4:
        break;
    case 5:
        break;
    }
}




// 主菜单功能函数没有代码。你可以自定义
void func_menu_home(int16_t n){}




// <黑屏>菜单功能函数没有代码。你可以自定义
void func_menu_black(int16_t n){}




// 通过 过采样 来提高adc采样的精度。简单来说，就是对原始数据进行非常多次的采样，然后计算平均值。
#include <Oversampling.h> // 具体请看作者github页面和代码

Oversampling adc(12, 18, 5);

// 读取电池电压函数
void func_read_batt_v(){
    long aread = adc.read(pin_battery);

    double cur_batt_v = aread / (double)(4095 * 64) * (double)3.3;
    
    //线性矫正。通过外部的万用表和esp32采样的电压值计算得出。
    cur_batt_v = (double)2.59091 * cur_batt_v  + (double)0.249091;
    
    // 将数值保存到全局变量 adc.adc_v_3v3 里。 func_show_batt_d1()将用到此变量。
    adc.adc_v_3v3 = cur_batt_v;

    // 打印电池电压，用来和万用表的值做对比，然后计算出矫正参数。
    if(serial_print_batt) Serial.println(cur_batt_v, 4);

    // 由于adc.read()已经花了很多时间，不需要delay了。
    // delay(0);
}


// 在oled屏幕1打印电池电压
void func_show_batt_d1(){
    display1.setTextSize(1); display1.setCursor(5, 5);
    display1.print(adc.adc_v_3v3, 3); // 3 表示打印后三位小数

}
// 在oled屏幕2打印电池电压
void func_show_batt_d2(){
    display2.setTextSize(1); display2.setCursor(5, 5);
    display2.print(adc.adc_v_3v3, 3);
}
