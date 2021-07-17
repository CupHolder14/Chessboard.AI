// set up pin numbers for rows and columns 
int LED_row_pins[8] = {22,23,27,49,28,45,43,25};      // pins for LED rows                   
   
int LED_col_R_pins[8] = {47,41,51,39,26,53,24,29};    // pins for LED columns (Red) 

int LED_col_G_pins[8] = {47,41,51,39,26,53,24,29};    // pins for LED columns (Green) 

int sensor_rows_pins[8] = {2,3,4,5,6,7,8,9};          // pins for the sensor rows  - need to be aligned with positions 0 to 7           
    
int sensor_cols_pins[8] = {10,11,12,13,14,15,16,17};   // pins for the sensor columns - need to be aligned with positions 0 to 7


int last_sensor_board_state[8][8] = {{1,1,1,1,1,1,1,1},
                                    {1,1,1,1,1,1,1,1},
                                    {0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0},
                                    {0,0,0,0,0,0,0,0},
                                    {1,1,1,1,1,1,1,1},
                                    {1,1,1,1,1,1,1,1}};         // INITIAL STATE 

int current_sensor_board_state[8][8];

bool turn_on_RED = false;                  // set to true depending on the move received
bool turn_on_GREEN = false;


void setup() {
    Serial.begin(9600);         // SET BAUD RATE
    
    for (int thisPin_sensor = 0; thisPin_sensor < 8; thisPin_sensor++) {
  
    // initialize the output/input pins for sensor matrix:                     // set sensor output pins 
    pinMode(sensor_cols_pins[thisPin_sensor], INPUT);                                // columns will be read for high signal
    pinMode(sensor_rows_pins[thisPin_sensor], OUTPUT);                               // a row will be set to HIGH one at a time for reading
                                                                         // sensor setup done 
    }
    for (int thisPinLED = 0; thisPinLED < 8; thisPinLED++) {
    // initialize the output pins for LED matrix:
    pinMode(LED_col_R_pins[thisPinLED], OUTPUT);                                 // set LED output pins
    pinMode(LED_col_G_pins[thisPinLED], OUTPUT);
    pinMode(LED_row_pins[thisPinLED], OUTPUT);
    // take the col pins (i.e. the cathodes) high to ensure that
    // the LEDS are off:
    digitalWrite(LED_row_pins[thisPinLED], HIGH);    // turns off LEDs
    
    }
}


void loop() {
  receive_data;

  turn_on_LEDs;

  read_current_board_state;

  compare_board_states;

}

void read_current_board_state() {
  // iterate over the rows:
  for (int row_now = 0; row_now < 8; row_now++) {
    digitalWrite(sensor_rows_pins[0], LOW);                 // set all rows to low 
    digitalWrite(sensor_rows_pins[1], LOW);
    digitalWrite(sensor_rows_pins[2], LOW);
    digitalWrite(sensor_rows_pins[3], LOW);
    digitalWrite(sensor_rows_pins[4], LOW);
    digitalWrite(sensor_rows_pins[5], LOW);
    digitalWrite(sensor_rows_pins[6], LOW);
    digitalWrite(sensor_rows_pins[7], LOW);
   
    digitalWrite(sensor_rows_pins[row_now], HIGH);                  // set rows HIGH one at a time to start scanning               
    delay(2000);
    
    for (int col_now = 0; col_now < 8; col_now++) { 
       current_sensor_board_state[row_now][col_now] = {digitalRead(sensor_cols_pins[col_now])};
    }
  }
}


void compare_board_states() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      int difference = current_sensor_board_state[row][col] - last_sensor_board_state[row][col];
      switch(difference) {
        case -1 : 
          // piece removed 
            Serial.print("picked up piece=");
            Serial.print("[");
            Serial.print(row); 
            Serial.print(",");
            Serial.print(col);
            Serial.println("]");    
            delay(2000);  
          break;
        case 0 : 
          break;
        case 1 :
          // piece moved(landed)
            Serial.print("playing piece landed here=");
            Serial.print("[");
            Serial.print(row); 
            Serial.print(",");
            Serial.print(col);
            Serial.println("]");   
          break;
      }
    }
  }
  last_sensor_board_state[8][8] = current_sensor_board_state;
}



