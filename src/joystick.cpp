#include "joystick.h"


// init
void JOYSTICK_AXIS::init(uint8_t which_pin, bool is_invert){
    pin = which_pin;
    pinMode(pin, INPUT);
    invert = is_invert;
}


// 设置低通滤波的参数。LPFa & LPFb
void JOYSTICK_AXIS::setLSP(float a, float b){
    LPFa = a;
    LPFb = b;
}


// analog read and update pre_val, cur_val, lpf_val
void JOYSTICK_AXIS::update(){
    if (invert) cur_val = adc_maxval - analogRead(pin);
    else cur_val = analogRead(pin);

    lpf_val = (float)cur_val * LPFb + (float)pre_val * LPFa; //一阶数字低通滤波
    pre_val = cur_val;
}


// get filtered value. range 0-4095
uint16_t JOYSTICK_AXIS::get_LPFval(){
    return lpf_val;
}


// get filter value. range 0-255
uint16_t JOYSTICK_AXIS::get_LPFval255(){
    return map(lpf_val, adc_minval, adc_maxval, 0, 255);
}


// get raw value. range 0-4095
uint16_t JOYSTICK_AXIS::get_RAWval(){
    return cur_val;
}


// get raw value. range 0-255
uint16_t JOYSTICK_AXIS::get_RAWval255(){
    return map(cur_val, adc_minval, adc_maxval, 0, 255);
}


// set calibration parameters
void JOYSTICK_AXIS::set_CALIval(uint16_t mid){
    cali_mid_val = mid;
}


// set calibration parameters
void JOYSTICK_AXIS::set_CALIval(uint16_t min, uint16_t max){
    cali_min_val = min;
    cali_max_val = max;
}


// set calibration parameters
void JOYSTICK_AXIS::set_CALIval(uint16_t min, uint16_t mid, uint16_t max){
    cali_min_val = min;
    cali_mid_val = mid;
    cali_max_val = max;
}


// get calibrated and filtered value. range 0-255
uint16_t JOYSTICK_AXIS::get_CALI_LPFval255(){
    if(lpf_val >= cali_mid_val) return map(lpf_val, cali_mid_val, cali_max_val, adc_midval255, adc_maxval255);
    else return map(lpf_val, cali_min_val, cali_mid_val, adc_minval255, adc_midval255);
}





// ------------------------------------------分割线--------------------------------------------------------------





void JOYSTICK::init(JOYSTICK_AXIS *lx, JOYSTICK_AXIS *ly, JOYSTICK_AXIS *rx, JOYSTICK_AXIS *ry){
    joysticks[0] = lx;
    joysticks[1] = ly;
    joysticks[2] = rx;
    joysticks[3] = ry;

    // use eeprom to store and restore calibration parameters
    // store format -> j1_min, j1_mid, j1_max, j2_min, j2_mid, j2_max ... j3_min, j3_mid, j3_max, is_calibrated
    EEPROM.begin(2*3*4+1); // int16_t 2byte, 3 calibration values, 4 potentiometers, 1 extra byte to check if have calibration values stored
    if(EEPROM.readByte(24) == 1){

        // 4 potentiometers
        for(int16_t i=0; i<4; i++){

            // 3 calibration values
            int16_t temp[3];
            for(int16_t j=0; j<3; j++) temp[j] = EEPROM.readShort(i*6+j*2);

            joysticks[i]->set_CALIval(temp[0], temp[1], temp[2]);
        }

    }

}


 // check if min mid max are all calibrated
bool JOYSTICK::is_calibrated(){
    if(calibrated_mid and calibrated_min_max) return 1;
    else return 0;
}


// 校准全部
void JOYSTICK::calibrate(){
    calibrate_mid();
    calibrate_min_max();
}


