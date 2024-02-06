
class BytePack{
  public:
  byte statusByte;
  byte valueByte;
  byte IDByte;
  BytePack(byte IDByteInit, byte ValueByte, byte StatusByte) {
    IDByte=IDByteInit;
    statusByte=StatusByte;
    valueByte=ValueByte;
  }
};


class Switch{
  public:
  int valuePort;
  int checkPort;
  int ID;
  int powerPort;
  boolean isBool;
  
  Switch(int inputID, byte IsBool, int PowerPort, int CheckPort, int ValuePort){
    valuePort=ValuePort;
    checkPort=CheckPort;
    ID=inputID;
    powerPort=PowerPort;
    isBool=(IsBool==255);
    SetPower(true);
  }
  
   BytePack getValue(){  
    if (isBool){
      return getBoolValue();
    }
    return getIntValue();
  }

   BytePack getBoolValue(){
    if (digitalRead(checkPort)||digitalRead(valuePort)){
      return BytePack(ID, boolToByte(digitalRead(valuePort)), 255);
    }
    return BytePack(ID, boolToByte(digitalRead(valuePort)), 0);
  }
  
  BytePack getIntValue(){
    if (digitalRead(checkPort)||digitalRead(valuePort)){
      return BytePack(ID, analogRead(valuePort)/4, 255);
    }
    return BytePack(ID, analogRead(valuePort)/4, 0);
  }


  byte boolToByte(boolean toConvert){
    if (toConvert){
      return 255;
    }
    return 0;
  }

  void SetPower(boolean setVal){
    digitalWrite(powerPort, setVal);
  }
  
};


Switch *SwitchArray;
int SwitchCount;

/*
 * expeted file format
 * 0x00, 0x00, 0x00 Pattern to help the reader identify the start of a package
 * 1 byte to tell the compiler how long to expect the package to be, does not include headers or footers but does include the length value itself
 * 5 byte packages for the deffinition of switches ordered ID, Type, powerPort, CheckPort, ValuePort
 * 0x255, 0x255, 0x255 Pattern to conferm the ending of a package
 * 
 */
void setup() {
  //sets up the serial conntection
  Serial.begin(9600);

  Serial.write("connected");
  //filters out bytes until the package starting pattern 0x00, 0x00, 0x00 is found 
  //byte * InitPackage=readPackage();

  byte InitPackage[]={6, 01, 255, 22, 34, 35};
  while (InitPackage==NULL){
    Serial.println("bad or missing package");
    //InitPackage=readPackage();
  }

   int InitPackageLen=InitPackage[0];
   SwitchCount=(InitPackageLen-1)/5;
   SwitchArray=(Switch*)malloc(SwitchCount * sizeof(Switch));

   //reads groups of five bytes into switch objects to be stored for loop
   for (int i = 1, n=0; n<SwitchCount; i+=5, n++){
      SwitchArray[n]=*new Switch(InitPackage[i], InitPackage[i+1], InitPackage[i+2], InitPackage[i+3], (InitPackage[i+4]));
    }

   //sends back the recived package so the rio can conferm the package was correctly recived
   Serial.write(InitPackage, InitPackageLen);
   Serial.println("package Recived");
   digitalWrite(30, true);                                                
 
    
  
  
}


//returns a list of all of the bytes in a package with the first index being the length byte
byte* readPackage(){
   boolean packageHasBeenFound=false;
   byte *ByteArray;

   //byte ByteArray[]={6, 01, 00, 22, 34, 35};
   return ByteArray;

   
   while (!packageHasBeenFound){
    int initByteCount = 0;
    while (initByteCount<3){
      if (Serial.available()){
        if (Serial.read()==0){
          initByteCount++;
        }
      }
      else{
        
        return NULL;
        
      }
    }
  
    //reads the number of bytes to be recived in the package
    int ByteArrayLen=Serial.read();
    
    ByteArray = (byte*)malloc(ByteArrayLen);
//   
    ByteArray[0]=ByteArrayLen;
    //reads the package bytes into a list
    for (int i = 0; i<ByteArrayLen; i++){
      ByteArray[i]=Serial.read();
    }
  
    packageHasBeenFound=true;
    
    for (int i = 0; i<3; i++){
      if (Serial.read()!=255){
        packageHasBeenFound=false;
      }
    }
      
  }
  return ByteArray;
  
}


void loop() {
  byte packageToBeSent[7+SwitchCount*3];

  //places package start pattern
  //packageToBeSent[0]=0;
  //packageToBeSent[1]=0;
  //packageToBeSent[2]=0;

  //places package byte count
  packageToBeSent[3]=SwitchCount*3+1;

  //places package end pattern
  //packageToBeSent[-1]=255;
  //packageToBeSent[-2]=255;
  //packageToBeSent[-3]=255;
  
  for (int i=0, n=4; i<SwitchCount; i++, n+=3){
    BytePack CurrentPack = SwitchArray[i].getValue();
    packageToBeSent[n]=CurrentPack.IDByte;
    packageToBeSent[n+1]=CurrentPack.statusByte;
    packageToBeSent[n+2]=CurrentPack.valueByte;
  }

  Serial.write(packageToBeSent, 1+SwitchCount*3);
  if(packageToBeSent[2]==255){
    digitalWrite(30, true);
  }
  Serial.println(SwitchArray[0].isBool);
  delay(20);


}
