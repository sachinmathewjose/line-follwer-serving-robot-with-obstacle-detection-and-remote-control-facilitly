#include <IRremote.h> 

#define white HIGH

#define totalTable_no 5      //no of tables
///////////////////////////////////////////pin set up ////////////////////////////////////////////////////////
/*#define lcd_1 A5
#define lcd_2 A
#define lcd_3 A
#define lcd_3 A
#define lcd_3 A
#define lcd_3 A
*/

#define bottonPin A0
#define remote_pin 13  
#define lmotor_1 11
#define lmotor_2 10
#define rmotor_1 9
#define rmotor_2 8
#define triPin 12  
#define echoPin 10
#define line_l1 3
#define line_l2 5
#define line_r1 7
#define line_r2 9
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////ir remote////////////////////////////////////////////////////////////////////////////
IRrecv irrecv(remote_pin); // create instance of 'irrecv'
decode_results results;       //create an instance of decode_results both of which are defined in <IRremote.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////void setup()//////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(9600); // for serial monitor output
  irrecv.enableIRIn(); // Start the receiver
  pinMode(lmotor_1,OUTPUT);
  pinMode(lmotor_2,OUTPUT);
  pinMode(rmotor_1,OUTPUT);
  pinMode(rmotor_2,OUTPUT);
  pinMode(triPin,OUTPUT);
  pinMode(echoPin,INPUT);
  pinMode(bottonPin,INPUT_PULLUP);  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////function motor motion///////////////////////////////////////////////////
#define FF 0                                           // if pin_1 of motor is high robot moves forward
#define FR 1
#define FL 2
#define BB 3
#define BR 4
#define BL 5
#define ST -1
void motorMotion(int dir){
  Serial.println(dir);
  if(dir<=2 && dir>=-1 ){
       digitalWrite(lmotor_2,LOW);
       digitalWrite(rmotor_2,LOW);
       if(dir==0) { digitalWrite(lmotor_1,HIGH);  digitalWrite(rmotor_1,HIGH);  }
       if(dir==1) { digitalWrite(lmotor_1,HIGH);  digitalWrite(rmotor_1,LOW);  }
       if(dir==2) { digitalWrite(lmotor_1,LOW);  digitalWrite(rmotor_1,HIGH);  }
       if(dir==-1) { digitalWrite(lmotor_1,LOW);  digitalWrite(rmotor_1,LOW);  }       
  }  
  if(dir<=5 && dir>=3){
       digitalWrite(lmotor_1,LOW);
       digitalWrite(rmotor_1,LOW);
       if(dir==3) { digitalWrite(lmotor_2,HIGH);  digitalWrite(rmotor_2,HIGH);  }
       if(dir==4) { digitalWrite(lmotor_2,HIGH);  digitalWrite(rmotor_2,LOW);  }
       if(dir==5) { digitalWrite(lmotor_2,LOW);  digitalWrite(rmotor_2,HIGH);  }   
  }  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////remote motion of robotc & remote input recive//////////////////////////////////////////////
int remoteMotion(){
   while(1){
    if (irrecv.decode(&results)) // have we received an IR signal?
  {
    Serial.println(results.value, HEX); // display it on serial monitor in hexadecimal
    switch (results.value){
     case 0x1fed827: motorMotion(FF); 
              break;
      case 0x1fef00f: motorMotion(BB);
              break;
       case 0x1fe50af:motorMotion(FL);                
              break;
       case 0x1fef807:motorMotion(FR);   
              break;
       case 0x1fe00ff:motorMotion(BL);   
              break;
       case 0x1fe9867:motorMotion(BR);   
              break;
       case 0x1fe58a7: motorMotion(ST); return 0;  
              break;
       case 0:
       case 0xffffffff:   
              break;
       default:motorMotion(ST);
              break;
    }
    irrecv.resume(); // receive the next value
  }
  }   return 0;
}


long int remoteRecive(){
  irrecv.resume();
  while(1){
   if(irrecv.decode(&results)){
     if( results.value!=0 ) 
       return results.value; 
     else irrecv.resume();
   }
  }
}
////////////////////////////line motion of robot////////////////////////////////////////////////////////////////
void lineMotion(){     //get out of this function when no line is detect or when a brak point is detected
  boolean l1,r1,l2,r2;
  while(1){ 
   distance();    
   l1=digitalRead(line_l1);
   l2=digitalRead(line_l2);
   r1=digitalRead(line_r1);
   r2=digitalRead(line_r2);
  if(r2==white || l2==white)
       break;
  else
    {
      if(r1==white || l1==white)
         motorMotion(FF);
      else if(r1!=white)
            motorMotion(FR);
            else motorMotion(FL);
    } 
 }
 while(1){    
   distance();   
   l1=digitalRead(line_l1);
   l2=digitalRead(line_l2);
   r1=digitalRead(line_r1);
   r2=digitalRead(line_r2);
  if(r2!=white || l2!=white)
      { motorMotion(ST); break;}
  else
    {
     if(r1==white || l1==white)
         motorMotion(FF);
      else if(r1!=white)
            motorMotion(FR);
            else motorMotion(FL); 
    } 
 }
  
 }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////distance measure///////////////////////////////////////////////////////////////////
void distance(){
  long duration, distance;
   checkDistance:
   digitalWrite(triPin, LOW);  
   delayMicroseconds(2); 
   digitalWrite(triPin, HIGH);
   delayMicroseconds(10); 
   digitalWrite(triPin, LOW);
   duration = pulseIn(echoPin, HIGH);
   distance = (duration/2) / 29.1;
   if (distance<15){
     motorMotion(ST);
     goto checkDistance;   //sounds alarm when this happens
  }  
                          //turn off the alarm
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////void loop()/////////////////////////////////////////////////////////////////////////
void loop()
{
  remoteMotion();
  lineMotion();
  long int remote;
   while(remote=remoteRecive()){
      if(remote==0x1fe807f) break;
      if(remote==0x1fec03f) lineMotion();      
   }
  //game starts here
  while(1){
       remote=remoteRecive();  
   int mov_no,presentTable,preTable_no;  
   switch(remote){
      case 0x1fe50af: mov_no=1; 
                break;
      case 0x1fed827: mov_no=2;
                break;
      case 0x1fef807: mov_no=3;
                break;
      case 0x1fe30cf: mov_no=4;
                break;
      default: mov_no=0;
                break; 
   }
   presentTable=mov_no;
nextTable:
   for(int i=0;i<mov_no;i++){
   lineMotion();
   }
   irrecv.resume();
   boolean flag=LOW;
   while(1){ 
      if( irrecv.decode(&results) ){
         if(results.value==0) 
              irrecv.resume();
         else{flag=HIGH; 
              remote=results.value; 
              switch(remote){                             //here mov_no is actually next table no.
                     case 0x1fe50af: mov_no=1; 
                          break;
                     case 0x1fed827: mov_no=2;
                           break;
                     case 0x1fef807: mov_no=3;
                           break;
                     case 0x1fe30cf: mov_no=4;
                           break;
                     default: mov_no=0;
                           break; 
                     }
              preTable_no=presentTable;
              presentTable=mov_no;
              mov_no=mov_no-preTable_no;
              if(mov_no<0) mov_no=totalTable_no+mov_no;
              break;}  
      }
      if(!(digitalRead(bottonPin))){
         break;                     
       }
   }
  if(flag)
     goto nextTable;
  else
    { for(int i=0;i<(totalTable_no-presentTable);i++) lineMotion();}
           
  }  
}



