#include "mynrf.h"
#include "config.h"



RF24 radio(pin_nrf_ce, pin_nrf_csn); // CE, CSN


void MYNRF::init(){
    radio.begin();
    radio.enableDynamicPayloads();
    radio.enableAckPayload();
    // radio.setChannel(1); //设置通道
    for(int i=0; i<6; i++) radio.openReadingPipe(i, addr_receiving[i]); 
    radio.openWritingPipe(addr_sending[5]);//default sending
    radio.setPALevel(RF24_PA_MAX);
    radio.stopListening();
}


// 改变通信 地址 和 通道
void MYNRF::change_commu_pipe(uint8_t pipe, uint8_t channel=0){
    radio.setChannel(1);
    radio.openWritingPipe(addr_sending[pipe]);
}


// 发送数据
bool MYNRF::send_control(bool en, bool extra, byte x, byte y, byte z, byte t){
    if(locked == 1) return false; // 如果没解锁，返回 false
    data_control.en = en;
    data_control.extra = extra;
    data_control.channels[0] = x;
    data_control.channels[1] = y;
    data_control.channels[2] = z;
    data_control.channels[3] = t;

    return radio.write(&data_control, sizeof(data_control));
}


// 接受数据
bool MYNRF::receive(bool report){
    if(!report) return 0;
    if (radio.available()) 
    {
        radio.read(&data_droneData, sizeof(data_droneData));
        last_receive_time = millis();
    }
    return 1;
}


// 检查是否丢失连接。
bool MYNRF::is_lost(){
    if (millis() - last_receive_time > 500) {
        data_droneData.voltage = 0.0;
        for (int8_t i=0; i<4; i++) data_droneData.motorOut[i] = 0;
        for (int8_t i=0; i<3; i++) data_droneData.cur_gyro[i] = 0;
        for (int8_t i=0; i<3; i++) data_droneData.cur_angle[i] = 0.0;
        return 1;
    }
    return 0;
}

// 锁定nrf
void MYNRF::lock(){ locked = 1; }

// 解锁nrf
void MYNRF::unlock(){ locked = 0; }

// 查看是否上锁
bool MYNRF::is_locked(){ return locked; }
