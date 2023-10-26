#include "mymenu_functions.h"



// gamepad ------------------------------------------------------
#define blegamepad_update_rate 5
BleGamepad bleGamepad(gamepad_name, gamepad_manufactor, 100);

void func_menu_bl(int16_t n)
{
    static bool gamepad_begined = 0;
    static unsigned long timer_blegamepad = millis();
    switch (n)
    {
    case 1:
        display1.clearDisplay();

        display1.setTextSize(1);
        display1.setCursor(1, 1);
        if (bleGamepad.isConnected()) display1.println(F("bl yes"));
        else display1.println(F("bl no"));

        display1.setTextSize(2);
        display1.setCursor(2, 25);
        display1.print(F("t"));
        display1.println(joysticks.get_lx());
        display1.setCursor(2, 45);
        display1.print(F("y"));
        display1.println(joysticks.get_ly());
        display1.setCursor(70, 25);
        display1.print(F("p"));
        display1.print(joysticks.get_rx());
        display1.setCursor(70, 45);
        display1.print(F("r"));
        display1.print(joysticks.get_ry());

        display1.display();
        break;

    case 2:
        display2.clearDisplay();

        func_show_batt_d2();
        
        display2.setTextSize(3);
        display2.setCursor(40, 20);
        display2.print(F("bl"));

        display2.display();
        break;

    case 3:
        if (!(gamepad_begined))
        {
            bleGamepad.begin();
            gamepad_begined = 1;
        }
        break;

    case 4:
        if(millis() - timer_blegamepad >= blegamepad_update_rate){
            timer_blegamepad = millis();
            if (bleGamepad.isConnected())
            {
                bleGamepad.setLeftThumb(joysticks.get_lx() * 5, joysticks.get_ly() * 5);
                bleGamepad.setRightThumb(joysticks.get_rx() * 5, joysticks.get_ry() * 5);
            }
        }
        break;

    case 5:
        bleGamepad.end(); // this function is useless
        break;
    }
}


// nrf--------------------------------------------------------
#define mynrff_update_rate 5
MYNRF mynrff;

void func_menu_nrf(int16_t n)
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

        display2.setTextSize(3);
        display2.setCursor(40, 20);
        display2.print(F("nrf"));

        display2.display();
        break;

    case 3:
        mynrff.init();
        mynrff.lock();
        break;

    case 4:
    
    break;
    
    case 5:
        mynrff.lock();
        break;
    }
}


void func_menu_nrf_device1(int16_t n){
    static bool report;
    static unsigned long timer_mynrff = millis();
    switch (n)
    {
    case 1:
        display1.clearDisplay();

        // lock for safe
        if (mynrff.is_locked() == 1)
        {
            display1.setTextSize(2);
            display1.setCursor(1, 15);
            display1.println(F("<--scroll\n<-down to\n<--unlock"));
        }
        else
        {
            display1.setTextSize(1);
            display1.setCursor(1, 1);
            display1.print(F("a"));
            display1.print(mynrff.data_droneData.motorOut[0]);
            display1.setCursor(1, 18);
            display1.print(F("b"));
            display1.print(mynrff.data_droneData.motorOut[1]);
            display1.setCursor(1, 36);
            display1.print(F("c"));
            display1.print(mynrff.data_droneData.motorOut[2]);
            display1.setCursor(1, 54);
            display1.print(F("d"));
            display1.print(mynrff.data_droneData.motorOut[3]);

            display1.setCursor(38, 1);
            display1.print("v");
            display1.print(mynrff.data_droneData.voltage);

            display1.setCursor(38, 18);
            display1.print(F("x"));
            display1.print(mynrff.data_droneData.cur_gyro[0]);
            display1.setCursor(38, 36);
            display1.print(F("y"));
            display1.print(mynrff.data_droneData.cur_gyro[1]);
            display1.setCursor(38, 54);
            display1.print(F("z"));
            display1.print(mynrff.data_droneData.cur_gyro[2]);

            display1.setCursor(85, 18);
            display1.print(F("x"));
            display1.print(mynrff.data_droneData.cur_angle[0]);
            display1.setCursor(85, 36);
            display1.print(F("y"));
            display1.print(mynrff.data_droneData.cur_angle[1]);
            display1.setCursor(85, 54);
            display1.print(F("z"));
            display1.print(mynrff.data_droneData.cur_angle[2]);
        }

        display1.display();
        break;

    case 2:
        display2.clearDisplay();

        func_show_batt_d2();

        display2.setTextSize(3);
        display2.setCursor(40, 20);
        display2.print(F("nrf1"));

        display2.display();
        break;

    case 3:
        mynrff.change_commu_pipe(0);
        mynrff.lock();
        break;

    case 4:
        if(millis() - timer_mynrff >= mynrff_update_rate){
            timer_mynrff = millis();
            if (joysticks.get_lx() <= 3 and not(digitalRead(pin_switch2)) ? 1 : 0) mynrff.unlock();
            report = mynrff.send_control(not(digitalRead(pin_switch1)) ? 1 : 0, not(digitalRead(pin_switch2)) ? 1 : 0, joysticks.get_rx(), joysticks.get_ry(), joysticks.get_ly(), joysticks.get_lx());
            mynrff.receive(report);
            mynrff.is_lost();
        }
    break;
    
    case 5:
        mynrff.lock();
        break;
    }
}


