#include <LiquidCrystal.h>
#include <Servo.h>
#include <OneButton.h>
#include <EEPROM.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal.h>


//set pins
const int swOpen = 14;
const int motorPWM = 10;


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


void setup() {
//  Serial.begin(9600);
  
  //set pins
  pinMode(6,OUTPUT);
  digitalWrite(6,LOW);
  pinMode(14,INPUT);
  digitalWrite(14,HIGH);

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
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter Password:");
}

void loop() {
  //to refresh button statue
  btn.tick();
  //open door by button inside
  btn.attachClick(openDoor);
  
  // enter password from keypad
  char key = keypad.getKey();

  lcd.setCursor(pointer,1);
  
  if (key){
//    Serial.println(key);
    if(key == '#') {
      changePswd = !changePswd;
      lcd.setCursor(15,1);
      if(changePswd){
        lcd.print("#");
      }else {
        lcd.print(" ");
      }
      tone(6,2000,50);
    }
    if(key != '*' && key != '#'){
      input[pointer] = key-48;
      lcd.print("*");
      tone(6,1000,5);
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
          tone(6,1500,400);
          delay(400);
          tone(6,1500,400);
          delay(400);
          tone(6,1500,400);
          delay(1000);
          LCDenter();
          break;

        }

        if(i == 3) {
//          Serial.println("correct!");
//          tone(6,1000,250);
//          delay(250);
//          tone(6,1500,250);
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
  int cycle = 0;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Press#to Change.");
  lcd.setCursor(0,1);
  lcd.print("Others to Quit.");
  
  tone(6,1000,100);
  delay(100);
  tone(6,1000,100);
  delay(100);
  
  while(true){
    char key = keypad.getKey();
    delay(10);
    cycle++;
    
    if(key == '#') {
      tone(6,2000,50);
      initialize();
      break;
    }else if(key != 0){
      LCDenter();
      break;
    }else if(cycle == 1000){
      LCDenter();
      break;
    }
  }

}

void initialize() {
  lcd.clear();
  lcd.print("Set Password:");
  lcd.setCursor(pointer,1);
  
  tone(6,2000,250); 
  delay(250);
  tone(6,1000,250);
  delay(255);
  
  while(true){
    char key = keypad.getKey();
    delay(10);
    
    if(0 <= (key-48) && (key-48) <= 9){
      passwd[pointer] = key-48;
      lcd.print("*");
//      Serial.println(key);
      delay(10);
      tone(6,1500,5);
      pointer++;
    }
    
    if(pointer == 4){
      pointer = 0;
      tone(6,1000,250); 
      delay(250);
      tone(6,1500,250);
      delay(250);
      tone(6,2000,250);
      break;
    }
  }

  for(int i = 0; i < 4; i++){
    EEPROM.update(10+i, passwd[i]);
  }
  EEPROM.update(0,1);
}

static void openDoor(){
  motor(2,45);
  delay(5000);
  motor(1,45);
  delay(10);
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
