#include "stdint.h"
#include "led.h"
#include "application.h"
#include "config.h"

LED::LED(uint8_t pin,uint16_t delay, uint8_t weak, uint8_t inverse){
    m_pin=pin;
    m_delay=delay;
    m_weak=weak;
    m_state=OFF;
    m_inverse=inverse;
    
    m_state=OFF; 
    m_next_action=0;
    
    clear();
};

void LED::clear(){
    for(int i=0;i<MAX_MODE_HIST;i++){
        m_mode[i]=OFF;
    }
    hw_off();
}

void LED::on_delayed(){
    #ifdef DEBUG_JKW_LED
    Serial.print("Swtich led ");
    Serial.print(m_pin);
    Serial.println(" on delayed");
    #endif
    
    for(int i=MAX_MODE_HIST-1;i>0;i--){
        m_mode[i]=m_mode[i-1];
    }
    m_mode[0]=DELAYED_ON;

    hw_on();
    m_next_action=millis()+m_delay;
};

// invert LED
void LED::toggle(){
    if(m_state==OFF){
        hw_on();
    } else {
        hw_off();
    }
}


// standard "on()" should save the old mode of the LED, so call with with remember
void LED::on(){
    #ifdef DEBUG_JKW_LED
    Serial.print("Swtich led ");
    Serial.print(m_pin);
    Serial.println(" on");
    #endif
    
    // mode saving
    for(int i=MAX_MODE_HIST-1;i>0;i--){
        m_mode[i]=m_mode[i-1];
    }
    m_mode[0]=ON;

    hw_on();
    m_next_action=0;
}


void LED::hw_on(){
    // just change it if we haven't been in that state anyway
    if(m_state!=ON){
        if(!m_inverse){
            // not-invertet is tricky, is it pullup or HIGH?
            if(m_weak){
                pinMode(m_pin,INPUT_PULLUP);
            } else {
                pinMode(m_pin,OUTPUT);
                digitalWrite(m_pin,1);
            };
        } else {
            // invertet is straight GND
            pinMode(m_pin,OUTPUT);
            digitalWrite(m_pin,0);
        }
        m_state=ON;
    };
}

void LED::off(){
    #ifdef DEBUG_JKW_LED
    Serial.print("Swtich led ");
    Serial.print(m_pin);
    Serial.println(" to off");
    #endif
    
    // save the state
    for(int i=MAX_MODE_HIST-1;i>0;i--){
        m_mode[i]=m_mode[i-1];
    }
    m_mode[0]=OFF;
    
    hw_off();
    m_next_action=0;
}
    
void LED::hw_off(){
    // only call it if needed
    if(m_state!=OFF){
        if(!m_inverse){
            // off without inversion is straight GND
            pinMode(m_pin,OUTPUT);
            digitalWrite(m_pin,0);
        } else {
            // invertet is tricky, is it pullup or HIGH?
            if(m_weak){
                pinMode(m_pin,INPUT_PULLUP);
            } else {
                pinMode(m_pin,OUTPUT);
                digitalWrite(m_pin,1);
            };
        }
        m_state=OFF;
    };
};


void LED::check(){
    if(m_next_action!=0 && millis()>m_next_action){
        if(m_mode[0]==BLINK){
            toggle();
            m_next_action=millis()+BLINK_DELAY;
        } else if(m_mode[0]==DELAYED_ON) {
            resume();
        };
    };
};


void LED::blink(){
    #ifdef DEBUG_JKW_LED
    Serial.print("Swtich led ");
    Serial.print(m_pin);
    Serial.println(" to blink");
    #endif
    
    for(int i=MAX_MODE_HIST-1;i>0;i--){
        m_mode[i]=m_mode[i-1];
    }
    m_mode[0]=BLINK;
    

    m_next_action=millis()-1;
    check();
}

void LED::resume(){
    #ifdef DEBUG_JKW_LED
    Serial.print("resume led ");
    Serial.println(m_pin);
    #endif
    
    
    for(int i=0;i<MAX_MODE_HIST-1;i++){
        m_mode[i]=m_mode[i+1];
    }
    m_mode[MAX_MODE_HIST-1]=OFF;
    
    
    #ifdef DEBUG_JKW_LED
    Serial.print("to state ");
    Serial.println(m_mode[0]);
    #endif
    
    
    if(m_mode[0]==BLINK){
        m_next_action=millis()-1;
        check();
    } else if(m_mode[0]==DELAYED_ON){
        hw_on();
        m_next_action=millis()+m_delay;
    } else if(m_mode[0]==ON){
        hw_on();
        m_next_action=0;
    } else if(m_mode[0]==OFF){
        hw_off();
        m_next_action=0;
    };
}

uint8_t LED::getState(){
    return m_state;
}
