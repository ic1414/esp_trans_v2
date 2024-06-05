

// arduino esp32 board version 2.0.13

#include "inclu.h"


// joystick--------------------------------------------------------
JOYSTICK_AXIS j_rx;
JOYSTICK_AXIS j_ry;
JOYSTICK_AXIS j_lx;
JOYSTICK_AXIS j_ly;
JOYSTICK joysticks;


// rotary encoder---------------------------------------------------
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(pin_rotary_a, pin_rotary_b, pin_rotary_button, pin_rotary_vcc, rotary_steps);
void IRAM_ATTR readEncoderISR() { rotaryEncoder.readEncoder_ISR(); }


// iic oled--------------------------------------------------------
Adafruit_SSD1306 display1(oled_width, oled_hight, &Wire, oled_reset);
Adafruit_SSD1306 display2(oled_width, oled_hight, &Wire, oled_reset);


// read battery voltage ----------------------------------------------
TaskHandle_t task_read_batt;
void task_read_batt_code(void *);


// menu -------------------------------------------------------------
MYMENU mymenu_exit;
MYMENU mymenu_black;
MYMENU mymenu_home;
MYMENU mymenu_bl;
MYMENU mymenu_cali;
MYMENU mymenu_nrf; MYMENU mymenu_nrf_d1; MYMENU mymenu_nrf_d2;


NAVI menu_navi;




//------------------------------------------------------------
void setup()
{

    mymenu_exit.init(&func_menu_exit);

    // 在这里设置你的菜单
    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    mymenu_bl.init(&func_menu_bl); mymenu_bl.set_submenu(&mymenu_exit);

    mymenu_nrf_d1.init(&func_menu_nrf_device1); mymenu_nrf_d1.set_submenu(&mymenu_exit);
    mymenu_nrf_d2.init(&func_menu_nrf_device2); mymenu_nrf_d2.set_submenu(&mymenu_exit);
    mymenu_nrf.init(&func_menu_nrf); mymenu_nrf.set_submenu(&mymenu_nrf_d1, &mymenu_nrf_d2, &mymenu_exit, &mymenu_exit);

    mymenu_cali.init(&func_menu_cali); mymenu_cali.set_submenu(&mymenu_exit);
    
    mymenu_home.init(&func_menu_home); mymenu_home.set_submenu(&mymenu_bl, &mymenu_nrf, &mymenu_cali, &mymenu_exit, &mymenu_exit);

    mymenu_black.init(&func_menu_black); mymenu_black.set_submenu(&mymenu_home);
    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    xTaskCreatePinnedToCore(task_read_batt_code, "task_read_batt", 10000, NULL,  1, &task_read_batt, 0);

    
    Serial.begin(250000);

    pinMode(pin_switch1, INPUT_PULLUP);
    pinMode(pin_switch2, INPUT_PULLUP);
    j_rx.init(pin_joystick1X, invert_joystick1X);
    j_ry.init(pin_joystick1Y, invert_joystick1Y);
    j_lx.init(pin_joystick2X, invert_joystick2X);
    j_ly.init(pin_joystick2Y, invert_joystick2Y);
    joysticks.init(&j_rx, &j_ry, &j_lx, &j_ly);

    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);
    rotaryEncoder.disableAcceleration();
    rotaryEncoder.setBoundaries(0, 0, 1); // just to initiate
    rotaryEncoder.reset();


    menu_navi.init(&mymenu_black, &mymenu_exit, &rotaryEncoder);
    
    
    // display begin
    display1.begin(SSD1306_SWITCHCAPVCC, oled_address1);
    display2.begin(SSD1306_SWITCHCAPVCC, oled_address2);

    // set display brightness
    display1.setTextColor(SSD1306_WHITE); display2.setTextColor(SSD1306_WHITE);
    display1.ssd1306_command(SSD1306_SETCONTRAST); display1.ssd1306_command((byte)10);
    display2.ssd1306_command(SSD1306_SETCONTRAST); display2.ssd1306_command((byte)10);
    
    display1.clearDisplay(); display1.display();
    display2.clearDisplay(); display2.display();

    display1.setTextSize(2);
    display1.setCursor(1, 15);
    display1.println(F("press to\nunlock"));
    display1.display();

    // unlock when pressed
    while (not(rotaryEncoder.isEncoderButtonClicked())){}

    display1.clearDisplay(); display1.display(); 
    display2.clearDisplay(); display2.display();

}

//------------------------------------------------------------
void loop()
{
    joysticks.update();
    menu_navi.update();
}



void task_read_batt_code(void *pvParameters)
{
  for(;;){
    func_read_batt_v();
    delay(1);
  } 
}