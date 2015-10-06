// This #include statement was automatically added by the Particle IDE.
#include "led.h"

// This #include statement was automatically added by the Particle IDE.
#include "HttpClient/HttpClient.h"

#include "application.h"
#include "stdint.h"

/**
* Declaring the variables.
*/
// hardware
#define MAX_JUMPER_PIN 5
#define MAX_LED_PINS 2

#define RELAY_PIN D7
#define DB_READ_LED_PIN D6
#define TAG_READ_LED_PIN D5
#define TAG_IN_RANGE_INPUT D4

// storage
#define MAX_KEYS 20 // max number of keys
#define TAGSTRINGSIZE 5 // tag length

// macros
#define RELAY_CLOSED 1
#define RELAY_OPEN 0

// debug
#define DEBUG_JKW

// settings
#define DB_UPDATE_TIME 10*60 // seconds between two auto updates from the server
#define MIN_UPDATE_TIME 10 // seconds between two database request, to avoid flooding
#define TAG_READ_LED_DELAY 1000 // ms
#define DB_READ_LED_DELAY 1000 // ms

// network
#define HOSTNAME "52.24.157.229"
#define HOSTPORT 80
//#define HOSTNAME "192.168.1.84"
//#define HOSTPORT 90


uint8_t keys_available=0;
uint32_t keys[MAX_KEYS];

uint8_t currentTagBuf[TAGSTRINGSIZE];
uint8_t currentTagIndex=0;
uint32_t currentTag;

uint8_t current_relay_state=RELAY_OPEN;
uint32_t nextTime = 0;    // Next time to contact the server
uint8_t id=-1; //255
uint32_t last_key_update=0;
uint8_t tagInRange=0;


LED db_read_led(DB_READ_LED_PIN,DB_READ_LED_DELAY);
LED tag_read_led(TAG_READ_LED_PIN,TAG_READ_LED_DELAY);


void setup() {
    for(uint8_t i=10; i<=MAX_JUMPER_PIN+10; i++){   // A0..7 is 10..17, used to read my ID
       pinMode(i,INPUT_PULLUP);
    }
    
    pinMode(RELAY_PIN,OUTPUT);          // driver for the relay
    pinMode(TAG_IN_RANGE_INPUT,INPUT);
    
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial.print("yiha!");
    
    update_ids();
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
                relay(1);
                create_report(1,currentTag);
                tries=0;
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
                    Serial.println("key still not valid");
                    #endif
                    tries=0;   
                    create_report(2,currentTag);
                }
            }
        }
    }
    
    // open the relay as soon as the tag is gone
    if(digitalRead(TAG_IN_RANGE_INPUT)==0 && current_relay_state==RELAY_CLOSED){
        relay(0);
        create_report(0,0);
        currentTagIndex=0;
    }
    
    // time based update the storage from the server (every 10 min?) 
    if(nextTime<millis()){
        nextTime=millis()+DB_UPDATE_TIME*60*1000;
        update_ids(); 
    }
    
    // see if we should switch off the leds by now
    tag_read_led.check();
    db_read_led.check();
}



