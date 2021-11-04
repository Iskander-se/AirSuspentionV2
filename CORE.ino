byte defDeltaSuspention = 21;

void fLevelBain()
{
  int16_t frontDeviation= (curSuspention[0].Avg + curSuspention[1].Avg) - PresetLevels[0]*20;
  int16_t rearDeviation = (curSuspention[2].Avg + curSuspention[3].Avg) - PresetLevels[1]*20;
  int16_t twixDelta = frontDeviation - rearDeviation;
  int16_t targetLevels[4]={PresetLevels[0]*10,PresetLevels[0]*10,PresetLevels[1]*10,PresetLevels[1]*10};
  //int16_t frontDelta= (curSuspention[0].Delta+curSuspention[1].Delta)/2;
  byte absFDeviation = abs(frontDeviation)/5;
  byte absRDeviation = abs(rearDeviation)/5;
  byte intentFDelta = curSuspention[0].Delta+curSuspention[1].Delta;
  byte intentRDelta = curSuspention[2].Delta+curSuspention[3].Delta;
  int16_t heelFront =curSuspention[0].Avg - curSuspention[1].Avg;
  int16_t heelRear =curSuspention[2].Avg - curSuspention[3].Avg;
  byte blurF=5;
  byte blurR=5;

  //blur if extra suspension position
  if((curSuspention[0].Max>1000)||(curSuspention[1].Max>1000)) {frontDeviation=frontDeviation-cWarningArr.Levels*blurF; blurF=2;};
  if((curSuspention[2].Max>1000)||(curSuspention[3].Max>1000)) {rearDeviation=frontDeviation-cWarningArr.Levels*blurR;  blurR=2;};
  if((curSuspention[0].Min<0)||(curSuspention[1].Min<0)) frontDeviation=frontDeviation+cWarningArr.Levels*blurF;
  if((curSuspention[2].Min<0)||(curSuspention[3].Min<0)) rearDeviation=frontDeviation+cWarningArr.Levels*blurR;
  
  ///////SERVICEMOD ===========
  //Нужна скорейшая реакция
  if(servicemode){
    intentFDelta=0;
    intentRDelta=0;
  }

  // twixDelta //// front < 0 < rear   
  // _Deviation - отклонение от плана
  // intent_Delta - minimax дребезг
  // twixDelta - разница отклонений от планам по осям
  
  if ((frontDeviation < -defDeltaSuspention) || (rearDeviation < -defDeltaSuspention)) {  //    _ x _   ///////////////////////////////////// если низко
      //    _ x ~   /////////////////////////////////////      
      if ((twixDelta < 0 && intentFDelta < absFDeviation) || (twixDelta < defDeltaSuspention*3 && IntentSetBL.CurRELAY == 1)){
        IntentSetBL.FL+=(curSuspention[0].Avg<targetLevels[0])? 1 : -2;
        IntentSetBL.FR+=(curSuspention[1].Avg<targetLevels[1])? 1 : -2;
        IntentSetBL.RL=0; 
        IntentSetBL.RR=0;
        IntentSetBL.CurRELAY = 1;
      }else if ( intentRDelta < absRDeviation) {  //    ~ x _   /////////////////////////////////////
        IntentSetBL.FL=0; 
        IntentSetBL.FR=0;
        IntentSetBL.RL+=(curSuspention[2].Avg<targetLevels[2])? 1 : -2;
        IntentSetBL.RR+=(curSuspention[3].Avg<targetLevels[3])? 1 : -2;
        IntentSetBL.CurRELAY = 2;
      }
      
  } else if ((frontDeviation > defDeltaSuspention) || (rearDeviation > defDeltaSuspention)) { //   ^ x ^   ///////////////////////////////////// если высоко
       //    ^ x -   /////////////////////////////////////
      if ((twixDelta > 0 && intentFDelta < absFDeviation) || (twixDelta > -defDeltaSuspention*3 && IntentSetBL.CurRELAY == 3)|| (waitLowUpR&&!waitLowUpF)){
        IntentSetBL.FL+=(curSuspention[0].Avg>targetLevels[0])? 1 : -2;
        IntentSetBL.FR+=(curSuspention[1].Avg>targetLevels[1])? 1 : -2; 
        IntentSetBL.RL=0; 
        IntentSetBL.RR=0;       
        IntentSetBL.CurRELAY = 3;
      }else if( intentRDelta < absRDeviation) {  //    - x ^   ///////////////////////////////////// 
        IntentSetBL.FL=0; 
        IntentSetBL.FR=0;       
        IntentSetBL.RL+=(curSuspention[2].Avg>targetLevels[2])? 1 : -2;
        IntentSetBL.RR+=(curSuspention[3].Avg>targetLevels[3])? 1 : -2;
        IntentSetBL.CurRELAY = 4;
      }
      
  } else {
    IntentSetBL.FL=0; IntentSetBL.FR=0;
    IntentSetBL.RL=0; IntentSetBL.RR=0;
    if(intentFDelta<15){
     //крен 
      if(heelFront>60&&heelRear>0) {       ////Крен в право 
        IntentSetBL.FL=10; IntentSetBL.FR=0;
        IntentSetBL.RL=0; IntentSetBL.RR=10;
        IntentSetBL.CurRELAY = 0;
      }
      else if(heelFront<-60&&heelRear<0) { ////Крен в лево 
        IntentSetBL.FL=0; IntentSetBL.FR=10;
        IntentSetBL.RL=10; IntentSetBL.RR=0;
        IntentSetBL.CurRELAY = 0;
      }
    }
  }


  if(IntentSetBL.FL>50) IntentSetBL.FL=50; 
  if(IntentSetBL.FR>50) IntentSetBL.FR=50;
  if(IntentSetBL.RL>50) IntentSetBL.RL=50; 
  if(IntentSetBL.RR>50) IntentSetBL.RR=50;
  if(IntentSetBL.FL<1) IntentSetBL.FL=0; 
  if(IntentSetBL.FR<1) IntentSetBL.FR=0;
  if(IntentSetBL.RL<1) IntentSetBL.RL=0; 
  if(IntentSetBL.RR<1) IntentSetBL.RR=0;  

  //Если намерения не ясны => их нет
  if(IntentSetBL.FL==0&&IntentSetBL.FR==0&&IntentSetBL.RL==0&&IntentSetBL.RR==0)    IntentSetBL.CurRELAY = 0;
}

