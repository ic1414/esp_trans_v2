#include <AiEsp32RotaryEncoder.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "joystick.h"
#include "mymenu.h"
#include "mymenu_functions.h"




// joystick--------------------------------------------------------

// j（joystick - 摇杆），r（right - 右），x（x-axis - x轴）
JOYSTICK_AXIS j_rx; // 右手边摇杆x轴
JOYSTICK_AXIS j_ry; // 右手边摇杆y轴
JOYSTICK_AXIS j_lx; // 左手边摇杆x轴
JOYSTICK_AXIS j_ly; // 左手边摇杆y轴
JOYSTICK joysticks; // 将四个摇杆轴打包，方便运算


// rotary encoder---------------------------------------------------
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(pin_rotary_a, pin_rotary_b, pin_rotary_button, pin_rotary_vcc, rotary_steps);
void IRAM_ATTR readEncoderISR() { rotaryEncoder.readEncoder_ISR(); } // 设置中断。当转动编码器的时候触发中断并更新数值。


// iic oled--------------------------------------------------------
Adafruit_SSD1306 display1(oled_width, oled_hight, &Wire, oled_reset);
Adafruit_SSD1306 display2(oled_width, oled_hight, &Wire, oled_reset);


// read battery voltage ----------------------------------------------
// esp32 的cpu是双核的。读取电池电压的函数单独占用一个cpu核心。使用 oversampling（过采样），来获得更高采样分辨率，但会降低采样速度。
TaskHandle_t task_read_batt; // batt - battery
void task_read_batt_code(void *);


// menu -------------------------------------------------------------
// <abc> 表示 abc 是一个特殊菜单。
MYMENU mymenu_exit; // <退出>菜单的作用是返回到当前菜单的父级菜单。除了<退出>和<黑屏>以外的菜单都需要 >=1 的<退出>作为子菜单。
MYMENU mymenu_black; // <黑屏>。这个菜单不能有<退出>。
MYMENU mymenu_home; // 主菜单。（自定义的主页面）
MYMENU mymenu_bl; // 遥控通过蓝牙连接电脑。（自定义的菜单）
MYMENU mymenu_cali; // 校准遥控的摇杆。（自定义的菜单）
MYMENU mymenu_nrf; MYMENU mymenu_nrf_d1; MYMENU mymenu_nrf_d2; // 使用nrf24l01模块收发2.4g无线电。（自定义的菜单）


NAVI menu_navi; // 菜单导航




