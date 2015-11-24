
#include "application.h"
#include "stdint.h"
#include "config.h"

class FindSSID
{
    char *SSID_to_search;
    bool found;
    // This is the callback passed to WiFi.scan()
    // It makes the call on the `self` instance - to go from a static
    // member function to an instance member function.
    static void handle_ap(WiFiAccessPoint* wap, FindSSID* self)
    {
        self->next(*wap);
    }

    // determine if this AP is stronger than the strongest seen so far
    void next(WiFiAccessPoint& ap)
    {
        #ifdef DEBUG_JKW_WIFI
        //Serial.print("found ");
        //Serial.println(ap.ssid);
        //delay(1000);
        #endif
        
        if(strcmp(ap.ssid,SSID_to_search)==0){
            #ifdef DEBUG_JKW_WIFI
            Serial.print("found ");
            Serial.println(SSID_to_search);
            delay(100);
            #endif
            found=true;
        }
    }

public:
    /**
     * Scan WiFi Access Points and retrieve the strongest one.
     */
    bool check_SSID_in_range(char *SSID)
    {
        #ifdef DEBUG_JKW_WIFI
        //Serial.print("check SSID for ");
        //Serial.println(SSID);
        #endif
        // initialize data
        found = false;
        SSID_to_search = SSID;
        
        // avoid scanning for invaid data
        if(strlen(SSID)==0){
            return false;
        }
        
        WiFi.scan(handle_ap,this);
        // perform the scan#
        #ifdef DEBUG_JKW_WIFI
        //Serial.println("EOS!!!!");
        #endif
        return found;
    }
};


// set the config for the update mode
// if the jumper is in position this function will be called in a loop until we return true
// our steps are: 
// 1. Clear all old wifi credentials to avoid connecting back to the MACS operational network
// 2. Load data set 1 from EEPROM
// 3. Run the SSID finder for this config, they will return true if SSID in scan results
// 4. set the credentials
// 4.1. if this fails, try config 2 (backup)
// 5. if we don't set any credentials: give the user 10 sec to add some wifis via serial
// 6. assuming we've set credentials, we'll return true, otherwise false and come back in a second
bool set_update_login(LED *green, LED *red){
    WiFi.off();
    WiFi.on();
    WiFi.clearCredentials();
    
    String SSID[2];
    String pw[2];
    int mode[2];
    
    
    // Now use the class
    FindSSID ssidFinder;
    char SSID_char[20];
    bool try_backup=true;
    
    Serial.println("get  wifi");
    delay(300);
    if(get_wifi_config(WIFI_UPDATE_1,&SSID[0],&pw[0],&mode[0])){
        Serial.println("scan");
        delay(300);
        SSID[0].toCharArray(SSID_char,20);
        if(ssidFinder.check_SSID_in_range(SSID_char)){
            try_backup=false;
            Serial.println("setting crededentials");
            Serial.println(SSID[0]);
            WiFi.setCredentials(SSID[0], pw[0], mode[0]);
        };
    };
    

    if(try_backup){
        if(get_wifi_config(WIFI_UPDATE_2,&SSID[1],&pw[1],&mode[1])){
            SSID[1].toCharArray(SSID_char,20);
            if(ssidFinder.check_SSID_in_range(SSID_char)){
                //Serial.println("setting crededentials");
                //Serial.println(SSID[1]);
                WiFi.setCredentials(SSID[1], pw[1], mode[1]);
            };
        };
    };
    


    for(int i=0; i<10 && !WiFi.hasCredentials(); i++){
        // take new info
        parse_wifi();
        // set info
        delay(100);
        if(i%2==0){
            green->off();
            red->on();
        } else {
            green->on();
            red->off();
        }
    }
    
    if(WiFi.hasCredentials()){
        WiFi.connect();
        //Serial.println("return true");
        delay(1000);
        return true;
    }
    
    //Serial.println("return false");
    delay(1000);
    return false;
}

// set the config for the regular operational mode
bool set_macs_login(){
    WiFi.on();
    WiFi.clearCredentials();
    
    String SSID;
    String pw;
    int mode;
    
    
    // Now use the class
    FindSSID ssidFinder;
    char SSID_char[20];
    SSID.toCharArray(SSID_char,20);
    if(get_wifi_config(WIFI_MACS,&SSID,&pw,&mode) & ssidFinder.check_SSID_in_range(SSID_char)){
        WiFi.setCredentials(SSID, pw, mode);
    }
    

    for(int i=0; i<10 && !WiFi.hasCredentials(); i++){
        // take new info
        parse_wifi();
        // set info
        delay(1000);
    }
    
    if(WiFi.hasCredentials()){
        WiFi.connect();
        return true;
    }
    
    return false;
}


