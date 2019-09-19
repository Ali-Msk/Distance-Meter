/* Distance Meter
 * insert file name here.ino
 * Detects distance and displays on LCD screen, also has hold and review functions
 * Version: 4/date/2019
 * Authors: Ali & Victor
 */

// Adding required libraries
#include <LiquidCrystal.h>

bool save = false; // Save button
bool metric = true; // Unit values
int button = 0; // Button press
int mode = 0; // 0 = Measure | 1 = Hold | 2 = Review
bool hold = false; // uses this to remember if it has measured the distance on hold already 
const int trigPin = 23; // Trigger pin
const int echoPin = 22; // Echo pin
String buttonName; // Button press
double cmDistance, inDistance;
double holdValue;
int period = 500; //500ms for the refresh rate (delay)
unsigned long time_now = 0; //stores time of the previous iteration



// Array for saved measurements 
double inSaves [20];
double cmSaves [20];
int viewIndex;//the index for viewing the values in review mode

int index = 0; // stores the index of the array when viewing 

// Initialize LCD Pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  Serial.begin (9600);

  //sets up pins 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.begin(16, 2);


  //welcome screen
  //because of the lcd refresh style this makes a 
  //nice looking welcome screen
  for (int i = 0; i<= 400;i++){
    lcd.clear();
    lcd.print("WELCOME");
    lcd.setCursor(0,1);
    lcd.print("WELCOME");
    delay(5);
  }
  
  lcd.clear();
}

void loop() {
 
  //-----Check which mode the detector is currently at |default is measure mode|-----

  if(mode == 0){ // Measure Mode
 
   if(millis() > time_now + period){ //delays the program without interfering with the button inputs
     time_now = millis(); //updates time elapsed
      lcd.clear();
      // Display mode
      lcd.setCursor(0,1);
      lcd.print("Measure Mode");
      lcd.setCursor(0,0);

      // Display measurement
      cmDistance = readDistance(true);
      inDistance = cmDistance * 0.393701;
      
      if(metric){
        lcd.print(cmDistance);
        lcd.print(" cm");
      }else {
        lcd.print(inDistance);
        lcd.print(" in");
      }
      Serial.println(readDistance(metric));
    }
  }
  else if(mode == 1){ // Hold Mode
    
    if(!hold){//if not already measured (will not measure again after this iteration)   
      hold = true;
      holdValue = readDistance(metric);// stores hold value
      lcd.print(holdValue);
      if(metric){
        lcd.print(" cm");
      }else {
        lcd.print(" in");
      }
    }
  }
  else{ // Review Mod

      //lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Review Mode");
      lcd.setCursor(0,0);

    
    if(metric){
                                                           
      lcd.print(viewIndex + 1);
      lcd.print("/");
      lcd.print(index);
      lcd.print(": ");
      lcd.print(cmSaves[viewIndex]);
      lcd.print(" cm             ");//the empty space ensures values with more digits will be erased 

      if(index != 0){// if there are saved valuess
        if(buttonName == "left"){ // left is pressed 
          if(viewIndex != 0){viewIndex = viewIndex - 1;}
          else{viewIndex = index - 1;} // loops the index from 0 to max value
        }
        else if (buttonName == "right"){ // right is pressed                            
          if(viewIndex != index - 1){viewIndex = viewIndex + 1;}                        
          else{viewIndex = 0;}                                                          
        }                                                                             
      }
      else{ // if no saves
        lcd.clear(); // clears screen 
        lcd.print("There are no");
        lcd.setCursor(0,1);
        lcd.print("saved values");           
        lcd.setCursor(0,0);
        delay(2000); // delays for user to read the msg     
        lcd.clear();                             
        mode = 0; // Switch back to measure mode
        lcd.setCursor(0,1);
        lcd.print("Measure Mode");
        lcd.setCursor(0,0);
      }
    }else { // if not metric 
      
      lcd.print(viewIndex + 1);
      lcd.print("/");
      lcd.print(index);
      lcd.print(": ");
      lcd.print(inSaves[viewIndex]);
      lcd.print(" in                   ");

      if(index != 0){// if there are saved valuess
        if(buttonName == "left"){ // left is pressed 
         
        }
        else if (buttonName == "right"){ // right is pressed                            
          if(viewIndex != index - 1){viewIndex = viewIndex + 1;}                        
          else{viewIndex = 0;}                                                          
        }                                                                               
      }
      else{ // if no saves
        lcd.clear(); // clears screen 
        lcd.print("There are no");
        lcd.setCursor(0,1);
        lcd.print("saved values");           
        lcd.setCursor(0,0);
        delay(2000); // delays for user to read the msg     
        lcd.clear();                             
        mode = 0; // Switch back to measure mode
        lcd.setCursor(0,1);
        lcd.print("Measure Mode");
        lcd.setCursor(0,0);                                             
      }
    }
  }
  //-----Checking for button presses input-----
  
  // Read button press
  buttonName = readButtons(); 
  
  // UP Changes device mode
  if(buttonName == "up") {
    delay(200);
    lcd.clear(); //clears screen
    if(mode == 0){
      mode = 1;
      lcd.setCursor(0,1);
      lcd.print("Hold Mode");
      lcd.setCursor(0,0);

    }else if (mode == 1){
      hold = false;
      mode = 2;
      lcd.setCursor(0,1);
      lcd.print("Review Mode");
      lcd.setCursor(0,0);
    }else{
      mode = 0;
      
    }
  }
  // DOWN saves current value
  else if(buttonName == "down"){
    delay(200);
    // Save measurement if in measure or hold mode
    if(mode != 2){save = true;}
  }
  else if(buttonName == "select"){
    delay(200);
    // If in Imperial, change to Metric
    if(metric == false){metric = true;}
    // If not, change to Imperial
    else{metric = false;}
  }else if(buttonName == "right"){
   delay(200);
    if(mode == 2){
      if(viewIndex != index - 1){
        viewIndex = viewIndex + 1;
      }else{
        viewIndex = 0;
      }    
   }
  }else if(buttonName == "left"){
   
    delay(200);
    if(mode == 2){
     if(viewIndex != 0){
        viewIndex = viewIndex - 1;
      }else{
        viewIndex = index - 1;
      } // loops the index from 0 to max value
   } 
  }

  //saves the distance if requested 
  if (save){
    if(index <=19){//if there is space
      
      //desplays to user
      lcd.clear();
      lcd.print("saved!");
      delay(500);
    
      //if in hold mode will show the value again
      if(mode ==1 ){

        //reprint hold mode  and value
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Hold Mode");
        lcd.setCursor(0,0);

        lcd.print(holdValue);
        if(metric){
          lcd.print(" cm");
        }else {
          lcd.print(" in");
        }
      }

      //saves in both inches and cm
      inSaves[index] = inDistance;
      cmSaves[index] = cmDistance;
      Serial.print("saved ");
      Serial.print(inSaves[index]);
      Serial.print(" ");
      Serial.print(cmSaves[index]);
            index = index + 1;

    }else {//if not tell the user 
      lcd.clear();
      lcd.print("Full Memory");
      delay(2000);
    }
  }

  //resets vars 
  buttonName = "";
  save = false; 

}

