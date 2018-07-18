
#include <Extint_lib.h>

const byte ledPin = 13;
const byte interruptPin = 12;

Extint ext;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  ext.begin();
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  Serial.println("Interrupt test...");
  digitalWrite(ledPin, LOW);
  pinMode(interruptPin, INPUT_PULLUP);
  bool bAttached = ext.attachI(digitalPinToInterrupt(interruptPin),blink,LOW);
  Serial.print("Is the interrupt attached? ");
  Serial.println (bAttached == true ? "YES" : "NO");

}

void loop() {
  Serial.print("The interrupt pin is at ");
  Serial.println(digitalRead(interruptPin) == 1 ? "HIGH" : "LOW");
  Serial.println("Going to sleep.\n");
  ext.standbyMode();
  Serial.println("Woke up\n");
  Serial.print("The interrupt pin is at ");
  Serial.println(digitalRead(interruptPin) == 1 ? "HIGH" : "LOW");
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);



}
// Currently doesn't get called.  Testing 1-2-3
void blink() {
  

}
