//By Vitaly Vasiliev 09.2013

#include <EEPROM.h>
#include <MIDI.h> //You need this third-based library for MIDI interation
#include <alloc.h>

volatile byte currentPreset;
byte switchPin = 10;
byte saveButtonPin = 8;
byte deleteButtonPin = 7;
byte statusLedPin = 6;
volatile boolean isDriveOn = false;
volatile boolean isMidiPlugged = false;
volatile int m_eepromSize;
byte *m_eepromValues;

void ProgramChangeHandle(byte channel, byte number) {
  
   isMidiPlugged = true;
   currentPreset = number;
     
   if (m_eepromValues == NULL || m_eepromSize == NULL) return;
   
       boolean found = findPreset(currentPreset);
       if (found && isDriveOn == false)
       {
         digitalWrite(switchPin, HIGH);
         isDriveOn = true;         
         return;
       }
       if (!found && isDriveOn == true)
       {
         digitalWrite(switchPin, LOW);
         isDriveOn = false;
         return;
       }
    delay(500);    
}
///////////////////////////////////////////////////////////////////////////////////////////

boolean findPreset(byte number)
{  
   for (int i = 0; i<m_eepromSize; i++)
     {
       if (m_eepromValues[i] == number) return true;
     }
   return false;
}

int eepromSize() {
  int address = 0;
  byte value = EEPROM.read(address);
  if (value == 255) return 0; //на всякий случай при пустой памяти
  while(value!=255)
  {
    value = EEPROM.read(address);
    address++;
  }
  return address - 1;
}
///////////////////////////////////////////////////////////////////////////////////////


void getValues()
{
   m_eepromValues = (byte*)malloc(m_eepromSize);  
  for (int i=0; i<m_eepromSize; i++)
   {
     m_eepromValues[i] = EEPROM.read(i);
   }
}
////////////////////////////////////////////////////////////////////////////////////// 

		
void setup() {  
  pinMode(switchPin, OUTPUT);
  pinMode(statusLedPin, OUTPUT);
  pinMode(saveButtonPin, INPUT);
  pinMode(deleteButtonPin, INPUT);
                                              
  MIDI.begin();
  MIDI.turnThruOff();
  MIDI.setHandleProgramChange(ProgramChangeHandle);
  
  m_eepromSize = eepromSize();
  getValues();
 
  digitalWrite(statusLedPin, HIGH);  //////
  delay(1000);                       //////
  digitalWrite(statusLedPin, LOW);   ////// отладка 
  
}

void loop() {
  MIDI.read();
  if (digitalRead(saveButtonPin) == HIGH) saveButtonPressed();
  if (digitalRead(deleteButtonPin) == HIGH) deleteButtonPressed();  
}


//Buttons
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void saveButtonPressed()
{
  delay(500); 
  if (!isMidiPlugged) return;
    
  boolean found = findPreset(currentPreset);
  if (found) return;
  else
  {
    digitalWrite(statusLedPin, HIGH);
    EEPROM.write(m_eepromSize, currentPreset);
    free(m_eepromValues);
    m_eepromSize++; 
    getValues();
  }
  
  digitalWrite(switchPin, HIGH);
  isDriveOn = true;
  
  delay(1000);
  digitalWrite(statusLedPin, LOW);
      
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void deleteButtonPressed()
{
  delay(500);
  if (!isMidiPlugged) return;
                       
  boolean found = findPreset(currentPreset);
  if (!found) return;
  
  digitalWrite(statusLedPin, HIGH);  
  
  int tempCounter = 0;
  for (int i = 0; i < m_eepromSize; i++)
  {
    if (m_eepromValues[i] == currentPreset) continue;
    EEPROM.write(tempCounter,m_eepromValues[i]);
    tempCounter++;
  }
  
  //обнуляем последнюю запись в EEPROM
  EEPROM.write(m_eepromSize, 255);
  free(m_eepromValues);
  m_eepromSize--; 
  getValues();    
  
  digitalWrite(switchPin, LOW);
  isDriveOn = false;
  
  delay(1000);
  digitalWrite(statusLedPin, LOW);
  delay(500);
  digitalWrite(statusLedPin, HIGH);
  delay(500);
  digitalWrite(statusLedPin, LOW); 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////



/*
void debugEepromValues() 
{
  if (m_eepromValues == NULL || m_eepromSize == NULL) return;
  Serial.print("EEPROM size: ");
  Serial.println(m_eepromSize);
  Serial.print("Values: ");
  for (int i=0; i<m_eepromSize; i++)
  {                                 
    Serial.println(m_eepromValues[i]);
  }                                 
}*/




