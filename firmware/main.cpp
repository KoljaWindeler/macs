
/* LED Patter
* === STARTUP ===
* red,green,red,green  -> i give you 10 sec to connect to me, before I start
* red on, green on -> I'm trying to connect to my server
*
* === UPDATE MODE ===
* green flashes 3 times (10Hz) -> I've found valid WiFi config 1 in EEPROM and now I'm scannning for it! 
* green flashes 5 times (10Hz) -> I've found WiFi config 1 in scan results and try to conenct now! 
* red flashes 3 times (10Hz) -> I've found valid WiFi config 2 in EEPROM and now I'm scannning for it! 
* red flashes 5 times (10Hz) -> I've found WiFi config 2 in scan results and try to conenct now! 
* red and green simulatnious, 2 fast blinks (10Hz) -> connected to wifi!
* red,green,red,greed fast (10sec, 20Hz) -> Waiting for the Wifi to save credentials AND give you time to add new credentials via serial
* red and green blink simultainous -> I'm ready for an update
* 
* === MACS MODE (status can be combined) ===
* red and green simulatnious, flashes 3 times (10Hz) -> I've found valid WiFi config in EEPROM and now I'm scannning for it! 
* red and green simulatnious, flashes 5 times (10Hz) -> I've found WiFi config in scan results and try to conenct now! 
* red and green simulatnious, 2 fast flashes (10Hz)  -> connected to wifi!
* red blinking -> no connection to the MACS Server
* red solid -> card rejected
* green blinking -> connected to the MACS Server
* green solid -> card accepted
*/

// This #include statement was automatically added by the Particle IDE.
#include "application.h"
#include "stdint.h"
#include "config.h"
#include "application.h"

// network
IPAddress HOSTNAME(192,168,188,23);

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
uint32_t last_tag_read=0;

LED db_led(DB_LED_AND_UPDATE_PIN,DB_LED_DELAY,1,1); // weak + inverse
LED red_led(RED_LED_PIN,RED_LED_DELAY,0,0);
LED green_led(GREEN_LED_PIN,GREEN_LED_DELAY,0,0);

SYSTEM_MODE(MANUAL);// do not connect on your own

// http server
RestClient client = RestClient(HOSTNAME);

//////////////////////////////// SETUP ////////////////////////////////
void setup() {
    // set adress pins
    for(uint8_t i=10; i<=MAX_JUMPER_PIN+10; i++){   // A0..7 is 10..17, used to read my ID
       pinMode(i,INPUT_PULLUP);
    }
    pinMode(RELAY_PIN,OUTPUT);          // driver for the relay
    pinMode(TAG_IN_RANGE_INPUT,INPUT);
    
    // register system handles
    System.on(wifi_listen, listen);
    //System.on(network_status, listen);
    
    // antenna selection
    pinMode(ANTENNA_PIN,INPUT_PULLUP);
    
    if(digitalRead(ANTENNA_PIN)==LOW){ // high == no jumper to pull it down 
        WiFi.selectAntenna(ANT_EXTERNAL);
    } else {
        WiFi.selectAntenna(ANT_AUTO);
    }
    
    // the db led is a weak and inverterted LED on the same pin as the update_input, this will set the pin to input_pullup anyway //pinMode(DB_LED_AND_UPDATE_PIN,INPUT_PULLUP);
    Serial.begin(115200);
    Serial1.begin(9600);
    
    // read mode to starting with
    if(digitalRead(DB_LED_AND_UPDATE_PIN)){
        
        // ############ MACS MODUS ############ // 
        #ifdef DEBUG_JKW_MAIN
        Serial.println("- MACS -");
        #endif
        
        //red_led.on();
        //green_led.on();
        
        set_connected(0); // init as not connected
        if(update_ids(true)){ // true = force update, update_ids will initiate the connect
            set_connected(1);
        } else {
            set_connected(0,true); // force LED update for not connected
            read_EEPROM();
        }
        // ############ MACS MODUS ############ // 
        
    } else {
        
        // ############ UPDATE MODUS ############ // 
        red_led.on();
        green_led.on();
        db_led.on();
        
        #ifdef DEBUG_JKW_MAIN
        Serial.println("- Cloud -");
        #endif
        
        // satrt loop that will set wifi data and connect to cloud,
        // and if anything fails start again, until there is an update
        while(1){
            // set_update_login will return true, if we've read a valid config from 
            // the EEPROM memory AND that WIFI was in range AND the module has saved the login
            if(set_update_login(&green_led,&red_led)){
                Serial.println("set update login done");
                Particle.connect();
                uint8_t i=0;

                // backup, if connect didn't work, repeat it
                while(!WiFi.ready()){
                    Serial.print(".");
                    Particle.connect();
                }

                // stay in update mode forever
                while(WiFi.ready()){
                    if(i!=millis()/1000){
                        
                        #ifdef DEBUG_JKW_MAIN
                        Serial.print(i);
                        Serial.print(": ");
                        #endif
                        
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
                            
                            // check incomming data, unlikely here, because at this point we are already connected to an update wifi
                            parse_wifi();
                            
                            // keep blinking
                            red_led.check();
                            green_led.check();
                            db_led.check();
                            
                            #ifdef DEBUG_JKW_MAIN
                            Serial.println("Photon connected");
                            #endif
                            
                        } else {
                            
                            #ifdef DEBUG_JKW_MAIN
                            Serial.println("Photon NOT connected");
                            #endif
                            
                            // constant on == not yet connected
                            red_led.on();
                            green_led.on();
                            db_led.on();
                        }
                        i=millis()/1000;
                    } // i!=millis()/1000
                    delay(200); // don't go to high as blink will look odd
                } // end while(WiFi.ready())
                // reaching this point tells us that we've set the wifi login, tried to connect but lost the connection, as the wifi is not (longer) ready
            } // if(set_update_login())
        } // end while(1)
        // ############ UPDATE MODUS ############ // 
    }
}
//////////////////////////////// SETUP ////////////////////////////////

