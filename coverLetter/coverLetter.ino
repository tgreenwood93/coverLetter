//include libraries
#include <avr/pgmspace.h>
#include <LiquidCrystal.h>

//char arrays
const static char coverLetter[] PROGMEM = "Pressing this button has started the leter to play. Pressing it again will pause the letter. Pressing button 2 will start the letter from the beginning again.        Here we go!                     My name is Tom Greenwood, I am a recent graduate of the University of Oregon. Throughout my graduate studies, I have been extensively using your products to fuel my designs and creations. When I saw that you had an open position, I knew that I had to apply. Building electronics and DIY creations has been my passion since entering college. I love to tinker with all sorts of electronics to see how everything works.                          I have an ample background working with Arduinos and other microprocessors for building and creating projects. I have my own oscilloscope that I use regularly to test for proper voltage levels on the projects I,m working on. I enjoy creating new ways for humans to interact with computers through augmenting everyday objects into controllers or building new creations from the ground up. I also have experience working with laser cutters and 3d printers to build custom enclosures or pieces for projects.                Education is an important part of who I am, and I want to be able to work at a place where I can keep learning and participate in an environment that shares that value with its employees and customers. I believe SparkFun is dedicated to the pursuit of knowledge and the well-being of everyone they work with and for.               SparkFun,s dedication to open source code, projects, and community oriented culture is very important to me, as it helps drive knowledge and learning for emplyees and community. I try to employ the same values with my projects through my website and Github account that I regularly update with the projects that I am working on. I post all my code and detailed instructions on how I worked through the engineering of the project. I also have branched out to Medium.com to write tutorials and share my passion for electronics with the world.              Developing tutorials for other people to read and share is important to me as an engineer, and something that I love doing. I try to accommodate a variety of learning types by creating and using graphics, videos, and animations. I also find documenting my own work and processes with pictures and videos so people can see the evolution of my project, as well as have reference material for myself.                       My passion and drive to work on electronics and all things IoT have led me to do a lot of independent research and development. My thirst for knowledge has grown the more I learn about what,s possible with electronics. I have accumulated, and am still collecting, a plethora of books that cover topics from wireless networking, to micropython development, and much more. I love to analyze other projects and see how other engineers built their projects, and learn from and analyze their methodology.                   I believe SparkFun is a great company, with a great mission, in one of the greatest parts of the country. I would love to join the people, engineers, and creators of SparkFun in the pursuit of electronics education, development, and community development.                Please visit my embedded electronics and software development portfolio to find out more about my projects ... tom greenwood portfolio.com                 Thank you for your time!                    P.S. I owe a few SparkFun Engineers a beer or two for all the libraries they,ve developed!                                ";
char bottomRow[] = "Hello, SparkFun!";
char topRow[] = "Press Button One";
char startTop[] = "Hello, SparkFun!";
char startBottom[] = "Press Button One";
char blank[] = " ";

//global pointers in data
volatile int n = 0;
volatile int q = 0; 

//create button vars
int numLines = 275;
const byte ledPin = 8;
const byte interruptPin = 2;
const byte interruptPin2 = 3;
volatile byte state = LOW;
volatile byte newState = LOW;
volatile byte restart = false;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 6, 5, 4, 1);

void setup() {
  //define inputs and outputs
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(interruptPin2, INPUT_PULLUP); 
  //define interputs
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), blink2, CHANGE);
  
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  // Print a message to the LCD.
  lcd.print(bottomRow);
  lcd.setCursor(0,1);
  // Print a message to the LCD.
  lcd.print(topRow);

  delay(1000);

}

void blink() {
  //wait 30ms to avoid false triggers
  delay(30);
  //update the state (ON/OFF)
  state = !state; 
  //if system was reset, set it back to operational
  restart = false;
  //only when system is ON will the letter play
  if(state == HIGH){
    newState = !newState;
    //turn on LED
    digitalWrite(ledPin, newState); 
  } 
}

void blink2() {
  //wait 30ms to avoid false triggers
  delay(30);
  restart = true;
  //reset all vars
  newState = LOW;
  state = LOW;
} 

void lcdWrite() {
   //update top row
   lcd.setCursor(0,0);
   lcd.print(topRow);   
   //update bottom row
   lcd.setCursor(0,1);
   lcd.print(bottomRow); 
}

void restartFunc(){
  if(restart == true){
    //reset vars
    n = 0;
    q = 0;
    //reset LED
    digitalWrite(ledPin, LOW);  
    newState = LOW;
    state = LOW;
    //load first two rows of text into buffer
    for(int i = 0; i < 16; i++){
      topRow[i] = startTop[i];
      bottomRow[i] = startBottom[i];
    } 
    lcdWrite();   
  }
}

void parseFunc(){
  //global stop
  while(newState){
  //copy bottom text into top text
  for(int i = 0; i < 16; i++){
      topRow[i] = bottomRow[i];
    }  
    //copy section of main cover letter into bottom row
    int j = 0;
    for(int i = n; i < (n+16); i++){
      bottomRow[j] =  pgm_read_byte_near(coverLetter + i); 
      j += 1;
    }
    //check to see if system has been reset
    if(restart == true){
      n = 0;
      restart = false;
    }else{  
      //move array pointer forward 
      n += 16;
    }  
    //check to see if words are split between rows
    //if word is split, move pointer backward and replace text with spaces
    if(bottomRow[15] != blank[0]){
     //find where last space is
     for(int t = 15; t > 0; t--){
       //write over char with space
       if(bottomRow[t] == blank[0]){
         break;
       }else{
         bottomRow[t] = blank[0];
         //move pointer back every time we dont hit a space
         n -= 1;
       }  
     }
   }
   //update LCD display
   lcdWrite();
   //slow down rate of refresh
   delay(400);
   //update global pointer   
   q += 1;
    //if we reach the end of the letter, stop and reset to beginning
    if(q > numLines){
      restart = true;
      restartFunc();
    }
  }
} 

void loop() {
 //check to see if reset button has been pressed
 restartFunc();
 //parse coverletter to fit on LCD
 parseFunc();
}
