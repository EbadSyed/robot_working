#define FWD 1
#define REV -1
#define MIN_SPEED 20
#define MAX_SPEED 165

#include <IRLib.h>
const int ir = 8;
IRrecv My_Receiver(ir);
IRdecode My_Decoder;

const int motor1a = 11;
const int motor1b = 10; //PWM

const int motor2a = 12; 
const int motor2b = 9; //PWM

const int buzzer = 3;
const int lights = A3;

unsigned long IR_previousMillis = 0;        // will store last time LED was updated
unsigned long line_previousMillis = 0;        // will store last time LED was updated
const long IR_interval = 200;           // interval at which to blink (milliseconds)
const long line_interval = 5;           // interval at which to blink (milliseconds)

void setup() {
  Serial.begin(9600);
  Serial.println("STARTING");
  My_Receiver.enableIRIn();
  delay(1000);
  pinMode(motor1a, OUTPUT);
  pinMode(motor1b, OUTPUT);
  pinMode(motor2a, OUTPUT);
  pinMode(motor2b, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(lights, OUTPUT);
  
  digitalWrite(lights, LOW);
  delay(100);
  digitalWrite(lights, HIGH);
  delay(100);
  digitalWrite(lights, LOW);
  delay(100);
  digitalWrite(lights, HIGH);
  delay(100);  
}

int follow=0;
void loop() {
  unsigned long currentMillis = millis();

//  if ( follow==1 && (currentMillis - line_previousMillis >= line_interval)) {
  if ( follow==1 ){
  // save the last time you blinked the LED
    line_previousMillis = currentMillis;
    int center = analogRead(A2);
    int left   = analogRead(A6);
    int right  = analogRead(A1);
    
    left  = (left >400?100:(left <300?0:map(left, 300,400,0,100)));
    right = (right>250?100:(right<160?0:map(right,160,250,0,100)));
    center = (center>370?100:(center<270?0:map(center,270,370,0,100)));
    //left = 100-left;
    //right= 100-right;
    Serial.print(left);
    Serial.print(", ");
    Serial.print(center);
    Serial.print(", ");
    Serial.println(right);
    if(left<=10 && right<=10 && center<=50){
      leftMotor(FWD, 100);    
      rightMotor(FWD, 100);    
    }else if(left>=90 && right>=90){
      leftMotor(FWD, 0);    
      rightMotor(FWD, 0);        
    }else{
      if(center<=30){
//      if(left>10 && left<30)
        rightMotor(FWD,right*1.25);  
//      if(right>10 && right<30)
        leftMotor(FWD,left*1.25);  
    }
    }
  }

  if (currentMillis - IR_previousMillis >= IR_interval) {
    // save the last time you blinked the LED
    IR_previousMillis = currentMillis;
    if (My_Receiver.GetResults(&My_Decoder)) {
        My_Decoder.decode();      //Decode the data
        switch(My_Decoder.value){
          case 0x843501FE: // PAUSE
            turnLeft(100,50);
            break;  
          case 0x843502FD: // FORWARD
            forward(100);
            break;  
          case 0x843506F9: // REVERSE
            reverse(50);
            break;  
          case 0x843507F8: // RIGHT
            rotateRight(75);
            break;  
          case 0x843505FA: // LEFT
            rotateLeft(100);
            break;
          case 0x843504FB: // MENU
            horn(random(100,255));
            break;  
          case 0x843508F7: // MOUSE
            if(digitalRead(lights))
              headLights(HIGH);
            else
              headLights(LOW);
            break;
          case 0x843503FC: // STOP
            turnRight(100,80);
            break;
          case 0x843518E7: // MAGNIFYING Glass
            digitalWrite(lights, LOW);
            follow=1;
            break;
          default:
            digitalWrite(lights, HIGH);
            follow=0;
            halt();
        }
        //My_Decoder.DumpResults(); //Show the results on serial monitor
        My_Receiver.resume();     //Restart the receiver
    }
  }


}

void calibrate(){
   
}

void horn(int val){
  analogWrite(buzzer, val);
}

void headLights(int val){
  // inverse LOGIC
  if(val==LOW){
    digitalWrite(lights, HIGH);  
  }else if(val==HIGH){
    digitalWrite(lights, LOW);  
  }
}

void forward(int spd){
  rightMotor(FWD, spd);
  leftMotor(FWD, spd);
}

void reverse(int spd){
  rightMotor(REV, spd);
  leftMotor(REV, spd);
}

void rotateRight(int spd){
  leftMotor(FWD, spd);
  rightMotor(REV, spd);
}

void rotateLeft(int spd){
  leftMotor(REV, spd);
  rightMotor(FWD, spd);
}

void turnRight(int spd, int turn){
  leftMotor(FWD, spd);
  rightMotor(FWD, spd-turn);
}

void turnLeft(int spd, int turn){
  leftMotor(FWD, spd-turn);
  rightMotor(FWD, spd);
}

void halt(){
  leftMotor(FWD, 0);
  rightMotor(FWD, 0);
}

void rightMotor(int dir, int spd){
  int val = spd==0 ? 0: map(spd,0,100, MIN_SPEED ,MAX_SPEED);
  if(dir==FWD){
    digitalWrite(motor2a, HIGH);
    analogWrite(motor2b, 255-val);
  }else if(dir==REV){
    digitalWrite(motor2a, LOW);  
    analogWrite(motor2b, val);    
  }
}

void leftMotor(int dir, int spd){
  int val = spd==0 ? 0: map(spd,0,100, MIN_SPEED ,MAX_SPEED);
  if(dir==FWD){
    digitalWrite(motor1a, HIGH);  
    analogWrite(motor1b, 255-val);    
  }else if(dir==REV){
    digitalWrite(motor1a, LOW);
    analogWrite(motor1b, val);
  }
}
