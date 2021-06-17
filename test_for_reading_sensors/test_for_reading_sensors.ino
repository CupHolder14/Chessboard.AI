// set up pin numbers for rows and columns (will need an extra column for second LED)
/*
int row; // for testing
int col;  // for testing

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
  */

int test_sensor = 7;                 // define which digital pin the sensor is connected to (use A# for analog pins)
int test_LED = 2;

void setup() {
    Serial.begin(9600);             // SET BAUD RATE
    pinMode(test_sensor,INPUT);    // sets the mode of digital pin 7
    pinMode(test_LED,OUTPUT);      // sets the mode of digital pin 2
}

void loop() {
  int test_sensor_value=digitalRead(test_sensor);
  Serial.println(test_sensor_value);
  if (digitalRead(test_sensor)==HIGH){
    digitalWrite(test_LED),HIGH);}
    else{
      digitalWrite(test_LED),LOW);
    }
    }
    
  
  /*
  row = 2;
  col = 3;
  Serial.print(row);                      // use row_now for actual code
  Serial.print(",");   
  Serial.println(col);                      // use col_now for actual code
  delay(2000);
  // iterate over the rows:
  //for (int row_now = 0; row_now < 8; row_now++) {
    // check for high sensor signal
   // if (digitalRead(row2[row_now])==HIGH) {
   */
   
}
