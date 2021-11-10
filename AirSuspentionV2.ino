#include <LiquidCrystal_I2C_OLED.h>
#include <EEPROM.h> 

#define DEBUG

////////// HardWare
const byte sensSuspention[4] = {A3,A2,A0,A1}; // потомучто так!
const byte sensEngine = A0; 
const byte sensBrake = A2; 
const byte piezoPin = 10;
const byte bSETpin = 12;
const byte bN1pin = 13;
const byte bN2pin = 11;
const int vFR = 4;
const int vRR = 6;
const int vRES = 8; // 
const int vEXH = 3; //
const int vRL = 7;  
const int vPC = 2;  // реле компрессора
const int vFL = 5; 


////////// Constants
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
byte up[] =   {  B00000,  B00100,  B01110,  B10101,  B00100,  B00100,  B00000,  B00000};
byte down[] = {  B00000,  B00000,  B00100,  B00100,  B10101,  B01110,  B00100,  B00000};
const byte levelHwAddress[3]={4,12,20};          // Переменная для хранения первых адресов
const int8_t CompressorTimeLimit=50;
const byte PresetAddr=30;
const byte ManualAddr=31;
const byte firstValueAddress=60;

///////// Variables
int8_t PresetLevels[2]={50,50};
int8_t nomPreset=0; 
bool manual=false;
bool servicemode=false;
bool waitLowUpF=false;
bool waitLowUpR=false;
byte alertHiFlags[4];
byte alertLoFlags[4];
int16_t LowLevels[4]={200,934,925,196}; 
int16_t HighLevels[4]={967,194,187,930}; 
int16_t curArr[4];
int16_t engineCount=0, brakeCount=0, tasker1=0, tasker2=0;
struct stJack {
  int16_t RAW;
  int16_t Min; 
  int16_t Max;
  int16_t Avg; 
  int16_t Delta;
  int16_t Intent;
} curSuspention[4];

struct intentStruct {
  int FL; int FR;
  int RL; int RR;
  byte CurRELAY; 
  byte HOPE;
  bool startI;
} IntentSetBL ={0,0,0,0,0,0,1};

struct WarningArray {
    byte Levels=0;
    byte Valves=0;
    byte BanksF=0;
    byte BanksR=0;
    byte Power=0;
} cWarningArr;

struct Alert {
  bool flag=false;
  String dump="";
};

struct AlertArray {
    Alert Levels;
    Alert Valves;
    Alert BanksF;
    Alert BanksR;
    Alert Power;
    Alert RES;
} cAlertArr;

struct valveSetBstruct {
  bool FL; bool FR;
  bool RL; bool RR;
  byte RELAY;   // 0=Stop; 1=UP; 2=DOWN; 3=CHARGE; 4=Free
  byte WP;      // 0=Stop; 1=UP; 2=CHARGE
  byte CR;
} ValveSet ={0,0,0,0, 4,0,0};

struct pressure {
    int VAG=500;
    int RES=900;
} Pressure;

struct airpower {
    int Time=0;
    bool RELAY=true;
    int Count=0;
} AirPower; 

struct SerialPack {
    char command;
    byte nom;
    int8_t data[4];
    char str[2];
};
SerialPack packFromHU;

void setup() {
  pinMode(vFL, OUTPUT);
  pinMode(vFR, OUTPUT);
  pinMode(vRL, OUTPUT);
  pinMode(vRR, OUTPUT);
  pinMode(vRES,OUTPUT);
  pinMode(vEXH,OUTPUT);
  pinMode(vPC, OUTPUT);
  digitalWrite(vFL, 0);
  digitalWrite(vFR, 0);
  digitalWrite(vRL, 0);
  digitalWrite(vRR, 0);
  digitalWrite(vRES,0);
  digitalWrite(vEXH, 0);
  digitalWrite(vPC, 0);

  pinMode(bSETpin,INPUT_PULLUP);
  pinMode(bN1pin, INPUT_PULLUP);
  pinMode(bN2pin, INPUT_PULLUP);
  
  Serial.begin(9600);
  lcd.init();
  lcd.clear();
  manual=(EEPROM.read(ManualAddr)==1);
  nomPreset=EEPROM.read(PresetAddr);
  GetLevelHwSetup(); // Инициализация записаных значений
  GetLevelPreset(nomPreset);
  lcd.init();
  lcd.clear();
  delay(150);
  lcd.backlight();
  lcd.setCursor(3,0); 
  lcd.print("GALANT  EA"); 
  lcd.setCursor(1,1); 
  lcd.print("Air Suspention");  
  lcd.createChar(0, up);
  lcd.createChar(1, down);
  GetPressure();
  if(manual) {
     delay(150);
    lcd.setCursor(0,1); lcd.print(" b:"); lcd.print(Pressure.VAG);lcd.print("    ");
    lcd.setCursor(7,1); lcd.print("res:"); lcd.print(Pressure.RES);lcd.print("  ");
    delay(200);
  }
  delay(200);
  for(byte i=0;i<20;i++)CalcLevels();
  lcd.clear();

  for(byte i=0;i<4;i++) alertHiFlags[i]=0;
  for(byte i=0;i<4;i++) alertLoFlags[i]=0;
}


