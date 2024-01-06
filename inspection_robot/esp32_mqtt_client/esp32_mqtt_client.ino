#include <WiFi.h>
#include <PubSubClient.h>
#include <ezButton.h>
#include <string>

#define VRX_PIN  36 // ESP32 pin GPIO36 (ADC0) connected to VRX pin
#define VRY_PIN  39 // ESP32 pin GPIO39 (ADC0) connected to VRY pin
#define SW 17

ezButton button(SW);

int valueX = 0; // to store the X-axis value
int valueY = 0; // to store the Y-axis value
int swtch = 0;

// WiFi
const char *ssid = "misterroboto"; // Enter your Wi-Fi name
const char *password = "R3b3ll10n!";  // Enter Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "192.168.50.164";
const char *topic = "/test/topic2";
//const char *mqtt_username = "emqx";
//const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    button.setDebounceTime(50);
    // Set software serial baud to 115200;
    Serial.begin(115200);
    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str())) {
            Serial.println("Broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    // Publish and subscribe
    client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
    client.loop();
    valueX = analogRead(VRX_PIN);
    valueY = analogRead(VRY_PIN);
    button.loop();
    swtch = button.getState();
    char swt[] = {'S', 'D', '\n'};
    
    if( (valueX <= 1500 || valueX >= 2000) || (valueY <= 1500 || valueY >= 2000)) {      
      char msg_xy[11];
      snprintf(msg_xy, sizeof(msg_xy), "%c%u%c%u", 'X', valueX, 'Y', valueY);
    
      client.publish("/test/topic2", msg_xy);
    }  

    if(button.isPressed()) {
       swt[1] = 'P';
       client.publish("/test/topic2", swt);
    }
    if(button.isReleased()) {
       swt[1] = 'R';  
       client.publish("/test/topic2", swt);
    }
  delay(350);    
}
