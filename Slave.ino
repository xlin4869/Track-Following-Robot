//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//  Slave_ELEC1601_Student_2019_v3                                                                           //
//  The Instructor version of this code is identical to this version EXCEPT it also sets PIN codes           //
//  20191008 Peter Jones                                                                                     //
//                                                                                                           //
//  Bi-directional passing of serial inputs via Bluetooth                                                    //
//  Note: the void loop() contents differ from "capitalise and return" code                                  //
//                                                                                                           //
//  This version was initially based on the 2011 Steve Chang code but has been substantially revised         //
//  and heavily documented throughout.                                                                       //
//                                                                                                           //
//  20190927 Ross Hutton                                                                                     //
//  Identified that opening the Arduino IDE Serial Monitor asserts a DTR signal which resets the Arduino,    //
//  causing it to re-execute the full connection setup routine. If this reset happens on the Slave system,   //
//  re-running the setup routine appears to drop the connection. The Master is unaware of this loss and      //
//  makes no attempt to re-connect. Code has been added to check if the Bluetooth connection remains         //
//  established and, if so, the setup process is bypassed.                                                   //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

#include <SoftwareSerial.h>   //Software Serial Port
#include <Servo.h>
#define RxD 7
#define TxD 6
#define ConnStatus A1


#define DEBUG_ENABLED  1

// ##################################################################################
// ### EDIT THE LINES BELOW TO MATCH YOUR SHIELD NUMBER AND CONNECTION PIN OPTION ###
// ##################################################################################

int shieldPairNumber = 4;

// CAUTION: If ConnStatusSupported = true you MUST NOT use pin A1 otherwise "random" reboots will occur
// CAUTION: If ConnStatusSupported = true you MUST set the PIO[1] switch to A1 (not NC)

boolean ConnStatusSupported = true;   // Set to "true" when digital connection status is available on Arduino pin

// #######################################################

// The following two string variable are used to simplify adaptation of code to different shield pairs

String slaveNameCmd = "\r\n+STNA=Slave";   // This is concatenated with shieldPairNumber later

SoftwareSerial blueToothSerial(RxD,TxD);
Servo servoLeft;
Servo servoRight;
int VRx=A0;
int VRy=A1;
int SW=2;
int x=0;
int y=0;
int z=0;
char steps[500];
int count=0;
int i=0;
int countBall=0;

void setup()
{
    Serial.begin(9600);
    blueToothSerial.begin(38400);                    // Set Bluetooth module to default baud rate 38400
      
  //infrared inputs pin 9 is left ir LED, pin2 is right ir LED, 5 is for ball detection ir LED

  pinMode(9, OUTPUT);
  pinMode(2, OUTPUT);
  
  //pin 10 is left detector, 3 is right detector, 5 is for ball detection sensor

  pinMode(10, INPUT); 
  pinMode(3, INPUT);
  
  //servo outputs
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
    pinMode(RxD, INPUT);
    pinMode(TxD, OUTPUT);
    pinMode(ConnStatus, INPUT);
    
    pinMode(VRx, INPUT);
    pinMode(VRy, INPUT);
    pinMode(SW, INPUT_PULLUP);
    servoLeft.attach(13);
    servoRight.attach(12);


    //  Check whether Master and Slave are already connected by polling the ConnStatus pin (A1 on SeeedStudio v1 shield)
    //  This prevents running the full connection setup routine if not necessary.

    if(ConnStatusSupported) Serial.println("Checking Slave-Master connection status.");

    if(ConnStatusSupported && digitalRead(ConnStatus)==1)
    {
        Serial.println("Already connected to Master - remove USB cable if reboot of Master Bluetooth required.");
    }
    else
    {
        Serial.println("Not connected to Master.");
        
        setupBlueToothConnection();   // Set up the local (slave) Bluetooth module

        delay(1000);                  // Wait one second and flush the serial buffers
        Serial.flush();
        blueToothSerial.flush();
    }
}

  int irdetect(int irLedPin, int irReceiverPin, long frequency)
{
  tone(irLedPin, frequency, 8);              // IRLED 38 kHz for at least 1 ms
  delay(1);                                  // Wait 1 ms
  int ir = digitalRead(irReceiverPin);       // IR receiver -> ir variable
  delay(1);                                  // Down time before recheck
  return ir;                                 // Return 1 no detect, 0 detect
}  
void followLine(){


  int irLeft = irdetect(9, 10, 38000);       
  int irRight = irdetect(2, 3, 38000);

  Serial.println("irLeft:");
  Serial.println(irLeft);
  Serial.println("irRight:");
  Serial.println(irRight);

   if (irLeft  == 1 && irRight == 1){
    servoLeft.writeMicroseconds(1500);   
    servoRight.writeMicroseconds(1500);
    delay(100);
    Serial.println("stop");
    blueToothSerial.print("ball was found");
  }  


  else if (irLeft  == 0 && irRight == 0){
    servoLeft.writeMicroseconds(1700);   
    servoRight.writeMicroseconds(1300);
    delay(100);
    servoLeft.writeMicroseconds(1500);   
    servoRight.writeMicroseconds(1500);

    
  }  
  
  else if (irLeft == 0 && irRight == 1) {
    servoLeft.writeMicroseconds(1700);   
    servoRight.writeMicroseconds(1700);
    delay(200);

    servoLeft.writeMicroseconds(1500);   
    servoRight.writeMicroseconds(1500);
    
    
  }
  
  else if (irRight == 0 && irLeft == 1) {
    servoLeft.writeMicroseconds(1300);   
    servoRight.writeMicroseconds(1300);
    delay(200);

    servoLeft.writeMicroseconds(1500);   
    servoRight.writeMicroseconds(1500);
   
  }
  


    

}

