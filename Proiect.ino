#include <EEPROM.h>

#include <LiquidCrystal.h>

#define ADR 100
#define IADR 600


LiquidCrystal lcd(6, 7, 2, 3, 4, 5);


bool LedState = false;
byte Led = 8;
byte btnPlus = 9;
byte btnMinus = 10;
byte btnOk = 11;
byte btnCancel = 12;
byte msgInd = 0;
byte Number = 0;
byte vIndex = 0;
bool Manual = false;
int Temp = 1000;

int TempSenz = A0;
float Temperature;
byte menuPage = 0;
bool Menu[5][11];
bool Entered = false;

const int timeConstant = 2500;   
int lastEntry = 0;                 
boolean prevResult;                
boolean sameButton;                
boolean previResult; 
boolean fail;              
byte buttonPress = 0; 

byte nrMesaje = 0;

bool Blocked = true;
bool passMode = false;

typedef struct message{
  char mesaj[25];
  bool Viewed=false;
}Message_t;

typedef struct eeprMsg {
  Message_t mesaje[10];
  byte index;
}eeprMsg_t;

eeprMsg_t mesaje;
eeprMsg_t inundatii;

char serialData;
char chArray[25]="";
int currentCharIdx = 0;

void debugMsg(void)
{
  int i =0;
  Serial.print("Index: ");
  Serial.println(mesaje.index);
  for (i =0; i<10; i++)
  {
    Serial.print(i); Serial.print(": ");
    Serial.println(mesaje.mesaje[i].mesaj);
  }
   i =0;
  Serial.print("Index iinund: ");
  Serial.println(inundatii.index);
  for (i =0; i<10; i++)
  {
    Serial.print(i); Serial.print(": ");
    Serial.println(inundatii.mesaje[i].mesaj);
  }

}

void setup()
{
  Serial.begin(9600);
  for(byte a = 0; a < 5; a++)
  for(byte i = 0; i < 11; i++)
          Menu[a][i] = false;
  Menu[1][0] = true;
  pinMode(btnPlus, INPUT_PULLUP);
  pinMode(btnMinus, INPUT_PULLUP);
  pinMode(btnOk, INPUT_PULLUP);
  pinMode(btnCancel, INPUT_PULLUP);
  pinMode(TempSenz, INPUT_PULLUP);
  pinMode(Led, OUTPUT);
  lcd.begin(16, 2);
  Serial.println("EEPROM Data:");
  EEPROM.get(ADR, mesaje);
  EEPROM.get(IADR, inundatii);
  Serial.println("Loaded"); 
  debugMsg();
}

