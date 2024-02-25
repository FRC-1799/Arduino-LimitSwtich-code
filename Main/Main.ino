
const byte trueVal =0x31;
const byte falseVal = 0x30;
const byte milisTillTrust = 0;

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

//
//
// class inputSwitch{
//  public:
//  byte port;
//  boolean value;
//  byte trust;
//  bool last;
//  bool sample[10];
//  byte count;
//  long ignoreUntil;
//
//  //255 is used as the null value since no arduino has port values that high
//  byte portB=255;
//  inputSwitch(byte Port){
//    port=Port;
//  }
//  inputSwitch(){
//    //just exists so it doesn't get mad at me
//  }
//  inputSwitch(byte PortA, byte PortB){
//     port=PortA;
//     portB=PortB;
//  }
//  void periodic(){
//    bool current = digitalRead(port);
////    if (last!=current){
////      last=current;
////      trust=0;
////      return;
////    }
//    if (portB!=255){
//      sample[count]=digitalRead(port)||digitalRead(portB);
//    }
//    else{
//      sample[count]=digitalRead(port);
//    }
//    
//    count++;
//    if (count==10){
//      byte falseCount;
//      for (byte i =0; i<10; i++){
//        if (sample[i]==false){
//          falseCount++;
//          }
//        }
//        
//      if (falseCount>2){
//        value=false;
//      }
//      else{
//        value=true;
//      }
//    }
//    
////    if (trust==milisTillTrust && value!=current){
////        value=current;
////        return;
////    }
////    trust++;
////
////   if (current!=last){
////    trust=0;
////   }
////   if (trust=milisTillTrust){
////      value=current;
////      trust=0;
////      return;
////    }   
////   }
////   trust++;
//
//   
//  }
//  boolean get(){
//    return digitalRead(port);
//    //return value;
//////     ignore pin changes until after this delay time
////    if (ignoreUntil > millis())
////    {
////      // ignore any changes during this period
////    }
////    
////    // pin has changed 
////    else if (digitalRead(port) != value)
////    {
////      ignoreUntil = millis() + milisTillTrust;
////      value = !value;
////      //_has_changed = true;
////    }
////    
////    return value; 
////    return digitalRead(port);
//  }
//};


class Switch{
  public:
  byte ID;
  byte powerPort;
  boolean isBool;
  byte valuePort;
  byte statusPort;

  
  Switch(int inputID, byte IsBool, byte StatusPort, byte ValuePort){
    
    ID=inputID;
    valuePort=ValuePort;
    statusPort=StatusPort;
    valuePort=ValuePort;

    isBool=(IsBool==trueVal);
    
  }
  
   BytePack getValue(){  
    return BytePack(ID, boolToByte(digitalRead(valuePort)), boolToByte(digitalRead(statusPort)||digitalRead(valuePort)));

  }

   BytePack getBoolValue(){
    return BytePack(ID, boolToByte(digitalRead(valuePort)), boolToByte(digitalRead(statusPort)));
   }
  BytePack getIntValue(){
     return BytePack(ID, analogRead(valuePort)/4, boolToByte(digitalRead(statusPort)||digitalRead(valuePort)));
  }

//  void periodic(){
//    valueSwitch.periodic();
//    statusSwitch.periodic();
//  }


  byte boolToByte(boolean toConvert){
    if (toConvert){
      return trueVal;
    }
    return falseVal;
  }



  
};


Switch *SwitchArray;
byte SwitchCount;
byte count=0;
byte waitCount=20;
double lastMS=0;

/*
 * expeted file format for init
 * 0x00, 0x00, 0x00 Pattern to help the reader identify the start of a package
 * 1 byte to tell the compiler how long to expect the package to be, does not include headers or footers but does include the length value itself
 * 4 byte packages for the deffinition of switches ordered ID, Type, CheckPort, ValuePort
 * 0x255, 0x255, 0x255 Pattern to conferm the ending of a package
 * 
 */

void setup() {
  //sets up the serial conntection
  Serial.begin(9600);

  Serial.write("connected");
  //filters out bytes until the package starting pattern 0x00, 0x00, 0x00 is found 
  //byte * InitPackage=readPackage();

  byte InitPackage[]={6, 65, trueVal, 5, 2};
  while (InitPackage==NULL){
    Serial.println("bad or missing package");
    //InitPackage=readPackage();
  }

   byte InitPackageLen=InitPackage[0];
   SwitchCount=(InitPackageLen-1)/5;
   SwitchArray=(Switch*)malloc(SwitchCount * sizeof(Switch));

   //reads groups of five bytes into switch objects to be stored for loop
   for (byte i = 1, n=0; n<SwitchCount; i+=4, n++){
      SwitchArray[n]=*new Switch(InitPackage[i], InitPackage[i+1], InitPackage[i+2], InitPackage[i+3]);
    }

   //sends back the recived package so the rio can conferm the package was correctly recived
   Serial.write(InitPackage, InitPackageLen);
   Serial.println("package Recived");
   digitalWrite(7, true);                                                
 
    
  
  
}


//returns a list of all of the bytes in a package with the first index being the length byte



byte* readPackage(){
   boolean packageHasBeenFound=false;
   byte *ByteArray;

   //byte ByteArray[]={6, 01, 00, 22, 34, 35};
   return ByteArray;

   
   while (!packageHasBeenFound){
    byte initByteCount = 0;
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

/* expected send format
 * 3 0x00 bytes as a package init
 * one bit to define the lenght of the package
 * 3 byte groups to send data formated ID, status, Value
 * 3 0xff bytes to finish the package
 */
void loop() {
  count++;
  //readInputs();
  if(lastMS+millis()>lastMS+20){
    lastMS+=20;
    //Serial.println(millis());
    report();

  }

}

void readInputs(){
//  for(byte i=0; i<SwitchCount; i++){
//     SwitchArray[i].periodic();
//  }
}

void report(){
  byte packageToBeSent[1+SwitchCount*3];

  //places package start pattern
  //packageToBeSent[0]=0;
  //packageToBeSent[1]=0;
  //packageToBeSent[2]=0;

  //places package byte count
  packageToBeSent[0]=SwitchCount*3+1;

  //places package end pattern
  //packageToBeSent[-1]=255;
  //packageToBeSent[-2]=255;
  //packageToBeSent[-3]=255;
  
  for (int i=0, n=1; i<SwitchCount; i++, n+=3){
    BytePack CurrentPack = SwitchArray[i].getValue();
    packageToBeSent[n]=CurrentPack.IDByte;
    packageToBeSent[n+1]=CurrentPack.statusByte;
    packageToBeSent[n+2]=CurrentPack.valueByte;
  }

  if(packageToBeSent[2]==falseVal){
    Serial.print("WIREING ISSUE, WIREING ISSUE, JACE SUCKS");
  }
  else{
    Serial.print("wiring is fine... for now");
  }
  Serial.print("   Value byte = ");
  Serial.write(packageToBeSent[3]);
  //Serial.write(packageToBeSent, 1+SwitchCount*3);
//  if(packageToBeSent[2]==255){
//    digitalWrite(30, true);
//  }
  Serial.println();
  //Serial.println(SwitchArray[0].isBool);
  //delay(20);
  
}
