#ifndef mynrf_h
#define mynrf_h

#include <Arduino.h>
#include "config.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>



// nrf24l01 发送的数据格式
struct nrf_sending_control{
    byte data_type = 0b10000001;
    bool en = 0;
    bool extra = 0;
    uint8_t channels[4] = {0, 0, 0, 0};
    uint8_t fill_gap[25] = {(byte)0};
};


// nrf24l01 接收的数据格式
struct nrf_receiving_droneData{
    byte data_type = 0b10001111;
    uint8_t voltage = 0;
    uint8_t motorOut[4];
    uint8_t cur_gyro[6];
    uint8_t cur_angle[3];
    uint8_t fill_gap[17] = {(byte)0};
};



#define nrf_sending_addr1 "00001"
#define nrf_sending_addr2 "00002"
#define nrf_sending_addr3 "00003"
#define nrf_sending_addr4 "00004"
#define nrf_sending_addr5 "00005"
#define nrf_sending_addr6 "00006" //dont use this one。忘记为什么不要用这个了。

#define nrf_receiving_addr1 "00007"
#define nrf_receiving_addr2 "00008"
#define nrf_receiving_addr3 "00009"
#define nrf_receiving_addr4 "00010"
#define nrf_receiving_addr5 "00011"
#define nrf_receiving_addr6 "00012" //dont use this one。忘记为什么不要用这个了。



class MYNRF{
    private:
        bool locked = 1; // 记录是否上锁
        unsigned long last_receive_time = millis();
        const byte defautX = 127; // 摇杆默认值。范围是0-255。
        const byte defautY = 127; // 摇杆默认值。范围是0-255。
        const byte defautZ = 127; // 摇杆默认值。范围是0-255。
        const byte defautT = 0; // 摇杆默认值。范围是0-255。（T- throttle - 油门）
        const byte addr_sending[6][6] = {nrf_sending_addr1, nrf_sending_addr2, nrf_sending_addr3,
                                         nrf_sending_addr4, nrf_sending_addr5, nrf_sending_addr6};
        const byte addr_receiving[6][6] = {nrf_receiving_addr1, nrf_receiving_addr2, nrf_receiving_addr3,
                                           nrf_receiving_addr4, nrf_receiving_addr5, nrf_receiving_addr6};
        struct nrf_sending_control data_control;  
        
    public:
        void init();
        void change_commu_pipe(uint8_t, uint8_t);
        bool send_control(bool, bool, byte, byte, byte, byte);
        bool receive(bool);
        bool is_lost();
        void lock();
        void unlock();
        bool is_locked();
        struct nrf_receiving_droneData data_droneData;

};




#endif