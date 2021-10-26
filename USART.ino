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


void SerialLevelSend2HU(char nom, int8_t cLevels[4]) {
    Serial.print("@l:");Serial.print(nom);Serial.print(":");
    if(cLevels[0]<15)Serial.print('0'); Serial.print(cLevels[0],HEX);
    if(cLevels[1]<15)Serial.print('0'); Serial.print(cLevels[1],HEX); 
    if(nom=='c')
    {
      if(cLevels[2]<15)Serial.print('0'); Serial.print(cLevels[2],HEX);
      if(cLevels[3]<15)Serial.print('0'); Serial.print(cLevels[3],HEX); 
    }
    Serial.println("^");
    Serial.flush();
}

void SerialAlertSend2HU(char pp[], int var){
    Serial.print("@a:");
    //Serial.print(pp);Serial.print(":");
    //Serial.print(var,HEX);
    Serial.println("^");
    Serial.flush();
}
//void fUSART(){
//  
//}