void receive_data() {                                     // if statements for boolean data -NEEDS WORK
  if (Serial.available() > 0) {                          // check if serial data is available from python
  // read the data
  if (Serial.find("Legal moves:")) {                     // possibly receive as Legal moves: [ , ], [ , ],...
    while (Serial.available()) {
      
          
    }
      
    int legal_row_on = Serial.parseInt();                  // parses numeric characters before the comma
    int legal_col_on = Serial.parseInt();              // parses numeric characters after the comma
    digitalWrite(LED_row_pins[legal_row_on], LOW);      // set LED row coordinate to LOW
    digitalWrite(LED_col_R_pins[legal_col_on], LOW);     // set RED off
    digitalWrite(LED_col_G_pins[legal_col_on], HIGH);     // set GREEN on for legal moves 
    }

    
  }
  else if (Serial.find("AI move:")) {                 // need to light up two spaces
        int AI_row_on = Serial.parseInt();            // parses numeric characters before the comma
        int AI_col_on = Serial.parseInt();               // parses numeric characters after the comma
        digitalWrite(LED_row_pins[AI_row_on], LOW);       // set LED row coordinate to LOW
        digitalWrite(LED_col_R_pins[AI_col_on], HIGH); 
        digitalWrite(LED_col_G_pins[AI_col_on], HIGH);    // turn on both for yellow (AI move)
     
    }
  else if (Serial.find("Illegal move:")) {               // need to light up wrong move and last move
        int illegal_row_on = Serial.parseInt();                    // parses numeric characters before the comma
        int illegal_col_on = Serial.parseInt();                        // parses numeric characters after the comma
        digitalWrite(LED_row_pins[illegal_row_on], LOW);                 // set LED row coordinate to LOW
        digitalWrite(LED_col_R_pins[illegal_col_on], HIGH);                 // turn on RED for illegal move 
        digitalWrite(LED_col_G_pins[illegal_col_on], LOW);               // turn off GREEN
      
    }
  else {
  for (int row_now = 0; row_now < 8; row_now++) {
    digitalWrite(LED_row_pins[row_now], HIGH);                 // set all rows to HIGH (OFF) if we don't receive any of the above conditions
  }
  }

}


void turn_on_LEDs() {
  for (int row_LED = 0; row_LED < 8; row_LED++) {     // iterate over LED rows to check which LEDs need to be turned on
        digitalWrite(LED_row_pins[0], HIGH);               // turn all LEDs off 
        digitalWrite(LED_row_pins[1], HIGH);
        digitalWrite(LED_row_pins[2], HIGH);
        digitalWrite(LED_row_pins[3], HIGH);
        digitalWrite(LED_row_pins[4], HIGH);
        digitalWrite(LED_row_pins[5], HIGH);
        digitalWrite(LED_row_pins[6], HIGH);
        digitalWrite(LED_row_pins[7], HIGH);

        digitalWrite(LED_row_pins[row_LED], LOW);

        delay(1000);
       
        
        for (int col_LED = 0; col_LED < 8; col_LED++) {
          digitalWrite(LED_col_R_pins[0], LOW);               // turn all RED LEDs off 
          digitalWrite(LED_col_R_pins[1], LOW);
          digitalWrite(LED_col_R_pins[2], LOW);
          digitalWrite(LED_col_R_pins[3], LOW);
          digitalWrite(LED_col_R_pins[4], LOW);
          digitalWrite(LED_col_R_pins[5], LOW);
          digitalWrite(LED_col_R_pins[6], LOW);
          digitalWrite(LED_col_R_pins[7], LOW);
          
          digitalWrite(LED_col_G_pins[0], LOW);               // turn all GREEN LEDs off 
          digitalWrite(LED_col_G_pins[1], LOW);
          digitalWrite(LED_col_G_pins[2], LOW);
          digitalWrite(LED_col_G_pins[3], LOW);
          digitalWrite(LED_col_G_pins[4], LOW);
          digitalWrite(LED_col_G_pins[5], LOW);
          digitalWrite(LED_col_G_pins[6], LOW);
          digitalWrite(LED_col_G_pins[7], LOW);
        
          if (turn_on_RED == true) {
            digitalWrite(LED_col_R_pins[col_LED], HIGH); {
            }
          }
            else {
              digitalWrite(LED_col_R_pins[col_LED], LOW);
            }
          
           if (turn_on_GREEN == true) {
            digitalWrite(LED_col_G_pins[col_LED], HIGH); {
            }
           }
            else {
              digitalWrite(LED_col_G_pins[col_LED], LOW);
        }
        
      }
  }
  }
