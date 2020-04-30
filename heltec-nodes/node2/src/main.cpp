#include <Arduino.h>
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output
}

void loop()
{
  Serial.begin(115200);
  Serial.println("Oi Old node, baud: 115200");
  digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on by making the voltage LOW
  delay(1000);                     // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  delay(2000);                     // Wait for two seconds
}
