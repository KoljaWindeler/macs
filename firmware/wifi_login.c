#include "wifi_login.h"
#include "application.h"

void set_update_login(){
    WiFi.on();
    WiFi.clearCredentials();
    while(!WiFi.hasCredentials()){
        WiFi.setCredentials("ajlokert", "qweqweqwe",WPA2);
        WiFi.setCredentials("shop", "xxxx",WPA2);
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
    while(!WiFi.hasCredentials()){
        WiFi.setCredentials("macs", "xxxxx",WPA2);
        delay(1000);
        if(WiFi.hasCredentials()){
            Serial.println("WiFi credentials saved");
        } else {
            Serial.println("Ohoh, WiFi has NOT saved the credentials");
        }
    }
}
