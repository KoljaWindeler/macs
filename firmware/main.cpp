/* LED Patter
* red,green,red,green  -> i give you 10 sec to connect to me, before I start
* red on, green on -> I'm trying to connect to my server
* UPDATE MODE
* red and green blink simultainous -> I'm ready for an update
* MACS MODE (status can be combined)
* red blinking -> no connection to the MACS Server
* red solid -> card rejected
* green blinking -> connected to the MACS Server
* green solid -> card accepted
*/

// This #include statement was automatically added by the Particle IDE.
#include "wifi_login.h"
#include "rest_client.h"
#include "led.h"
#include "application.h"
#include "stdint.h"

// hardware
#define MAX_JUMPER_PIN 6 // 2^(6+1)-1=127

#define RELAY_PIN D3
#define RED_LED_PIN D4
#define GREEN_LED_PIN D6
#define DB_LED_AND_UPDATE_PIN D0
#define TAG_IN_RANGE_INPUT D1

// storage
#define MAX_KEYS 200 // max number of keys
#define TAGSTRINGSIZE 5 // tag length

// macros
#define RELAY_CONNECTED 1
#define RELAY_DISCONNECTED 0

#define LOG_RELAY_CONNECTED 1
#define LOG_RELAY_DISCONNECTED 2
#define LOG_LOGIN_REJECTED 3
#define LOG_NOTHING 4

// debug
#define DEBUG_JKW

// settings
#define DB_UPDATE_TIME 10*60 // seconds between two auto updates from the server
#define MIN_UPDATE_TIME 30 // seconds between two database request, to avoid flooding, remember this has to be smaller then db_update_time
#define RED_LED_DELAY 1000 // ms
#define GREEN_LED_DELAY 1000 // ms
#define DB_LED_DELAY 1000 // ms
#define SEC_WAIT_BOOTUP 5 // 5sec of led toggling to show that we are starting

// network
IPAddress HOSTNAME(192,168,188,23);
#define HOSTPORT 80

uint8_t keys_available=0;
uint32_t keys[MAX_KEYS];

uint8_t currentTagBuf[TAGSTRINGSIZE];
uint8_t currentTagIndex=0;
uint8_t connected=0;
uint32_t currentTag=-1;

uint8_t current_relay_state=RELAY_DISCONNECTED;
uint8_t id=-1; //255, my own id
uint8_t tagInRange=0;
uint32_t last_key_update=0;
uint32_t last_server_request=0;
uint32_t relay_open_timestamp=0;

LED db_led(DB_LED_AND_UPDATE_PIN,DB_LED_DELAY,1,1); // weak + inverse
LED red_led(RED_LED_PIN,RED_LED_DELAY,0,0);
LED green_led(GREEN_LED_PIN,GREEN_LED_DELAY,0,0);

SYSTEM_MODE(MANUAL);// do not connect on your own

// http server
RestClient client = RestClient(HOSTNAME);


void setup() {
    for(uint8_t i=10; i<=MAX_JUMPER_PIN+10; i++){   // A0..7 is 10..17, used to read my ID
       pinMode(i,INPUT_PULLUP);
    }
    pinMode(RELAY_PIN,OUTPUT);          // driver for the relay
    pinMode(TAG_IN_RANGE_INPUT,INPUT);
    // the db led is a weak and inverterted LED on the same pin as the update_input, this will set the pin to input_pullup anyway //pinMode(DB_LED_AND_UPDATE_PIN,INPUT_PULLUP);
    
    Serial.begin(115200);
    Serial1.begin(9600);

    red_led.on();

    // start sequence, to remind user to set mode
    for(uint i=0;i<SEC_WAIT_BOOTUP;i++){
        Serial.print(i+1);
        Serial.print("/");
        Serial.println(5);
        
        // 1 sec
        for(uint8_t ii=0; ii<5; ii++){
            delay(200);
            red_led.toggle();
            green_led.toggle();
        }
    }
    red_led.off();
    green_led.off();
    
    
    // read mode to starting with
    if(digitalRead(DB_LED_AND_UPDATE_PIN)){
        Serial.println("- MACS -");
        red_led.on();
        green_led.on();
        
        set_macs_login();
        WiFi.connect();
        
        uint8_t i=0;
        uint8_t j=0;
        Serial.println("- Wifi on -");
        while(i<20){
            if(j!=millis()/1000){
                j=millis()/1000;
                i++;
                Serial.print("try ");
                Serial.print(i);
                Serial.println("/20 wifi");
                if(WiFi.ready()){
                    Serial.println("connected!");
                    break;
                }
            }
            delay(200);
        }
        Serial.println("- Updating -");
        
        if(!update_ids()){
            set_connected(0,true); // force LED update
            read_EEPROM();
        } else {
            set_connected(1);
        }
    
    } else {
        red_led.on();
        green_led.on();
        db_led.on();
        Serial.println("- Cloud -");
        set_update_login();
        Particle.connect();
        uint8_t i=0;
        // stay in update mode forever
        while(1){
            if(i!=millis()/1000){
                Serial.print(i);
                Serial.print(": ");
                if(Particle.connected()){
                    // as soon as we are connected, swtich to blink mode to make it visible
                    if(!connected){
                        red_led.blink();
                        green_led.blink();
                        db_led.blink();
                        connected=1;
                    } else {
                        Particle.process();
                    }
                    
                    // keep blinking
                    red_led.check();
                    green_led.check();
                    db_led.check();
                    
                    Serial.println("Photon connected");
                } else {
                    Serial.println("Photon NOT connected");
                    // constant on == not yet connected
                    red_led.on();
                    green_led.on();
                    db_led.on();
                }
                i=millis()/1000;
            } // i!=millis()/1000
            delay(200); // don't go to high as blink will look odd
        }
    }
}


