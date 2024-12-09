const int ldrpin = A0;
const int buzzerpin = 5;
const int Pinout_ldr = 19; // PC5


void setup() {
Serial.begin (9600);
pinMode(buzzerpin, OUTPUT);
pinMode(ldrpin, INPUT);
pinMode (Pinout_ldr, OUTPUT);
  // put your setup code here, to run once:

}

void loop() {
int ldrvalue = analogRead (ldrpin);
Serial.println(ldrvalue);
// Output LDR value from Pin 19 (PC5)
digitalWrite(Pinout_ldr, ldrvalue > 100 ? HIGH : LOW); // Set Pin 19 HIGH if LDR value is above 100, otherwise LOW
  
if (ldrvalue < 100) {
digitalWrite (buzzerpin,LOW);
}
else {
digitalWrite (buzzerpin,HIGH);
}


  // put your main code here, to run repeatedly:

}
