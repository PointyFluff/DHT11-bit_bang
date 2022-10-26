/* (c) 2008 Samuel E. Bray
 * Arduino
 * DHT-11 Temp sensor, bit-bang driver. 
 * And yeah, it's terrible. 
 */ 

#include <Arduino.h>


int data, out, val = 0;
byte sensorData[5]; // sensor data array + checksum (40bits total)
int off;
int on;

void sensorInit() {
  pinMode(data, OUTPUT);
  digitalWrite(data, HIGH);
}

void setup() {
  Serial.begin(9600);
  Serial.print("Setting up...");
  //pins
  data = 6;
  out = 13;
  pinMode(data, INPUT); // pulls line high
  pinMode(out, OUTPUT);
  sensorInit();
  delay(2000); // give sensor time to startup and stabilize
  Serial.println("Ready Bitches!\n");
}

byte readByte() {
  byte out = 0;
  for(byte i = 0; i<8; i++) {
    while(digitalRead(data) == LOW) {
      Serial.print("_");
    } // expect low, wait for high;
    //data sheet states a '0' lasts only 27us and a '1' lasts 70us
    //if we wiat for the '0' expire the line will show the logic
    delayMicroseconds(40);
    //if we get a 1, push it left into place and xor it into the output
    //else don't because 0 | 0 = 0
    if(digitalRead(data == HIGH)) {
      out |= (1<<(7-i));
    }
    while(digitalRead(data)==HIGH) {
      Serial.print("-"); // call me Morse! (looks like morsel, mmmm...so, buttery... *drool*)
    }
  }
  return out;
}

int request() {
  //request data from sensor
  int val = 0;
  pinMode(data, OUTPUT);
  digitalWrite(data, LOW);
  delay(18); //wake up pulse
  digitalWrite(data, HIGH);
  delayMicroseconds(30);
  pinMode(data, INPUT);
  delayMicroseconds(40);
  val = digitalRead(data);
  if(val == HIGH) {
    Serial.println("Start Con 1 FAIL");
  }
  else {
    Serial.println("Start Con 1 ACK");
  }
  delayMicroseconds(40);
  val = digitalRead(data);
  if(val == LOW) {
    Serial.println("Start Con 2 FAIL");
    return 1;
  }
  else {
    Serial.println("Start Con 2 ACK");
  }

  for(int i=0; i<5; i++ ) {
    sensorData[i] = readByte();
  }
  Serial.println("Done");
  byte checksum = 0;
  for(int i = 0; i<4; i++) {
    checksum += sensorData[i];
  }
  if(checksum != sensorData[4]) {
    Serial.println("Checksum mismatch");
  }
  Serial.print("Calculated: "); Serial.println(checksum, BIN);
  Serial.print("Returned: "); Serial.println(sensorData[4], BIN);

  Serial.println("Data: ");
  for(int i = 0; i<4; i++) {
    Serial.print(sensorData[i], BIN);
    Serial.print("\t");
    Serial.println(sensorData[i], DEC);
  }
}

void loop() {
  Serial.println("Requesting Data");
  if(request()) {
    Serial.println("FAIL");
  }
  else {
    Serial.println("Good");
  }
  delay(5000);
}
