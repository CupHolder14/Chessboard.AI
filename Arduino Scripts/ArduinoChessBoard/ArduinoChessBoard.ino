/* Libraries */
#include <LiquidCrystal_I2C.h>

/* Debugging Variables */
bool DEBUGGING = true;

/* Pin Variables */
// LCD PINS
int upButton = 7;
int downButton = 8;
int acceptButton = 12;
int denyButton = 13;
int WhiteButton = 7;
int BlackButton = 8;
// Board State Pins
int LED_row_pins[8] = {22, 23, 27, 49, 28, 45, 43, 25}; // pins for LED rows
int LED_col_R_pins[8] = {47, 41, 51, 39, 26, 53, 24, 29}; // pins for LED columns (Red)
int LED_col_G_pins[8] = {47, 41, 51, 39, 26, 53, 24, 29}; // pins for LED columns (Green)
int sensor_rows_pins[8] = {2, 3, 4, 5, 6, 7, 8, 9};   // pins for the sensor rows  - need to be aligned with positions 0 to 7
int sensor_cols_pins[8] = {10, 11, 12, 13, 14, 15, 16, 17}; // pins for the sensor columns - need to be aligned with positions 0 to 7

/* Variables */
// LCD Variables
LiquidCrystal_I2C lcd(0x27, 20, 4);

int currentMenu;
int menuIndex;
int maxIndex;

// menu options
String mainMenu[] = {"Play", "Settings"};                             // 0
String playMenu[] = {"Vs. AI", "Vs. Player"};                         // 1
String settingsMenu[] = {"Timer Length", "AI Difficulty"};            // 2
String ChangeSideMenu[] = {"White", "Black"};                         // 3
String pauseMenu[] = {"Resume", "Quit"};                              // 4
String AcceptMenu[] = {"Yes", "No"};                                  // 5
String TimerLengthMenu[] = {"    < 10 mins >", "Confirm"};            // 6
String AiDifficultyMenu[] = {"    < Easy >", "Confirm"};              // 7


// Board State Variables
int last_sensor_board_state[8][8] =
{ {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1}
};
int current_sensor_board_state[8][8];

bool turn_on_RED = false;                  // set to true depending on the move received
bool turn_on_GREEN = false;



/* Classes */
// Data to be received
class InData {
  public:
    bool GAMEOVER; // True = game is over, false = game is active
    int Winner; // 0 = white, 1 = black, 2 = stalemate
    // operation

    InData() {
      this->GAMEOVER = false;
      this->Winner = 0;
    }

    
};

// Data to be sent
class OutData {
  public:
    bool BlackPlayer; // false = vs. AI, true = vs. Human
    bool GAMEOVER; // True = game is over, false = game is active

    OutData() {
      this->BlackPlayer = false;
      this->GAMEOVER = 0;
    }

    void SendMove(){
      
    }
};

// Internal Settings
class Settings {
  private:

    void UpdateClock(int Mins, int Secs, bool IsWhitesTurn) {
      if (IsWhitesTurn) {
        lcd.setCursor(9, 2);
      }
      else {
        lcd.setCursor(15, 2);
      }
      String mins = Mins >= 10 ? String(Mins) : "0" + String(Mins);
      String secs = Secs >= 10 ? String(Secs) : "0" + String(Secs);
      lcd.print(mins + ":" + secs);
    }
    
  public:
    bool GameInProgress;
    bool InfLength;
    int TimerLength;
    int Mins_White;
    int Mins_Black;
    int Secs_White;
    int Secs_Black;
    String AiDifficulty;
    bool AgainstAI;
    bool IsWhitesTurn;
    bool IsWhitePlayer;
    bool IsPaused;
    unsigned long PrevTime;
    InData Data_IN;
    OutData Data_OUT;

    Settings() {
      this->GameInProgress = false;
      this->InfLength = false;
      this->TimerLength = 10;
      this->AiDifficulty = "Easy";
      this->PrevTime = 0;
      this->IsWhitesTurn = true;
      this->IsPaused = false;
    }

