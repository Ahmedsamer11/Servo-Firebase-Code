#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <ESP32Servo.h>

#define WIFI_SSID "Ahmed's iPhone"
#define WIFI_PASSWORD "12345678999"

Servo myservo;  // create servo object to control a servo
Servo myservo2;
// 16 servo objects can be created on the ESP32

int pos = 0;  // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33
// Possible PWM GPIO pins on the ESP32-S2: 0(used by on-board button),1-17,18(used by on-board LED),19-21,26,33-42
// Possible PWM GPIO pins on the ESP32-S3: 0(used by on-board button),1-21,35-45,47,48(used by on-board LED)
// Possible PWM GPIO pins on the ESP32-C3: 0(used by on-board button),1-7,8(used by on-board LED),9-10,18-21
#if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
int servoPin = 2;
int servoPin2 = 16;
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
int servoPin = 2;
int servoPin2 = 16;
#else
int servoPin = 2;
int servoPin2 = 16;
#endif

/* 2. Define the API Key */
#define API_KEY "AIzaSyBjVSWfK-KRoao5ddtVI4pLzas9xyMEx4A"

/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID "smart-parking-5eff5"

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "saifkamelae@gmail.com"
#define USER_PASSWORD "Saif123123"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
String myString = "";

void setup() {
  myString.reserve(300);
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);           // standard 50 hz servo
  myservo.attach(servoPin, 400, 7000);  // attaches the servo on pin 18 to the servo object
                                        // using default min/max of 1000us and 2000us
                                        // different servos may require different min/max settings
                                        // for an accurate 0 to 180 sweep

  myservo2.setPeriodHertz(50);           // standard 50 hz servo
  myservo2.attach(servoPin2, 400, 7000);
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h

  // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
  Firebase.reconnectNetwork(true);

  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  // fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  // // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);
}

void loop() {



  String documentPath = "gates/gates_status";
  String mask = "entry";
  delay(100);
  Serial.print("Get a document... ");

  if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), mask.c_str())){
    Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
  myString = fbdo.payload().c_str();
  int gate_stat = myString.indexOf("open");
  Serial.println(gate_stat);
  if (gate_stat>0) {
    myservo.write(110);
    FirebaseJson content;
    String data;
     data = "closed";
  content.clear();
  content.set("fields/entry/stringValue", data.c_str());
  Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(), "entry" /* updateMask */);
  delay(2000);
  } 
  else {
    myservo.write(0);
  }
  }
  else Serial.println(fbdo.errorReason());

   mask = "exit";
  Serial.print("Get a document... ");

  if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), mask.c_str())){
    Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
  myString = fbdo.payload().c_str();
  int gate_stat = myString.indexOf("open");
  Serial.println(gate_stat);
  if (gate_stat>0) {
    myservo2.write(110);
    FirebaseJson content;
    String data;
     data = "closed";
  content.clear();
  content.set("fields/exit/stringValue", data.c_str());
  Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(), "exit" /* updateMask */);
  delay(2000);
  } 
  else {
    myservo2.write(0);
  }
  }
  else Serial.println(fbdo.errorReason());
 
}
