#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>


//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "lrbhotspot"
#define WIFI_PASSWORD "LRB@12345lrb"

//user auth params
#define USER_EMAIL "ahmadlaaraib@gmail.com"
#define USER_PASSWORD "Ahmed@12345"


// Insert Firebase project API Key
#define API_KEY "AIzaSyBBj3aLC-NYQAbrGPGx0acQ_sSbEm0c_JI"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp32-project-cao-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;


unsigned long sendDataPrevMillis = 0;
int count = 0;
bool trigger=false;

#define switch 32
#define led 2

void setup(){
  pinMode(switch,INPUT_PULLUP);
  pinMode(led,OUTPUT);
  
  Serial.begin(115200);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  auth.user.email=USER_EMAIL;
  auth.user.password=USER_PASSWORD;

  
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  
}

void loop(){
  count=analogRead(switch);
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 500)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path data/int
    if (Firebase.RTDB.setInt(&fbdo, "data", count)){
      Serial.println("PASSED");
//      Serial.println("PATH: " + fbdo.dataPath());
//      Serial.println("TYPE: " + fbdo.dataType());
        if(count!=0)
        {
          Serial.println("led on..");
          trigger=true;
          digitalWrite(led,HIGH);
          Firebase.RTDB.setBool(&fbdo, "led", trigger);
        }
        else{
          Serial.println("led off..");
          trigger=false;
          digitalWrite(led,LOW);
          Firebase.RTDB.setBool(&fbdo, "led", trigger);
        }
    }
    else {
        Serial.println("FAILED");
//      Serial.println("REASON: " + fbdo.errorReason());
    }
    
  }
}
