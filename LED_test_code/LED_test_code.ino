int LED_row_pins[8] = {22, 23, 27, 49, 28, 45, 43, 25}; // pins for LED rows
int LED_col_R_pins[8] = {47, 41, 51, 39, 26, 53, 24, 29}; // pins for LED columns (Red)
int LED_col_G_pins[8] = {47, 41, 51, 39, 26, 53, 24, 29}; // pins for LED columns (Green)



void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

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
  // put your main code here, to run repeatedly:
for (int row_LED = 0; row_LED < 8; row_LED++) {     // iterate over LED rows to check which LEDs need to be turned on
    // ChangeLEDState(sensor_rows_pins, true);
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
      // ChangeLEDState(LED_col_R_pins, false);
      digitalWrite(LED_col_R_pins[0], LOW);               // turn all RED LEDs off
      digitalWrite(LED_col_R_pins[1], LOW);
      digitalWrite(LED_col_R_pins[2], LOW);
      digitalWrite(LED_col_R_pins[3], LOW);
      digitalWrite(LED_col_R_pins[4], LOW);
      digitalWrite(LED_col_R_pins[5], LOW);
      digitalWrite(LED_col_R_pins[6], LOW);
      digitalWrite(LED_col_R_pins[7], LOW);

      // ChangeLEDState(LED_col_G_pins, false);
      digitalWrite(LED_col_G_pins[0], LOW);               // turn all GREEN LEDs off
      digitalWrite(LED_col_G_pins[1], LOW);
      digitalWrite(LED_col_G_pins[2], LOW);
      digitalWrite(LED_col_G_pins[3], LOW);
      digitalWrite(LED_col_G_pins[4], LOW);
      digitalWrite(LED_col_G_pins[5], LOW);
      digitalWrite(LED_col_G_pins[6], LOW);
      digitalWrite(LED_col_G_pins[7], LOW);

        digitalWrite(LED_col_R_pins[col_LED], HIGH);     // col_LED needs to be linked to coordinates from JOE
  
        digitalWrite(LED_col_R_pins[col_LED], HIGH);
  
      }

    }
  }
