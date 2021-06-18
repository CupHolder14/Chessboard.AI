// set up pin numbers for rows and columns (will need an extra column for second LED)
int row1[] = {                  // LED ROWS
  22,23,27,49,28,45,43,25};    
 
int col1a[] = {                 //LED COLS 1
  47,41,51,39,26,53,24,29};

int col1b[] = {                 //LED COLS 2
  47,41,51,39,26,53,24,29};

int row2[] = {                  //SENSOR ROWS
  2,3,4,5,6,7,8,9};    
 
int col2[] = {                  //SENSOR COLS
  10,11,12,13,14,15,16,17};

int LED_out                     // Define variable for LED coordinates 


void setup() {
    Serial.begin(9600);      // SET BAUD RATE
    
    for (int thisPin = 0; thisPin < 8; thisPin++) {
  
    // initialize the output pins for sensor matrix:                     // set sensor output pins 
    pinMode(col2[thisPin], OUTPUT); 
    pinMode(row2[thisPin], OUTPUT); 
 
    // take the col pins  high to ensure that
   
    digitalWrite(col2[thisPin], HIGH);   // don't think I need this
}
for (int thisPinLED = 0; thisPinLED < 8; thisPinLED++) {
    // initialize the output pins for LED matrix:
    pinMode(col1a[thisPinLED], OUTPUT);                                 //   LED output pins
    pinMode(col1b[thisPinLED], OUTPUT);
    pinMode(row1[thisPinLED], OUTPUT);
    // take the col pins (i.e. the cathodes) high to ensure that
    // the LEDS are off:
    digitalWrite(col1a[thisPinLED], HIGH);
    digitalWrite(col1b[thisPinLED], HIGH);
 
}
}

void loop() {
if (Serial.available())                                // check if serial data is available first 
  {
    LED_out = Serial.read()                            // the serial data will always give LED output info [(,), (,), ...] *** POSSIBLY USE DIFFERENT SERIAL PORTS FOR SENSOR VS. LED INFO
    digitalWrite(row1[LED_out(1)], HIGH)               // NEED TO SET UP LED_out as an array maybe???
    digitalWrite(col1a[LED_out(1)], LOW)               // LED TURNS ON WHEN ROW IS HIGH AND COL IS LOW
  }
  else{                                                 // REFER TO PROGRESS PRESENTATION TO DECIDE WHICH LEDS TO TURN ON 
  // iterate over the rows:
  for (int row_now = 0; row_now < 8; row_now++) {
    // check for high sensor signal
   if (digitalRead(row2[row_now])==HIGH){               // send which row , wait some time and then read the column 
        Serial.print(row_now);
    for (int col_now = 0; col_now < 8; col_now++) { 
      if (digitalRead(col2[col_now])==HIGH){
        Serial.print("x");
        Serial.println(col_now);
        delay(2000);                                     // need to wait (dante was using 20 miliseconds) 
      }
      }
    }
   }
    
   }
}
