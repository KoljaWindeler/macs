#include "stdint.h"

class LED {
    private:
        uint32_t m_switch_off=0;
        uint8_t m_pin=0;
        uint16_t m_delay=0;
    
    public:
        LED(uint8_t pin,uint16_t delay);
        void check();
        void on();
        void on_dalayed();
        void off();
};
