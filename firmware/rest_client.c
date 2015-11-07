/**
 ******************************************************************************
 * @file    rest_client.cpp
 * 
 * details: https://github.com/llad/spark-restclient
 * 
 * credit: https://github.com/csquared/arduino-restclient
 * 
 ******************************************************************************

*/

#include "rest_client.h"

//#define HTTP_DEBUG

#ifdef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string) (Serial.print(string))
#endif

#ifndef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string)
#endif

RestClient::RestClient(IPAddress _host){
    
  host = _host;
  port = 80;
  num_headers = 0;
  contentTypeSet = false;
}

RestClient::RestClient(IPAddress _host, int _port){
  host = _host;
  port = _port;
  num_headers = 0;
  contentTypeSet = false;
}

// GET path
int RestClient::get(const char* path){
  return request("GET", path, NULL, NULL);
}

//GET path with response
int RestClient::get(const char* path, String* response){
  return request("GET", path, NULL, response);
}

// POST path and body
int RestClient::post(const char* path, const char* body){
  return request("POST", path, body, NULL);
}

// POST path and body with response
int RestClient::post(const char* path, const char* body, String* response){
  return request("POST", path, body, response);
}

// PUT path and body
int RestClient::put(const char* path, const char* body){
  return request("PUT", path, body, NULL);
}

// PUT path and body with response
int RestClient::put(const char* path, const char* body, String* response){
  return request("PUT", path, body, response);
}

// DELETE path
int RestClient::del(const char* path){
  return request("DELETE", path, NULL, NULL);
}

// DELETE path and response
int RestClient::del(const char* path, String* response){
  return request("DELETE", path, NULL, response);
}

// DELETE path and body
int RestClient::del(const char* path, const char* body ){
  return request("DELETE", path, body, NULL);
}

// DELETE path and body with response
int RestClient::del(const char* path, const char* body, String* response){
  return request("DELETE", path, body, response);
}

void RestClient::write(const char* string){
  HTTP_DEBUG_PRINT(string);
  client.print(string);
}

void RestClient::setHeader(const char* header){
  headers[num_headers] = header;
  num_headers++;
}

// The mother- generic request method.
//
int RestClient::request(const char* method, const char* path,
                  const char* body, String* response){
                      
  const char HOSTNAME[9]={((host>>24)&0xff)+'0','.',((host>>16)&0xff)+'0','.',((host>>8)&0xff)+'0','.',((host)&0xff)+'0','.',0x00}; // needed for the print

  HTTP_DEBUG_PRINT("HTTP: connect\n");
  
  //Serial.println("und los");
  delay(100);
  /*Serial.print("connect to ");
  Serial.print(host[0],DEC);
  Serial.print(".");
  Serial.print(host[1],DEC);
  Serial.print(".");
  Serial.print(host[2],DEC);
  Serial.print(".");
  Serial.print(host[3],DEC);
  Serial.println("!");*/
  //delay(2000);

  if(client.connect(host, port)){
    
    delay(200);
    Serial.flush();
    Serial.println("\r\nconnected");
    delay(200);
  
    HTTP_DEBUG_PRINT("HTTP: connected\n");
    HTTP_DEBUG_PRINT("REQUEST: \n");
    // Make a HTTP request line:
    write(method);
    write(" ");
    write(path);
    write(" HTTP/1.1\r\n");
    for(int i=0; i<num_headers; i++){
      write(headers[i]);
      write("\r\n");
    }
    write("Host: ");
    write(HOSTNAME);
    write("\r\n");
    write("Connection: close\r\n");

    if(body != NULL){
      char contentLength[30];
      sprintf(contentLength, "Content-Length: %d\r\n", strlen(body));
      write(contentLength);

      if(!contentTypeSet){
        write("Content-Type: application/x-www-form-urlencoded\r\n");
      }
    }

    write("\r\n");

    if(body != NULL){
      write(body);
      write("\r\n");
      write("\r\n");
    }

    //make sure you write all those bytes.
    delay(200);

    HTTP_DEBUG_PRINT("HTTP: call readResponse\n");
    int statusCode = readResponse(response);
    HTTP_DEBUG_PRINT("HTTP: return readResponse\n");

    //cleanup
    HTTP_DEBUG_PRINT("HTTP: stop client\n");
    num_headers = 0;
    client.stop();
    delay(50);
    HTTP_DEBUG_PRINT("HTTP: client stopped\n");

    return statusCode;
  }else{
    HTTP_DEBUG_PRINT("HTTP Connection failed\n");
    return 0;
  }
}

int RestClient::readResponse(String* response) {

  // an http request ends with a blank line
  boolean currentLineIsBlank = true;
  boolean currentLineIsBlank2 = false;
  boolean httpBody = false;
  boolean inStatus = false;

  char statusCode[4];
  int i = 0;
  int code = 0;

  if(response == NULL){
    HTTP_DEBUG_PRINT("HTTP: NULL RESPONSE POINTER: \n");
  }else{
    HTTP_DEBUG_PRINT("HTTP: NON-NULL RESPONSE POINTER: \n");
  }

  HTTP_DEBUG_PRINT("HTTP: RESPONSE: \n");
  uint32_t now=millis();
  while (client.connected() && millis()<now+2000) {
    HTTP_DEBUG_PRINT(".");
    if (client.available()) {

      char c = client.read();
      HTTP_DEBUG_PRINT(c);

      if(c == ' ' && !inStatus){
        inStatus = true;
      }

      if(inStatus && i < 3 && c != ' '){
        statusCode[i] = c;
        i++;
      }
      if(i == 3){
        statusCode[i] = '\0';
        code = atoi(statusCode);
      }

      //only write response if its not null
      if(httpBody){
        if(response != NULL) response->concat(c);
      }
      if (c == '\n' && httpBody){
        HTTP_DEBUG_PRINT("HTTP: return readResponse2\n");
        //return code;
      }
      if (c == '\n' && currentLineIsBlank2) {
        httpBody = true;
        HTTP_DEBUG_PRINT("Here comes the body\n");
      }
      if (c == '\n') {
        // you're starting a new lineu
        if(currentLineIsBlank){
            currentLineIsBlank2 = true;
        }
        currentLineIsBlank = true;
      }
      else if (c != '\r') {
        // you've gotten a character on the current line
        currentLineIsBlank = false;
      }
    }
    //delay(100);
  }
  client.flush();

  HTTP_DEBUG_PRINT("HTTP: return readResponse3\n");
  return code;
}
