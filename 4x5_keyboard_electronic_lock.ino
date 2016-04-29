/******************************************************
4x5 keyboard matrix electronic lock

By Chris Rouse April. 2016

Connect:
Row 1 to Arduino pin 2
Row 2 to Arduino pin 3
Row 3 to Arduino pin 4
Row 4 to Arduino pin 5
Row 5 to Arduino pin 6

Column A to Arduino pin 8
Column B to Arduino pin 9
Column C to Arduino pin 10
Column D to Arduino pin 11

Key values with connector at top

4  9  14  19
3  8  13  18
2  7  12  17
1  6  11  16
0  5  10  15
  
Connector (from left to right with keys facing up, connector at top):

  Columns      Rows
  X  X  X  X   X  X  X  X  X  
  A  B  C  D   1  2  3  4  5
  
  (Rows link keys from left to right, 
  columns link keys from top to bottom)

****************************************************/

int rowCounter =0; // row counter
int columnCounter =0; // column counter
int foundColumn = 0;
boolean foundCol = false;
int keyValue = 0;
int noKey = 0;
boolean readKey = false;
int debounce = 300; // set this to the lowest value that gives the best result
String keyString;
int keyNum; // used for numeric keys
boolean numericValue = false; // flag to show if number selected
boolean newAttempt = false;
int counter = 0;
String lockCode = ""; // entered code
boolean unlock = true; // used to show the state of the lock 1
boolean unlock2 = true; // used to show the state of the lock 2
boolean locationLock = false; // lock number 1 if true then lock number 2
//********************************************
int maxChars = 3; // maximum number of numbers in the unlock code -1
// this is the code for the lock 1
String key = "1,9,4,7"; // the unlock code
// this is the code for the lock 2
String key2 = "1,5,2,0"; // the unlock code
//
//********************************************

const int row1 = 2;
const int row2 = 3;
const int row3 = 4;
const int row4 = 5;
const int row5 = 6;
const int colA = 8;
const int colB = 9;
const int colC = 10;
const int colD = 11;