// read data from EEPROM, check them and set them if the check is passed
bool get_wifi_config(uint8_t id, String *_SSID, String *_pw, int *_type){
    uint16_t data_start=0;
    if(id==WIFI_MACS){
        data_start=START_WIFI_MACS;
    } else if(id==WIFI_UPDATE_1){
        data_start=START_WIFI_UPDATE_1;
    } else if(id==WIFI_UPDATE_2){
        data_start=START_WIFI_UPDATE_2;
    } else {
        #ifdef DEBUG_JKW_WIFI
        Serial.println("set wifi unknown id");
        #endif
        return false;
    }
    
    uint8_t SSID[21];
    uint8_t pw[21];
    uint8_t type=0x00;
    uint8_t chk=0x00;
    uint8_t read_char=0x00;
    uint8_t p=0x00;
    
    // read ssid
    bool all_FF=true;
    read_char=0x01; // avoid instand stop
    for(uint8_t i=0; i<20 && read_char!=0x00; i++){
        read_char=EEPROM.read(data_start+i);
        SSID[i]=read_char;
        p=i;
        
        if(read_char!=0xFF){ 
            all_FF=false; 
        }
    }
    
    if(all_FF){
        #ifdef DEBUG_JKW_WIFI
        Serial.println("invalid wifi data FF");
        #endif
        return false;
    }
    
    SSID[p+1]=0x00;
    
    // read pw
    read_char=0x01; // avoid instand stop
    for(uint8_t i=0; i<20 && read_char!=0x00; i++){
        read_char=EEPROM.read(data_start+i+20);
        pw[i]=read_char;
        p=i;
    }
    pw[p+1]=0x00;
    
    type=EEPROM.read(data_start+40);
    chk=EEPROM.read(data_start+41);
    
    #ifdef DEBUG_JKW_WIFI
    Serial.println("set wifi, data:");
    Serial.print("SSID:");
    Serial.print((const char*)SSID);
    Serial.println(".");
    delay(1000);
    Serial.print("PW:");
    Serial.print((const char*)pw);
    Serial.println(".");
    delay(1000);
    Serial.print("type:");
    Serial.print(type+'0');
    Serial.println(".");
    delay(1000);
    Serial.print("chk:");
    Serial.print(chk);
    Serial.println(".");
    delay(1000);
    #endif
    
    if(!check_wifi_config((const char*)SSID,(const char*)pw,type,chk)){
        
        #ifdef DEBUG_JKW_WIFI
        Serial.println("set wifi, data invalid");
        #endif
        *_SSID="";
        *_pw="";
        *_type=0;
        
        return false;
    }
    
    *_SSID=(const char*)SSID;
    *_pw=(const char*)pw;
    *_type=type;
    //WiFi.setCredentials((const char*)SSID, (const char*)pw, type);
    
    return true;
}

// checks if the data from the given configuration is valid
bool check_wifi_config(String SSID,String pw,uint8_t type,uint8_t chk){
    uint8_t checksum=0x00;
    for(uint8_t i=0;i<SSID.length();i++){
        checksum^=SSID.charAt(i);
    }
    for(uint8_t i=0;i<pw.length();i++){
        checksum^=pw.charAt(i);
    }
    checksum^=type;
    
    if(checksum!=chk){
        
        #ifdef DEBUG_JKW_WIFI
        Serial.println("check wifi data, data invalid");
        #endif
        
        return false;
    }
    
    #ifdef DEBUG_JKW_WIFI
    //Serial.println("Checksum ok");
    #endif 
    
    return true;
}

