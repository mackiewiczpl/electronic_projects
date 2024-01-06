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

  char y_num_a[5] ;
  char x_num_a[5] ;
  int turnRateL = 0;
  int turnRateR = 0;
 
  for (int i = 0; i < length; i++) {
     if((char)payload[i] == 'X') {
      //Serial.println("getting x axis");
       int j = i;
       int k = 0;
       while(isDigit((char)payload[j+1]) && k < sizeof(x_num_a) - 1) {
          x_num_a[k] = (char)payload[j+1];
          //Serial.println(x_num_a[k]);
          j++;k++;
       }
       x_num_a[k] = '\0';
     }
     if((char)payload[i] == 'Y') {
      //Serial.println("getting y axis");
       int j = i;
       int k = 0;
       while(isDigit((char)payload[j+1]) && k < sizeof(y_num_a) - 1) {
          y_num_a[k] = (char)payload[j+1];
          //Serial.println(y_num_a[k]);
          j++;k++;        
       }
       y_num_a[k] = '\0';
    }
  }

  // Control motors
  
  int y_num, x_num; 
  sscanf(y_num_a, "%d", &y_num);
  sscanf(x_num_a, "%d", &x_num);


  int motorSpeedF = map(y_num, 0, 4095, 0, 255);
  int motorSpeedB = map(y_num, 0, 4095, 255, 0);
   
  if(x_num >= 2000 || x_num < 1500) {
    turnRateL = map(x_num, 0, 4095, 0, 255);
    turnRateR = map(x_num, 0, 4095, 255, 0);
  }

 // Serial.println((String)"speed: "+motorSpeed);
 // Serial.println((String)"left turn: "+turnRateL);
 // Serial.println((String)"right turn: "+turnRateR);


//1500-1800 is range of readouts when joystick position is neutral(in each axis)
//Do not move motor when joystick in neutral state

//  if forward
    if(y_num >= 2000) {
        char buffer[40];
       // sprintf(buffer, "Moving forward! y=%d, x=%d", y_num, x_num );
       // Serial.println(buffer);
       // Serial.println(motorSpeed);
        //motorSpeed = 500;
//    if left  
 
        analogWrite(pwmMotorA, motorSpeedF - turnRateL);
        analogWrite(pwmMotorB, motorSpeedF - turnRateR);
        //digitalWrite(pwmMotorB, abs(motorSpeed - turnRate));
 	      digitalWrite(dirMotorA, LOW );
        digitalWrite(dirMotorB, HIGH );
        delay(350);
    }


// if reverse  

    if(y_num < 1500) {
      //  char buffer[40];
      //  sprintf(buffer, "Moving backward! y=%d, x=%d", y_num, x_num );
      //  Serial.println(buffer);
      //  Serial.println(motorSpeed);
        //motorSpeed = 500;
//    if left  
 
        analogWrite(pwmMotorA, motorSpeedB - turnRateR);
        analogWrite(pwmMotorB, motorSpeedB - turnRateL);
        //digitalWrite(pwmMotorB, abs(motorSpeed - turnRate));
 	      digitalWrite(dirMotorA, HIGH );
        digitalWrite(dirMotorB, LOW );
        delay(350);
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

}
