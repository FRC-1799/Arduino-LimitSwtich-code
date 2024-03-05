
byte SwitchCount;
byte waitCount = 20;
unsigned long lastMS = 0;
byte falseVal = 0x00;
byte trueVal = 0xff;


//class to hold data so it can be passed around easyer
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


//class to represend the switch
class Switch{
  public:
  byte ID;
  byte valuePort;
  byte statusPort;
  
  Switch(int inputID,byte StatusPort, byte ValuePort){
    ID = inputID;
    valuePort = ValuePort;
    statusPort = StatusPort;
  }
   //returns the switch data in the form of a byte pack
   BytePack getValue(){  
    return BytePack(ID, boolToByte(digitalRead(valuePort)), boolToByte(digitalRead(statusPort)||digitalRead(valuePort)));
  }
  //turns bools in bytes based on the true and false vals 
  byte boolToByte(boolean toConvert){
    if (toConvert){
      return trueVal;
    }
    return falseVal;
  }
};



/*
 * expeted file format for init
 * 0x00, 0x00, 0x00 Pattern to help the reader identify the start of a package
 * 1 byte to tell the compiler how long to expect the package to be, does not include headers or footers but does include the length value itself
 * 4 byte packages for the deffinition of switches ordered ID, CheckPort, ValuePort
 * 0x255, 0x255, 0x255 Pattern to conferm the ending of a package
 * 
 */
Switch *SwitchArray;
byte *InitPackage;

void setup() {
  //sets up the serial conntection
  Serial.begin(9600);

   InitPackage = readPackage();

   //creates an array of switchs based on the init package
   byte InitPackageLen=InitPackage[0];
   SwitchCount = (InitPackageLen-1) / 3;
   SwitchArray = (Switch*)malloc(SwitchCount * sizeof(Switch));

   //reads groups of five bytes into switch objects to be stored for loop
   for (byte i = 1, n = 0; n<SwitchCount; i += 3, n++){
      SwitchArray[n] = *new Switch(InitPackage[i], InitPackage[i+1], InitPackage[i+2]);
    }

   //sends back the recived package so the rio can conferm the package was correctly recived
   Serial.write(InitPackage, InitPackageLen);
}

//reads the inital pack into an array 
byte* readPackage(){
   boolean packageHasBeenFound = false;
   byte *ByteArray;

   //byte ByteArray[]={6, 01, 00, 22, 34, 35};
   //return ByteArray;

   //reads for a pack
   while (!packageHasBeenFound){
      byte initByteCount = 0;

      //reads untill it finds a pack and will wait untill the serial port gets data if the port has no data
      while (initByteCount < 3){       
        if(!Serial.available())
        {
          delay(20);
          continue;
        }

        if(Serial.read() == 0){
          initByteCount++;
        }
        else{
            initByteCount = 0;
        }
      }
  
      //reads the number of bytes to be recived in the package
      int ByteArrayLen = Serial.read();
      ByteArray = (byte*)malloc(ByteArrayLen);
      ByteArray[0] = ByteArrayLen;
      
      //reads the package bytes into a list
      for (int i = 1; i < ByteArrayLen; i++){
        ByteArray[i] = Serial.read();
      }

      //makes sure there is a package finisher
      packageHasBeenFound = true;
      for (int i = 0; i < 3; i++){
        if (Serial.read() != 255){
          packageHasBeenFound = false;
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
  //sends a pack every 20 ms
  if(lastMS + millis() <= lastMS + 20){
    return;
  }

  lastMS += 20;

  //creates the list used to create the msg
  byte packageToBeSent[7 + SwitchCount * 3];

  //places package start pattern
  packageToBeSent[0] = 0;
  packageToBeSent[1] = 0;
  packageToBeSent[2] = 0;

  //places package byte count
  packageToBeSent[3] = SwitchCount * 3 + 1;

  //places package end pattern
  packageToBeSent[-1] = 255;
  packageToBeSent[-2] = 255;
  packageToBeSent[-3] = 255;

  //reads all the jucy data into the pack
  for (int i = 4, n = 0; n < SwitchCount; i += 3, n++){
    BytePack CurrentPack = SwitchArray[n].getValue();
    packageToBeSent[i] = CurrentPack.IDByte;
    packageToBeSent[i + 1] = CurrentPack.statusByte;
    packageToBeSent[i + 2] = CurrentPack.valueByte;
  }

  //sends out the pack
  Serial.write(packageToBeSent, 7 + SwitchCount * 3);
}
