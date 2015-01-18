
 
#include <Servo.h>

#define MAX_STRING_LEN 200
#define MAX_TIME_LIMIT 60
//                    A1  A0   DAC
#define HL_ADDS 0  // 0    0    A
#define HR_ADDS 1  // 0    1    B
#define VL_ADDS 2  // 1    0    C
#define VR_ADDS 3  // 1    1    D

// Delcare pins for inputs
  byte HL_pin =  9 ;
  byte HR_pin = 10 ;
  byte VL_pin = 11 ;
  byte VR_pin = 12 ;

  Servo HL_servo ;
  Servo HR_servo ;
  Servo VL_servo ;
  Servo VR_servo ;


void setup() {
  
  // initialize the digital pin as an output.
  //Horizontal Right
  pinMode(HL_pin, OUTPUT);   
  pinMode(HR_pin, OUTPUT); 
  pinMode(VL_pin, OUTPUT); 
  pinMode(VR_pin, OUTPUT); 

  //Servo configuration
  HL_servo.attach(HL_pin);
  HR_servo.attach(HR_pin);
  VL_servo.attach(VL_pin);
  VR_servo.attach(VR_pin);
  
  HL_servo.writeMicroseconds(1500); 
  HR_servo.writeMicroseconds(1500); 
  VL_servo.writeMicroseconds(1500); 
  VR_servo.writeMicroseconds(1500);
  
  delay(500);

  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only 
  
  }
  


}

// Function to count number of occurences of a character
int countChars( char* s, char c )
{
    return *s == '\0'
              ? 0
              : countChars( s + 1, c ) + (*s == c);
}

// Function to return a substring defined by a delimiter at an index
char* subStr (char* str, char *delim, int index) {
   char *act, *sub, *ptr;
   static char copy[MAX_STRING_LEN];
   int i;
   strcpy(copy, str);

   for (i = 1, act = copy; i <= index; i++, act = NULL) {
      sub = strtok_r(act, delim, &ptr);
      if (sub == NULL) break;
   }
   return sub;

}


int toServo(int input)
{
  return (input*4)+1500; 
}

void enableMotors(int inHL, int inHR, int inVL,int inVR,int intime){

  if(inHL > 100 || inHL < -100 || inHR  > 100 || inHR < -100 || inVL > 100 || inVL < -100 || inVR > 100 || inVL < -100 || intime > MAX_TIME_LIMIT || intime < 0)
  {
   Serial.print("\n********Invalid value!!********\n");
    return ; 
  }
  
  HL_servo.writeMicroseconds(toServo(inHL)); 
  HR_servo.writeMicroseconds(toServo(inHR)); 
  VL_servo.writeMicroseconds(toServo(inVL)); 
  VR_servo.writeMicroseconds(toServo(inVR));
  
  Serial.println("Countdown:");
  for(int i=0; i<intime ; i++) 
  {
  delay(1000);
  Serial.println(intime-i);
  }
  
  HL_servo.writeMicroseconds(1500); 
  HR_servo.writeMicroseconds(1500); 
  VL_servo.writeMicroseconds(1500); 
  VR_servo.writeMicroseconds(1500);
}

void loop() {

  int incomingByte = 0;
  char tmpBuff[MAX_STRING_LEN] ="" ;
  int HL,HR,VL,VR,time;
  int instructions = 0;
  char tmpIns[20] = "";
  bool initflag = false;
  
  enableMotors(0,0,0,0,0);
  Serial.println(" \nThruster control output for DAC AD7225 ...\n\n");  

  while(true)
 { 
  Serial.println(">Input: #INS;HL,HR,VL,VR,delay;...\n"); 

    while(Serial.available() == 0);

    Serial.readBytesUntil('\n', tmpBuff, MAX_STRING_LEN);
    Serial.println(tmpBuff);
    if (tmpBuff[0]=='#')
    {    
        instructions = countChars(tmpBuff,';');
        Serial.print("Number of Instructions: ");
        Serial.println(instructions);   
        
        for(int i=0; i < instructions ; i++)
        {      
             strcpy (tmpIns,subStr(tmpBuff, ";", 2+i));
             //Serial.println(tmpIns);
             if(countChars(tmpIns,',')==4)
             {
               HL = atoi(subStr(tmpIns, ",", 1));    
               HR = atoi(subStr(tmpIns, ",", 2));    
               VL = atoi(subStr(tmpIns, ",", 3));    
               VR = atoi(subStr(tmpIns, ",", 4));         
               time = atoi(subStr(tmpIns, ",", 5));    
               enableMotors(HL,HR,VL,VR,time);
             }
             else Serial.println("Incorrect number of parameters!");
        }
        enableMotors(0,0,0,0,0);
      }
      else Serial.println("Invalid Syntax!");
     memset(tmpBuff,0,MAX_STRING_LEN); 

 }
 

}
