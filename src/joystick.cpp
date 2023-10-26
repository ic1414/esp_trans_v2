#include "joystick.h"


void JOYSTICK_AXIS::init(uint8_t which_pin, bool is_invert){
    pin = which_pin;
    pinMode(pin, INPUT);
    invert = is_invert;

}



void JOYSTICK_AXIS::setLSP(float a, float b){
    LPFa = a;
    LPFb = b;
}

void JOYSTICK_AXIS::update(){
    if (invert) cur_val = adc_maxval - analogRead(pin);
    else cur_val = analogRead(pin);
    lpf_val = (float)cur_val * LPFb + (float)pre_val * LPFa;
    pre_val = cur_val;
}


uint16_t JOYSTICK_AXIS::get_LPFval(){
    return lpf_val;
}
uint16_t JOYSTICK_AXIS::get_LPFval255(){
    return map(lpf_val, adc_minval, adc_maxval, 0, 255);
}
uint16_t JOYSTICK_AXIS::get_RAWval(){
    return cur_val;
}
uint16_t JOYSTICK_AXIS::get_RAWval255(){
    return map(cur_val, adc_minval, adc_maxval, 0, 255);
}


void JOYSTICK_AXIS::set_CALIval(uint16_t mid){
    cali_mid_val = mid;
}

void JOYSTICK_AXIS::set_CALIval(uint16_t min, uint16_t max){
    cali_min_val = min;
    cali_max_val = max;
}

void JOYSTICK_AXIS::set_CALIval(uint16_t min, uint16_t mid, uint16_t max){
    cali_min_val = min;
    cali_mid_val = mid;
    cali_max_val = max;
}


uint16_t JOYSTICK_AXIS::get_CALI_LPFval255(){
    if(lpf_val >= cali_mid_val) return map(lpf_val, cali_mid_val, cali_max_val, adc_midval255, adc_maxval255);
    else return map(lpf_val, cali_min_val, cali_mid_val, adc_minval255, adc_midval255);
}






void JOYSTICK::init(JOYSTICK_AXIS *lx, JOYSTICK_AXIS *ly, JOYSTICK_AXIS *rx, JOYSTICK_AXIS *ry){
    joysticks[0] = lx;
    joysticks[1] = ly;
    joysticks[2] = rx;
    joysticks[3] = ry;

    EEPROM.begin(2*3*4+1);
    if(EEPROM.readByte(24) == 1){
        for(int16_t i=0; i<4; i++){
            int16_t temp[3];
            for(int16_t j=0; j<3; j++){
                temp[j] = EEPROM.readShort(i*6+j*2);
            }
            joysticks[i]->set_CALIval(temp[0], temp[1], temp[2]);
        }
    }
}

bool JOYSTICK::is_calibrated(){
    if(calibrated_mid and calibrated_min_max) return 1;
    else return 0;
}

void JOYSTICK::calibrate(){
    calibrate_mid();
    calibrate_min_max();
}


// dont calibrate throttle !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void JOYSTICK::calibrate_mid(){
    const int16_t avg_mid_iteration = 20;
    float avg_mid[4] = {adc_midval, adc_minval, adc_minval, adc_minval};
    for(int16_t i=0; i<avg_mid_iteration; i++){
        for(int16_t j=1; j<4; j++){
            joysticks[j]->update();
            avg_mid[j] += (float)(joysticks[j]->get_LPFval())/(float)avg_mid_iteration;
        }
        delay(50);
    }
    for(int16_t i=0; i<4; i++) joysticks[i]->set_CALIval((int16_t)avg_mid[i]);
    calibrated_mid = 1;

    for(int16_t i=0; i< 4; i++){
        for(int16_t j=0; j< 3; j++){
            if(j==1) EEPROM.writeShort(i*6+j*2, (int16_t)avg_mid[i]);
            
        }
    }

    if(is_calibrated()) EEPROM.writeByte(24, 1);
    EEPROM.commit();
    
}


void JOYSTICK::calibrate_min_max(){

    const int16_t max_min_iteration = 1500;
    int16_t max_read[4] = {adc_midval, adc_midval, adc_midval, adc_midval};
    int16_t min_read[4] = {adc_midval, adc_midval, adc_midval, adc_midval};
    for(int16_t i=0; i<max_min_iteration; i++){
        for(int16_t j=0; j<4; j++){
            joysticks[j]->update();
            if(joysticks[j]->get_LPFval() > max_read[j]) max_read[j] = joysticks[j]->get_LPFval();
            if(joysticks[j]->get_LPFval() < min_read[j]) min_read[j] = joysticks[j]->get_LPFval();
        }
        delay(5);
    }

    for(int16_t k=0; k<4; k++) joysticks[k]->set_CALIval(min_read[k], max_read[k]);
    calibrated_min_max = 1;

    for(int16_t i=0; i< 4; i++){
        for(int16_t j=0; j< 3; j++){
            if(j==0) EEPROM.writeShort(i*6+j*2, min_read[i]);
            if(j==2) EEPROM.writeShort(i*6+j*2, max_read[i]);
            
        }
    }
    
    if(is_calibrated()) EEPROM.writeByte(24, 1);
    EEPROM.commit();
}


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



int16_t JOYSTICK::get_lx(){ return joysticks[index_lx]->get_CALI_LPFval255(); }
int16_t JOYSTICK::get_ly(){ return joysticks[index_ly]->get_CALI_LPFval255(); }
int16_t JOYSTICK::get_rx(){ return joysticks[index_rx]->get_CALI_LPFval255(); }
int16_t JOYSTICK::get_ry(){ return joysticks[index_ry]->get_CALI_LPFval255(); }

