SerialPack SerialReadHU()
{
  char bufferArr[100], ch;
  SerialPack cPack;
  cPack.command=0;
  int i=0;

  
    while( Serial.available()) {
    delay(1);
    ch = Serial.read();
        if(ch=='^') {cPack.command=bufferArr[1]; break;}
        if(ch=='@') i=0;
        if(ch>30) bufferArr[i++] = ch;  
     }
  if(i>0)
  {
       Serial.print("packFromHU_L="); 
       Serial.println(i); 
       if(i==5){
                cPack.nom=bufferArr[2]-'0';
       }
       else if (i==8){
              cPack.nom=bufferArr[2]-'0'; 
              if(cPack.command=='m'){    
                cPack.data[0]=bufferArr[4]-'0';
                cPack.data[1]=bufferArr[5]-'0';
                cPack.data[2]=bufferArr[6]-'0';
                cPack.data[3]=bufferArr[7]-'0';
              }else{
                cPack.data[0]=Dchar2hex(bufferArr[4], bufferArr[5]);
                cPack.data[1]=Dchar2hex(bufferArr[6], bufferArr[7]);
              }
       }else{
            cPack.nom=bufferArr[2]-'0';
            if(cPack.command=='c'){ 
                cPack.data[0]=bufferArr[4];
                cPack.data[1]=bufferArr[5];
                cPack.data[2]=0;
                cPack.data[3]=0;
                }
       }
       Serial.print("SerialReadHU");Serial.print(cPack.nom);Serial.println("");
  }  
  return cPack;
  // @m1:1100^
  // @s1:*^   // установить настройки
  // @r1:*^
  // @ca:*^
  // @l*:*^
  // @g*:*^   //GetPressure
  // @setup^
  //   @w0:0507^ @w1:1011^ @w2:1920^ @w3:2323^ @w4:2A2A^ @w5:3232^ @w6:4141^ @w7:4B4B^ @w8:5959^  
  // @vu:1111^
}

void SerialCurLevelSend2HU(int16_t cLevels[4]) {
    Serial.print("@l:c:");
    if(cLevels[0]<15)Serial.print('0'); Serial.print(cLevels[0],HEX);
    if(cLevels[1]<15)Serial.print('0'); Serial.print(cLevels[1],HEX); 
    if(cLevels[2]<15)Serial.print('0'); Serial.print(cLevels[2],HEX);
    if(cLevels[3]<15)Serial.print('0'); Serial.print(cLevels[3],HEX); 
    Serial.println("^");
    Serial.flush();
}

void SerialLevelSend2HU(char nom, int8_t cLevels[4]) {
    Serial.print("@l:");Serial.print(nom);Serial.print(":");
    if(cLevels[0]<15)Serial.print('0'); Serial.print(cLevels[0],HEX);
    if(cLevels[1]<15)Serial.print('0'); Serial.print(cLevels[1],HEX); 
    Serial.println("^");
    Serial.flush();
}

void SerialWorkSend2HU(valveSetBstruct ValveSet){
    Serial.print("@w:b:");
    Serial.print(ValveSet.RELAY);
    Serial.print(ValveSet.FL);
    Serial.print(ValveSet.FR);
    Serial.print(ValveSet.RL);
    Serial.print(ValveSet.RR);
    if (ValveSet.RELAY == 1 && Pressure.RES > 810) Serial.print("7");
    else Serial.print(ValveSet.WP);
    Serial.println("^");
    Serial.flush();
}


void SerialAlertSend2HU(String pp, String str){
    Serial.print("@a:");
    Serial.print(pp);Serial.print(":");
    Serial.print(str);
    Serial.println("^");
    Serial.flush();
}
