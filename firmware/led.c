#include "stdint.h"
#include "led.h"
#include "application.h"

//#define DEBUG_JKW


LED::LED(uint8_t pin,uint16_t delay, uint8_t weak, uint8_t inverse){
    m_pin=pin;
    m_delay=delay;
    m_weak=weak;
    m_state=OFF;
    m_inverse=inverse;
    
    // nonsense state to force update with next off()-call
    m_state=99; 
    off(0);
    
    m_next_action=0;
    m_current_mode=OFF;
    m_last_mode=OFF;
};

void LED::on_delayed(){
    #ifdef DEBUG_JKW
    Serial.print("Swtich led ");
    Serial.print(m_pin);
    Serial.println(" on delayed");
    #endif
    
    if(m_current_mode!=DELAYED_ON){
        m_last_mode=m_current_mode;
        m_current_mode=DELAYED_ON;
    }
    
    on(0);  // on, without remember mode, to keep our mode
    m_next_action=millis()+m_delay;
};

// invert LED
void LED::toggle(){
    if(m_state==OFF){
        on();
    } else {
        off();
    }
}


// standard "on()" should save the old mode of the LED, so call with with remember
void LED::on(){
    on(1);
}
void LED::on(uint8_t remember){
    #ifdef DEBUG_JKW
    Serial.print("Swtich led ");
    Serial.print(m_pin);
    Serial.println(" on");
    #endif
    
    // mode saving?
    if(remember){
        if(m_current_mode!=ON){
            m_last_mode=m_current_mode;
            m_current_mode=ON;
        }
    }
    
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
        m_next_action=0;
    };
}

// standard "off()" should save the old mode of the LED, so call with with remember
void LED::off(){
    off(1);
};
void LED::off(uint8_t remember){
    // save the state
    if(remember){
        if(m_current_mode!=OFF){
            m_last_mode=m_current_mode;
            m_current_mode=OFF;
        }
    }
    
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
        m_next_action=0;
    };
};

void LED::check(){
    if(m_next_action!=0 && millis()>m_next_action){
        if(m_current_mode==BLINK){
            if(m_state==ON){
                off(0);
            } else {
                on(0);
            };
            m_next_action=millis()+BLINK_DELAY;
        } else if(m_current_mode==DELAYED_ON) {
            resume();
        };
    };
};


void LED::blink(){
    #ifdef DEBUG_JKW
    Serial.print("Swtich led ");
    Serial.print(m_pin);
    Serial.println(" to blink");
    #endif
    
    if(m_current_mode!=BLINK){
        m_last_mode=m_current_mode;
        m_current_mode=BLINK;
    }
    m_next_action=millis()-1;
    check();
}

void LED::resume(){
    #ifdef DEBUG_JKW
    Serial.print("resume led ");
    Serial.println(m_pin);
    #endif
    
    if(m_current_mode==m_last_mode){
        off();
    } else {
        m_current_mode=m_last_mode;
    };
    
    if(m_current_mode==BLINK || m_current_mode==DELAYED_ON){
        m_next_action=millis()-1;
        check();
    } else if(m_current_mode==ON){
        on();
    } else if(m_current_mode==OFF){
        off();
    };
}

uint8_t LED::getState(){
    return m_state;
}