    void IncreaseTimer(String* TimerLengthMenu) {
      if (TimerLength == 20 && InfLength == false) {
        InfLength = true;
        TimerLengthMenu[0] = "    < unlimited >";
      }
      else if (TimerLength == 20 && InfLength) {
        TimerLength = 5;
        InfLength = false;
        TimerLengthMenu[0] = "    < " + String(TimerLength) + " mins >";
      }
      else {
        TimerLength = TimerLength + 5;
        TimerLengthMenu[0] = "    < " + String(TimerLength) + " mins >";
      }
    }

    void IncreaseDifficulty(String* AiDifficultyMenu) {
      if (AiDifficulty == "Easy") {
        AiDifficulty = "Medium";
      }
      else if (AiDifficulty == "Medium") {
        AiDifficulty = "Hard";
      }
      else if (AiDifficulty == "Hard") {
        AiDifficulty = "Easy";
      }
      AiDifficultyMenu[0] = "    < " + AiDifficulty + " >";
    }

    void PauseScreen() {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Game Paused");
      lcd.setCursor(1, 1);
      lcd.print("Resume");
      lcd.setCursor(1, 2);
      lcd.print("Quit");
      lcd.setCursor(0, 1);
      lcd.print(">");
    }

    void ResumeScreen() {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("In Game");
      lcd.setCursor(9, 1);
      lcd.print("White Black");
      lcd.setCursor(0, 2);
      lcd.print("Time");
      lcd.setCursor(9, 2);
      if (InfLength) {
        lcd.print("--:-- --:--");
      }
      else {
        UpdateClock(Mins_White, Secs_White, true);
        if (AgainstAI) {
          lcd.setCursor(15, 2);
          lcd.print("--:--");
        }
        else {
          UpdateClock(Mins_Black, Secs_Black, false);
        }
      }
    }

    void StartGame(bool againstAI) {
      // To start the game, we need to display the in game screen (timer, last move, etc.)
      // We need to initalize the clock by calculating the minutes, and seconds
      // We need to then tell the python to start the AI code, if we are versing an AI

      GameInProgress = true;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("In Game");
      lcd.setCursor(9, 1);
      lcd.print("White Black");
      lcd.setCursor(0, 2);
      lcd.print("Time");
      lcd.setCursor(9, 2);
      if (InfLength) {
        lcd.print("--:-- --:--");
      }
      else {
        Mins_White = TimerLength;
        Mins_Black = TimerLength;
        Secs_White = 0;
        Secs_Black = 0;
        String mins = TimerLength >= 10 ? String(TimerLength) : "0" + String(TimerLength);
        if (AgainstAI) {
          lcd.print(mins + ":" + "00 " + "--" + ":" + "--");
        }
        else {
          lcd.print(mins + ":" + "00 " + mins + ":" + "00");
        }
      }
      lcd.setCursor(0, 3);
      lcd.print("Last Move");

      if (againstAI) {
        // Tell python to start ai engine
      }
    }

    void Tick() {
      if (IsWhitesTurn) {
        if (Secs_White == 0) {
          if (Mins_White != 0) {
            Mins_White--;
            Secs_White = 59;
            UpdateClock(Mins_White, Secs_White, true);
          }
          else {
            // Timer Depleted for white
            GameInProgress = false;
            lcd.print("                    ");
            lcd.print("Game Over - Black Wins");
          }
        }
        else {
          Secs_White--;
          UpdateClock(Mins_White, Secs_White, true);
        }
      }
      else {
        if (AgainstAI) {
          return;
        }
        if (Secs_Black == 0) {
          if (Mins_Black != 0) {
            Mins_Black--;
            Secs_Black = 59;
            UpdateClock(Mins_Black, Secs_Black, false);
          }
          else {
            // Timer Depleted for black
            GameInProgress = false;
            lcd.setCursor(0, 0);
            lcd.print("                    ");
            lcd.setCursor(0, 0);
            lcd.print("Game Over - White Wins");
          }
        }
        else {
          Secs_Black--;
          UpdateClock(Mins_Black, Secs_Black, false);
        }
      }
    }
};

/* Global Variables */
Settings settings;
OutData outdata;

