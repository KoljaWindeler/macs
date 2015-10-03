// This #include statement was automatically added by the Particle IDE.
#include "HttpClient/HttpClient.h"

#include "application.h"
#include "stdint.h"

/**
* Declaring the variables.
*/

#define MAX_JUMPER_PIN 5
#define MAX_KEYS 20
#define RELAY_CLOSED 1
#define RELAY_OPEN 0
#define DEBUG_JKW
#define RELAY_PIN D7
#define DB_READ_LED_PIN D6
#define TAG_READ_LED_PIN D5
#define MIN_UPDATE_TIME 10

uint8_t keys_available=0;
uint32_t keys[MAX_KEYS];
uint32_t current_tag=0;
uint8_t current_relay_state=RELAY_OPEN;
uint32_t nextTime = 0;    // Next time to contact the server
uint8_t id=-1; //255
uint32_t last_key_update=0;



void setup() {
    for(uint8_t i=10; i<=MAX_JUMPER_PIN+10; i++){   // A0..7 is 10..17, used to read my ID
       pinMode(i,INPUT_PULLUP);
    }
    pinMode(RELAY_PIN,OUTPUT);  // driver for the relay
    pinMode(TAG_READ_LED_PIN,OUTPUT);  // driver for tag read LED
    pinMode(DB_READ_LED_PIN,OUTPUT);  // driver for db read LED
    
    Serial.begin(9600);
    Serial.print("yiha!");
}


// woop woop main loop
void loop() {
    // check if we found a tag
    if(tag_found(&current_tag)){
        
        #ifdef DEBUG_JKW
        Serial.print("Tag ");
        Serial.print(current_tag);
        Serial.println(" found. Checking database for matching key");
        Serial.println("==============");
        #endif
        
        bool found=0;
        for(uint8_t i=0;i<MAX_KEYS && i<keys_available && !found; i++){

        #ifdef DEBUG_JKW
        Serial.print(i+1);
        Serial.print(" / ");
        Serial.print(keys_available);
        Serial.print(" Compare current read tag ");
        Serial.print(current_tag);
        Serial.print(" to stored key ");
        Serial.println(keys[i]);
        #endif
        
            if(keys[i]==current_tag){

        #ifdef DEBUG_JKW
        Serial.println("Key valid, closing relay");
        #endif
        
                relay(1);
                found=1; // will stop the loop 
            }
        }

        #ifdef DEBUG_JKW
        Serial.println("==============");
        #endif
        
        // if we still have an active machine but the tag is gone, shut it off
        if(current_relay_state==RELAY_CLOSED && !found){

        #ifdef DEBUG_JKW
        Serial.println("Key not valid, relay was still closed, will open now");
        #endif
        
            relay(0);
        }
        
        // if we have a card that is not known to be valid we should maybe check our database
        if(!found){

            #ifdef DEBUG_JKW
            Serial.println("Key not valid, requesting update from server");
            #endif
            
            update_ids(); 
        }
    }
    
    // time based (every 10 min?) 
    //update_ids();
}


// hardware controll
void relay(int8_t input){
    if(input==1){

        #ifdef DEBUG_JKW
        Serial.println("Closing relay!");
        #endif
        
        //digitalWrite(RELAY_PIN,HIGH);
        current_relay_state=RELAY_CLOSED;
    } else {

        #ifdef DEBUG_JKW
        Serial.println("Opening relay!");
        #endif
        
        //digitalWrite(RELAY_PIN,LOW);
        current_relay_state=RELAY_OPEN;
    }
}


// returns true if tag found
bool tag_found(uint32_t *tag){
    //serial.availealbe ... 
    if (nextTime < millis()) {
        nextTime = millis() + 1000;
        *tag=random(10);

        #ifdef DEBUG_JKW
        Serial.println("");
        Serial.print("Generated fake-random key:");
        Serial.println(*tag);
        #endif
        
        return true;
    }
    return false;
}


// sends a request to the amazon server, this server should be later changed to 
// be the local Raspberry pi. It will call the get_my_id() function
// return true if http request was ok
// false if not - you might want to set a LED if it returns false
bool update_ids(){
    if(last_key_update+MIN_UPDATE_TIME>millis()/1000){
        return false;
    }
    last_key_update=millis()/1000;
    // http vars ave to be local, otherwise there is a photon SOS problem at the second call .. freaky bad implimentation in the TCP socket ?
    HttpClient http;
    http_header_t headers[] = {{ "Accept" , "*/*"},{ NULL, NULL }}; // NOTE: Always terminate headers will NULL
    http_request_t request;
    http_response_t response;
    keys_available=0;
    // Request path and body can be set at runtime or at setup.
    request.hostname = "52.24.157.229";
    request.port = 80;
    
    
    request.path = "/m2m.php?mach_nr="+String(get_my_id());
    http.get(request, response, headers);
    //Serial.print("Application>\tResponse status: ");
    //Serial.println(response.status);
    if(response.status!=200){
        return false;
    }

    //Serial.print("Application>\tHTTP Response Body: ");
    ////Serial.println(response.body);
    
    if(response.body.length()==0){
        return false;
    }

    uint8_t current_key=0;
    for(uint8_t i=0;i<sizeof(keys)/sizeof(keys[0]);i++){
        keys[i]=0;
    }
    for(uint8_t i=0;i<response.body.length();i++){
        if(response.body.charAt(i)==','){
            current_key++;
        } else if(response.body.charAt(i)>='0' && response.body.charAt(i)<='9') {
            keys[current_key]*=10;
            keys[current_key]+=response.body.charAt(i)-'0';
        }
    }
    keys_available=current_key+1;
    Serial.print("Total received keys for my id(");
    Serial.print(get_my_id());
    Serial.print("):");
    Serial.println(keys_available);
    for(uint8_t i=0;i<keys_available;i++){
        Serial.print("Valid Database Key Nr ");
        Serial.print(i+1);
        Serial.print(": ");
        Serial.println(keys[i]);
    };
    
    return true;
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