void fSUBcore() {
  int suff=2;
  // 1: Front up;    2: Rear up;     3: Front down;    4: Rear down;
  ValveSet.CR=ValveSet.RELAY;

  if(brakeCount<5){
    ValveSet.FL=0;
    ValveSet.FR=0;
    ValveSet.RL=0;
    ValveSet.RR=0;
    ValveSet.RELAY = 4;
    fVAGBlockWork();
    return;
  }
  switch (IntentSetBL.CurRELAY)
  {
    case 1 : // Front up
        if (Pressure.VAG > 150 && Pressure.VAG < 760) ValveSet.RELAY = 1;  
        else{ValveSet.RELAY = 0; IntentSetBL.HOPE++; if(IntentSetBL.HOPE>suff) cWarningArr.BanksF++; }      
      break;
    case 2:  // Rear up
        if (Pressure.VAG > 140 && Pressure.VAG < 430) ValveSet.RELAY = 1;
        else{ValveSet.RELAY = 0; IntentSetBL.HOPE++; if(IntentSetBL.HOPE>suff) cWarningArr.BanksR++; }     
      break;
    case 3 : // Front down
        if (Pressure.VAG > 200) ValveSet.RELAY = 2;
        else{ValveSet.RELAY = 0; IntentSetBL.HOPE++; if(IntentSetBL.HOPE>suff)cWarningArr.BanksF++; }     
      break;
    case 4:  // Rear down
        if (Pressure.VAG > 180) ValveSet.RELAY = 2;
        else{ValveSet.RELAY = 0; IntentSetBL.HOPE++; if(IntentSetBL.HOPE>suff) cWarningArr.BanksR++; }     
      break;
    default:
        IntentSetBL.HOPE = 0;
        ValveSet.RELAY = 4;
  }
  
  if(cWarningArr.BanksF>0||cWarningArr.BanksR>0) {
      lcd.setCursor(4,0);  lcd.print(" "); lcd.print(Pressure.VAG); lcd.print(" "); lcd.print(IntentSetBL.CurRELAY);lcd.print("A");
  }
  if(ValveSet.CR!=ValveSet.RELAY&&ValveSet.RELAY==4) { cWarningArr.Valves=0; IntentSetBL.HOPE = 0;}
  else if((ValveSet.RELAY==1)||(ValveSet.RELAY==2)) cWarningArr.Valves++;

  if(IntentSetBL.FL>3) ValveSet.FL=1;
  else if(IntentSetBL.FL<2) ValveSet.FL=0;
  if(IntentSetBL.FR>3) ValveSet.FR=1;
  else if(IntentSetBL.FR<2) ValveSet.FR=0;
  if(IntentSetBL.RL>3) ValveSet.RL=1;
  else if(IntentSetBL.RL<2) ValveSet.RL=0;
  if(IntentSetBL.RR>3) ValveSet.RR=1;
  else if(IntentSetBL.RR<2) ValveSet.RR=0;

  ///////SERVICEMOD ===========
  ////   3: Front down;    4: Rear down;
  //Разгрузка подушек висящих на подьемнике или домкрате
  if(waitLowUpF&&(IntentSetBL.CurRELAY == 3)) {
    ValveSet.FL=0; ValveSet.FR=0;
    ValveSet.RELAY=0;
    cWarningArr.Valves=0;
  }
  if(waitLowUpR&&(IntentSetBL.CurRELAY == 4)) {
    ValveSet.RL=0; ValveSet.RR=0;
    ValveSet.RELAY=0;
    cWarningArr.Valves=0;
  }

  if(Pressure.VAG>350){
    waitLowUpF=false;
    waitLowUpR=false;
  }
  
}

