#ifndef joystick_h
#define joystick_h

#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"




// esp32 的adc是12位的，所以 analogread() 值的范围是0-4095
#define adc_minval 0 
#define adc_midval 2047 // adc_maxval/2
#define adc_maxval 4095

// 用来将 analogread() 的值转换成0-255。（这是我自定义的）
#define adc_minval255 0
#define adc_midval255 127 // adc_maxval255/2
#define adc_maxval255 255

// 你也可以定义为0-100
// #define adc_minval100 0
// #define adc_midval100 50
// #define adc_maxval100 100




class JOYSTICK_AXIS{
    private:
        bool invert; // invert the output of potentiometer
        uint8_t pin; // joystick potentiometer pin
        uint16_t pre_val = adc_midval; // previous raw potentiometer value
        uint16_t cur_val = adc_midval; // current raw potentiometer value

        float LPFa = 0.2; // low pass filter parameter
        float LPFb = 0.8; // low pass filter parameter
        uint16_t lpf_val; // current filtered potentiometer value

        int16_t cali_min_val = adc_minval; // for calibration
        int16_t cali_mid_val = adc_midval; // for calibration
        int16_t cali_max_val = adc_maxval; // for calibration

    public:
        void init(uint8_t, bool);
        void setLSP(float, float);
        void update();
        void set_CALIval(uint16_t);
        void set_CALIval(uint16_t, uint16_t);
        void set_CALIval(uint16_t, uint16_t, uint16_t);
        uint16_t get_LPFval();
        uint16_t get_LPFval255();
        uint16_t get_RAWval();
        uint16_t get_RAWval255();
        uint16_t get_CALI_LPFval255();
};




#define joystick_update_rate 5 // 每隔x毫秒采样/更新一次摇杆

class JOYSTICK{
    private:
        enum joystick_index {index_lx, index_ly, index_rx, index_ry}; // enum
        JOYSTICK_AXIS *joysticks[4]; // 
        bool calibrated_mid = 0; // is mid calibrated ? 
        bool calibrated_min_max = 0;  // is min max calibrated ? 

    public:
        void init(JOYSTICK_AXIS*, JOYSTICK_AXIS*, JOYSTICK_AXIS*, JOYSTICK_AXIS*);
        void calibrate();
        void calibrate_mid();
        void calibrate_min_max();
        bool is_calibrated();
        void update();
        int16_t get_lx();
        int16_t get_ly();
        int16_t get_rx();
        int16_t get_ry();
};




#endif