// callen from main loop as soon as a tag has been found to test if it matches one of the saved keys
bool access_test(uint32_t tag){
    #ifdef DEBUG_JKW
    Serial.print("Tag ");
    Serial.print(tag);
    Serial.println(" found. Checking database for matching key");
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

// hardware controll, writing to the pin
void relay(int8_t input){
    if(input==1){

        #ifdef DEBUG_JKW
        Serial.println("Closing relay!");
        #endif
        
        digitalWrite(RELAY_PIN,HIGH);
        current_relay_state=RELAY_CLOSED;
    } else {

        #ifdef DEBUG_JKW
        Serial.println("Opening relay!");
        #endif
        
        digitalWrite(RELAY_PIN,LOW);
        current_relay_state=RELAY_OPEN;
    }
}


// returns true if tag found, does the UART handling
bool tag_found(uint8_t *buf,uint32_t *tag){
    uint8_t temp;
    
    while(Serial1.available()){
        tag_read_led.on_dalayed(); // switch on the LED
        temp=Serial1.read();
        buf[currentTagIndex]=temp;
        currentTagIndex=(currentTagIndex+1)%TAGSTRINGSIZE;
        
        if(currentTagIndex==0){
            return validate_tag(buf,tag);
        };
    }
    return false;
}


// just check if the data are corrumpeted or equal the checksum and convert them to the correct oriented unsigned long
bool validate_tag(uint8_t *buf,uint32_t *tag){
    uint8_t expected=0;
    for(uint8_t i=0;i<TAGSTRINGSIZE-1;i++){
        expected^=buf[i];
    }
    //Serial.println("comparing");
    if(expected==buf[TAGSTRINGSIZE-1]){
        // flip it around
        for(uint8_t i=0;i<TAGSTRINGSIZE-1;i++){
            *tag=(*tag<<8)+buf[i];
        };
        return true;
    }
    //Serial.println("nope");
    return false;
}


// sends a request to the amazon server, this server should be later changed to 
// be the local Raspberry pi. It will call the get_my_id() function
// return true if http request was ok
// false if not - you might want to set a LED if it returns false
bool update_ids(){
    if(last_key_update+MIN_UPDATE_TIME>millis()/1000){
        #ifdef DEBUG_JKW
        Serial.println("db read blocked, too frequent");
        #endif
        
        return false;
    }
    last_key_update=millis()/1000;
    db_read_led.on(); // turn the led on
    
    // http vars ave to be local, otherwise there is a photon SOS problem at the second call .. freaky bad implimentation in the TCP socket ?
    HttpClient http;
    http_header_t headers[] = {{ "Accept" , "*/*"},{ NULL, NULL }}; // NOTE: Always terminate headers will NULL
    http_request_t request;
    http_response_t response;
    keys_available=0;
    // Request path and body can be set at runtime or at setup.
    request.hostname = HOSTNAME;
    request.port = HOSTPORT;
    
    
    request.path = "/m2m.php?mach_nr="+String(get_my_id());
    http.get(request, response, headers);
    //Serial.print("Application>\tResponse status: ");
    //Serial.println(response.status);
    if(response.status!=200){
        db_read_led.off(); // turn the led off
        return false;
    }

    //Serial.print("Application>\tHTTP Response Body: ");
    ////Serial.println(response.body);
    
    if(response.body.length()==0){
        db_read_led.off(); // turn the led off
        return false;
    }

    uint8_t current_key=0;
    for(uint8_t i=0;i<sizeof(keys)/sizeof(keys[0]);i++){
        keys[i]=0;
    }
    for(uint8_t i=0;i<response.body.length();i++){
        if(response.body.charAt(i)==','){
            current_key++;
        } else if(response.body.charAt(i)>='0' && response.body.charAt(i)<='9') { // zahl
            keys[current_key]=keys[current_key]*10+(response.body.charAt(i)-'0');
        }
    }
    
    
    keys_available=current_key+1;
    
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
    
    db_read_led.off(); // turn the led off
    return true;
}


void create_report(uint8_t event,uint32_t badge){
    db_read_led.on(); // turn the led on
    
    HttpClient http;
    http_header_t headers[] = {{ "Accept" , "*/*"},{ NULL, NULL }}; // NOTE: Always terminate headers will NULL
    http_request_t request;
    http_response_t response;
    // Request path and body can be set at runtime or at setup.
    request.hostname = HOSTNAME;
    request.port = HOSTPORT;
    
    if(event==1){
        request.path = "/history.php?logme&badge="+String(badge)+"&mach_nr="+String(get_my_id())+"&event=Unlocked";
    } else if(event==2){
        request.path = "/history.php?logme&badge="+String(badge)+"&mach_nr="+String(get_my_id())+"&event=Rejected";
    } else {
        request.path = "/history.php?logme&mach_nr="+String(get_my_id())+"&event=Locked";
    }
    
    #ifdef DEBUG_JKW
    Serial.print("calling:");
    Serial.println(request.path);
    #endif
    
    http.get(request, response, headers);
    db_read_led.off(); // turn the led off
}

// shall later on read the device jumper and return that number
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
        Serial.print("id for this device as ");
        Serial.println(id);
        #endif
        
    }
    return id;
}
