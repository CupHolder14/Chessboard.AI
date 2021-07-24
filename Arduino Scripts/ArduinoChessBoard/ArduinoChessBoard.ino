/* Libraries */
#include <LiquidCrystal_I2C.h>

/* Debugging Variables */
bool DEBUGGING = false;

/* Pin Variables */
// LCD PINS
int upButton = A14;
int downButton = A8;
int acceptButton = A12;
int denyButton = A10;
int WhiteButton = A8  ;
int BlackButton = A14;
// Board State Pins
int LED_row_pins[8] = {13, 12, 11, 10, 9, 8, 7, 6}; // pins for LED rows
int LED_col_R_pins[8] = {22, 23, 24, 25, 26, 27, 28, 29}; // pins for LED columns (Red)
int LED_col_G_pins[8] = {32, 33, 34, 35, 36, 37, 38, 39}; // pins for LED columns (Green)
int sensor_rows_pins[8] = {51, 50, 49, 48, 47, 46, 45, 44};   // pins for the sensor rows
int sensor_cols_pins[8] = {A0, A1, A2, A3, A4, A5, A6, A7}; // pins for the sensor columns
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

    void SendData(String opCode, String value) {
      String Data = opCode + ":" + value;
      Serial.println(Data);
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
    int AiDifficultyValue;
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
      this->AiDifficultyValue = 2;
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
        AiDifficultyValue = 2;
      }
      else if (AiDifficulty == "Medium") {
        AiDifficulty = "Hard";
        AiDifficultyValue = 3;
      }
      else if (AiDifficulty == "Hard") {
        AiDifficulty = "Easy";
        AiDifficultyValue = 1;
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
        Data_OUT.SendData("VsHuman", "");
        Data_OUT.SendData("Difficulty", String(AiDifficultyValue));
      }
      else{
        Data_OUT.SendData("VsHuman", "True");
      }
      Data_OUT.SendData("StartGame", "True");
      
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
            lcd.setCursor(0, 0);
            lcd.print("Game Over - Black Wins");
            Data_OUT.SendData("TimeOut", "True");
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
            lcd.print("Game Over - White Wins");
            Data_OUT.SendData("TimeOut", "True");
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
    receive_data;     // Read serial port for data from python

    read_current_board_state;       // Read the current board state

    compare_board_states;       // Find out how the board state changed
  }
  else if (settings.GameInProgress && settings.IsPaused) {
    if (!digitalRead(upButton)) {
      if (menuIndex != 1) {
        menuIndex--;
        RefreshScreen("up");
      }
      while (!digitalRead(upButton));
      delay(0.5);
    }
    if (!digitalRead(downButton)) {
      if (menuIndex != 3 && menuIndex != maxIndex) {
        menuIndex++;
        RefreshScreen("down");
      }
      while (!digitalRead(downButton));
      delay(0.5);
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
          outdata.SendData("Quit", "True");
          WriteToScreen("Classical Chess", mainMenu);
          break;
      }
      while(!digitalRead(acceptButton));
      delay(0.5);
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
      delay(0.5);
    }
    if (!digitalRead(downButton)) {
      PrintLog("downButton");
      if (menuIndex != 3 && menuIndex != maxIndex) {
        menuIndex++;
        RefreshScreen("down");
      }
      while (!digitalRead(downButton));
      delay(0.5);
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
      delay(0.5);
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
    delay(0.5);
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
    delay(200);

    for (int col_now = 0; col_now < 8; col_now++) {
      current_sensor_board_state[row_now][col_now] = {digitalRead(sensor_cols_pins[col_now])};
    }
  }
}


void compare_board_states() {
  String data;
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      int difference = current_sensor_board_state[row][col] - last_sensor_board_state[row][col];
      switch (difference) {
        case -1 :
        // Piece Picked Up
          data = "(" + String(row) + "," + String(col) + ")";
          outdata.SendData("InitialTile", data);
          break;
        case 0 :
        // Nothing happened
          break;
        case 1 :
        // Piece Moved
          data = "(" + String(row) + "," + String(col) + ")";
          outdata.SendData("NextTile", data);
          break;
      }
    }
  }
  last_sensor_board_state[8][8] = current_sensor_board_state;
}

int rowMoves[21];
int colMoves[21];
// [(5,3),(4,3)]

int ParseData() {
  int j = 99;
  int index = 0;
  for (int i = 0; i < 42; i++) {
    j = Serial.parseInt();
    if (j != 99) {
      if (i % 2 == 0) {
        rowMoves[index] = j;
      }
      else {
        colMoves[index] = j;
        index++;
      }
      j = 99;
    }
  }
  return index;
}

void WipeArray() {
  for (int i = 0; i < 21; i++) {
    rowMoves[i] = 99;
    colMoves[i] = 99;
  }
}

void LEDScan(bool isGreen, bool isRed) {
  int maxMoves = ParseData();
  ChangeLEDState(LED_row_pins, true);
  for (int i = 0; i < maxMoves; i++) {
    turn_on_LEDs(rowMoves[i], colMoves[i], isGreen, isRed);
  }
}

void receive_data() {                                     // if statements for boolean data
  if (Serial.available() > 0) {
    if (Serial.find("LegalMoves:")) {
      LEDScan(true, false);
    }
    else if (Serial.find("AIMove:")) {                // 2 coordinates will be sent
      LEDScan(true, true);
    }
    else if (Serial.find("IllegalMove:")) {             // 1 coordinate will be sent
      LEDScan(false, true);
    }
    else {
      LEDScan(false, false);
    }
  }
}

void turn_on_LEDs(int row, int col, bool isGreen, bool isRed) {
  digitalWrite(LED_row_pins[row], LOW);
  digitalWrite(LED_col_G_pins[col], isGreen);
  digitalWrite(LED_col_R_pins[col], isRed);
}