// woop woop main loop
void loop() {
    // check if we found a tag
    if(tag_found(currentTagBuf,&currentTag)){
        // if we found a tag, test it
        // if it works close relay,
        // if not - ask the server for an update and try again
        uint8_t tries=2; // two tries to check the card
        while(tries){
            // compares known keys, returns true if key is known
            if(access_test(currentTag)){
                relay(RELAY_CONNECTED);
                tries=0;
                // takes long
                create_report(LOG_RELAY_CONNECTED,currentTag,0);
                green_led.on();
            } else {
                // if we have a card that is not known to be valid we should maybe check our database
                if(tries>1){
            
                    #ifdef DEBUG_JKW
                    Serial.println("Key not valid, requesting update from server");
                    #endif
                    
                    update_ids();
                        
                    
                    #ifdef DEBUG_JKW
                    if(tries>0){
                        Serial.println("Trying once more if key is valid now");
                    };
                    #endif
                    
                    tries-=1;
                } else {
                    
                    #ifdef DEBUG_JKW
                    Serial.println("key still not valid. :P");
                    #endif
                    
                    tries=0;   
                    // takes long
                    create_report(LOG_LOGIN_REJECTED,currentTag,0);
                    red_led.on();
                }
            }
        }
    }
    
    
    // card moved away
    if(digitalRead(TAG_IN_RANGE_INPUT)==0 && currentTag!=-1){
        // open the relay as soon as the tag is gone
        if(current_relay_state==RELAY_CONNECTED){
            uint32_t open_time_sec=relay(RELAY_DISCONNECTED);
            green_led.resume();
            // last because it takes long
            create_report(LOG_RELAY_DISCONNECTED,currentTag,open_time_sec);
        } else {
            red_led.resume();    
        }
        
        currentTag=-1;      // reset current user
        currentTagIndex=0;  // reset index counter for incoming bytes
        
    }
    
    // time based update the storage from the server (every 10 min?) 
    if(last_key_update+DB_UPDATE_TIME<(millis()/1000)){
        update_ids(); 
    }
    
    
    // maintain status led's
    if(red_led.getState()==BLINK && connected){
        red_led.off();
        green_led.blink();
    } else if(green_led.getState()==BLINK && !connected){
        red_led.blink();
        green_led.off();
    }
    
    // see if we should switch off the leds by now
    db_led.check();
    red_led.check();
    green_led.check();
    
    // test
    /*if(last_server_request+100<millis()){
        static int i=0;
        Serial.println(i);
        i++;
        //if(i%2){
        //     create_report(LOG_RELAY_DISCONNECTED,6215027,7);
        //} else {
        //    create_report(LOG_RELAY_CONNECTED,6215027,0);
        //}
        create_report(LOG_NOTHING,0,0);
    }*/
}



// callen from main loop as soon as a tag has been found to test if it matches one of the saved keys
bool access_test(uint32_t tag){
    #ifdef DEBUG_JKW
    Serial.print("Tag ");
    Serial.print(tag);
    Serial.print(" found. Checking database (");
    Serial.print(keys_available);
    Serial.print(") for matching key");
    Serial.println("==============");
    #endif
    
    for(uint8_t i=0;i<MAX_KEYS && i<keys_available; i++){

        #ifdef DEBUG_JKW
        Serial.print(i+1);
        Serial.print(" / ");
        Serial.print(keys_available);
        Serial.print(" Compare current read tag ");
        Serial.print(tag);
        Serial.print(" to stored key ");
        Serial.print(keys[i]);
        Serial.println("");
        #endif
        
        if(keys[i]==tag){

    #ifdef DEBUG_JKW
    Serial.println("Key valid, closing relay");
    #endif
    
            return true;
        }
    }

    #ifdef DEBUG_JKW
    Serial.println("==============");
    #endif
    
    return false;
}