// 校准自然状态下的摇杆。 因为油门不回中，所以不要校准！
void JOYSTICK::calibrate_mid(){

    const int16_t avg_mid_iteration = 20; // 采样20次
    float avg_mid[4] = {adc_midval, adc_minval, adc_minval, adc_minval}; // 记录采样平均值

    for(int16_t i=0; i<avg_mid_iteration; i++){
        // joysticks[0] 是油门，所以for循环从1开始
        for(int16_t j=1; j<4; j++){
            joysticks[j]->update();
            avg_mid[j] += (float)(joysticks[j]->get_LPFval())/(float)avg_mid_iteration;
        }
        delay(50); // 50 x 20 = 1000，算上其他的总共需要一秒多一点
    }

    // 计算出平均值后设置校准参数
    for(int16_t i=0; i<4; i++) joysticks[i]->set_CALIval((int16_t)avg_mid[i]);
    calibrated_mid = 1; // 记录下 mid(middle) 已校准

    // store format -> j1_min, j1_mid, j1_max, j2_min, j2_mid, j2_max ... j3_min, j3_mid, j3_max, is_calibrated
    for(int16_t i=0; i< 4; i++){
        for(int16_t j=0; j< 3; j++){
            if(j==1) EEPROM.writeShort(i*6+j*2, (int16_t)avg_mid[i]);            
        }
    }
    
    // 在eeprom记录已校准
    if(is_calibrated()) EEPROM.writeByte(24, 1);
    EEPROM.commit(); // 保存 mid 校准参数
}


// 校准摇杆的最高值和最低值
void JOYSTICK::calibrate_min_max(){

    const int16_t max_min_iteration = 1500; // 采样1500次
    int16_t max_read[4] = {adc_midval, adc_midval, adc_midval, adc_midval}; // 记录采样最高值
    int16_t min_read[4] = {adc_midval, adc_midval, adc_midval, adc_midval}; // 记录采样最低值

    // 获取最高和最低值
    for(int16_t i=0; i<max_min_iteration; i++){
        for(int16_t j=0; j<4; j++){
            joysticks[j]->update();
            if(joysticks[j]->get_LPFval() > max_read[j]) max_read[j] = joysticks[j]->get_LPFval();
            if(joysticks[j]->get_LPFval() < min_read[j]) min_read[j] = joysticks[j]->get_LPFval();
        }
        delay(5);
    }

    // 设置校准参数
    for(int16_t k=0; k<4; k++) joysticks[k]->set_CALIval(min_read[k], max_read[k]);
    calibrated_min_max = 1; // 记录下 min & max 已校准

    // store format -> j1_min, j1_mid, j1_max, j2_min, j2_mid, j2_max ... j3_min, j3_mid, j3_max, is_calibrated
    for(int16_t i=0; i< 4; i++){
        for(int16_t j=0; j< 3; j++){
            if(j==0) EEPROM.writeShort(i*6+j*2, min_read[i]);
            if(j==2) EEPROM.writeShort(i*6+j*2, max_read[i]);
            
        }
    }
    
    // 在eeprom记录已校准
    if(is_calibrated()) EEPROM.writeByte(24, 1);
    EEPROM.commit(); // 保存 min，max 校准参数
}


// 每隔x秒 更新/采样 全部摇杆
void JOYSTICK::update(){
    static unsigned long timer_joystick = millis();
    static unsigned long timer_serial_print = millis();

    if(millis() - timer_joystick >= joystick_update_rate){
        timer_joystick = millis();
        joysticks[0]->update();
        joysticks[1]->update();
        joysticks[2]->update();
        joysticks[3]->update();    
    }

    // 串口打印摇杆值
    if(serial_print_joy and millis() - timer_serial_print >= joystick_update_rate*10){
        timer_serial_print = millis();
        Serial.print(get_lx());
        Serial.print("  ");
        Serial.print(get_ly());
        Serial.print("  ");
        Serial.print(get_rx());
        Serial.print("  ");
        Serial.print(get_ry());
        Serial.print("\n");
    }
}


// get potentiometer value
int16_t JOYSTICK::get_lx(){ return joysticks[index_lx]->get_CALI_LPFval255(); }

// get potentiometer value
int16_t JOYSTICK::get_ly(){ return joysticks[index_ly]->get_CALI_LPFval255(); }

// get potentiometer value
int16_t JOYSTICK::get_rx(){ return joysticks[index_rx]->get_CALI_LPFval255(); }

// get potentiometer value
int16_t JOYSTICK::get_ry(){ return joysticks[index_ry]->get_CALI_LPFval255(); }

