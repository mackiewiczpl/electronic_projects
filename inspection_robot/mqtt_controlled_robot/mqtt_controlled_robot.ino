#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string>

// Update these with values suitable for your network.

const char* ssid = "misterroboto";
const char* password = "R3b3ll10n!";
const char* mqtt_server = "192.168.50.164";
const int pwmMotorA = D1;
const int pwmMotorB = D2;
const int dirMotorA = D3;
const int dirMotorB = D4;

int motorSpeed = 500;
int turnRate = 0;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  #define VRX_PIN  36 // ESP32 pin GPIO36 (ADC0) connected to VRX pin
#define VRY_PIN  39 // ESP32 pin GPIO39 (ADC0) connected to VRY pin

int valueX = 0; // to store the X-axis value
int valueY = 0; // to store the Y-axis value
}

void setup() {
  Serial.begin(9600) ;
}

void loop() {
  // read X and Y analog values
  valueX = analogRead(VRX_PIN);
  valueY = analogRead(VRY_PIN);

  // print data to Serial Monitor on Arduino IDE
  Serial.print("x = ");
  Serial.print(valueX);
  Serial.print(", y = ");
  Serial.println(valueY);
  delay(200);
}
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  char y_num_a[4];
  char x_num_a[4];
  for (int i = 0; i < length; i++) {
    if((char)payload[i] == 'Y') {
       int j = i+1;
       int k = 0; 
       while (isDigit((char)payload[j])){
         y_num_a[k] = payload[j];
         j++;k++;
       }
    }
    if((char)payload[i] == 'X') {
       int j = i+1;
       int k = 0;
       while (isDigit((char)payload[j])){
         x_num_a[k] = payload[j];
         j++;k++;
       }
    }   
  }

  // Control motors
  uint y_num = atoi(y_num_a);
  uint x_num = atoi(x_num_a);
  

//1500-1800 is range of readouts when joystick position is neutral(in each axis)
//Do not move motor when joystick in neutral state

//  if forward
    if(y_num >= 2000) {
        motorSpeed = (y_num/4) - 500;
//    if left  
      if (x_num < 1500) {
        turnRate = 500 - (x_num/3);
        digitalWrite(pwmMotorA, motorSpeed);
        digitalWrite(pwmMotorB, abs(motorSpeed - turnRate));
 	      digitalWrite(dirMotorA, LOW);
        digitalWrite(dirMotorB, LOW);
        delay(dist);
      }
// if right    
      if (x_num >= 2000) {
        turnRate = (x_num/4) - 500;
        digitalWrite(pwmMotorA, abs(motorSpeed - turnRate));
        digitalWrite(pwmMotorB, motorSpeed);
 	      digitalWrite(dirMotorA, LOW);
        digitalWrite(dirMotorB, LOW);
        delay(dist);
     }
   }
// if reverse   
    if (y_num < 1500) {
        motorSpeed = (y_num/4) - 500;
//    if left  
      if (x_num < 1500) {
        turnRate = 500 - (x_num/3);
        digitalWrite(pwmMotorA, abs(motorSpeed - turnRate));
        digitalWrite(pwmMotorB, motorSpeed);
 	      digitalWrite(dirMotorA, HIGH);
        digitalWrite(dirMotorB, HIGH;
        delay(dist);
      }
// if right    
      if (x_num >= 2000) {
        turnRate = (x_num/4) - 500;
        digitalWrite(pwmMotorA, motorSpeed);
        digitalWrite(pwmMotorB, abs(motorSpeed - turnRate));
 	      digitalWrite(dirMotorA, HIGH);
        digitalWrite(dirMotorB, HIGH);
        delay(dist);
     }
    }

    digitalWrite(pwmMotorA, 0);
    digitalWrite(pwmMotorB, 0);
 	  digitalWrite(dirMotorA, LOW);
    digitalWrite(dirMotorB, LOW);

  }

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("/test/topic", "hello from ESP");
      // ... and resubscribe
      client.subscribe("/test/topic2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(pwmMotorA , OUTPUT);
 	pinMode(pwmMotorB, OUTPUT);
 	pinMode(dirMotorA, OUTPUT);
 	pinMode(dirMotorB, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer("192.168.50.164", 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("/test/topic", msg);
  }
}
