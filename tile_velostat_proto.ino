int led = 3;
int sensor = A0;
int threshold = 30;

void setup() {
pinMode(led,OUTPUT);
pinMode(A0,INPUT);  
Serial.begin(9600);
  // put your setup code here, to run once:

}

void loop() {
 int pressure = analogRead(sensor);
 if (pressure >=threshold ){
   digitalWrite(led,HIGH);
 } 
 else digitalWrite(led,LOW);
 Serial.println(pressure);
  // put your main code here, to run repeatedly:

}