//////////////////////////////// MAIN LOOP ////////////////////////////////
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
                // 1. assuming that we are NOT connected, we reached this point and the create_report has success to reconnet than it will call set_connected()
                // this will turn red off (which is fine (was blinking = not connected)) and green to blink (ok), so we have to override it
                // 2. assuming that we are NOT connected, we reached this point and the create_report has NO success to reconnet than it will not call set_connected()
                // the red will keep blinking (ok) but we still want to show that this card was good, turn green on
                // 3. assuming that we are connected, we reached this point then create_report will not try to reconnect and the report is send just fine
                // the red will be off anywa (ok), we want to show that this card was good, turn green on
                // 4. assuming that we are connected, we reached this point then create_report will not try to reconnect, but the report failed, create_report will set us to not conneted
                // the red will be blinkin (ok), we want to show that this card was good, turn green on
                set_connected(connected,1); // force to resume LED pattern
                green_led.on();
            } else {
                // if we have a card that is not known to be valid we should maybe check our database
                if(tries>1){
            
                    #ifdef DEBUG_JKW_MAIN
                    Serial.println("Key not valid, requesting update from server");
                    #endif
                    
                    update_ids(false); // unforced update
                        
                    
                    #ifdef DEBUG_JKW_MAIN
                    if(tries>0){
                        Serial.println("Trying once more if key is valid now");
                    };
                    #endif
                    
                    tries-=1;
                } else {
                    
                    #ifdef DEBUG_JKW_MAIN
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
    // if serial1 has avaioable chars, it means that TAG IN RANGE had to be low at some point, its just a new card there now!
    if((digitalRead(TAG_IN_RANGE_INPUT)==0 || Serial1.available()) && currentTag!=-1){ 
        // open the relay as soon as the tag is gone
        if(current_relay_state==RELAY_CONNECTED){
            uint32_t open_time_sec=relay(RELAY_DISCONNECTED);
            green_led.resume();
            // last because it takes long
            create_report(LOG_RELAY_DISCONNECTED,currentTag,open_time_sec);
        } else {
            red_led.resume();    
        }
        
        set_connected(connected,1); // force to resume LED pattern
    
        currentTag=-1;      // reset current user
        currentTagIndex=0;  // reset index counter for incoming bytes
        
    }
    
    // time based update the storage from the server (every 10 min?) 
    if(last_key_update+DB_UPDATE_TIME<(millis()/1000)){
        update_ids(false);  // unforced upate
    }
    
    
    // see if we should switch off the leds by now
    db_led.check();
    red_led.check();
    green_led.check();
    parse_wifi();
}
//////////////////////////////// MAIN LOOP ////////////////////////////////


//////////////////////////////// ACCESS TEST ////////////////////////////////
// callen from main loop as soon as a tag has been found to test if it matches one of the saved keys
bool access_test(uint32_t tag){
    #ifdef DEBUG_JKW_MAIN
    Serial.print("Tag ");
    Serial.print(tag);
    Serial.print(" found. Checking database (");
    Serial.print(keys_available);
    Serial.print(") for matching key");
    Serial.println("==============");
    #endif
    
    for(uint16_t i=0;i<MAX_KEYS && i<keys_available; i++){

        #ifdef DEBUG_JKW_MAIN
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

    #ifdef DEBUG_JKW_MAIN
    Serial.println("Key valid, closing relay");
    #endif
    
            return true;
        }
    }

    #ifdef DEBUG_JKW_MAIN
    Serial.println("==============");
    #endif
    
    return false;
}
//////////////////////////////// ACCESS TEST ////////////////////////////////

