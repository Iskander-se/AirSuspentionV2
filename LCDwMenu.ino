void fLCDView4HwInt()
 { 
  lcd.clear();
  lcd.setCursor(1,0); lcd.print( curSuspention[0].RAW);
  lcd.setCursor(12,0);  lcd.print(curSuspention[1].RAW);         
  lcd.setCursor(1,1); lcd.print(curSuspention[2].RAW);
  lcd.setCursor(12,1);  lcd.print(curSuspention[3].RAW);
 }

void fLCDViewSaveStr(byte nom)
 { 
    lcd.setCursor(0,nom); lcd.print("#");
    lcd.setCursor(15,nom); lcd.print("#");
    lcd.setCursor(6,nom); lcd.print("SAVE");
 }

void fLCDViewLevelsFbyte(int16_t cValue[])
 { 
   lcd.setCursor(1,0); lcd.print( cValue[0]); lcd.print(" ");
   lcd.setCursor(11,0);  if(cValue[1]>-1) lcd.print(" ");if(cValue[1]<100) lcd.print(" "); lcd.print(cValue[1]);if(cValue[1]<10) lcd.print(" "); lcd.print(" ");           
   lcd.setCursor(1,1); lcd.print(cValue[2]); lcd.print(" ");
   lcd.setCursor(11,1);  if(cValue[3]>-1) lcd.print(" ");if(cValue[3]<100) lcd.print(" "); lcd.print(cValue[3]); if(cValue[3]<10)lcd.print(" "); lcd.print(" ");
 }


 ////////
