#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);float flow; //Liters of passing water volume
#include <SoftwareSerial.h> //Create software serial object to communicate with SIM800L
SoftwareSerial sim800(7, 6);   //SIM800L Tx & Rx is connected to Arduino #8 & #9
String PHONE = ""; 
String phonee="+250786428081";
//For Multiple user
String msg;
#include<EEPROM.h>
#define blueLedPin 4
#define greenLedPin 10
#define whiteLedPin 9

unsigned long pulse_freq;
int Relay=5;
int stock;
float te;
int sum;
float restore;
void Welcome()
{
 u8g.setFont(u8g_font_8x13r);
u8g.setPrintPos(8,10);
u8g.print("  Welcome TO");
u8g.setPrintPos(4,30); 
u8g.print("    Group3");
u8g.setPrintPos(4,50);
u8g.print("     ....."); 
}
   
void draw(float flow)
{
 u8g.setFont(u8g_font_8x13r);
u8g.setPrintPos(8,10);
u8g.print("Smart Water");
u8g.setPrintPos(4,20); 
u8g.print("      meter");
u8g.setPrintPos(4,40);
u8g.print("Stock:"); 
u8g.print(stock);
u8g.setPrintPos(4,55);
u8g.print("Outlet:"); 
u8g.print(flow);
u8g.print("L");
}
void setup()
{
 u8g.firstPage(); 
 do {
    Welcome();
  } while( u8g.nextPage());  
pinMode(2, INPUT);
//pinMode(R,OUTPUT);
pinMode(Relay,OUTPUT);
pinMode(4, OUTPUT);
pinMode(10, OUTPUT);
pinMode(9, OUTPUT);

digitalWrite(Relay,HIGH);
 te=EEPROM.read(1);
if(EEPROM.read(3)>EEPROM.read(0))
{
digitalWrite(Relay,HIGH);
EEPROM.write(0,0);
//EEPROM.write(3,0);
EEPROM.write(1,0);  
} 
else
{
restore=EEPROM.read(3); 
//renew=EEPROM.read(0)-EEPROM.read(1);
EEPROM.write(0,restore);
}
  Serial.begin(9600);
 sim800.begin(9600); 
  Serial.println("Initializing Serial... ");

  sim800.begin(9600);
  Serial.println("Initializing GSM module...");
  delay(16000);

  sim800.print("AT+CMGF=1\r"); //SMS text mode
  delay(1000);  
  sim800.print("AT+CMGDA=DEL ALL\r");
  sim800.println("AT+CNMI=1,2,0,0,0");
  digitalWrite(Relay,LOW);
  attachInterrupt(0, pulse, RISING); // Setup Interrupt

  
}

void loop ()
{
    u8g.firstPage(); 
 do { 
    draw(flow);
  } while( u8g.nextPage());
  //Recharge();
sum=te+flow; 
EEPROM.read(3); 
 stock=EEPROM.read(0)-EEPROM.read(1);
// EEPROM.Clear(0);
 EEPROM.write(3,stock);
    if(EEPROM.read(3)<0.12)
{
digitalWrite(Relay,HIGH);
EEPROM.write(0,0);
EEPROM.write(3,0);
sendsms();
//EEPROM.write(1,0);
}    
    if(EEPROM.read(3)>3.12){
digitalWrite(blueLedPin, HIGH);
    }
    if(EEPROM.read(3)<3.12 && EEPROM.read(3)>0.12){
digitalWrite(greenLedPin, HIGH);
digitalWrite(blueLedPin, LOW);
    }
    if(EEPROM.read(3)<=0.12){
digitalWrite(greenLedPin, LOW);
digitalWrite(blueLedPin, LOW);
digitalWrite(whiteLedPin, HIGH);
    }

    flow = .00225 * pulse_freq;
    Serial.print(flow, 2);
    Serial.println("L");
    delay(500);
    EEPROM.write(1,flow);
    //EEPROM.write(0,stock);        
//     if(flow>stock)
// {
// digitalWrite(R,HIGH);
// //stock=stock-flow;  
// }  
  while (sim800.available())
  {
    parseData(sim800.readString());//Calls the parseData function to parse SMS
  }
  //doAction();//Does necessary action according to SMS message
  while (Serial.available())
  {
    sim800.println(Serial.readString());
  }
   
}
void pulse () // Interrupt function

{
  pulse_freq++;
//stock=stock-pulse_freq;  
}
void parseData(String buff) {
  Serial.println(buff);

  unsigned int index;

  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index + 2);
  buff.trim();

  if (buff != "OK") {
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();

    buff.remove(0, index + 2);

    //Parse necessary message from SIM800L Serial buffer string
    if (cmd == "+CMT") {
      //get newly arrived memory location and store it in temp
      index = buff.lastIndexOf(0x0D);//Looking for position of CR(Carriage Return)
      msg = buff.substring(index + 2, buff.length());//Writes message to variable "msg"
      msg.toLowerCase();//Whole message gets changed to lower case
      Serial.println(msg);

      index = buff.indexOf(0x22);//Looking for position of first double quotes-> "
      PHONE = buff.substring(index + 1, index + 14); //Writes phone number to variable "PHONE"
      Serial.println(PHONE);
if(PHONE=="+250786428081")      
{

EEPROM.write(0,(msg.toInt()+EEPROM.read(0)));
EEPROM.write(3,0);
EEPROM.write(1,0);
sim800.print("AT+CMGDA=DEL ALL\r");
    }
  }
}
}
void sendsms()
{
  sim800.println("AT");
     delay(1000);
         sim800.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
 Serial.println ("Set SMS Number");
  sim800.println("AT+CMGS=\"" + phonee + "\"\r"); //Mobile phone number to send message
  delay(1000);
  String SMS = "you need to Refill your water" ;
 sim800.println(SMS);
  delay(100);
  sim800.println((char)26);// ASCII code of CTRL+Z
  delay(1000); 
}
