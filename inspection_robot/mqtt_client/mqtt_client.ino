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
  Serial.print("Connecting to ");
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
  String num = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    if (i >=2 && i <=5) {
       if(isDigit(payload[i])) {
          num += (char)payload[i];
       }
    }
  }
  Serial.println(num);

  // Control motors
  int dist = num.toInt();
  if ((char)payload[0] == 'A') {
    if ((char)payload[1] == 'F') {
       digitalWrite(pwmMotorA, motorSpeed);
 	     digitalWrite(dirMotorA, LOW);
       delay(dist);
    }
    if ((char)payload[1] == 'R') {
       digitalWrite(pwmMotorA, motorSpeed);
 	     digitalWrite(dirMotorA, HIGH);
       delay(dist);

    }
    digitalWrite(pwmMotorA, 0);
 	  digitalWrite(dirMotorA, LOW);

  }
  if ((char)payload[0] == 'B') {
    if ((char)payload[1] == 'F') {
       digitalWrite(pwmMotorB, motorSpeed);
 	     digitalWrite(dirMotorB, LOW);
       delay(dist);
    }
    if ((char)payload[1] == 'R') {
       digitalWrite(pwmMotorB, motorSpeed);
 	     digitalWrite(dirMotorB, HIGH);
       delay(dist);

    }
    digitalWrite(pwmMotorB, 0);
 	  digitalWrite(dirMotorB, LOW);
  }
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
