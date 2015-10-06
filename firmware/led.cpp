#include "stdint.h"
#include "led.h"
#include "application.h"

LED::LED(uint8_t pin,uint16_t delay){
    m_pin=pin;
    m_delay=delay;
    pinMode(m_pin,OUTPUT);
};

void LED::on_dalayed(){
    if(m_switch_off==0){
        digitalWrite(m_pin,1);
    };
    m_switch_off=millis()+m_delay;
};

void LED::on(){
    digitalWrite(m_pin,1);
    m_switch_off=0;
}

void LED::check(){
    if(m_switch_off>0 && m_switch_off<millis()){
        digitalWrite(m_pin,0);
        m_switch_off=0;
    };
};

void LED::off(){
    digitalWrite(m_pin,0);
    m_switch_off=0;
};


