#include "config.h"

BACKUP::BACKUP(){
    m_position=0;
};


uint8_t BACKUP::add(uint8_t event,uint32_t badge,uint32_t extrainfo){
    m_event[m_position]=event;
    m_badge[m_position]=badge;
    m_extrainfo[m_position]=extrainfo;
    
    return m_position++;
}
    
bool BACKUP::has_backups(){
    if(m_position>0){
        return true;
    }
    return false;
}
    
uint8_t BACKUP::get(uint8_t *event,uint32_t *badge,uint32_t *extrainfo){
    if(m_position>0){
        *event=m_event[m_position-1];
        *badge=m_badge[m_position-1];
        *extrainfo=m_extrainfo[m_position-1];
        return (m_position-1);
    }
    return -1;
}
    
void BACKUP::rem(uint8_t pos){
    if(pos==m_position-1){
        m_position--;
    } else if(pos>m_position){
        return;
    } // else move all entries ...
}

void BACKUP::try_fire(){
    bool run=true;
    while(has_backups() && run){
        uint8_t event;
        uint32_t badge;
        uint32_t extrainfo;
        uint8_t pos;
        
        pos=get(&event,&badge,&extrainfo);
        if(fire_report(event, badge, extrainfo)){
            rem(pos);
        } else {
            run=false;
        }
    }
}