void fVAGBlockWork() {
  if((ValveSet.RELAY!=4&&tasker1%2)||(tasker1==2)) SerialWorkSend2HU(ValveSet);
  GetPressure();
  if(ValveSet.FL||ValveSet.FR||ValveSet.RL||ValveSet.RR){
      switch (ValveSet.RELAY){
          case 1 : //UP
              digitalWrite(vEXH, 0);
              if (Pressure.RES > 810) { ///  Работаем от рессивера
                digitalWrite(vRES, 1);
                digitalWrite(vPC, 0);
              } else if (Pressure.RES < 630) { ///  Работаем без рессивера
                digitalWrite(vRES, 0);
                digitalWrite(vPC, 1);
              } else {// Компрессор + рессивер
                ValveSet.WP=1;
                digitalWrite(vRES, 1);                
                digitalWrite(vPC, AirPower.RELAY); 
              }
              break;
          case 2:  //DOWN
              digitalWrite(vEXH, 1);
              digitalWrite(vRES, 0);
              digitalWrite(vPC, 0);
            break;
          default:
              digitalWrite(vEXH, 0);
              digitalWrite(vRES, 0);
              digitalWrite(vPC, 0);
        }
  }else{
    switch (ValveSet.RELAY){
      case 3:  //CRARGE 
            AirPower.RELAY = true;
            fChargeRES();
          break;
      case 4:  //FREE2CRARGE
          if ((Pressure.RES > 250 && Pressure.RES < 860) || ValveSet.WP == 1) {
            ValveSet.WP = 1;
            fChargeRES();
          }
          else {     
            digitalWrite(vEXH, 0);    digitalWrite(vRES, 0);    digitalWrite(vPC, 0);     
          }
        break;
      case 5:  //Transit
          digitalWrite(vEXH, 0);
          digitalWrite(vRES, 1);
          digitalWrite(vPC, 0);
        break;
      default:
          digitalWrite(vEXH, 0);
          digitalWrite(vRES, 0);
          digitalWrite(vPC, 0);
    }
  }
  lcd.setCursor(0, 0);  if (ValveSet.FL) lcd.write(0xFF); else lcd.print(" ");
  lcd.setCursor(15, 0); if (ValveSet.FR) lcd.write(0xFF); else lcd.print(" ");
  lcd.setCursor(0, 1);  if (ValveSet.RL) lcd.write(0xFF); else lcd.print(" ");
  lcd.setCursor(15, 1); if (ValveSet.RR) lcd.write(0xFF); else lcd.print(" ");
  digitalWrite(vFL, ValveSet.FL);
  digitalWrite(vFR, ValveSet.FR);
  digitalWrite(vRL, ValveSet.RL);
  digitalWrite(vRR, ValveSet.RR);
  if(IntentSetBL.HOPE>1){
    delay(100);
  }
}

/////////////////////////////////////////////////////////////////////////////////////

void fCompressorTimer() {
  if(engineCount < 50) {
    AirPower.RELAY = false;
    return;
  }
  
   if (ValveSet.WP && AirPower.RELAY) {
    AirPower.Time++;
    if (AirPower.Time > CompressorTimeLimit) {
      AirPower.RELAY = false;
      cWarningArr.Power++;
    }
  }   
  else {
    if (AirPower.Time < 2) {AirPower.RELAY = true; if(!ValveSet.WP&&cWarningArr.Power>0) cWarningArr.Power--;}
    else AirPower.Time--;
  }
}

void fChargeRES()
{
  if (Pressure.RES > 960 && ValveSet.RELAY != 3) {
    ValveSet.WP=0;
    lcd.setCursor(5,1);
    lcd.print("        ");    
    digitalWrite(vEXH, 1);
    digitalWrite(vRES, 0);
    digitalWrite(vPC, 0);
    delay(800);
    if(cWarningArr.Power>0) cWarningArr.Power--;
    if(cWarningArr.Power>0) cWarningArr.Power--;
    if(cWarningArr.Power>0) cWarningArr.Power--;
    digitalWrite(vEXH, 0);
    digitalWrite(vRES, 0);
    digitalWrite(vPC, 0);
    return;
  } else {     
    digitalWrite(vEXH, 0);
    digitalWrite(vRES, AirPower.RELAY);
    digitalWrite(vPC, AirPower.RELAY);
  }
  return;
}
