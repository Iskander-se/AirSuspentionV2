#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ADC_P(0x48);
Adafruit_ADS1115 ADC_B(0x49);

void Get_RAW_LS(){     
      curSuspention[0].RAW=ADC_B.readADC_SingleEnded(1)/24;
      curSuspention[1].RAW=ADC_B.readADC_SingleEnded(3)/24;
      curSuspention[2].RAW=ADC_B.readADC_SingleEnded(2)/24;
      curSuspention[3].RAW=ADC_B.readADC_SingleEnded(0)/24;       
}

void GetPressure()
{
    int16_t ADC_P0 = ADC_P.readADC_SingleEnded(0); 
    int16_t ADC_P1 = ADC_P.readADC_SingleEnded(1);
      
    Pressure.RES=map(ADC_P0, 2650, 23962, 0, 1034);     
    Pressure.VAG=map(ADC_P1, 3394, 8890, 220, 1070);   
    //if(Pressure.RES<500) fAlarm(2);    
}

int GetBankPressure(byte bank)
{
  digitalWrite(vRES, 0); 
  digitalWrite(vPC, 0);  
  digitalWrite(vEXH, 0);
  digitalWrite(vFL, bank==0);  
  digitalWrite(vFR, bank==1);
  digitalWrite(vRL, bank==2);  
  digitalWrite(vRR, bank==3);
  delay(1100);
  GetPressure();
  delay(200);
  digitalWrite(vFL, 0);  
  digitalWrite(vFR, 0);
  digitalWrite(vRL, 0);  
  digitalWrite(vRR, 0);
  return Pressure.VAG;
}

void Get4Pressure(){
       
      for(byte i=0;i<4;i++)  { 
        digitalWrite(vRES, 0); 
        digitalWrite(vPC, 0);  
        digitalWrite(vEXH, 1);
        delay(1500);
        curArr[i] = GetBankPressure(i);
        }   
      digitalWrite(vFL, 0);  
      digitalWrite(vFR, 0);
      digitalWrite(vRL, 0);  
      digitalWrite(vRR, 0);
      
      fLCDViewLevelsFbyte(curArr);  
}

void CalcLevels()
{
  byte i;
  int16_t cVar;
  Get_RAW_LS();
  for(i=0;i<4;i++){
    cVar=map(curSuspention[i].RAW, LowLevels[i], HighLevels[i], 0, 1000);
    curSuspention[i].Avg=(curSuspention[i].Avg*2+cVar)/3;
    curSuspention[i].Max=(curSuspention[i].Max<cVar)? cVar:(curSuspention[i].Max*5+curSuspention[i].Avg)/6;
    curSuspention[i].Min=(curSuspention[i].Min>cVar)? cVar:(curSuspention[i].Min*5+curSuspention[i].Avg)/6;
    curSuspention[i].Delta=(curSuspention[i].Max-curSuspention[i].Min)/2;

    if(curSuspention[i].Max>102) alertHiFlags[i]++; //сранение с верхним пределом  
    if(curSuspention[i].Min<-2)  alertLoFlags[i]++; //сранение с нижним пределом

    if((alertHiFlags[i]>5)&&!servicemode) { alertHiFlags[i]=0; cWarningArr.Levels++;}
    if((alertLoFlags[i]>5)&&!servicemode) { alertLoFlags[i]=0; cWarningArr.Levels++;}
  }
}

void GetSA()
{    
  if(analogRead(sensEngine)<200)
  {
    engineCount++;
    if(engineCount>1000) { engineCount=500; }
  }
  else engineCount=0;    

  if(analogRead(sensBrake)>200)
  {
    brakeCount++;
    if(brakeCount>1000) { brakeCount=500; }
  }
  else brakeCount=0; 
}