/**
 * readButtons 
 * returns a string showing which button was pressed 
 * reads analog input signal and diffrentiates based on range
 */
String readButtons(){
  String buttonNames;
  button = analogRead(0);
  
  if (button > 1500) buttonName = "none";
  else if (button < 50)  buttonNames = "right" ; // Scroll right in review
  else if (button < 195)  buttonNames = "up"; // Change modes
  else if (button < 380) buttonNames = "down"; // Store current value
  else if (button < 500) buttonNames = "left" ; // Scroll left in review
  else if (button < 700)  buttonNames = "select"; // Changes units
  
  return buttonNames;
}

/**
 * readDistance
 * uses the ultrasontic sensor and the pulse travel time 
 * to calculate the distance in both inches and cm
 * returns cm if metric parameter is true and inches if false 
 * measures 5 times and takes average
 */
double readDistance(bool metric){
  
  double duration, cm, inches, sum=0; // Distance variables

    for (int i = 0; i<= 4; i++){ // measure 5 times 
      
      // Send out pulse through Trigger pin
      digitalWrite(trigPin, LOW);
      delayMicroseconds(5);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
  
      // Read the Echo pin
      pinMode(echoPin, INPUT);
      sum += pulseIn(echoPin, HIGH); // get the duration of the pulse
    }
  duration = sum/5;//take average
  
  // Convert duration to CM
  cm = (duration/2) / 29.1;
  // Convert duration to INCHES
  inches = (duration/2) / 74; 

  if(metric){return cm;}else{return inches;}
}