// hardware controll, writing to the pin and log times
uint32_t relay(int8_t input){
    if(input==1){
        #ifdef DEBUG_JKW
        Serial.println("Connecting relay!");
        #endif
        
        digitalWrite(RELAY_PIN,HIGH);
        current_relay_state=RELAY_CONNECTED;
        relay_open_timestamp=millis()/1000;
    } else {
        #ifdef DEBUG_JKW
        Serial.println("Disconnecting relay!");
        #endif
        
        digitalWrite(RELAY_PIN,LOW);
        current_relay_state=RELAY_DISCONNECTED;
        return ((millis()/1000) - relay_open_timestamp);
    }
}


// returns true if tag found, does the UART handling
bool tag_found(uint8_t *buf,uint32_t *tag){
    uint8_t temp;
    
    while(Serial1.available()){
        temp=Serial1.read();
        buf[currentTagIndex]=temp;
        currentTagIndex=(currentTagIndex+1)%TAGSTRINGSIZE;
        
        if(currentTagIndex==0){
            return validate_tag(buf,tag);
        };
    }
    return false;
}


// just check if the data are corrumpeted or equal the checksum 
// and convert them to the correct oriented unsigned long
bool validate_tag(uint8_t *buf,uint32_t *tag){
    uint8_t expected=0;
    for(uint8_t i=0;i<TAGSTRINGSIZE-1;i++){
        expected^=buf[i];
    }

    if(expected==buf[TAGSTRINGSIZE-1]){
        // checksum correct, flip data around to get the uint32_t
        for(uint8_t i=0;i<TAGSTRINGSIZE-1;i++){
            *tag=(*tag<<8)+buf[i];
        };
        return true;
    }

    return false;
}


bool read_EEPROM(){
    #ifdef DEBUG_JKW
    Serial.println("-- This is EEPROM read --");
    #endif
    
    uint8_t temp;
    uint16_t num_keys=0;
    uint16_t num_keys_check=0;
    
    temp=EEPROM.read(2044);
    num_keys=temp<<8;
    temp=EEPROM.read(2045);
    num_keys+=temp;
    
    #ifdef DEBUG_JKW
    Serial.print("# of keys =");
    Serial.println(num_keys);
    #endif
    
    
    temp=EEPROM.read(2046);
    num_keys_check=temp<<8;
    temp=EEPROM.read(2047);
    num_keys_check+=temp;
    
    #ifdef DEBUG_JKW
    Serial.print("# of keys+1 =");
    Serial.println(num_keys_check);
    #endif
    
    if(num_keys_check==num_keys+1){
        keys_available=num_keys;
        for(uint8_t i=0;i<num_keys;i++){
            temp=EEPROM.read(i*4+0);
            keys[i]=temp<<24;
            temp=EEPROM.read(i*4+1);
            keys[i]+=temp<<16;
            temp=EEPROM.read(i*4+2);
            keys[i]+=temp<<8;
            temp=EEPROM.read(i*4+3);
            keys[i]+=temp;
            
            #ifdef DEBUG_JKW
            Serial.print("Read key ");
            Serial.print(i);
            Serial.print("=");
            Serial.print(keys[i]);
            Serial.println(" from eeprom");
            #endif
        }
    }
    
    #ifdef DEBUG_JKW
    Serial.println("-- End of EEPROM read --");
    #endif
}

