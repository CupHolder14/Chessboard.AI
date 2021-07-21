String InBytes;
int flg = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(7, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
}

void loop() {
  while(flg){
    if (Serial.available() > 0) {
//    tone(9, 1000);
//    delay(3000);
//    noTone(9);
    InBytes = Serial.readStringUntil("\n");
    String DATA = "PRINT:Test";
    SendData(DATA);
    flg = false;
  }
  if(!digitalRead(7)){
    Serial.println("InitialTile:(0,6)");
    while(!digitalRead(7));
  }
  if(!digitalRead(6)){
    Serial.println("NextTile:(0,5)");
    while(!digitalRead(6));
  } 
  }
}

void SendData(String  Data){
  Serial.write("PRINT:Test");
}
