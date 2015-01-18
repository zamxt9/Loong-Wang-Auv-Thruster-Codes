
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>

// *********Configuration Block**************

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
int KS = 2; //kill switch pin no.

//Ball Drop 

int BD = 3;

// *******************************************

ros::NodeHandle  nh;

int toDAC(int DACinput)
{
  int tmp = 0;
  tmp = (DACinput*127)/100;
  if(DACinput >= 0) tmp|= 1 << 7;  
  else tmp = 127 + tmp ;

  return tmp;
  
}

void dataout(int data,int addrs)
{
  
  if(data > 100 || data < -100) return;

  digitalWrite(WR, HIGH);   
  delay(1);               
  digitalWrite(LR, HIGH);
  delay(1);
  
  data = toDAC(data);
 
  digitalWrite(D0, (data >> 0) & 1);   //LSB
  digitalWrite(D1, (data >> 1) & 1);   
  digitalWrite(D2, (data >> 2) & 1);  
  digitalWrite(D3, (data >> 3) & 1);   
  digitalWrite(D4, (data >> 4) & 1);   
  digitalWrite(D5, (data >> 5) & 1);   
  digitalWrite(D6, (data >> 6) & 1);   
  digitalWrite(D7, (data >> 7) & 1);   //MSB

  digitalWrite(AD0, (addrs >> 0) & 1);
  digitalWrite(AD1, (addrs >> 1) & 1);

  digitalWrite(WR, LOW);   
  delay(1);               
  digitalWrite(WR, HIGH);
  delay(10);

  digitalWrite(LR, LOW);   
  delay(1);
  digitalWrite(LR, HIGH);
  delay(2);  
}


void horz_l( const std_msgs::Int16& cmd_msg){
   dataout(cmd_msg.data,HL_ADDS);  
}

void horz_r( const std_msgs::Int16& cmd_msg){
   dataout(cmd_msg.data,HR_ADDS);     
}

void vert_l( const std_msgs::Int16& cmd_msg){
   dataout(cmd_msg.data,VL_ADDS);     
}

void vert_r( const std_msgs::Int16& cmd_msg){
   dataout(cmd_msg.data,VR_ADDS);     
}

void balldrop( const std_msgs::Int16& cmd_msg){
   if(cmd_msg.data == 1) digitalWrite(BD, HIGH);
    else digitalWrite(BD, LOW);   
}

void reset_all(void)
{
   for (int i = 0 ; i < 4 ; i++) dataout(0,i);
}

ros::Subscriber<std_msgs::Int16> sub1("thrust_hl", horz_l);
ros::Subscriber<std_msgs::Int16> sub2("thrust_hr", horz_r);
ros::Subscriber<std_msgs::Int16> sub3("thrust_vl", vert_l);
ros::Subscriber<std_msgs::Int16> sub4("thrust_vr", vert_r);
ros::Subscriber<std_msgs::Int16> sub5("ball_drop", balldrop);

std_msgs::String str_msg;
ros::Publisher chatter("chatter", &str_msg);

char hello[13] = "hello world!";

void setup()
{
  // initialize the digital pin as an output.
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
  pinMode(BD, OUTPUT);
  pinMode(KS, INPUT_PULLUP);
  nh.initNode();
  //nh.advertise(chatter);
  nh.subscribe(sub1);
  nh.subscribe(sub2);
  nh.subscribe(sub3);
  nh.subscribe(sub4);
  nh.subscribe(sub5);
  reset_all();

}

void loop()
{
  //str_msg.data = hello;
  if(KS==0) 
  {
    while(KS==0)
    {
      reset_all();
    }
  }
  nh.spinOnce();
  delay(1);
}