// sends a request to the amazon server, this server should be later changed to 
// be the local Raspberry pi. It will call the get_my_id() function
// return true if http request was ok
// false if not - you might want to set a LED if it returns false
bool update_ids(){
    // avoid flooding
    if(last_key_update+MIN_UPDATE_TIME>millis()/1000 && last_key_update>0){
        #ifdef DEBUG_JKW
        Serial.println("db read blocked, too frequent");
        #endif
        
        return false;
    }
    last_key_update=millis()/1000;
    db_led.on(); // turn the led on
    
    // request data
    uint32_t now=millis();
    String response;
    String url="/m2m.php?mach_nr="+String(get_my_id());
    
    green_led.on();
    red_led.on();
    int statusCode = client.get(url, &response);
    green_led.resume();
    red_led.resume();
    
    #ifdef DEBUG_JKW
    Serial.print("db request took ");
    Serial.print(millis()-now);
    Serial.println(" ms");
    delay(1000);
    Serial.println("Requested:");
    Serial.println(url);
    delay(1000);
    Serial.println("Recevied:");
    Serial.println(response);
    #endif
    
    
    // check status
    if(statusCode!=200){
        db_led.off(); // turn the led off
        set_connected(0);

        #ifdef DEBUG_JKW
        Serial.println("No response from server");
        #endif
        
        return false;
    }

    // check length
    if(response.length()==0){
        db_led.off(); // turn the led off
        
        #ifdef DEBUG_JKW
        Serial.println("Empty response");
        #endif
    }

    set_connected(1);
    
    // clear all existing keys and then, import keys from request
    keys_available=0;
    uint8_t current_key=0;
    for(uint8_t i=0;i<sizeof(keys)/sizeof(keys[0]);i++){
        keys[i]=0;
    }

    for(uint8_t i=0;i<response.length();i++){
        Serial.print(response.charAt(i));

        if(response.charAt(i)==','){
            if(current_key<MAX_KEYS){
                // store to EEPROM
                EEPROM.update(current_key*4+0, (keys[current_key]>>24)&0xff);
                EEPROM.update(current_key*4+1, (keys[current_key]>>16)&0xff);
                EEPROM.update(current_key*4+2, (keys[current_key]>>8)&0xff);
                EEPROM.update(current_key*4+3, (keys[current_key])&0xff);
            
                current_key++;
            }
        } else if(response.charAt(i)>='0' && response.charAt(i)<='9') { // zahl
            keys[current_key]=keys[current_key]*10+(response.charAt(i)-'0');
        }
    }
    
    
    // csv does not end with a ',', therefore we have to add +1
    keys_available=current_key;
    EEPROM.update(2044,(keys_available>>8)&0xff);
    EEPROM.update(2045,(keys_available)&0xff);
    
    // checksum
    EEPROM.update(2046,((keys_available+1)>>8)&0xff);
    EEPROM.update(2047,((keys_available+1))&0xff);
    
    #ifdef DEBUG_JKW
    Serial.print("Total received keys for my id(");
    Serial.print(get_my_id());
    Serial.print("):");
    Serial.println(keys_available);
    for(uint8_t i=0;i<keys_available;i++){
        Serial.print("Valid Database Key Nr ");
        Serial.print(i+1);
        Serial.print(": ");
        Serial.print(keys[i]);
        Serial.println("");
    };
    #endif
    
    db_led.off(); // turn the led off
    return true;
}


// create a log entry on the server for the action performed
void create_report(uint8_t event,uint32_t badge,uint32_t extrainfo){
    
    db_led.on(); // turn the led on
    String request_path;
    if(event==LOG_RELAY_CONNECTED){
        request_path = "/history.php?logme&badge="+String(badge)+"&mach_nr="+String(get_my_id())+"&event=Unlocked";
    } else if(event==LOG_LOGIN_REJECTED){
        request_path = "/history.php?logme&badge="+String(badge)+"&mach_nr="+String(get_my_id())+"&event=Rejected";
    } else if(event==LOG_RELAY_DISCONNECTED){
        request_path = "/history.php?logme&badge="+String(badge)+"&mach_nr="+String(get_my_id())+"&event=Locked&timeopen="+String(extrainfo);
    } else if(event==LOG_NOTHING){
        request_path = "/history.php";
    } else {
        return;
    }
    
    #ifdef DEBUG_JKW
    Serial.print("calling:");
    Serial.println(request_path);
    #endif
    
    uint32_t now=millis();
    String response;
    
    green_led.on();
    red_led.on();
    int statusCode = client.get(request_path, &response);
    green_led.resume();
    red_led.resume();
    
    
    if(statusCode==200){
        set_connected(1);
    } else if(statusCode!=200){
        set_connected(0);
    }
    
    #ifdef DEBUG_JKW
    Serial.print("db request took ");
    Serial.print(millis()-now);
    Serial.println(" ms");
    #endif
    
    db_led.off(); // turn the led off
}


void set_connected(int status){
    set_connected(status,false);
};

void set_connected(int status, bool force){
    if(status==1 && connected==0){
        connected=1;
        green_led.blink();
        red_led.off();
    } else if(status==0 && (connected==1 || force)){
        connected=0;
        green_led.off();
        red_led.blink();
    }
}

// shall later on read the device jumper and return that number
// will only do the interation with the pins once for performance
uint8_t get_my_id(){
    if(id==(uint8_t)-1){
        id=0;
        
        #ifdef DEBUG_JKW
        Serial.print("ID never set, reading");
        #endif
        
        for(uint8_t i=10+MAX_JUMPER_PIN; i>=10; i--){   // A0..7 is 10..17
            id=id<<1;
            if(!digitalRead(i)){
                id++;
            };
        }
        
        #ifdef DEBUG_JKW
        Serial.print(" id for this device as ");
        Serial.println(id);
        #endif
        
    }
    return id;
}