///interfaces
bool bs, b1, b2, mute;
int mStep=0, menu=0, WAIT=0;

//Get_RAW_LS();
//fLCDView4HwInt(RAWLevels);
void loop() {
  GetSA();
  CalcLevels();
if(WAIT>0) WAIT--;

  fLevelBain();  // по высотам создание намерения для VAG-блока
  fCompressorTimer();
  GetPressure();
    
  if(tasker1==8||tasker1==16){
      if(!manual&&!menu)fSUBcore();
      CheckWarnings();
  }

  if(tasker1>16){
      CheckAlerts();
      tasker1=0;
  }
  tasker1++;

  GetKey();
  
  if(menu) fLCDmenu();
  else {  
      packFromHU = SerialReadHU(); //Чтение команд от ГУ
      
      if(packFromHU.command!=0)
      {    byte addr=0;
            Serial.println("packFromHU");
           switch (packFromHU.command){
              case 'w':  /////////////////////////////////////////////////////////////////////////////////////// запись в EEPROM пресета
                addr=firstValueAddress+packFromHU.nom*2;
                EEPROM_Level_write(addr, packFromHU.data);                
                break;  
              case 'r':       /////////////////////////////////////////////////////////////////////////////// чтение из EEPROM пресета на выбор
                addr=firstValueAddress+packFromHU.nom*2;
                int8_t data[2];
                EEPROM_Level_read(addr,data);
                SerialLevelSend2HU(packFromHU.nom+'0',data);
                break;
              case 's':       /////////////////////////////////////////////////////////////////////////////// выбор установленнго пресета
                nomPreset=packFromHU.nom;
                GetLevelPreset(nomPreset); 
                SetLevelPreset(nomPreset);         
                break;
              case 'm':     //@m1:1100^ ///////////////////////////////////////////////////////////////////// ручное управление из головного устройства
                manual=true;
                EEPROM.update(ManualAddr,1);
                ValveSet.RELAY=(packFromHU.nom>0)?1:2;
                ValveSet.WP=0;
                ValveSet.FL=(packFromHU.data[0]>0);  
                ValveSet.FR=(packFromHU.data[1]>0);
                ValveSet.RL=(packFromHU.data[2]>0); 
                ValveSet.RR=(packFromHU.data[3]>0); 
                WAIT=2; 
                fVAGBlockWork();                
                break;  
              case 'g':  ////////////////////////////////////////////////////// ////////////////////////////////////
                //fGetCurValues(packFromHU);
                break;
              case 'v':  ////////////////////////////////////////////////////// ////////////////////////////////////
                //fGetCurValues(packFromHU);
                break;            
              case 'c':  ////////////////////////////////////////////////////// ////////////////////////////////////
                if(packFromHU.nom==49){  //'a'   clear alerts
                  String stringOne = String((char *)packFromHU.data); 
                  ConfirmAlerts(stringOne)
                }
                break;
           }
           packFromHU.command=0;
      }
    
    for(byte i=0;i<4;i++) curArr[i]=curSuspention[i].Avg/10; 
    fLCDViewLevelsFbyte(curArr);   

    if(servicemode)
    {
      lcd.setCursor(5,1); lcd.print("SEVICE ");
    }

    if(manual&&tasker1%2)
              if(ValveSet.WP==1) {
                lcd.setCursor(5,0); lcd.print(Pressure.VAG);lcd.print(" M ");
              }else{
                lcd.setCursor(5,0); lcd.print(Pressure.VAG);lcd.print("kpa ");
              }
              
    if(tasker1%4==0) { 
      tasker2++;
      lcd.setCursor(4,1);
      switch(tasker2)
        {
          case 1:
              if(cAlertArr.Valves.flag) {
                lcd.print("VAG ERR!");
                tasker2=1;
                break;
              }
          case 2:
              if(cAlertArr.BanksF.flag) {
                lcd.print("FB ERR! ");
                tasker2=2;
                break;
              }
          case 3:
              if(cAlertArr.BanksR.flag) {
                lcd.print("RB ERR! ");
                tasker2=3;
                break;
              }
          case 4:
              if(cAlertArr.Power.flag) {
                lcd.print("AIReRR! ");
                tasker2=4;
                break;
              }
          case 5:
              if(cAlertArr.RES.flag) {
                lcd.print("RESeRR!");
                tasker2=5;
                break;
              }
          default:
              if(ValveSet.WP==1) {                
                if(manual) {lcd.print(Pressure.RES);lcd.print(" ");lcd.print(AirPower.RELAY);lcd.print("-");lcd.print(cWarningArr.Power);}
              }else{
                if(manual) lcd.print("Manual ");
                else lcd.print("       ");
              }
              tasker2=0;
        }
        if(tasker2&&!mute)tone(piezoPin, 1100, 110);
    }

    
  }
  
  fVAGBlockWork(); //работа VAG блока  
  delay(50);
}