// Initializing Arduino Pins
void setup() {
  // Begin Serial Port on Buad Rate 9600
  Serial.begin(9600);

  // LCD Initialization
  lcd.init();
  lcd.backlight();
  lcd.clear();

  /* Pin Initialization*/
  // LCD Pins
  pinMode(acceptButton, INPUT_PULLUP);
  pinMode(denyButton, INPUT_PULLUP);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);

  // Board State Pins
  for (int thisPin_sensor = 0; thisPin_sensor < 8; thisPin_sensor++) {
    // initialize the output/input pins for sensor matrix:                            // set sensor output pins
    pinMode(sensor_cols_pins[thisPin_sensor], INPUT);                                 // columns will be read for high signal
    pinMode(sensor_rows_pins[thisPin_sensor], OUTPUT);                                // a row will be set to HIGH one at a time for reading
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

  /* Variable Initialization */
  menuIndex = 1;
  maxIndex = 2;


  /* Display Inital Screen */
  WriteToScreen("Classical Chess", mainMenu);
  currentMenu = 0;
}

void loop() {
  if (settings.GameInProgress && !settings.IsPaused) {
    if (!digitalRead(denyButton)) {
      PrintLog("Paused");
      settings.IsPaused = true;
      menuIndex = 1;
      settings.PauseScreen();
      while (!digitalRead(denyButton));
    }
    if (!settings.InfLength) {
      unsigned long currentTime = millis();
      if (currentTime - settings.PrevTime >= 1000) {
        settings.PrevTime = currentTime;
        settings.Tick();
      }
    }
    if (!digitalRead(WhiteButton) && settings.IsWhitesTurn) {
      settings.IsWhitesTurn = !settings.IsWhitesTurn;
      while (!digitalRead(WhiteButton));
    }
    if (!digitalRead(BlackButton) && !settings.IsWhitesTurn) {
      settings.IsWhitesTurn = !settings.IsWhitesTurn;
      while (!digitalRead(WhiteButton));
    }
    receive_data;

    turn_on_LEDs;

    read_current_board_state;

    compare_board_states;
  }
  else if (settings.GameInProgress && settings.IsPaused) {
    if (!digitalRead(upButton)) {
      if (menuIndex != 1) {
        menuIndex--;
        RefreshScreen("up");
      }
      while (!digitalRead(upButton));
    }
    if (!digitalRead(downButton)) {
      if (menuIndex != 3 && menuIndex != maxIndex) {
        menuIndex++;
        RefreshScreen("down");
      }
      while (!digitalRead(downButton));
    }
    if (!digitalRead(acceptButton)) {
      switch (menuIndex) {
        case 1:
          settings.IsPaused = false;
          settings.ResumeScreen();
          break;
        case 2:
          menuIndex = 1;
          maxIndex = 2;
          currentMenu = 0;
          settings.GameInProgress = false;
          settings.IsPaused = false;
          WriteToScreen("Classical Chess", mainMenu);
          break;
      }
    }
  }
  else {
    if (!digitalRead(upButton)) {
      PrintLog("upButton");
      if (menuIndex != 1) {
        menuIndex--;
        RefreshScreen("up");
      }
      while (!digitalRead(upButton));
    }
    if (!digitalRead(downButton)) {
      PrintLog("downButton");
      if (menuIndex != 3 && menuIndex != maxIndex) {
        menuIndex++;
        RefreshScreen("down");
      }
      while (!digitalRead(downButton));
    }
    if (!digitalRead(acceptButton)) {
      PrintLog("acceptButton");
      switch (currentMenu) {
        // Main Menu
        case 0:
          switch (menuIndex) {
            case 1:
              currentMenu = 1;
              menuIndex = 1;
              WriteToScreen("Play", playMenu);
              break;
            case 2:
              currentMenu = 2;
              menuIndex = 1;
              WriteToScreen("Settings", settingsMenu);
              break;
          }
          break;
        // Play Menu
        case 1:
          switch (menuIndex) {
            case 1:
              currentMenu = 3;
              menuIndex = 1;
              settings.AgainstAI = true;
              settings.StartGame(settings.AgainstAI);
              break;
            case 2:
              currentMenu = 3;
              menuIndex = 1;
              settings.AgainstAI = false;
              settings.StartGame(settings.AgainstAI);
              break;
          }
          break;
        // Settings Menu
        case 2:
          switch (menuIndex) {
            case 1:
              currentMenu = 6;
              menuIndex = 1;
              WriteToScreen("Timer Length", TimerLengthMenu);
              break;
            case 2:
              currentMenu = 7;
              menuIndex = 1;
              WriteToScreen("AI Difficulty", AiDifficultyMenu);
              break;
          }
          break;
        case 6:
          switch (menuIndex) {
            case 1:
              settings.IncreaseTimer(TimerLengthMenu);
              PrintLog(String(settings.TimerLength));
              WriteToScreen("Timer Length", TimerLengthMenu);
              break;
            case 2:
              currentMenu = 2;
              menuIndex = 1;
              WriteToScreen("Settings", settingsMenu);
              break;
          }
          break;
        case 7:
          switch (menuIndex) {
            case 1:
              settings.IncreaseDifficulty(AiDifficultyMenu);
              WriteToScreen("AI Difficulty", AiDifficultyMenu);
              break;
            case 2:
              currentMenu = 2;
              menuIndex = 1;
              WriteToScreen("Settings", settingsMenu);
              break;
          }
          break;
      }
      while (!digitalRead(acceptButton));
    }
    if (!digitalRead(denyButton)) {
      PrintLog("denyButton");
      switch (currentMenu) {
        case 0:
          break;
        case 1:
          currentMenu = 0;
          menuIndex = 1;
          WriteToScreen("Classical Chess", mainMenu);
          break;
        case 2:
          currentMenu = 0;
          menuIndex = 1;
          WriteToScreen("Classical Chess", mainMenu);
          break;
        case 6:
          currentMenu = 2;
          menuIndex = 1;
          WriteToScreen("Settings", settingsMenu);
          break;
        case 7:
          currentMenu = 2;
          menuIndex = 1;
          WriteToScreen("Settings", settingsMenu);
          break;
      }
    }
    while (!digitalRead(denyButton));
  }
}