//------------------------------------------------------------
void setup()
{

    mymenu_exit.init(&func_menu_exit);

    // 在这里开始设置你的菜单------------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------------------------------

    // 自定义菜单（bl）绑定对应的五合一函数。将<退出>设为自定义菜单（bl）的子菜单。
    mymenu_bl.init(&func_menu_bl); 
    static MYMENU * alist_mymenu_bl[] = {&mymenu_exit};
    mymenu_bl.set_submenu(alist_mymenu_bl, sizeof(alist_mymenu_bl)/sizeof(MYMENU *));

    // 自定义菜单（nrf_d1, nrf_d2）绑定对应的五合一函数。并将<退出>设为自定义菜单（nrf_d1, nrf_d2）的子菜单。
    static MYMENU * alist_mymenu_nrf_dx[] = {&mymenu_exit};
    mymenu_nrf_d1.init(&func_menu_nrf_device1); mymenu_nrf_d1.set_submenu(alist_mymenu_nrf_dx, sizeof(alist_mymenu_nrf_dx)/sizeof(MYMENU *));
    mymenu_nrf_d2.init(&func_menu_nrf_device2); mymenu_nrf_d2.set_submenu(alist_mymenu_nrf_dx, sizeof(alist_mymenu_nrf_dx)/sizeof(MYMENU *));
    // 自定义菜单（nrf）绑定对应的五合一函数。 并将 <退出>，nrf_d1，nrf_d2 设为自定义菜单 nrf 的子菜单。
    static MYMENU * alist_mymenu_nrf_main[] = {&mymenu_nrf_d1, &mymenu_nrf_d2, &mymenu_exit, &mymenu_exit};
    mymenu_nrf.init(&func_menu_nrf); mymenu_nrf.set_submenu(alist_mymenu_nrf_main, sizeof(alist_mymenu_nrf_main)/sizeof(MYMENU *));

    // hello
    static MYMENU * alist_mymenu_cali[] = {&mymenu_exit};
    mymenu_cali.init(&func_menu_cali); mymenu_cali.set_submenu(alist_mymenu_cali, sizeof(alist_mymenu_cali)/sizeof(MYMENU *));

    // world
    static MYMENU * alist_mymenu_home[] = {&mymenu_bl, &mymenu_nrf, &mymenu_cali, &mymenu_exit, &mymenu_exit};
    mymenu_home.init(&func_menu_home); mymenu_home.set_submenu(alist_mymenu_home, sizeof(alist_mymenu_home)/sizeof(MYMENU *));

    // 。
    static MYMENU * alist_mymenu_black[] = {&mymenu_home};
    mymenu_black.init(&func_menu_black); mymenu_black.set_submenu(alist_mymenu_black, sizeof(alist_mymenu_black)/sizeof(MYMENU *));
    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

    // esp32 有两个cpu核心。arduino的代码在核心0上面运行。我们将电压采样函数绑定到cpu核心1。
    xTaskCreatePinnedToCore(task_read_batt_code, "task_read_batt", 10000, NULL,  1, &task_read_batt, 0);

    // hello world
    Serial.begin(250000);


    // j（joystick - 摇杆），r（right - 右），x（x-axis - x轴）
    pinMode(pin_switch1, INPUT_PULLUP); // 设置拨动开关
    pinMode(pin_switch2, INPUT_PULLUP); // 设置拨动开关
    j_rx.init(pin_joystick1X, invert_joystick1X); // 初始化摇杆
    j_ry.init(pin_joystick1Y, invert_joystick1Y); // 初始化摇杆
    j_lx.init(pin_joystick2X, invert_joystick2X); // 初始化摇杆
    j_ly.init(pin_joystick2Y, invert_joystick2Y); // 初始化摇杆
    joysticks.init(&j_rx, &j_ry, &j_lx, &j_ly); // 初始化摇杆们

    rotaryEncoder.begin(); // 旋转编码器初始化
    rotaryEncoder.setup(readEncoderISR); // 旋转编码器初始中断
    rotaryEncoder.disableAcceleration(); // 关闭加速度。（如果你突然以很快的速度去移动鼠标，你会发现光标在屏幕上走了很长的距离）
    rotaryEncoder.setBoundaries(0, 0, 1); // 设置 记录变量 的边界和是否转圈。
    rotaryEncoder.reset(); // 重设当前记录变量的值为0

    // 菜单导航
    menu_navi.init(&mymenu_home, &mymenu_exit, &rotaryEncoder);

    // display begin
    display1.begin(SSD1306_SWITCHCAPVCC, oled_address1);
    display2.begin(SSD1306_SWITCHCAPVCC, oled_address2);

    // set display brightness
    display1.setTextColor(SSD1306_WHITE); display2.setTextColor(SSD1306_WHITE);
    display1.ssd1306_command(SSD1306_SETCONTRAST); display1.ssd1306_command((byte)10);
    display2.ssd1306_command(SSD1306_SETCONTRAST); display2.ssd1306_command((byte)10);

    // 清除显示屏上的内容。
    display1.clearDisplay(); display1.display();
    display2.clearDisplay(); display2.display();

    // 在屏幕1上显示 press to unlock
    display1.setTextSize(2); display1.setCursor(1, 15);
    display1.println(F("press to\nunlock"));
    display1.display();

    // 当按下旋转编码器解锁遥控。
    while (not(rotaryEncoder.isEncoderButtonClicked())){}

    // 清除显示屏上的内容。
    display1.clearDisplay(); display1.display(); 
    display2.clearDisplay(); display2.display();
}


//------------------------------------------------------------
void loop()
{   
    joysticks.update();
    menu_navi.update();
}




// 电压采样task
void task_read_batt_code(void *pvParameters)
{
  for(;;){
    func_read_batt_v();
    vTaskDelay(1); // 用delay重设下看门狗防止esp32报错/重启。
  }
}

