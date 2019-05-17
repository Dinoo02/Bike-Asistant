#include <SoftwareSerial.h>

SoftwareSerial GPRS(2,3);

void setup() {
  // put your setup code here, to run once:
   GPRS.begin(9600);
   Serial.begin(9600);

   GPRS.println("AT");
   GPRS.println("AT+CGNSPWR=1");
   GPRS.println("AT+CGNSSEQ=\"RMC\"");
   while(!GPRS.available());
  while(GPRS.available())GPRS.read();
 // GPRS.println("AT+CGNSINF");
}
char lat[10];
char lon[10];
char buffer[130];
int bufferCount=0;
void loop() {
  
      GPRS.println("AT+CGNSINF"); //Sending a request to SIM808

      
      while(!GPRS.available());
      if(GPRS.available()>0){
        while(GPRS.available()>0){
              buffer[bufferCount++]=GPRS.read();
              if(bufferCount==130)break;
              delay(2);
        }
       buffer[bufferCount]='\0';   // filling the buffer with data
       
      //  Serial.println(buffer);
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
      

      delay(2000);
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
