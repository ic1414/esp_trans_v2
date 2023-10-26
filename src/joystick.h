#ifndef joystick_h
#define joystick_h

#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"



/*
Example.h

JOYSTICK_AXIS j_rx;
JOYSTICK_AXIS j_ry;
JOYSTICK_AXIS j_lx;
JOYSTICK_AXIS j_ly;
JOYSTICK joysticks;

void setup()
{
    j_rx.init(pin_joystick1X, invert_joystick1X);
    j_ry.init(pin_joystick1Y, invert_joystick1Y);
    j_lx.init(pin_joystick2X, invert_joystick2X);
    j_ly.init(pin_joystick2Y, invert_joystick2Y);
    joysticks.init(&j_rx, &j_ry, &j_lx, &j_ly);

}

void loop()
{
    joysticks.update();
    Serial.println(joysticks.get_lx());
    Serial.println(joysticks.get_ly());
    Serial.println(joysticks.get_rx());
    Serial.println(joysticks.get_ry());
}

*/




#define adc_minval 0
#define adc_midval 2047
#define adc_maxval 4095
#define adc_minval255 0
#define adc_midval255 127
#define adc_maxval255 255


class JOYSTICK_AXIS{
    private:
        bool invert; // invert the output of potentiometer of joystick
        uint8_t pin; // joystick pin (normally a joystick has 2 pins)
        uint16_t pre_val = adc_midval; // previous raw potentiometer value
        uint16_t cur_val = adc_midval; // current raw potentiometer value

        float LPFa = 0.2; // for low pass filter
        float LPFb = 0.8; // for low pass filter
        uint16_t lpf_val; // current filtered potentiometer value

        int16_t cali_min_val = adc_minval; // for calibration
        int16_t cali_mid_val = adc_midval; // for calibration
        int16_t cali_max_val = adc_maxval; // for calibration

    public:
        void init(uint8_t, bool); // void init(uint8_t pin, bool invert);
        void setLSP(float, float); // set LPFa, LPFb
        void update(); // analog read and update pre_val, cur_val, lpf_val
        void set_CALIval(uint16_t); // set cali_mid_val
        void set_CALIval(uint16_t, uint16_t); // set cali_min_val,cali_max_val
        void set_CALIval(uint16_t, uint16_t, uint16_t); // set cali_min_val, cali_mid_val, cali_max_val
        uint16_t get_LPFval(); // get current filtered potentiometer value range 0-4095
        uint16_t get_LPFval255(); // get current filtered potentiometer value range 0-255
        uint16_t get_RAWval(); // get current raw potentiometer value range 0-4095
        uint16_t get_RAWval255(); // get current raw potentiometer value range 0-255
        uint16_t get_CALI_LPFval255();// get current filtered and calibrated potentiometer value range 0-255
};


#define joystick_update_rate 5 // miiili


class JOYSTICK{
    private:
        enum joystick_index {index_lx, index_ly, index_rx, index_ry};
        JOYSTICK_AXIS *joysticks[4];
        bool calibrated_mid = 0; // is mid calibrated ? 
        bool calibrated_min_max = 0;  // is min max calibrated ? 

    public:
        void init(JOYSTICK_AXIS*, JOYSTICK_AXIS*, JOYSTICK_AXIS*, JOYSTICK_AXIS*);
        void calibrate(); // calibrate all
        void calibrate_mid(); // calibrate mid
        void calibrate_min_max(); // calibrate min and max
        bool is_calibrated(); // check if min mid max are calibrated
        void update(); // updte each jotstick axis
        int16_t get_lx(); // get potentiometer value
        int16_t get_ly(); // get potentiometer value
        int16_t get_rx(); // get potentiometer value
        int16_t get_ry(); // get potentiometer value


};

#endif