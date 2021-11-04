// 2 знака => байт
byte Dchar2hex(char ch1,char ch2){
  int val=0;
  if (ch1 >= 'A' && ch1 <= 'F') val=(ch1-'A'+10)<<4;      
  else val=(ch1-'0')<<4;
  if (ch2 >= 'A' && ch2 <= 'F') val+=ch2-'A'+10;      
  else val+=ch2-'0';
  return val;
}


void CheckAlerts(){
  if(cAlertArr.Power.flag)  SerialAlertSend2HU("pc", cAlertArr.Power.dump);
  if(cAlertArr.Levels.flag)  SerialAlertSend2HU("lv", cAlertArr.Levels.dump);
  if(cAlertArr.Valves.flag)  SerialAlertSend2HU("vb", cAlertArr.Valves.dump);
  if(cAlertArr.BanksF.flag)  SerialAlertSend2HU("bf", cAlertArr.BanksF.dump);
  if(cAlertArr.BanksR.flag)  SerialAlertSend2HU("br", cAlertArr.BanksR.dump);
}

void ConfirmAlerts(String stringOne){
  if(stringOne=="pc")  {
    SerialAlertSend2HU("pc", cAlertArr.Power.dump);
  }
  if(stringOne=="lv")  {
    SerialAlertSend2HU("lv", cAlertArr.Levels.dump);
  }
  if(stringOne=="vb")  {
    cWarningArr.Valves=0;
    cAlertArr.Valves.flag=false;
  }
  if(stringOne=="bf")  {
    SerialAlertSend2HU("bf", cAlertArr.BanksF.dump);
  }
  if(stringOne=="br")  {
    SerialAlertSend2HU("br", cAlertArr.BanksR.dump);
  }
}

void CheckWarnings()
{
  String stringOne = "";
  if(cWarningArr.Levels>0) cWarningArr.Levels--;
  if(cWarningArr.Levels>8) {
    cWarningArr.Levels=0;
    if(!cAlertArr.Levels.flag){
       cAlertArr.Levels.flag=true;
       for(int i=0;i<4;i++){
          stringOne=String(stringOne+"["+String(curSuspention[i].Min, DEC)+":"+String(curSuspention[i].Max, DEC)+"],");
       }
       cAlertArr.Levels.dump=String(stringOne+":"+String(Pressure.VAG, DEC)); 
       SetManualMode();
    }
  }  

  if(cWarningArr.Power>6) {
     cAlertArr.Power.flag=true;
     stringOne=String(Pressure.VAG, DEC); 
     cAlertArr.Power.dump=String(stringOne+":"+String(Pressure.RES, DEC)); 
  }  

  //Ошибка отсутствия реакции - это страшно всегда.
  if(cWarningArr.Valves>15) {
    cWarningArr.Valves=0;
    if(!cAlertArr.Valves.flag){
       cAlertArr.Valves.flag=true;
       stringOne=String(Pressure.VAG,DEC);
       stringOne=stringOne+"["+String(ValveSet.FL,DEC)+":"+String(ValveSet.FR,DEC)+":"+String(ValveSet.RL,DEC)+":"+String(ValveSet.RR,DEC)+"::"+String(ValveSet.RELAY,DEC)+":"+String(ValveSet.WP,DEC)+"]"+String(Pressure.RES,DEC); 
       cAlertArr.Valves.dump=stringOne;
       SetManualMode();
    }
  }  
  if(servicemode) {
    if(cWarningArr.BanksF>1&&IntentSetBL.CurRELAY == 3) {cWarningArr.BanksF=0; cWarningArr.Valves=0; waitLowUpF=true;}
    if(cWarningArr.BanksR>1&&IntentSetBL.CurRELAY == 4) {cWarningArr.BanksR=0; cWarningArr.Valves=0; waitLowUpR=true;}
    return;
  } //Если в SERVICEMOD то все допустимо и дальше не смотрим
  
  if(cWarningArr.BanksF>3) {
    cWarningArr.BanksF=0;
    if(!cAlertArr.BanksF.flag){
      cAlertArr.BanksF.flag=true;
      stringOne=String(Pressure.VAG,DEC);
      stringOne=stringOne+"["+String(curSuspention[0].Avg,DEC);
      stringOne=stringOne+":"+String(curSuspention[1].Avg,DEC)+"]"; 
      cAlertArr.BanksF.dump=String(stringOne+":"+String(Pressure.RES, DEC)); 
      SetManualMode();
    }
  }
  if(cWarningArr.BanksR>3) {
    cWarningArr.BanksR=0;
    if(!cAlertArr.BanksR.flag){
      cAlertArr.BanksR.flag=true;
      stringOne=String(Pressure.VAG,DEC);
      stringOne=stringOne+"["+String(curSuspention[2].Avg,DEC);
      stringOne=stringOne+":"+String(curSuspention[3].Avg,DEC)+"]"; 
      cAlertArr.BanksR.dump=String(stringOne+":"+String(Pressure.RES, DEC));  
      SetManualMode();
    }
  }
  

}

void SetManualMode()
{
    manual=true;
    mute=false;
    EEPROM.update(ManualAddr,1);
    ValveSet.FL=0;  ValveSet.FR=0;
    ValveSet.RL=0;  ValveSet.RR=0;
    ValveSet.RELAY = 4;
}

void GetKey()
{
  if (digitalRead(bSETpin)!=HIGH&&!bs) {
    tone(piezoPin, 400, 10);
    WAIT=40;
    if(!mute) mute=true;
    menu++;
    
    bs=true;
    mStep=0;
    
  }else bs=false;

  if (digitalRead(bN1pin)!=HIGH&&!b1) {
    tone(piezoPin, 200, 10);
    WAIT=10;
    b1=true;
    mStep--;
  }else b1=false;

  if (digitalRead(bN2pin)!=HIGH&&!b2) {
    tone(piezoPin, 300, 10);
    WAIT=10;
    b2=true;
    mStep++;
  }else b2=false;

}
