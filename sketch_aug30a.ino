#include <LiquidCrystal.h>
#include <Servo.h>
#include <OneButton.h>
#include <EEPROM.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal.h>


//set pins
const byte swOpen = 14;
const byte motorPWM = 10;
const byte speaker = 6;
const byte LCDlight = 13;


//set button
OneButton btn = OneButton(
  swOpen,
  true,
  true
);


//set LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//set keypad
const byte rows = 4; //four rows
const byte cols = 3; //three columns
char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[rows] = {15, 16, 17, 18}; //connect to the row pinouts of the keypad
byte colPins[cols] = {19, 9, 8}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );


//password array
byte passwd[] = {1,2,3,4};
//password input
byte input[] = {10,10,10,10};
byte pointer = 0;
boolean changePswd = false;


//time var
unsigned long time0;


void setup() {
//  Serial.begin(9600);
  
  //set pins
  pinMode(speaker,OUTPUT);
  digitalWrite(speaker,HIGH);
  pinMode(swOpen,INPUT);
  digitalWrite(swOpen,HIGH);
  pinMode(LCDlight,OUTPUT);
  digitalWrite(LCDlight,HIGH);

  //startup LCD
  lcd.clear();
  lcd.begin(16,2);
  lcd.print("Starting Up...");
  
  // put your setup code here, to run once:
//  Serial.println(EEPROM.read(0));
  
  if(EEPROM.read(0) == 255) {
    initialize();
  }

  for(int i = 0; i < 4; i++){
    passwd[i] = EEPROM.read(10+i);
  }

  LCDenter();
}

void LCDenter() {
  digitalWrite(LCDlight,HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter Password:");
}

void loop() {

  //to refresh button statue
  btn.tick();
  //open door by button inside
  btn.attachClick(openDoor);
  
  if(millis() - time0 > 5000){
    //turn off LCD backlight in idle
    digitalWrite(LCDlight,LOW);
  }
  
  // enter password from keypad
  char key = keypad.getKey();

  lcd.setCursor(pointer,1);
  
  if (key){
    //turn on LCD backlight when anykey is pressed
    time0 = millis();
    digitalWrite(LCDlight,HIGH);

    tone(speaker,1000,5);
    
//    Serial.println(key);
    if(key == '#') {
      changePswd = !changePswd;
      lcd.setCursor(15,1);
      if(changePswd){
        lcd.print("#");
      }else {
        lcd.print(" ");
      }
      tone(speaker,2000,50);
    }
    if(key == '*') {
      if(pointer > 0) {
        pointer--;
        lcd.setCursor(pointer,1);
        lcd.print(" ");
      }
    }
    if(key != '*' && key != '#'){
      input[pointer] = key-48;
      lcd.print("*");
      pointer++;
    }
    
    if(pointer == 4) {
//      Serial.println("judge!");
      pointer = 0;
      
      for(byte i = 0; i < 4; i++){
        if(input[i] != passwd[i]){
//          Serial.println("wrong passwd!");
//          Serial.println(i);
//          Serial.println(input[i]);
//          Serial.println(passwd[i]);
//          Serial.println("wrong passwd!");
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Wrong Password!");
          tone(speaker,1500,400);
          delay(400);
          tone(speaker,1500,400);
          delay(400);
          tone(speaker,1500,400);
          delay(1000);
          LCDenter();
          break;

        }

        if(i == 3) {
//          Serial.println("correct!");
//          tone(speaker,1000,250);
//          delay(250);
//          tone(speaker,1500,250);
//          delay(250);
          if(changePswd) {
//            Serial.println("change!");
            changePassword();
          }
          LCDenter();
          openDoor();
          }
      }
      
      for(byte i = 0; i < 4; i++){
        input[i] = 10;
      }
    }
  }
}

void changePassword() {
  changePswd = false;
  time0= millis();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("# to Change Pswd");
  lcd.setCursor(0,1);
  lcd.print("OtherKey to Quit");
  
  tone(speaker,1000,100);
  delay(100);
  tone(speaker,1000,100);
  delay(100);
  
  while(true){
    char key = keypad.getKey();
    delay(10);
    
    if(key == '#') {
      tone(speaker,2000,50);
      initialize();
      break;
    }else if(key != 0){
      LCDenter();
      break;
    }else if(millis() - time0 > 5000){
      break;
    }
  }

}

void initialize() {
  lcd.clear();
  lcd.print("Set Password:");
  lcd.setCursor(pointer,1);
  
  tone(speaker,2000,250); 
  delay(250);
  tone(speaker,1000,250);
  delay(255);
  
  while(true){
    char key = keypad.getKey();
    delay(10);
    
    if(0 <= (key-48) && (key-48) <= 9){
      passwd[pointer] = key-48;
      lcd.print("*");
//      Serial.println(key);
      delay(10);
      tone(speaker,1500,5);
      pointer++;
    }
    if(key == '*') {
      if(pointer > 0) {
        pointer--;
        lcd.setCursor(pointer,1);
        lcd.print(" ");
      }
    }
    
    if(pointer == 4){
      pointer = 0;
      tone(speaker,1000,250); 
      delay(250);
      tone(speaker,1500,250);
      delay(250);
      tone(speaker,2000,250);
      break;
    }
  }

  for(int i = 0; i < 4; i++){
    EEPROM.update(10+i, passwd[i]);
  }
  EEPROM.update(0,1);
}

static void openDoor(){
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Welcome!");
  motor(2,450);
  delay(6500);
  motor(1,260);
  delay(10);
  LCDenter();
}

void motor(int dir, int cycle){
  // 1 round = 73 cycles
  //positive direction
  if(dir == 1){
    for(int i = 0; i < cycle; i++){
      digitalWrite(motorPWM, HIGH);
      delay(1);
      digitalWrite(motorPWM, LOW);
      delay(19);
    }
  }
  //negative direction
  if(dir == 2){
    for(int i = 0; i < cycle; i++){
      digitalWrite(motorPWM, HIGH);
      delay(2);
      digitalWrite(motorPWM, LOW);
      delay(18);
    }
  }
}
