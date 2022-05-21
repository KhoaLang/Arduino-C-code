#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <ArduinoJson.h>


#define WiFi_SSID "UiTiOt-E3.1"
#define WiFi_PASS "UiTiOtAP"

#define FIREBASE_HOST "arduino-c17e5-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "qBCHdKt3od8QYtJdGb7uT2vqULxvHzbb8I4bP81v"



FirebaseData firebaseData;
//WiFiClient client;
String path = "/";
FirebaseJson json;


byte ledSignal = 0;

     
long interval = 0;    

int LED = D3;
int FAN = D4;



void setup() {
  Serial.begin(115200);
  WiFi.begin(WiFi_SSID, WiFi_PASS);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  pinMode(LED, OUTPUT);
  pinMode(FAN, OUTPUT);
//  digitalWrite(LED, LOW);
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  if(!Firebase.beginStream(firebaseData, path)){
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }
  Serial.print("Connected with IP: ");
  Serial.print(WiFi.localIP());

}

void handleTimer(int interval){
  for(int i=0; i<=interval; ++i){
    delay(1000);
  }
  Firebase.getInt(firebaseData, path + "/device");
  if (ledSignal == 0) {

    if(firebaseData.intData() == 1){
      Firebase.setInt(firebaseData, path + "/LED/turnONOFF", 1);
    }else if(firebaseData.intData() == 2){
      Firebase.setInt(firebaseData, path + "/Fan/turnONOFF", 1);
    }
  } else {

   
    if(firebaseData.intData() == 1){
      Firebase.setInt(firebaseData, path + "/LED/turnONOFF", 0);
    }else if(firebaseData.intData() == 2){
       Firebase.setInt(firebaseData, path + "/Fan/turnONOFF", 0);
    }
  }
  Firebase.setInt(firebaseData, path + "/LED/setTimeout", 0);
  Firebase.setInt(firebaseData, path + "/Fan/setTimeout", 0);
//  Firebase.setInt(firebaseData, path + "/device", 0);
}

void loop() {
  if(Firebase.getInt(firebaseData, path + "/device")){
    if(firebaseData.intData() == 1 || firebaseData.intData() == 0){
      Firebase.getInt(firebaseData, path + "/LED/setTimeout");
      interval = firebaseData.intData();
      if(interval > 0){
        handleTimer(interval);
      }
      else{
          Firebase.getInt(firebaseData, path + "/LED/turnONOFF");
          ledSignal = firebaseData.intData();
          if(ledSignal == 1) {  
            digitalWrite(LED, HIGH);
            Serial.println("Den sang");
          }else{
            Serial.println("Den tat");
            digitalWrite(LED, LOW);    
          }
      }
     
    }
    else if(firebaseData.intData() == 2 || firebaseData.intData() == 0){
      Firebase.getInt(firebaseData, path + "/Fan/setTimeout");
      interval = firebaseData.intData();
      if(interval > 0){
         handleTimer(interval);
      }else{
        if(Firebase.getInt(firebaseData, path + "/Fan/turnONOFF")) ledSignal=firebaseData.intData();
        if(ledSignal == 1) {  
          digitalWrite(FAN, HIGH);
          Serial.println("Quat bat");
        }else{
          Serial.println("Quat tat");
          digitalWrite(FAN, LOW);    
        }
      }
      
    }
    
  }
}
  
