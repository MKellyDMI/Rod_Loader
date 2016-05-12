

// Rod loading sequence

byte directionPin = 9; //Control Direction on pin 9
byte stepPin = 8;  // Send steps on pin 8

byte EndstopPin = 10;  // Define endstop 'activation' input to pin 2 for interupt
byte RodSensePin = 11;  // Rod check step pin point
byte ResumePin = 7;  // Pin to send a signal to the Rambo
byte BuzzerPin = 13;  // Pin to output signal to a buzzer or LED
byte RestartPin = 12; // Pin to resume after an error

//Status LED's
byte WaitingPin = 6; // Green
byte ActivePin = 5; // Yellow
byte ErrorPin = 4; // Red

boolean EndstopTrigger = false; 
boolean RodSenseTrigger = false;

unsigned long StepsPerRev = 3200; // microsteps
unsigned long Attempts = 9; // Number of attempts to make before erroring out

int y; // Counter for error signals

void setup() {

  pinMode(directionPin, OUTPUT);  // Enable output pins
  pinMode(stepPin, OUTPUT);
  pinMode(ResumePin, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);
  pinMode(WaitingPin, OUTPUT);
  pinMode(ActivePin, OUTPUT);
  pinMode(ErrorPin, OUTPUT);
  
  pinMode(EndstopPin, INPUT_PULLUP);  // Enable read input pins
  pinMode(RodSensePin, INPUT_PULLUP);
  pinMode(RestartPin, INPUT_PULLUP); 
  
  digitalWrite(directionPin, HIGH); //Pull dir pin HIGH to move motor counter clockwise

  y = 1;
  
}

void loop() {

  readEndstop(); // Read endstop until it's triggered
  runSequence(); // Activate loading sequence
  
}


void readEndstop() {  //Routine to check that an endstop has been sensed

  EndstopTrigger= false;
   
  digitalWrite(WaitingPin, HIGH);
  digitalWrite(ErrorPin, LOW);
  digitalWrite(ActivePin, LOW);
    
  if (digitalRead(EndstopPin) == HIGH) {
    
    EndstopTrigger= true;
    
  }
  
  else if(digitalRead(EndstopPin) == LOW) { 
    
    delay(100); //Wait 100 ms to not burn out processor
    readEndstop(); //Run this routine till endstop is detected
    
  }

}


void runSequence() {
  
  digitalWrite(WaitingPin, LOW);
  digitalWrite(ErrorPin, LOW);
  digitalWrite(ActivePin, HIGH); 
  
  
  if (digitalRead(RodSensePin) == HIGH){// && RodSenseTrigger == false) { // Check for continuity across the sensor, HIGH = Blocked by rod 
    
    digitalWrite(WaitingPin, HIGH);
    digitalWrite(ErrorPin, LOW);
    digitalWrite(ActivePin, HIGH); 
    
    MediaLoad();
    y= 1;
    SendResume();
    RodSenseTrigger = false;

  }
  
 /* else if (RodSenseTrigger == true){ 
    
    y= 1;
    SendResume();
    RodSenseTrigger = false;
    
  }*/
  
  else if (digitalRead(RodSensePin) == LOW && RodSenseTrigger == false) { //  If the rod is not in position and the 
    
    if (y < Attempts){
      
      digitalWrite(WaitingPin, LOW);
      digitalWrite(ErrorPin, HIGH);
      digitalWrite(ActivePin, HIGH);
       
      MediaLoad();
      y++; 
      delay(1000);
      runSequence();
          
    }
    
    else if(y== Attempts){
        
    errorOut();
    
    } 
  }
  
}

void MediaLoad() {  // Check 10x per revolution to see if a rod has passed the sensor
  
  RodSenseTrigger = false;
  
  
  int z;   
  for(z=1; z<11; z++){
    
    int x;
    
    for(x= 1; x<StepsPerRev/10; x++)
    {
      digitalWrite(stepPin,HIGH); //Trigger 1 step
      delay(1);
      digitalWrite(stepPin,LOW);
      delay(1);
    } 
    if(digitalRead(RodSensePin) == HIGH || RodSenseTrigger == true){
      RodSenseTrigger = true;
    }
    else if(digitalRead(RodSensePin) == LOW && RodSenseTrigger == false){
      RodSenseTrigger = false;
    }
    
  }
  delay(500); // Wait to make sure movement finishes
}


void SendResume() { // Simulate a button press 
  
  digitalWrite(ResumePin, HIGH); //Toggle pin state
  delay(1000); 
  digitalWrite(ResumePin, LOW); //Toggle pin state
  delay(1000);
  readEndstop();
}


void errorOut() {  // Looks for a restart button to be pressed, if it has not been pressed, sends out a tone from a buzzer

  digitalWrite(WaitingPin, LOW);
  digitalWrite(ActivePin, LOW); 
  digitalWrite(ErrorPin, HIGH);

   
  if(digitalRead(RestartPin) == LOW){
  
  digitalWrite(WaitingPin, HIGH);
  digitalWrite(ActivePin, LOW); 
  digitalWrite(ErrorPin, HIGH);
  delay(100);
  
  y=1; // Reset counter to try again
  runSequence();
 
    
  }
  
  else if(digitalRead(RestartPin) == HIGH){

   digitalWrite(ErrorPin, LOW);
   delay(500);
   digitalWrite(ErrorPin, HIGH);
   delay(500);
   digitalWrite(ErrorPin, LOW);
   delay(500);
   digitalWrite(ErrorPin, HIGH);
   delay(500);
   
   // tone(BuzzerPin,3000,1000);
   // delay(1000);
    errorOut();
  }
}