//////////////////////////////// DRIVE THE RELAY ////////////////////////////////
// hardware controll, writing to the pin and log times
uint32_t relay(int8_t input){
    if(input==1){
        
        #ifdef DEBUG_JKW_MAIN
        Serial.println("Connecting relay!");
        #endif
        
        digitalWrite(RELAY_PIN,HIGH);
        current_relay_state=RELAY_CONNECTED;
        relay_open_timestamp=millis()/1000;
    } else {
        
        #ifdef DEBUG_JKW_MAIN
        Serial.println("Disconnecting relay!");
        #endif
        
        digitalWrite(RELAY_PIN,LOW);
        current_relay_state=RELAY_DISCONNECTED;
        return ((millis()/1000) - relay_open_timestamp);
    }
}
//////////////////////////////// DRIVE THE RELAY ////////////////////////////////

//////////////////////////////// SCAN FOR TAG ON SERIAL ////////////////////////////////
// returns true if tag found, does the UART handling
bool tag_found(uint8_t *buf,uint32_t *tag){
    uint8_t temp;
    
    while(Serial1.available()){
        
        // if we haven't received input for a long time, make sure that we writing to pos 1
        if(abs(millis()-last_tag_read)>100){
            currentTagIndex=0;
        }
        last_tag_read=millis();
        
        // read and store
        temp=Serial1.read();
        buf[currentTagIndex]=temp;
        currentTagIndex=(currentTagIndex+1)%TAGSTRINGSIZE;
        
        if(currentTagIndex==0){
            //Serial1.flush(); // flush it, as we just want to read this tag
            // according to the ref of particle, flush is not implemented
            // and there understanding is different after all
            // will manual flushing the input
            while(Serial1.available()){
                Serial1.read();
            }
            
            return validate_tag(buf,tag);
        };
    }
    return false;
}
//////////////////////////////// SCAN FOR TAG ON SERIAL ////////////////////////////////

//////////////////////////////// VERIFY CHECKSUM FOR TAG ////////////////////////////////
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
//////////////////////////////// VERIFY CHECKSUM FOR TAG ////////////////////////////////

//////////////////////////////// READ ID's FROM EEPROM ////////////////////////////////
// if we are running offline
bool read_EEPROM(){
    
    #ifdef DEBUG_JKW_MAIN
    Serial.println("-- This is EEPROM read --");
    #endif
    
    uint8_t temp;
    uint16_t num_keys=0;
    uint16_t num_keys_check=0;
    
    temp=EEPROM.read(KEY_NUM_EEPROM_HIGH);
    num_keys=temp<<8;
    temp=EEPROM.read(KEY_NUM_EEPROM_LOW);
    num_keys+=temp;
    
    
    
    #ifdef DEBUG_JKW_MAIN
    Serial.print("# of keys =");
    Serial.println(num_keys);
    #endif
    
    
    temp=EEPROM.read(KEY_CHECK_EEPROM_HIGH);
    num_keys_check=temp<<8;
    temp=EEPROM.read(KEY_CHECK_EEPROM_LOW);
    num_keys_check+=temp;
    
    #ifdef DEBUG_JKW_MAIN
    Serial.print("# of keys+1 =");
    Serial.println(num_keys_check);
    #endif
    
    if(num_keys_check==num_keys+1){
        keys_available=num_keys;
        for(uint16_t i=0;i<num_keys;i++){
            temp=EEPROM.read(i*4+0);
            keys[i]=temp<<24;
            temp=EEPROM.read(i*4+1);
            keys[i]+=temp<<16;
            temp=EEPROM.read(i*4+2);
            keys[i]+=temp<<8;
            temp=EEPROM.read(i*4+3);
            keys[i]+=temp;
            
            #ifdef DEBUG_JKW_MAIN
            Serial.print("Read key ");
            Serial.print(i);
            Serial.print("=");
            Serial.print(keys[i]);
            Serial.println(" from eeprom");
            #endif
        }
    }
    
    #ifdef DEBUG_JKW_MAIN
    Serial.println("-- End of EEPROM read --");
    #endif
}
//////////////////////////////// READ ID's FROM EEPROM ////////////////////////////////

