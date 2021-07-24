String InBytes;
int flg = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(7, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
}

void loop() {
  if(!digitalRead(7)){
    Serial.println("StartGame:True");
    while(!digitalRead(7));
  }
  if(!digitalRead(6)){
    Serial.println("InitialTile:(6,6)");
    while(!digitalRead(6));
  }
  if(!digitalRead(5)){
    Serial.println("NextTile:(4,6)");
    while(!digitalRead(5));
  }
}
//  while(flg){
//    if (Serial.available() > 0) {
//    InBytes = Serial.readStringUntil("\n");
//    String DATA = "PRINT:Test";
//    SendData(DATA);
//    flg = false;
//  }
//  if(!digitalRead(7)){
//    Serial.println("InitialTile:(0,6)");
//    while(!digitalRead(7));
//  }
//  if(!digitalRead(6)){
//    Serial.println("NextTile:(0,5)");
//    while(!digitalRead(6));
//  } 
//  }

//void SendData(String  Data){
//  Serial.write("PRINT:Test");
//}