void loop()
{
    char recvChar;

        if(blueToothSerial.available())   // Check if there's any data sent from the remote Bluetooth shield
        {
          int index=0;
            recvChar = blueToothSerial.read();
            Serial.println(recvChar);
            if(recvChar=='w'){
            servoLeft.writeMicroseconds(1700);
            servoRight.writeMicroseconds(1300); 
            delay(500); 
            steps[count]='w';
            count+=1;
            }
            if(recvChar=='s'){
            servoLeft.writeMicroseconds(1300);
            servoRight.writeMicroseconds(1700);
            delay(500); 
            servoLeft.writeMicroseconds(1500);
            servoRight.writeMicroseconds(1500);
            steps[count]='s';
            count+=1;
            }
            if(recvChar=='a'){
            servoLeft.writeMicroseconds(1300);
            servoRight.writeMicroseconds(1300);
            delay(500); 
            servoLeft.writeMicroseconds(1700);
            servoRight.writeMicroseconds(1300);
            steps[count]='a';
            count+=1;
            }
            if(recvChar=='d'){
            servoLeft.writeMicroseconds(1700);
            servoRight.writeMicroseconds(1700);
            delay(500); 
            servoLeft.writeMicroseconds(1700);
            servoRight.writeMicroseconds(1300);
            steps[count]='d';
            count+=1;
            }
            if (recvChar=='z'){
            servoLeft.writeMicroseconds(1500);
            servoRight.writeMicroseconds(1500);
            steps[count]='z';
            count+=1;
            }

            if (recvChar='q'){
              while(1){
                followLine();
              }
            }
            
            if (recvChar= 'r'){
              for (i;i<500;i++){
                if(steps[i]=='w'){
                  servoLeft.writeMicroseconds(1700);
            servoRight.writeMicroseconds(1300); 
                  
               }
              }
            }
               if(steps[i]=='s'){
                  servoLeft.writeMicroseconds(1300);
            servoRight.writeMicroseconds(1700);
            delay(1000); 
            servoLeft.writeMicroseconds(1500);
            servoRight.writeMicroseconds(1500);
                  
               }
               if(steps[i]=='a'){
                  servoLeft.writeMicroseconds(1300);
            servoRight.writeMicroseconds(1300);
            delay(300); 
            servoLeft.writeMicroseconds(1700);
            servoRight.writeMicroseconds(1300);
                  
               }
               if(steps[i]=='d'){
                  servoLeft.writeMicroseconds(1700);
            servoRight.writeMicroseconds(1700);
            delay(300); 
            servoLeft.writeMicroseconds(1700);
            servoRight.writeMicroseconds(1300);
                  
               }
               if(steps[i]=='z'){
                  servoLeft.writeMicroseconds(1500);
            servoRight.writeMicroseconds(1500);
                  
               }
               if(steps[i]=='q'){
                  while(1){
                followLine();
              }
                  
              

   
  }
        }

        if(Serial.available())      {      // Check if there's any data sent from the local serial terminal. You can add the other applications here.
        
            recvChar  = Serial.read();
            Serial.print(recvChar);
            blueToothSerial.print(recvChar);
            
        }

  
}


void setupBlueToothConnection()
{
    Serial.println("Setting up the local (slave) Bluetooth module.");

    slaveNameCmd += shieldPairNumber;
    slaveNameCmd += "\r\n";

    blueToothSerial.print("\r\n+STWMOD=0\r\n");      // Set the Bluetooth to work in slave mode
    blueToothSerial.print(slaveNameCmd);             // Set the Bluetooth name using slaveNameCmd
    blueToothSerial.print("\r\n+STAUTO=0\r\n");      // Auto-connection should be forbidden here
    blueToothSerial.print("\r\n+STOAUT=1\r\n");      // Permit paired device to connect me
    
    //  print() sets up a transmit/outgoing buffer for the string which is then transmitted via interrupts one character at a time.
    //  This allows the program to keep running, with the transmitting happening in the background.
    //  Serial.flush() does not empty this buffer, instead it pauses the program until all Serial.print()ing is done.
    //  This is useful if there is critical timing mixed in with Serial.print()s.
    //  To clear an "incoming" serial buffer, use while(Serial.available()){Serial.read();}

    blueToothSerial.flush();
    delay(2000);                                     // This delay is required

    blueToothSerial.print("\r\n+INQ=1\r\n");         // Make the slave Bluetooth inquirable
    
    blueToothSerial.flush();
    delay(2000);                                     // This delay is required
    
    Serial.println("The slave bluetooth is inquirable!");
}
