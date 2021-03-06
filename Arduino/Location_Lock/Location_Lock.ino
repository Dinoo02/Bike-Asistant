#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial GPRS(2,3);

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
Servo lockServo;


int motorPin1 = 8;
int motorPin2 = 9;
int motorPin3 = 10;
int motorPin4 = 11;
int delayTime = 10;
int allarmPin = 5;
int lockPin = 7;




const int lockSensor = 4;

bool Locked = false;
bool toLock = true;
bool lastToLock = false;

void allarm(){
     TCCR0B = TCCR0B & B11111000 | B00000100; // for PWM frequency of 244.14 Hz
  analogWrite(allarmPin,225);
    delay(50);
  analogWrite(allarmPin,0);
  TCCR0B = TCCR0B & B11111000 | B00000101; // for PWM frequency of 61.04 Hz

  analogWrite(allarmPin,225);
  delay(50);
  analogWrite(allarmPin,0);

  TCCR0B = TCCR0B & B11111000 | B00000011; // for PWM frequency of 976.56 Hz (The DEFAULT)
}

void lock(){
  Serial.println("Locking");
  while(!digitalRead(lockSensor)){
    lockServo.write(120);
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
  lockServo.write(60);
}


void unlock(){
  Serial.println("Unlocking");
  
    lockServo.write(120);
    for(int i=0;i<600;i++){
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
}

void setup(){
    pinMode(lockSensor, INPUT);
    pinMode(allarmPin, OUTPUT);
    pinMode(lockPin, INPUT);
    analogWrite(5,0);
  Serial.begin(9600);
    
  lockServo.attach(6);
  lockServo.write(120);

  GPRS.begin(9600);
   Serial.begin(9600);

   GPRS.println("AT");
   GPRS.println("AT+CGNSPWR=1");
   GPRS.println("AT+CGNSSEQ=\"RMC\"");
   while(!GPRS.available());
  while(GPRS.available())GPRS.read();

}
char lat[10];
char lon[10];
char buffer[130];
int bufferCount=0;

void loop(){
//--------------------Locking---------------------------

 if(digitalRead(lockPin))toLock = 1;
 else toLock = 0;

 if(digitalRead(lockSensor))Locked = 1;
 else Locked = 0; 

 if(toLock){
    
    if(!lastToLock)
    lock();
    else if(!Locked) allarm();
 }
 else if(lastToLock) unlock();
 lastToLock=toLock;

//if(toLock&&!Locked)allarm();

//----------------------GPS------------------------------
   GPRS.println("AT+CGNSINF"); //Sending a request to SIM808

      
      while(!GPRS.available());
      if(GPRS.available()>0){
        while(GPRS.available()>0){
              buffer[bufferCount++]=GPRS.read();
              if(bufferCount==130)break;
              delay(2);
        }
       buffer[bufferCount]='\0';   // filling the buffer with data
       
        Serial.println(buffer);
      char data[64];
      
      
      for(int i=0;i<3;i++)
         {  clearCharArray(data); 
           if(extractData(buffer,data));
           
         }                               //deleting unnecesary data
         
         clearCharArray(data);
         extractData(buffer,data);
         strcpy(lat,data);              //write lat
         
         extractData(buffer,data);
         strcpy(lon,data);              //write lon

         Serial.println(lat);
         Serial.println(lon);       //print lat/lon
         clearBufferArray();        //clear buffer
         
      }
      

      //delay(2000);

}

bool extractData(char string[],char Data[]){
   char extractBuffer[64];
   int count=0;
   char character;
   
   while(string && count<64){
    character=string[count];
    if(character!=','){
     extractBuffer[count++]=character; //adding to a local buffer the characters that aren't a comma
     //Serial.print(character);
    }
     else{
      extractBuffer[count]='\0';        //when it finds a comma ends the local buffer with an \0 
      strcpy(Data,extractBuffer);        //transfer the data back thru parameter "Data"
      strcpy(string, string+count+1);   
    
      return 1;}
   }
   return 0;
}

void clearCharArray(char array[]){
   for(int i=0;i<20;i++)
   array[i]=NULL;
}

void clearBufferArray(){
    for(int i=0;i<=bufferCount;i++)
        buffer[i]=NULL;
    bufferCount=0;
}