// save the wifi data to eeprom
bool save_wifi_config(uint8_t id,String SSID,String pw,uint8_t type,uint8_t chk){
    
    #ifdef DEBUG_JKW_WIFI
    Serial.println("save wifi, data:");
    Serial.print("SSID:");
    Serial.print(SSID);
    Serial.println(".");
    Serial.print("PW:");
    Serial.print(pw);
    Serial.println(".");
    Serial.print("type:");
    Serial.print(type);
    Serial.println(".");
    Serial.print("chk:");
    Serial.print(chk);
    Serial.println(".");
    delay(1000);
    #endif
    
    uint16_t data_start=0;
    // set data start, EEPROM adress
    if(id==WIFI_MACS){
        data_start=START_WIFI_MACS;
    } else if(id==WIFI_UPDATE_1){
        data_start=START_WIFI_UPDATE_1;
    } else if(id==WIFI_UPDATE_2){
        data_start=START_WIFI_UPDATE_2;
    } else {
        #ifdef DEBUG_JKW_WIFI
        Serial.println("save wifi unknown id");
        #endif
        return false;
    }
    
    // check if the submitted data are valid
    if(!check_wifi_config(SSID,pw,type,chk)){
        #ifdef DEBUG_JKW_WIFI
        Serial.println("save wifi, not valid");
        #endif
        return false;
    }
    
    // length check
    if(SSID.length()>20 || pw.length()>20){
        #ifdef DEBUG_JKW_WIFI
        Serial.println("save wifi, data to long");
        #endif
        return false;
    }
    
    //save the data
    // ssid
    for(uint8_t i=0;i<SSID.length() && i<20;i++){
        EEPROM.update(data_start+i+0,SSID.charAt(i));
    }
    if(SSID.length()<20){
        EEPROM.update(data_start+SSID.length()+0,0x00);
    }
    // pw
    for(uint8_t i=0;i<pw.length() && i<20;i++){
        EEPROM.update(data_start+i+20,pw.charAt(i));
    }
    if(pw.length()<20){
        EEPROM.update(data_start+pw.length()+20,0x00);
    }
    // type
    EEPROM.update(data_start+40,type);
    // checksum
    EEPROM.update(data_start+41,chk);
    
    return true;
}

// read config from serial port for wifi
bool parse_wifi(){
    if(!Serial.available()){
        return false;
    }
    
    uint8_t in=0x00;
    uint8_t tab_count=0;
    uint8_t SSID[20];
    uint8_t pw[20];
    uint8_t type=0x00;
    uint8_t chk=0x00;
    uint8_t id=0x00;
    uint8_t p=0x00;
    
    
    // buffer is 64 byte (id(1)+<tab>+SSID(20)+<tab>+pw(20)+<tab>+type(1)+<tab>+chk(1)+<tab>)=48
    // e.g. 00 09 6d 61 63 73 09 36 32 31 35 30 32 37 30 39 34 09 03 09 17 09
    // e.g. 01 09 61 6a 6c 6f 6b 65 72 74 09 71 77 65 71 77 65 71 77 65 09 03 09 60 09
    // e.g. 02 09 73 68 6f 70 09 61 62 63 64 65 66 67 68 09 03 09 0F 09
    //Serial.print("available:");
    //Serial.println(Serial.available());
    delay(1000); // give buffer time to fill
    
    while(Serial.available()){
        in=Serial.read();
        //Serial.print("read ");
        //Serial.print(in);
        //Serial.println(".");
        
        
        if(in==0x09){ // which is tab, our delimitter
            if(tab_count==1){
                SSID[p]=0x00;
            } else if(tab_count==2){
                pw[p]=0x00;
            }
            
            p=0x00;
            tab_count++;
            //Serial.print("tab is now ");
            //Serial.println(tab_count);
            
            
            // to identify if a macs unit is present i'll send a "i<tab>" and the unit will responde "MACS"
            if(tab_count==1 && id=='i'){
                tab_count=0;
                Serial.println("MACS");
            }
            
            if(tab_count==5){
                //Serial.println("try to save");
                if(save_wifi_config(id, (const char*)SSID, (const char*)pw, type, chk)){
                    Serial.println("saved");
                    return true;
                } else {
                    Serial.println("error");
                    return false;
                }
                tab_count=0;
            }
        } else if(tab_count==0){
            id=in;
        } else if(tab_count==1){
            SSID[p]=in;
            if(p<20){
                p++;
            };
        } else if(tab_count==2){
            pw[p]=in;
            if(p<20){
                
                p++;
            };
        } else if(tab_count==3){
            type=in;
        } else if(tab_count==4){
            chk=in;
        }
        
    }
    return false;
    //Serial.println("while end");
}
    
    
    
    
    
