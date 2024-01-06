#include <ezButton.h>

#define VRX_PIN  36 // ESP32 pin GPIO36 (ADC0) connected to VRX pin
#define VRY_PIN  39 // ESP32 pin GPIO39 (ADC0) connected to VRY pin
#define SW 17

ezButton button(SW);

int valueX = 0; // to store the X-axis value
int valueY = 0; // to store the Y-axis value
int swtch = 0;
void setup() {
  button.setDebounceTime(50);
  Serial.begin(9600) ;
}

void loop() {
  // read X and Y analog values
  valueX = analogRead(VRX_PIN);
  valueY = analogRead(VRY_PIN);
  button.loop();
  swtch = button.getState();

  // print data to Serial Monitor on Arduino IDE
  Serial.print("x = ");
  Serial.print(valueX);
  Serial.print(", y = ");
  Serial.println(valueY);
  Serial.print(" : button = ");
  Serial.println(swtch);
  if(button.isPressed()) {
     Serial.println("Switch pressed");
  }
    if(button.isReleased()) {
     Serial.println("Switch released");
  }
  delay(500);
}