const int ledPin = 13; // onboard LED
/****************************************************/
void setup(){
  Serial.begin(9600);
  pinMode(row1, OUTPUT);
  pinMode(row2, OUTPUT);
  pinMode(row3, OUTPUT);
  pinMode(row4, OUTPUT);
  pinMode(row5, OUTPUT);
  pinMode(colA, INPUT_PULLUP);
  pinMode(colB, INPUT_PULLUP);
  pinMode(colC, INPUT_PULLUP);
  pinMode(colD, INPUT_PULLUP);
  //
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // turn LED off
  //
  Serial.println("Electronic Lock Demonstration");
  Serial.println("***************************************");
  Serial.println("F1 selects Lock 1 - F2 selects Lock 2");
  Serial.println("# shows the state of both locks");
  Serial.println("* locks both locks");
  Serial.println("Esc aborts key entry");
  Serial.println("Ent locks current active lock");
  Serial.println("The Arrow keys have no action but could operate relays etc:");
  Serial.println("***************************************");
  Serial.println();
  Serial.println("This is lock number 1");
  Serial.println("To lock please press ENT");
  Serial.println();   
}
/***********************************************************/
void loop(){
  if(noKey == 20){ // no keys were pressed
    readKey = true;  // keyboard is ready to accept a new keypress
  }
  noKey = 0;
  for(rowCounter=row1; rowCounter<(row5 +1); rowCounter++){
    scanRow(); // switch on one row at a time
    for(columnCounter = colA; columnCounter <colD +1; columnCounter++){
      readColumn(); // read the switch pressed
      if (foundCol== true){
        keyValue =(rowCounter-row1) +5*(columnCounter - colA);
      }
    }
  }
  if(readKey==true && noKey == 19){  // a key has been pressed
    convertKey(); //convert key number to keypad value 
    //   
    /* used for debug   
    Serial.print(keyValue); 
    Serial.print("  -  ");
    Serial.println(keyString);
    */
   
    //-------------------------------------------------------
    // Lock routine
    // use ENT to lock the lock
    if(keyValue == 15){ 
      Serial.print("Lock number ");
      if(locationLock){
        Serial.print("2");
        unlock2 = false;
      }
      else{
        Serial.print("1");
        unlock = false;
      }
      Serial.println(" now LOCKED");
      Serial.println();      
      digitalWrite(ledPin, HIGH);
    }
    if(unlock == false && counter == maxChars){
      Serial.println("*");
      Serial.println();
      lockCode += String(keyNum);
      //Serial.println(lockCode); // show entered number
      // is the code correct?
      if((lockCode == key && locationLock == false) || (lockCode == key2 && locationLock)){
        Serial.print("Lock number ");
        if(locationLock) Serial.print("2");
        else Serial.print("1");        
        Serial.println(" now OPEN");
        Serial.println("To lock please press ENT");
        Serial.println();
        digitalWrite(ledPin, LOW);
        unlock = true; // show lock is open
        //
        // put code in here to unlock
        //
      }
      else Serial.println("Lock not released");
      newAttempt = true;
      counter = 0;
      lockCode = ""; // reset
    }
    else if(keyNum <20 && unlock == false){ // a numeric key pressed
      lockCode += String(keyNum) + ",";
      Serial.print("*");
      counter += 1;
    }
    else if(unlock && keyNum < 20 && counter == 0) Serial.println("keypress ignored, lock already open");
    //
    // now deal with non numeric keys
    //
    if(keyString == "F1"){
      locationLock = false;
      Serial.println("now switched to Lock Number 1");
      Serial.println();       
      if (unlock){
        digitalWrite(ledPin, LOW);
      }
      else digitalWrite(ledPin, HIGH);
    }
    if(keyString == "F2"){
      locationLock = true;
      Serial.println("now switched to Lock Number 2");
      Serial.println();       
      if (unlock2){
        digitalWrite(ledPin, LOW);
      }
      else digitalWrite(ledPin, HIGH);      
    }
    // show status of both locks
    if(keyString == "#"){
      Serial.println();      
      if(unlock == false){
      Serial.print("Lock Number 1 is LOCKED");
      } 
      if(unlock == true){
      Serial.print("Lock Number 1 is UNLOCKED");
      } 
      Serial.println();       
      if(unlock2 == false){
      Serial.print("Lock Number 2 is LOCKED");
      } 
      if(unlock2 == true){
      Serial.print("Lock Number 2 is UNLOCKED");
      } 
      Serial.println(); 
      Serial.println();                                        
    }
    // lock both locks at once
    if(keyString == "*"){
      Serial.println();
      Serial.println("Both locks now LOCKED");
      Serial.println(); 
      if(locationLock){
        Serial.println("lock number 2 is selected");
        Serial.println();     
      }
      else{
        Serial.println("lock number 1 is selected");
        Serial.println();          
      }
      unlock = false;
      unlock2 = false;
      digitalWrite(ledPin, HIGH);
    }
    // abort code entry
    if(keyString == "Esc"){
      Serial.println();
      Serial.println();      
      Serial.println("Code entry aborted");
      Serial.println();
      newAttempt = true;
      counter = 0;
      lockCode = ""; // reset      
    }
    //-------------------------------------------------------
    
    readKey = false; // reset the flag
    delay(debounce); // debounce
  }
}
/************************************************************/
void scanRow(){
  for(int j =row1; j < (row5 +1); j++){
    digitalWrite(j, HIGH);
  }
  digitalWrite(rowCounter , LOW); // switch on one row
}
/************************************************************/
void readColumn(){
  foundColumn = digitalRead(columnCounter);
  if(foundColumn == 0){
     foundCol = true;
  }
  else{
  foundCol=false;
  noKey=noKey +1; // counter for number of empty columns
  }
}
/************************************************************/
void convertKey(){
  // converts the key number to the corresponding key
  keyString ="";
  keyNum = 99;
  numericValue = false;  
  //
  switch (keyValue) {
    // column A
    case 0:
    keyString = "Left Arrow";
    break;
    case 1:
    keyString = "7";
    keyNum = 7;
    break;  
    case 2:
    keyString = "4";
    keyNum = 4;    
    break;
    case 3:
    keyString = "1";
    keyNum = 1;    
    break;
    case 4:
    keyString = "F1";
    break;
    // column B
    case 5:
    keyString = "0";
    keyNum = 0;
    break;
    case 6:
    keyString = "8";
    keyNum = 8;    
    break;  
    case 7:
    keyString = "5";
    keyNum = 5;    
    break;
    case 8:
    keyString = "2";
    keyNum = 2;    
    break;
    case 9:
    keyString = "F2";    
    break;   
    // column C
    case 10:
    keyString = "Right Arrow";
    break;
    case 11:
    keyString = "9";
    keyNum = 9;    
    break;  
    case 12:
    keyString = "6";
    keyNum = 6;    
    break;
    case 13:
    keyString = "3";
    keyNum = 3;    
    break;
    case 14:
    keyString = "#";   
    break;   
    // column D
    case 15:
    keyString = "Ent";
    break;
    case 16:
    keyString = "Esc";   
    break;  
    case 17:
    keyString = "Down Arrow";   
    break;
    case 18:
    keyString = "Up Arrow";  
    break;
    case 19:
    keyString = "*";   
    break;     
  }
  if (keyNum == 99){
      numericValue = false; // show a non numeric key pressed
  }
  else{
      numericValue = true;  // show a number key pressed
  }
}
/************************************************************/
