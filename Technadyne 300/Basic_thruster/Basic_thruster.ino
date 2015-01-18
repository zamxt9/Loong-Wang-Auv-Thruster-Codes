
 
#define MAX_STRING_LEN 200
#define MAX_TIME_LIMIT 60
//                    A1  A0   DAC
#define HL_ADDS 0  // 0    0    A
#define HR_ADDS 1  // 0    1    B
#define VL_ADDS 2  // 1    0    C
#define VR_ADDS 3  // 1    1    D

// Delcare pins for inputs
int D0 = 6;
int D1 = 7;
int D2 = 8;
int D3 = 9;
int D4 = 10;
int D5 = 11;
int D6 = 12;
int D7 = 13;
int AD0 = 53;
int AD1 = 52;
int WR = 51;
int LR = 48;



void setup() {
  
   // initialize the digital pin as an output.
  //Horizontal Right
  pinMode(D0, OUTPUT);   
  pinMode(D1, OUTPUT); 
  pinMode(D2, OUTPUT); 
  pinMode(D3, OUTPUT); 
  pinMode(D4, OUTPUT); 
  pinMode(D5, OUTPUT); 
  pinMode(D6, OUTPUT);   
  pinMode(D7, OUTPUT); 
  pinMode(AD0, OUTPUT); 
  pinMode(AD1, OUTPUT); 
  pinMode(WR, OUTPUT); 
  pinMode(LR, OUTPUT);
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


int toDAC(int DACinput)
{
  int tmp = 0;
  tmp = (DACinput*127)/100;
  if(DACinput >= 0) tmp|= 1 << 7;  
  else tmp = 127 + tmp ;

  return tmp;
  
}

void dataout(int data)
{
  digitalWrite(D0, (data >> 0) & 1);   //LSB
  digitalWrite(D1, (data >> 1) & 1);   
  digitalWrite(D2, (data >> 2) & 1);  
  digitalWrite(D3, (data >> 3) & 1);   
  digitalWrite(D4, (data >> 4) & 1);   
  digitalWrite(D5, (data >> 5) & 1);   
  digitalWrite(D6, (data >> 6) & 1);   
  digitalWrite(D7, (data >> 7) & 1);   //MSB
}

void setadds(int addrs){

  digitalWrite(AD0, (addrs >> 0) & 1);
  digitalWrite(AD1, (addrs >> 1) & 1);

  digitalWrite(WR, LOW);   
  delay(1);               
  digitalWrite(WR, HIGH);
  delay(10);  
}

void enableMotors(int inHL, int inHR, int inVL,int inVR,int intime){

  if(inHL > 100 || inHL < -100 || inHR  > 100 || inHR < -100 || inVL > 100 || inVL < -100 || inVR > 100 || inVL < -100 || intime > MAX_TIME_LIMIT || intime < 0)
  {
   Serial.print("\n********Invalid value!!********\n");
    return ; 
  }
  
  digitalWrite(WR, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1);               // wait for a second
  digitalWrite(LR, HIGH);    // turn the LED off by making the voltage LOW
  delay(1);
  
  dataout(toDAC(inHL));
  setadds(HL_ADDS);
 
  dataout(toDAC(inHR));
  setadds(HR_ADDS);

  dataout(toDAC(inVL));
  setadds(VL_ADDS);
  
  dataout(toDAC(inVR));
  setadds(VR_ADDS);
 
  digitalWrite(LR, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(1);               // wait for a second
  digitalWrite(LR, HIGH);    // turn the LED off by making the voltage LOW
  delay(2);
  
  Serial.println("Countdown:");
  for(int i=0; i<intime ; i++) 
  {
  delay(1000);
  Serial.println(intime-i);
  }
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
