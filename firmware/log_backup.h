#include "config.h"
#define BACKUP_MAX 50


class BACKUP
{
    private:
        uint8_t     m_event[BACKUP_MAX];
        uint32_t    m_badge[BACKUP_MAX];
        uint32_t    m_extrainfo[BACKUP_MAX];
        uint8_t     m_position;
    
    public:
        BACKUP();
        uint8_t add(uint8_t event,uint32_t badge,uint32_t extrainfo);
        uint8_t get(uint8_t *event,uint32_t *badge,uint32_t *extrainfo);
        bool has_backups();
        void rem(uint8_t pos);
        void try_fire();
};
