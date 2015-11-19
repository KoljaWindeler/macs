#include "application.h"
#include "stdint.h"

void set_update_login();
void set_macs_login();
bool set_wifi_config(uint8_t id);
bool check_wifi_config(String SSID,String pw,uint8_t type,uint8_t chk);
bool parse_wifi();