void func_menu_nrf_device2(int16_t n){
    switch (n)
    {
    case 1:
        func_menu_nrf_device1(1);
        break;
    case 2:
        display2.clearDisplay();

        func_show_batt_d2();

        display2.setTextSize(3);
        display2.setCursor(40, 20);
        display2.print(F("nrf2"));

        display2.display();
        break;
    case 3:
        mynrff.change_commu_pipe(1);
        mynrff.lock();
        break;
    case 4:
        func_menu_nrf_device1(4);
        break;
    case 5:
        func_menu_nrf_device1(5);
        break;
    }
}


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

        display2.setTextSize(3);
        display2.setCursor(40, 20); display2.print(F("cali"));
        
        display2.display();
        break;

    case 3:
        if (joysticks.is_calibrated()) break;

        display1.clearDisplay();
        display1.setTextSize(2);
        display1.setCursor(30, 20); display1.print(F("!move"));
        display1.display();

        joysticks.calibrate_mid();

        display1.clearDisplay();
        display1.setCursor(30, 20); display1.print(F("move"));
        display1.display();
        
        joysticks.calibrate_min_max();

        display1.clearDisplay();
        display1.setCursor(30, 20); display1.print(F("done"));
        display1.display();
        delay(1000);
        break;

    case 4:
        break;
    }
}



void func_menu_exit(int16_t n)
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

        display2.setTextSize(3);
        display2.setCursor(40, 20); display2.print(F("exit"));
        
        display2.display();
        break;
    case 3:
        display1.clearDisplay();
        display2.clearDisplay();
        display1.display();
        display2.display();
        break;
    case 4:
        break;
    }
}



void func_menu_home(int16_t n){}


void func_menu_black(int16_t n){}



#include <Oversampling.h>

Oversampling adc(12, 18, 5);


void func_read_batt_v(){
    long aread = adc.read(pin_battery);
    double cur_batt_v = aread / (double)(4095 * 64) * (double)3.3;
    cur_batt_v = (double)2.59091 * cur_batt_v  + (double)0.249091;
    adc.adc_v_3v3 = cur_batt_v;

    if(serial_print_batt){
        Serial.println(adc.adc_v_3v3, 4);
    }
}



void func_show_batt_d1(){
    display1.setTextSize(1);
    display1.setCursor(5, 5);
    display1.print(adc.adc_v_3v3, 3);

}
void func_show_batt_d2(){
    display2.setTextSize(1);
    display2.setCursor(5, 5);
    display2.print(adc.adc_v_3v3, 3);
}
