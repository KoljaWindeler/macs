// hardware
#define MAX_JUMPER_PIN 6 // 2^(6+1)-1=127

#define DB_LED_AND_UPDATE_PIN   D0
#define TAG_IN_RANGE_INPUT      D1
#define ANTENNA_PIN             D2
#define RELAY_PIN               D3
#define RED_LED_PIN             D4
#define READ_BACK_PIN           D5
#define GREEN_LED_PIN           D6

// storage design:  valid adresses are 0-2047
// [0..3] key 1
// [4..7] key 2
// ...
// [1908..1911] key 477
// [1912..1915] key 478

// [1918] SSID of Macs network, max 20 byte
// [1938] key of Macs network, max 20 byte
// [1958] wifi type of Macs network, max 1 byte
// [1959] checksum  for information above

// [1960] SSID of FIRST update network, max 20 byte
// [1980] key of FIRST update network, max 20 byte
// [2000] wifi type of FIRST update network, max 1 byte
// [2001] checksum for information above

// [2002] SSID of BACKUP update network, max 20 byte
// [2022] key of BACKUP update network, max 20 byte
// [2042] wifi type of BACKUP update network, max 1 byte
// [2043] checksum  for information above

// [2044,2045] total number of keys
// [2046,2047] key number check (key number+1) 

#define MAX_KEYS                478 // max number of keys, total nr +1 = 479
#define TAGSTRINGSIZE           5 // tag length
#define EEPROM_MAX              2047 // hw limit
#define START_WIFI_MACS         1918
#define START_WIFI_UPDATE_1     1960
#define START_WIFI_UPDATE_2     2002
#define KEY_NUM_EEPROM_HIGH     KEY_NUM_EEPROM_LOW-1
#define KEY_NUM_EEPROM_LOW      KEY_CHECK_EEPROM_HIGH-1
#define KEY_CHECK_EEPROM_HIGH   KEY_CHECK_EEPROM_LOW-1
#define KEY_CHECK_EEPROM_LOW    EEPROM_MAX
#define WIFI_MACS               0
#define WIFI_UPDATE_1           1
#define WIFI_UPDATE_2           2

// macros
#define RELAY_CONNECTED         1
#define RELAY_DISCONNECTED      0

#define LOG_RELAY_CONNECTED     1
#define LOG_RELAY_DISCONNECTED  2
#define LOG_LOGIN_REJECTED      3
#define LOG_NOTHING             4

// debug
//#define DEBUG_JKW 1
//#define DEBUG_JKW_MAIN 1
//#define DEBUG_JKW_LED 1
//#define DEBUG_JKW_WIFI  1

// settings
#define DB_UPDATE_TIME          10*60 // seconds between two auto updates from the server
#define MIN_UPDATE_TIME         30 // seconds between two database request, to avoid flooding, remember this has to be smaller then db_update_time
#define RED_LED_DELAY           1000 // ms
#define GREEN_LED_DELAY         1000 // ms
#define DB_LED_DELAY            1000 // ms
#define SEC_WAIT_BOOTUP         5 // 5sec of led toggling to show that we are starting

// network
#define HOSTPORT 80
