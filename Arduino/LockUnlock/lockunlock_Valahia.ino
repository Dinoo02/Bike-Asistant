#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial Serial1(2, 3);
Servo lockServo;


int motorPin1 = 8;
int motorPin2 = 9;
int motorPin3 = 10;
int motorPin4 = 11;
int delayTime = 10;




const int lockSensor = 4;
long timeout;
boolean connectedToServer = false;
boolean locked = false;

void printCommand(String comm) {
  Serial.print("--- ");
  Serial.println(comm);
  Serial1.println(comm);
  while(Serial1.available()) {
    Serial.write(Serial1.read());
  }
  delay(1000);
}

void connectToServer() {
  printCommand("AT+CIPSTART=\"TCP\",\"207.154.213.12\",\"443\"");
}

void disconnectFromServer() {
  printCommand("AT+CIPCLOSE");
}

void sendToServer(String message) {
  printCommand("AT+CIPSEND");
  Serial1.print(message);
  Serial1.write(0x1A);
}


bool Locked = false;

void lock(){
  Serial.println("Locking");
  while(!digitalRead(lockSensor)){
    lockServo.write(120);
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(delayTime);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(delayTime);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
  delay(delayTime);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
  delay(delayTime);
  }
  lockServo.write(60);
}


void unlock(){
  Serial.println("Unlocking");
  if(digitalRead(lockSensor)){
    lockServo.write(120);
    for(int i=0;i<600;i++){
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
  delay(delayTime);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
  delay(delayTime);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(delayTime);
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(delayTime);
  }
  }
}




void setup() {
 // pinMode(solenoid, OUTPUT);
  pinMode(lockSensor, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600);

  while(!Serial.available());
  Serial.println(Serial.read());
 // printCommand("AT+CPIN=\"0000\"");
  delay(5000);
  printCommand("AT+CIPSHUT");
  printCommand("AT+CIPMUX=0");
  printCommand("AT+CGPSPWR=1");
  printCommand("AT+CGATT=1");
  printCommand("AT+CSTT=\"net\",\"\",\"\"");
  printCommand("AT+CIICR");
  printCommand("AT+CIFSR");
  timeout = millis();

  lockServo.attach(5);
  lockServo.write(120);

  
}

void loop() {
  if(Serial.available()) {
    auto message = Serial.readStringUntil(0x7E);
    if(message == "CONNECT") {
      connectToServer();
      connectedToServer = true;
    }
    else if(message == "DISCONNECT") {
      disconnectFromServer();
      connectedToServer = false;
    }
    else if(connectedToServer == true) {
      sendToServer(message);
    }
    
  }

  if(millis() - timeout > 30000 && connectedToServer == true) {
    timeout = millis();
    Serial1.println("AT+CGPSINF=0");
    while(!Serial1.available());
    auto loc = Serial1.readString();
    // Serial.print(loc);
    // Serial.println("{\"loc\": \"" + loc + "\"}");
    sendToServer(loc);
  }

  if(digitalRead(lockSensor) == HIGH) {
    if(locked == true) {
      sendToServer("LOCKBRK");
      unlock();
      locked = false;
    }
  }
  
  if(Serial1.available()) {
    auto message = Serial1.readStringUntil(0x40);
    if(message == "lock") {
      locked = true;
      lock();
    }
    else if(message == "unlock") {
      locked = false;
      unlock();
    }
    Serial.println(message);
  }
}