void loop()
{

 if (Serial.available())
 {
 mesaje.index %= 10;

  int len = 0;
  serialData = Serial.read();
  if (serialData == '\n' || serialData == '\r')
  {
    //skip new line characters
  }
  else if (serialData != '.')
  {
    chArray[currentCharIdx++] = serialData;
  }
  else
  {
    if(chArray[0] == '[' && chArray[1] == 'i' && chArray[2] == ']')
    {
      strncpy(inundatii.mesaje[inundatii.index].mesaj, chArray, sizeof(chArray) - 1);
      inundatii.index++;
      memset(chArray, 0, sizeof(chArray));
      EEPROM.put(IADR, inundatii);
      debugMsg();
      currentCharIdx = 0; 
    }
    else if(chArray[0] == '[' && chArray[1] == 'b' && chArray[2] == ']')
    {
      if(LedState == false)
      { // [b]
        digitalWrite(Led, HIGH);
        LedState = true;    
      }
      else
      {
        digitalWrite(Led, LOW);
        LedState = false;
      }
    }
    else
    {
      strncpy(mesaje.mesaje[mesaje.index].mesaj, chArray, sizeof(chArray) - 1);
      mesaje.index++;
      memset(chArray, 0, sizeof(chArray));
      EEPROM.put(ADR, mesaje);
      debugMsg();
      currentCharIdx = 0;
    }
  }
}
if (Blocked == true)
{
  if (passMode == false)
  {
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.setCursor(0, 1);
    lcd.print(getTemperature());
    lcd.print("*C - ");
    lcd.print(nrMesaje);
    lcd.print(" mesaje");
    delay(100);
    if (digitalRead(btnPlus) == LOW || digitalRead(btnMinus) == LOW || digitalRead(btnOk) == LOW || digitalRead(btnCancel) == LOW)
    {
      passMode = true;
    }
  }
  else
  {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Insert code: ");
    lastEntry = millis();
    buttonPress = 0;
    combinationFind(btnPlus, btnMinus, btnPlus, btnMinus, btnPlus, btnMinus);

  }

}
else
{

  if (Menu[1][0] == true)
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Meniu: ");
    lcd.setCursor(0, 1);
    lcd.print("1. Mesaje");
    if (digitalRead(btnOk) == LOW)
    {
      Menu[1][0] = false;
      Menu[1][1] = true;
    }
    else if (digitalRead(btnPlus) == LOW)
    {
      Menu[1][0] = false;
      Menu[2][0] = true;
    }
    else if (digitalRead(btnCancel) == LOW)
    {
      Menu[1][0] = false;
      Blocked = true;
      passMode = false;
    }
    else if (digitalRead(btnMinus) == LOW)
    {

    }
  }
  else if (Menu[2][0] == true)
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Meniu: ");
    lcd.setCursor(0, 1);
    lcd.print("2. Control");
    if (digitalRead(btnOk) == LOW)
    {
      Menu[2][0] = false;
      Menu[2][1] = true;
    }
    else if (digitalRead(btnPlus) == LOW)
    {
      Menu[2][0] = false;
      Menu[3][0] = true;
    }
    else if (digitalRead(btnMinus) == LOW)
    {
      Menu[2][0] = false;
      Menu[1][0] = true;
    }
    else if (digitalRead(btnCancel) == LOW)
    {
      Menu[2][0] = false;
      Blocked = true;
      passMode = false;
    }
  }
  else if (Menu[3][0] == true)
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Meniu: ");
    lcd.setCursor(0, 1);
    lcd.print("3. Temperatura");
    if (digitalRead(btnOk) == LOW)
    {
      Menu[3][0] = false;
      Menu[3][1] = true;
    }
    else if (digitalRead(btnPlus) == LOW)
    {
      Menu[3][0] = false;
      Menu[4][0] = true;
    }
    else if (digitalRead(btnMinus) == LOW)
    {
      Menu[3][0] = false;
      Menu[2][0] = true;
    }
    else if (digitalRead(btnCancel) == LOW)
    {
      Menu[3][0] = false;
      Blocked = true;
      passMode = false;
    }
  }
  else if (Menu[4][0] == true)
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Meniu: ");
    lcd.setCursor(0, 1);
    lcd.print("4. Inundatii");
    if (digitalRead(btnOk) == LOW)
    {
      Menu[4][0] = false;
      Menu[4][1] = true;
    }
    else if (digitalRead(btnMinus) == LOW)
    {
      Menu[4][0] = false;
      Menu[3][0] = true;
    }
    else if (digitalRead(btnCancel) == LOW)
    {
      Menu[4][0] = false;
      Blocked = true;
      passMode = false;
    }
  }
  else if (Menu[1][1] == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1.1 Mesaje Necitite");
    lcd.setCursor(0, 1);
    if (Entered == true)
    {
      if (mesaje.index == 0)
      {
        lcd.print("Fara mesaje");
      }
      else
      {
        if(mesaje.mesaje[vIndex].Viewed == false)
        {
          lcd.print(Number);
          lcd.print(" ");
          lcd.print(mesaje.mesaje[vIndex].mesaj);  
        }
        else
        {
          while(mesaje.mesaje[vIndex].Viewed == true && vIndex  < mesaje.index)
          {
            vIndex++;
          }

          if(vIndex == mesaje.index)
            lcd.print("Fara mesaje");
          else
          {
            lcd.print(Number);
            lcd.print(" ");
            lcd.print(mesaje.mesaje[vIndex].mesaj);  
          }
        }
      }

      if(digitalRead(btnPlus) == LOW)
      {
        mesaje.mesaje[vIndex].Viewed = true;
        if(vIndex + 1 < mesaje.index && mesaje.mesaje[vIndex+1].Viewed == false)
        {
          vIndex++;
          Number++;
        }
        else
        {
          vIndex++;
          while(mesaje.mesaje[vIndex].Viewed == true)
          {
            vIndex++;
          }
          Number++;
        }
      }
      
      if(digitalRead(btnCancel) == LOW)
      {
        if(mesaje.mesaje[vIndex].Viewed == false)
          mesaje.mesaje[vIndex].Viewed = true;
        EEPROM.put(ADR, mesaje);
        Entered = false;
        vIndex = 0;
        Number = 0;
      }
    }
    else if (digitalRead(btnOk) == LOW)
    {
      Entered = true;
    }
    else if (digitalRead(btnPlus) == LOW)
    {
      Menu[1][1] = false;
      Menu[1][2] = true;
    }
    else if (digitalRead(btnCancel) == LOW)
    {
      Menu[1][1] = false;
      Menu[1][0] = true;
    }
  }
  else if (Menu[1][2] == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1.2 Mesaje Citite");
    lcd.setCursor(0, 1);
    if (Entered == true)
    {
      if (mesaje.index == 0)
      {
        lcd.print("Fara mesaje");
      }
      else
      {
        if(mesaje.mesaje[vIndex].Viewed == true)
        {
          lcd.print(Number);
          lcd.print(" ");
          lcd.print(mesaje.mesaje[vIndex].mesaj);  
        }
        else
        {
          while(mesaje.mesaje[vIndex].Viewed == false && vIndex  < mesaje.index)
          {
            vIndex++;
          }

          if(vIndex == mesaje.index)
            lcd.print("Fara mesaje");
          else
          {
            lcd.print(Number);
            lcd.print(" ");
            lcd.print(mesaje.mesaje[vIndex].mesaj);  
          }
        }
      }

      if(digitalRead(btnPlus) == LOW)
      {
        if(vIndex + 1 < mesaje.index)
        {
          vIndex++;
          Number++;
        }
      }

      if (digitalRead(btnCancel) == LOW)
      {
        Entered = false;
        vIndex = 0;
        Number = 0;
      }

      if(digitalRead(btnMinus) == LOW)
      {
        if(vIndex - 1 > 0)
          vIndex--;
      }
    }
    else if (digitalRead(btnOk) == LOW)
    {
      Entered = true;
    }
    else if (digitalRead(btnPlus) == LOW)
    {
      Menu[1][2] = false;
      Menu[1][3] = true;
    }
    else if (digitalRead(btnCancel) == LOW)
    {
      Menu[1][2] = false;
      Menu[1][0] = true;
    }else if(digitalRead(btnMinus) == LOW)
    {
      Menu[1][2] = false;
      Menu[1][1] = true;
    }
  }
  else if (Menu[1][3] == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1.3 Stergere");
    lcd.setCursor(0, 1);
    lcd.print("OK to confirm");
    if (digitalRead(btnOk) == LOW)
    {
      Menu[1][3] = false;
      Menu[1][0] = true;
      for (int i = 0; i<EEPROM.length() ; i++) 
        EEPROM.write(i, 0);

      EEPROM.get(ADR, mesaje);
    }
    else if (digitalRead(btnMinus) == LOW)
    {
      Menu[1][3] = false;
      Menu[1][2] = true;
    }
    else if (digitalRead(btnCancel) == LOW)
    {
      Menu[1][3] = false;
      Menu[1][0] = true;
    }
  }
  else if (Menu[2][1] == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("2. Control");
    lcd.setCursor(0, 1);
    if (Manual == true)
    {
      lcd.print("2.1 Manual[x]");
      if (digitalRead(btnPlus) == LOW)
      {
        Menu[2][1] = false;
        Menu[2][2] = true;
      }
      else if (digitalRead(btnCancel) == LOW)
      {
        Menu[2][1] = false;
        Menu[2][0] = true;
      }
    }
    else
    {
      lcd.print("2.1 Manual[ ]");
      if (digitalRead(btnOk) == LOW)
      {
        Manual = true;
      }
      if (digitalRead(btnPlus) == LOW)
      {
        Menu[2][1] = false;
        Menu[2][2] = true;
      }
      else if (digitalRead(btnCancel) == LOW)
      {
        Menu[2][1] = false;
        Menu[2][0] = true;
      }
    }
  }
  else if (Menu[2][2] == true)
  {
    if (Manual == true)
    {
      lcd.print("2.1 Automat[ ]");
      if (digitalRead(btnMinus) == LOW)
      {
        Menu[2][2] = false;
        Menu[2][1] = true;
      }
      else if (digitalRead(btnCancel) == LOW)
      {
        Menu[2][2] = false;
        Menu[2][0] = true;
      }
      else if (digitalRead(btnOk) == LOW)
      {
        Manual = false;
      }
    }
    else
    {
      lcd.print("2.2 Automat[x]");
      if (digitalRead(btnMinus) == LOW)
      {
        Menu[2][2] = false;
        Menu[2][1] = true;
      }
      else if (digitalRead(btnCancel) == LOW)
      {
        Menu[2][2] = false;
        Menu[2][0] = true;
      }
    }
  }
  else if (Menu[3][1] == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("3. Temperatura");
    lcd.setCursor(0, 1);
    lcd.print("3.1 Temp. ");
    lcd.print(getTemperature());
    lcd.print("*C");
    if (digitalRead(btnCancel) == LOW)
    {
      Menu[3][1] = false;
      Menu[3][0] = true;
    }
  }
  else if (Menu[4][1] == true)
  {
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("4. Inundatii");
     lcd.setCursor(0, 1);
     if (inundatii.index == 0)
      {
        lcd.print("Fara mesaje");
      }
      else
      {
          lcd.print(vIndex);
          lcd.print(" ");
          lcd.print(inundatii.mesaje[vIndex].mesaj); 
      }

      if(digitalRead(btnPlus) == LOW)
      {
        if(vIndex + 1 < inundatii.index)
        vIndex++;
      }
      if(digitalRead(btnMinus) == LOW)
      {
        if(vIndex - 1 > 0)
        vIndex--;
      }
      if(digitalRead(btnCancel) == LOW)
      {
        vIndex = 0;
        Menu[4][1] = false;
        Menu[4][0] = true;
      }

  }
}
  delay(200);
  Temp = Temp - 200;
  if(Temp == 0)
  {
    int Temp1 = getTemperature();
    String C = "Temp: ";
    String S = C + S;
    Temp = 1000;
    Serial.print(S);
  }
}