void RefreshScreen(String op) {
  if (op == "up") {
    lcd.setCursor(0, menuIndex + 1);
    lcd.print(" ");
    lcd.setCursor(0, menuIndex);
    lcd.print(">");
  }
  if (op == "down") {
    lcd.setCursor(0, menuIndex - 1);
    lcd.print(" ");
    lcd.setCursor(0, menuIndex);
    lcd.print(">");
  }
}

void WriteToScreen(String title, String options[]) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(title);
  for (int i = 1; i <= sizeof(options) && i <= 3; i++) {
    lcd.setCursor(1, i);
    lcd.print(options[i - 1]);
  }
  lcd.setCursor(0, menuIndex);
  lcd.print(">");
}

void PrintLog(String log) {
  if (DEBUGGING) {
    Serial.println(log);
  }
}

void ChangeLEDState(int *Array, bool state, int count = 8) {
  for (int i = 0; i <= count; i++) {
    digitalWrite(Array[i], state);
  }
}

void read_current_board_state() {
  // iterate over the rows:
  for (int row_now = 0; row_now < 8; row_now++) {
    ChangeLEDState(sensor_rows_pins, false);              // set all rows to low

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
      switch (difference) {
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
 if (Serial.find("Legal moves:")) {                     
        turn_on_GREEN = true;                               // turn on green LEDs for legal moves 
        turn_on_RED = false;

      } else if (Serial.find("AI move:")) {     
        turn_on_GREEN = true;
        turn_on_RED = true;                               // green and red is true for yellow LED (AI move)
      }

       else if (Serial.find("Illegal move:")) {  
        turn_on_GREEN = false;                            // turn on red LED for illegal move
        turn_on_RED = true;
       }
       else {
        turn_on_GREEN = false;
        turn_on_RED = false;
       }

}


void turn_on_LEDs(int row, int col, bool isRed, bool isGreen) {
  digitalWrite(LED_row_pins[row], LOW);
  digitalWrite(LED_col_R_pins[col], isRed);
  digitalWrite(LED_col_G_pins[col], isGreen);
  
  
  
  
  for (int row_LED = 0; row_LED < 8; row_LED++) {     // iterate over LED rows to check which LEDs need to be turned on
    ChangeLEDState(LED_row_pins, true);               // turn all LEDs off
    digitalWrite(LED_row_pins[row_LED], LOW);
    delay(1000);
    for (int col_LED = 0; col_LED < 8; col_LED++) {
      ChangeLEDState(LED_col_R_pins, false);          // turn all RED LEDs off      

      ChangeLEDState(LED_col_G_pins, false);          // turn all GREEN LEDs off          

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
