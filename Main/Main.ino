void setup() {
  //sets up the serial conntection
  Serial.begin(9600)

  boolean hasStartedUp=false;

  while (hasStartedUp){
    if (Serial.available()){
      int avalableByteCount=Serial.available();
      byte InitByteList[avalableByteCount]
      for(int i=0; i<avalableByteCount; i++){
        InitByteList[i]=Serial.read();
      }

      SwitchList
      while (int i=1; int<avalableByteCount;1+=2){
             
      }

      
      
    }
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

}




public class Switch(){
  public BooleanSwitch SwitchBool;
  public IntSwitch SwitchInt;
  public Switch(int valuePort, int checkPort, int PowerPort, int ID, boolean IsBool){
    if (IsBool){
      SwitchBool = new BooleanSwitch(valuePort, checkPort, PowerPort, ID);
    }
    else{
      SwitchInt = new BooleanSwitch(valuePort, checkPort, PowerPort, ID);
    }
  }


   
  }


}


public class BytePack{
  byte statusByte;
  byte valueByte;

  public BytePack(byte valueByte, byte statusByte) {
    this.statusByte=stausByte;
    this.valueByte=valueByte;
  }

  public byte getStatusByte(){
    return statusByte;
  }
  public byte getValueByte(){
    return valueByte;
  }
}

public class BooleanSwitch{
  int valuePort;
  int checkPort
  int powerPort
  int ID;
  public BooleanSwtich(int valuePort, int checkPort, int PowerPort, int ID){
     this.valuePort=valuePort;
     this.checkPort=checkPort;
     this.powerPort=powerPort;
     this.ID=ID;
  }
  public BooleanStatus getValue(){
    byte stausByte = 0x00;
    byte valueByte = 0x00;
    if (digitalRead(this.checkPort)||digitalRead(this.valuePort)){
      
    return new BytePack(digitalRead(this.checkPort), digitalRead(this.valuePort);
  }




}