int getTemperature()
{
  int temperature = analogRead(TempSenz);
  float voltage = temperature * 5.0;
  voltage = voltage / 1024.0;
  return ((voltage - 0.5) * 10);
Serial.println((voltage - 0.5) * 10);
}

void combinationFind (byte p1,byte p2,byte p3,byte p4,byte p5,byte p6){
  prevResult = true;
  buttonPressed(p1,p2);
  lcd.setCursor(3, 1);
  lcd.print("*");
  buttonPressed(p2,p3);
  lcd.print("*");
  buttonPressed(p3,p4);
  lcd.print("*");
  buttonPressed(p4,p5);
  lcd.print("*");
  buttonPressed(p5,p6);
  lcd.print("*");
  lastButtonPressed(p6);
  lcd.print("*");
  delay(10);
}//void
void buttonPressed (byte b1,byte b2){
  if (b1 == b2){             //Find out if the current button and the next button are the same
    sameButton = true;       //Write sameButton as true (the current digit and next digit are the same)
  } //if
  else{
    sameButton = false;      //Write sameButton as false (the current digit and next digit are not the same)
  }//else
  if (prevResult == true){
    while(millis() - lastEntry < timeConstant && prevResult == true){      //while 2.5 seconds haven't passed and the previous digit entered was correct
      if(sameButton == true){                                              //If the two buttons are the same run this code
        if(digitalRead(b1) == LOW){                                       //If the correct button is pressed
          while(digitalRead(b1) != HIGH){                                   //Only move on to the next line when the current button is let go of, so that holding down the button won't register for the next digit of the code
            }//while
          prevResult = false;
          previResult = false;
          delay(10);                                                       //Delay 10 milliseconds so the switch can be debounced
          lastEntry = millis(); 
          fail = false;
          break;                                                           //Exit the while loop
        }//if
      }//if
      else{
        if(digitalRead(b1) == LOW && digitalRead(b2) == HIGH){ //Check if the current button is being pressed and the other isn't, so that users can't push down on both buttons to pass
          while(digitalRead(b1) != HIGH){
          }//while
          prevResult = false;
          previResult = false;
          delay(10);                                                      //Delay 10 milliseconds to debounce
          lastEntry = millis();
          lastEntry = millis();
          fail = false;
          break;                                                          //Exit the while loop
        }//if
        else if (digitalRead(b1) == HIGH && digitalRead(b2) == LOW){     //If the user presses the wrong button
          prevResult = false;
          previResult = false;
          fail = true;
          delay(100);
          lastEntry = millis() - 3000;
          break;                                //Exit the while loop
        }//else if
      }//else
    }//while
    prevResult = false;
    previResult = false;
  }//if
  if(fail == false){                 //If the user didn't fail to put in the right digit for the code
    prevResult = true;               // Rewrite the values prevResult and previResult so the program knows the user inserted the right digit
    previResult = true;
  }
  else{                             //If the user did fail to put in the right digit for the code
    prevResult = false;             // Rewrite the values prevResult and previResult so the program knows the user inserted the wrong digit
    previResult = false;
  }
}//void

