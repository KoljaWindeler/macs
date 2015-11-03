#include "stdint.h"

#define ON 0
#define BLINK 1
#define DELAYED_ON 2
#define OFF 3

#define BLINK_DELAY 500

// debug
//#define DEBUG_JKW

class LED {
    private:
        uint8_t m_pin=0;
        uint16_t m_delay=0;
        uint32_t m_next_action=0;
        uint8_t m_weak=0;
        uint8_t m_state=0;
        uint8_t m_inverse=0;
        
        uint8_t  m_current_mode=OFF;
        uint8_t  m_last_mode=OFF;
        
    
    public:
        LED(uint8_t pin,uint16_t delay, uint8_t pullup,uint8_t inverse);
        void check();
        void on();
        void on(uint8_t remember);
        void on_delayed();
        void off();
        void off(uint8_t remember);
        void blink();
        void resume();
        void toggle();
        uint8_t getState();
};
