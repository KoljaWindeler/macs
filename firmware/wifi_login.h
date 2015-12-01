#include "config.h"

bool set_update_login(LED *green, LED *red);
bool set_macs_login(LED *green, LED *red);
bool set_login(LED *green, LED *red, uint8_t mode);
bool get_wifi_config(uint8_t id, String *_SSID, String *_pw, int *_type);
bool check_wifi_config(String SSID,String pw,uint8_t type,uint8_t chk);
bool parse_wifi();
