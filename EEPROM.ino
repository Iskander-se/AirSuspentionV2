///////////////////////////////
//EEPROM
//////////////////////////////

/// RW byte
 void EEPROM_Level_read(byte addr, int8_t curVal[2]) {   
  curVal[0] = EEPROM.read(addr);
  curVal[1] = EEPROM.read(addr+1);
}
void EEPROM_Level_write(byte addr, int8_t curVal[2]) {
   EEPROM.update(addr, curVal[0]); 
   EEPROM.update(addr+1, curVal[1]); 
}

// RW int
int EEPROM_Int_read(byte addr)
{
  byte byte1 = EEPROM.read(addr);
  byte byte2 = EEPROM.read(addr + 1);
  return (byte1 << 8) + byte2;
}

void EEPROM_Int_Write(byte addr, int number)
{ 
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;
  EEPROM.write(addr, byte1);
  EEPROM.write(addr + 1, byte2);
}

void fSaveMinMax(byte nom) 
{
  switch(nom){
   case 1:  //min F
         EEPROM_Int_Write(levelHwAddress[0]+0,curSuspention[0].RAW);
         EEPROM_Int_Write(levelHwAddress[0]+2,curSuspention[1].RAW);
         fLCDViewSaveStr(0);
         break;
   case 2:  //min R
         EEPROM_Int_Write(levelHwAddress[0]+4,curSuspention[2].RAW);
         EEPROM_Int_Write(levelHwAddress[0]+6,curSuspention[3].RAW);
         fLCDViewSaveStr(1);
         break;
   case 3:   //max F
         EEPROM_Int_Write(levelHwAddress[1]+0,curSuspention[0].RAW);
         EEPROM_Int_Write(levelHwAddress[1]+2,curSuspention[1].RAW);
         fLCDViewSaveStr(0);
         break;
   case 4:   //max R
         EEPROM_Int_Write(levelHwAddress[1]+4,curSuspention[2].RAW);
         EEPROM_Int_Write(levelHwAddress[1]+6,curSuspention[3].RAW);
         fLCDViewSaveStr(1);
         break;
     }
}

void GetLevelHwSetup()
{
  byte i;
  for(i=0;i<4;i++){
    LowLevels[i]=EEPROM_Int_read(levelHwAddress[0]+i*2);
  }
  for(i=0;i<4;i++){
    HighLevels[i]=EEPROM_Int_read(levelHwAddress[1]+i*2);
  }
}

void GetLevelPreset(byte addr)
{
   EEPROM_Level_read(firstValueAddress+addr*2,PresetLevels);    
}

void SetLevelPreset(byte nomPreset){
    EEPROM.update(PresetAddr,nomPreset);
    EEPROM.update(ManualAddr,0);                
    manual=false;                  
}
