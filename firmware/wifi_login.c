#include "wifi_login.h"
#include "application.h"

void set_update_login(){
    WiFi.on();
    WiFi.clearCredentials();
    int i=0;
    while(!WiFi.hasCredentials() && i<13){
        i++;
        WiFi.setCredentials("ajlokert", "qweqweqwe",WPA2);
        WiFi.setCredentials("shop", "abcdefgh",WPA2);
        delay(1000);
        if(WiFi.hasCredentials()){
            Serial.println("WiFi credentials saved");
        } else {
            Serial.println("Ohoh, WiFi has NOT saved the credentials");
        }
    }
}

void set_macs_login(){
    WiFi.on();
    WiFi.clearCredentials();
    int i=0;
    while(!WiFi.hasCredentials() && i<13){
        i++;
        WiFi.setCredentials("macs", "6215027094",WPA2);
        delay(1000);
        if(WiFi.hasCredentials()){
            Serial.println("WiFi credentials saved");
        } else {
            Serial.println("Ohoh, WiFi has NOT saved the credentials");
        }
    }
}