//////////////////////////////// UPDATE ID's ////////////////////////////////
// sends a request to the amazon server, this server should be later changed to 
// be the local Raspberry pi. It will call the get_my_id() function
// return true if http request was ok
// false if not - you might want to set a LED if it returns false
bool update_ids(bool forced){
    // avoid flooding
    if(last_key_update+MIN_UPDATE_TIME>millis()/1000 && last_key_update>0){
        
        #ifdef DEBUG_JKW_MAIN
        Serial.println("db read blocked, too frequent");
        #endif
        
        return false;
    }
    
    if(!WiFi.ready()){
        if(!set_macs_login(&green_led,&red_led)){
            set_connected(0);
            return false;
        }
    }
    
    last_key_update=millis()/1000;
    db_led.on(); // turn the led on
    
    // request data
    uint32_t now=millis();
    String response;
    String url="/m2m.php?mach_nr="+String(get_my_id())+"&forced=";
    if(forced){
        url=url+"1";
    } else {
        url=url+"0";
    }
    
    
    green_led.on();
    red_led.on();
    int statusCode = client.get(url, &response);
    green_led.resume();
    red_led.resume();
    
    #ifdef DEBUG_JKW_MAIN
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

        #ifdef DEBUG_JKW_MAIN
        Serial.println("No response from server");
        #endif
        
        Serial.println("No response from server");
        
        return false;
    }

    // check length
    if(response.length()==0){
        db_led.off(); // turn the led off
        
        #ifdef DEBUG_JKW_MAIN
        Serial.println("Empty response");
        #endif
    }

    // connection looks good
    set_connected(1);
    
    // check if we've received a "no update" message from the server
    // if we are unforced we'll just leave our EEPROM as is.
    // otherweise we'll go on
    //Serial.println("response length:");
    //Serial.println(response.length());
    //Serial.print(response.charAt(0));
    //Serial.println(response.charAt(1));
    
    if(!forced && response.length()>=2){
        if(response.charAt(0)=='n' && response.charAt(1)=='u'){
            // we received a 'no update'
            
            #ifdef DEBUG_JKW_MAIN
            Serial.println("No update received");
            #endif
        
            db_led.off(); // turn the led off
            return true;
        }
    }
    
    // clear all existing keys and then, import keys from request
    keys_available=0;
    uint16_t current_key=0;
    for(uint16_t i=0;i<sizeof(keys)/sizeof(keys[0]);i++){
        keys[i]=0;
    }

    for(uint16_t i=0;i<response.length();i++){
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
    keys_available=current_key;
    
    
    // log number of keys to the eeprom
    EEPROM.update(KEY_NUM_EEPROM_HIGH,(keys_available>>8)&0xff);
    EEPROM.update(KEY_NUM_EEPROM_LOW,(keys_available)&0xff);
    // checksum
    EEPROM.update(KEY_CHECK_EEPROM_HIGH,((keys_available+1)>>8)&0xff);
    EEPROM.update(KEY_CHECK_EEPROM_LOW,((keys_available+1))&0xff);
    
    #ifdef DEBUG_JKW_MAIN
    Serial.print("Total received keys for my id(");
    Serial.print(get_my_id());
    Serial.print("):");
    Serial.println(keys_available);
    for(uint16_t i=0;i<keys_available;i++){
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
//////////////////////////////// UPDATE ID's ////////////////////////////////

//////////////////////////////// CREATE REPORT ////////////////////////////////
// create a log entry on the server for the action performed
void create_report(uint8_t event,uint32_t badge,uint32_t extrainfo){
    
    if(!WiFi.ready()){
        if(set_macs_login(&green_led,&red_led)){
            set_connected(1); // this could potentially destroy our LED pattern? TODO
        } else {
            set_connected(0);
            return; // pointless to go on
        }
    }
    
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
    
    #ifdef DEBUG_JKW_MAIN
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
    
    #ifdef DEBUG_JKW_MAIN
    Serial.print("db request took ");
    Serial.print(millis()-now);
    Serial.println(" ms");
    #endif
    
    db_led.off(); // turn the led off
}
//////////////////////////////// CREATE REPORT ////////////////////////////////

//////////////////////////////// SET CONNECTED ////////////////////////////////
// set the LED pattern
void set_connected(int status){
    set_connected(status,false);
};

void set_connected(int status, bool force){
    if(status==1 && (connected==0 || force)){
        connected=1;
        green_led.blink();
        red_led.off();
    } else if(status==0 && (connected==1 || force)){
        connected=0;
        green_led.off();
        red_led.blink();
    }
}
//////////////////////////////// SET CONNECTED ////////////////////////////////

//////////////////////////////// GET MY ID ////////////////////////////////
// shall later on read the device jumper and return that number
// will only do the interation with the pins once for performance
uint8_t get_my_id(){
    if(id==(uint8_t)-1){
        id=0;
        
        #ifdef DEBUG_JKW_MAIN
        Serial.print("ID never set, reading");
        #endif
        
        for(uint8_t i=10+MAX_JUMPER_PIN; i>=10; i--){   // A0..7 is 10..17
            id=id<<1;
            if(!digitalRead(i)){
                id++;
            };
        }
        
        #ifdef DEBUG_JKW_MAIN
        Serial.print(" id for this device as ");
        Serial.println(id);
        #endif
        
    }
    return id;
}
//////////////////////////////// GET MY ID ////////////////////////////////

