int sensor_rows_pins[8] = {51, 50, 49, 48, 47, 46, 45, 44};   // pins for the sensor rows  
int sensor_cols_pins[8] = {A0, A1, A2, A3, A4, A5, A6, A7}; // pins for the sensor columns 


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
for (int thisPin_sensor = 0; thisPin_sensor < 8; thisPin_sensor++) {                  
    // initialize the output/input pins for sensor matrix:                            // set sensor output pins
    pinMode(sensor_cols_pins[thisPin_sensor], INPUT);                                 // columns will be read for high signal
    pinMode(sensor_rows_pins[thisPin_sensor], OUTPUT);   
}
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int row_now = 0; row_now < 8; row_now++) {
    // ChangeLEDState(sensor_rows_pins, false);
    digitalWrite(sensor_rows_pins[0], LOW);                 // set all rows to low
    digitalWrite(sensor_rows_pins[1], LOW);
    digitalWrite(sensor_rows_pins[2], LOW);
    digitalWrite(sensor_rows_pins[3], LOW); 
    digitalWrite(sensor_rows_pins[4], LOW);
    digitalWrite(sensor_rows_pins[5], LOW);
    digitalWrite(sensor_rows_pins[6], LOW);
    digitalWrite(sensor_rows_pins[7], LOW);

    digitalWrite(sensor_rows_pins[row_now], HIGH);                  // set rows HIGH one at a time to start scanning
   // delay(20);
 
    for (int col_now = 0; col_now < 8; col_now++) {
      int current_sensor_board_state = {digitalRead(sensor_cols_pins[col_now])};
    
        
        
        Serial.print(current_sensor_board_state);
      
     // delay(35);  
  }
  Serial.println();
  }
  Serial.println();
}













 
  