void lastButtonPressed(byte b1){
  if(previResult == true){
    while(millis()-lastEntry < timeConstant){
      if(digitalRead(b1)== LOW){
        delay(10);
        lastEntry = millis();
        fail = false;
        prevResult = true;
        previResult = true;
        lastEntry = millis();
        Blocked = false;
        Menu[1][0] = true;
        passMode = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Checking code...");
        for(int i = 0; i < 16; i++)
        {
          lcd.setCursor(i, 1);
          lcd.print("%");
          delay(150);
        }
        break;
      }
    }
  }
}

void checkCheating(){
  buttonPress++;           //Increment button press every time a button is let go of
  delay(10);
  if(buttonPress>5){       //If a button is pressed more than 5 times then restart the code
  
                            /*The reason the value 5 was selected instead of 6 is because the 6th button press is the last button pressed. 
                            In the function lastButtonPressed the code doesn't check how many times a button was pressed after the final button is pressed.
                            This will only make a difference if the user presses buttons 6 times before they press the final button in the code
                            */
  
    prevResult = false;     //Change parameters so the program will know cheating has occured and will restart the code
    previResult = false;
    fail = true;
    delay(100);
    lastEntry = millis() - 3000;
    buttonPress = 0;        //reset the value of buttonPress
  }
}
