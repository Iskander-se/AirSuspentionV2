// 2 знака => байт
byte Dchar2hex(char ch1,char ch2){
  int val=0;
  if (ch1 >= 'A' && ch1 <= 'F') val=(ch1-'A'+10)<<4;      
  else val=(ch1-'0')<<4;
  if (ch2 >= 'A' && ch2 <= 'F') val+=ch2-'A'+10;      
  else val+=ch2-'0';
  return val;
}

void CheckWarnings()
{
  if(servicemode) return; //Если SERVICEMOD то все допустимо
  
  if(cWarningArr.BanksF>5) {
    manual=true;
     EEPROM.update(ManualAddr,1);
     ValveSet.FL=0;  ValveSet.FR=0;
     ValveSet.RL=0;  ValveSet.RR=0;
     ValveSet.RELAY = 0;
  }
  if(cWarningArr.BanksR>5) {
    manual=true;
     EEPROM.update(ManualAddr,1);
     ValveSet.FL=0;  ValveSet.FR=0;
     ValveSet.RL=0;  ValveSet.RR=0;
     ValveSet.RELAY = 0;
  }
  if(cWarningArr.Valves>5) {
    manual=true;
     EEPROM.update(ManualAddr,1);
     ValveSet.FL=0;  ValveSet.FR=0;
     ValveSet.RL=0;  ValveSet.RR=0;
     ValveSet.RELAY = 0;
  }

}

void GetKey()
{
  if (digitalRead(bSETpin)!=HIGH&&!bs) {
    tone(piezoPin, 400, 10);
    WAIT=40;
    if(!mute) mute=true;
    else menu++;
    
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