/////     MENU
////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void fLCDmenu()
{
  engineCount=0;
  if(menu>0)
  {
    switch(menu)
          {
             case 1:
                lcd.clear();
                lcd.setCursor(5,0); lcd.print(" MENU");
                ValveSet.FL=0;  
                ValveSet.FR=0; 
                ValveSet.RL=0;   
                ValveSet.RR=0; 
                ValveSet.WP=0;
                ValveSet.RELAY=4; 
                fVAGBlockWork();
                break;
             case 2:  // Выбор пресета
                nomPreset=nomPreset+mStep;
                if(nomPreset<1)nomPreset=0;
                if(nomPreset>8)nomPreset=8;                
                if(WAIT>2){
                  lcd.setCursor(4,0); lcd.print("Preset "); lcd.print(nomPreset); GetLevelPreset(nomPreset);                                   
                  lcd.setCursor(3,1); lcd.print(PresetLevels[0]); lcd.print("%  "); 
                  lcd.setCursor(6,1); lcd.print(" <> "); lcd.print(PresetLevels[1]);lcd.print("% ");
                  if(PresetLevels[0]<0) PresetLevels[0]=0; 
                  if(PresetLevels[1]<0) PresetLevels[1]=0;
                }else SetLevelPreset(nomPreset);                
                mStep=0;
                break;
            case 3:  
                lcd.setCursor(0,0); lcd.print("  MANUAL  SET  "); 
                lcd.setCursor(0,1); lcd.print(" FRONT ][ REAR  ");
                if(mStep!=0){
                  manual=true;
                  EEPROM.update(ManualAddr,1);
                  delay(300);
                  if(mStep>0) {menu=31;}
                  else {menu=34;}
                  mStep=0;
                }
                break;
            case 31: 
                lcd.setCursor(3,0); lcd.print("   REAR   "); 
                lcd.setCursor(1,1); lcd.print(" UP/DOWN ");
                if(mStep!=0){
                  manual=true;
                  WAIT=20;
                  if(mStep<0) {ValveSet.RELAY=1;}
                  else {ValveSet.RELAY=2;}
                 }
                ValveSet.WP=0;
                ValveSet.FL=0;  
                ValveSet.FR=0; 
                ValveSet.RL=1;   
                ValveSet.RR=1; 
                if(WAIT<10) ValveSet.RELAY=0; 
                fVAGBlockWork();
                GetPressure();
                lcd.setCursor(10,1); lcd.print(Pressure.VAG);lcd.print("  ");
                lcd.setCursor(1,0); lcd.print( curSuspention[2].Avg/10);lcd.print(" ");
                if(curSuspention[3].Avg<100) lcd.setCursor(14,0); else lcd.setCursor(13,0); lcd.print(curSuspention[3].Avg/10);
                mStep=0;
                break;
            case 34: 
                lcd.setCursor(3,0); lcd.print("   FRONT  "); 
                lcd.setCursor(1,1); lcd.print(" UP/DOWN ");
                if(mStep!=0){
                  manual=true;
                  WAIT=20;
                  if(mStep<0) {ValveSet.RELAY=1;}
                  else {ValveSet.RELAY=2;}                  
                }
                ValveSet.WP=0;                 
                ValveSet.FL=1;  
                ValveSet.FR=1; 
                ValveSet.RL=0;   
                ValveSet.RR=0; 
                if(WAIT<10) ValveSet.RELAY=0; 
                fVAGBlockWork();
                GetPressure(); 
                lcd.setCursor(10,1); lcd.print(Pressure.VAG);lcd.print("  ");
                lcd.setCursor(1,0); lcd.print(curSuspention[0].Avg/10);lcd.print(" ");
                if(curSuspention[1].Avg<100) lcd.setCursor(14,0); else lcd.setCursor(13,0); lcd.print(curSuspention[1].Avg/10);
                mStep=0;
                break;
            case 4:  
                
                if(mStep!=0){
                  lcd.setCursor(1,0); lcd.print("   ...WAIT    ");
                  lcd.setCursor(1,1); lcd.print("               ");
                  Get4Pressure();
                  lcd.setCursor(5,1); lcd.print(Pressure.RES);lcd.print("kpa"); 
                  lcd.setCursor(5,0); lcd.print("PRESS");                   
                  WAIT=1;                 
                  delay(5000);
                }else{
                  lcd.setCursor(0,0); lcd.print("  GET PRESSURE  "); 
                  lcd.setCursor(0,1); lcd.print(" 4 BANK + TANK  ");
                }
                break;
            case 5:  
                lcd.setCursor(0,0); lcd.print("   MAINTENANCE  ");
                if(mStep==0){
                  lcd.setCursor(0,1); lcd.print("  TANK X HWSETS ");
                  if(WAIT<3) servicemode=true;
                }else{                  
                  delay(300);
                  if(mStep<0) {menu=41;}
                  else {menu=44;}
                  mStep=0;
                }
                break;
            case 6:  
                lcd.setCursor(0,0); lcd.print("  AIR TRANSIT  ");
                if(WAIT<15){
                 WAIT=15; 
                  ValveSet.FL=0;  
                  ValveSet.FR=0; 
                  ValveSet.RL=0;   
                  ValveSet.RR=0; 
                  ValveSet.RELAY=5; 
                 fVAGBlockWork();
                 GetPressure(); 
                 lcd.setCursor(1,1); lcd.print("VAG"); lcd.print(Pressure.VAG); lcd.print(" ");
                 lcd.setCursor(8,1); lcd.print("RES"); lcd.print(Pressure.RES); lcd.print(" ");
                  mStep=0;
                }else lcd.setCursor(0,1); lcd.print("    --------    ");
                break;
            case 41: 
                lcd.setCursor(1,1); lcd.print("Charge "); lcd.print(Pressure.RES);
                if(mStep!=0){
                  engineCount=40;
                  ValveSet.FL=0;  
                  ValveSet.FR=0; 
                  ValveSet.RL=0;   
                  ValveSet.RR=0; 
                  ValveSet.RELAY=3; 
                  fVAGBlockWork();
                  GetPressure(); 
                }
                mStep=0;
                break;
            case 44:
                fLCDView4HwInt();
                if(WAIT>2){
                  lcd.setCursor(5,0); lcd.print("Set LS"); 
                  lcd.setCursor(5,1);
                  switch(mStep){
                    case 1:    lcd.print("min F");  //min F
                      break;
                    case 2:    lcd.print("min R");  //min R
                      break;
                    case 3:    lcd.print("max F");  //max F
                      break;
                    case 4:    lcd.print("max R");   //max R
                      break;  
                    default:
                        lcd.print("      ");  
                        if(WAIT<5){Get_RAW_LS();WAIT=15;}                                  
                  }
                }else{
                  Get_RAW_LS();
                  fSaveMinMax(mStep);
                  WAIT=15;
                  mStep=0;
                }
                break;
            
            default: 
                if(WAIT>6)  WAIT=5;
                lcd.clear();
                lcd.setCursor(5,0); lcd.print(" MENU");
                lcd.setCursor(4,1); lcd.print(" "); lcd.print(menu); lcd.print(" "); lcd.print(mStep); lcd.print(" ");
                menu=1;
                break;
          }
          
    if(WAIT<2) 
          {  
                  ValveSet.FL=0;  
                  ValveSet.FR=0; 
                  ValveSet.RL=0;   
                  ValveSet.RR=0; 
                  ValveSet.RELAY=4; 
              lcd.clear();   menu=0; 
              tone(piezoPin, 1100, 110); delay(300); tone(piezoPin, 1000, 110); 
          }              
    fVAGBlockWork();
  }
 
 
 delay(150);
}
