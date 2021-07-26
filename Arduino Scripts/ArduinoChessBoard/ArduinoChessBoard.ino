/*
 * 
 * Library Imports, Class Declarations, and Variable Initializations
 * 
 */

/* Libraries */
#include <LiquidCrystal_I2C.h>

/* Debugging Variables */
bool DEBUGGING = true;

/* Pin Variables */
// LCD PINS
int upButton = A14;
int downButton = A8;
int acceptButton = A12;
int denyButton = A10;
int WhiteButton = A8  ;
int BlackButton = A14;
// Board State Pins
int LEDRowPins[8] = {13, 12, 11, 10, 9, 8, 7, 6}; // pins for LED rows
int LEDColPins_R[8] = {22, 23, 24, 25, 26, 27, 28, 29}; // pins for LED columns (Red)
int LEDColPins_G[8] = {32, 33, 34, 35, 36, 37, 38, 39}; // pins for LED columns (Green)
int sensorRowPins[8] = {51, 50, 49, 48, 47, 46, 45, 44};   // pins for the sensor rows
int sensorColPins[8] = {A0, A1, A2, A3, A4, A5, A6, A7}; // pins for the sensor columns
// Board State Arrays
int rowMoves[21];
int colMoves[21];


/* Global Variables */
// LCD Variables
LiquidCrystal_I2C lcd(0x27, 20, 4);         // Set address line for the 20x4 LCD Screen

int currentMenu;
int menuIndex;
int maxIndex;

// Menu Screens
String mainMenu[] = {"Play", "Settings"};                             // 0
String playMenu[] = {"Vs. AI", "Vs. Player"};                         // 1
String settingsMenu[] = {"Timer Length", "AI Difficulty"};            // 2
String ChangeSideMenu[] = {"White", "Black"};                         // 3
String pauseMenu[] = {"Resume", "Quit"};                              // 4
String AcceptMenu[] = {"Yes", "No"};                                  // 5
String TimerLengthMenu[] = {"    < 10 mins >", "Confirm"};            // 6
String AiDifficultyMenu[] = {"    < Easy >", "Confirm"};              // 7


// Board State Variables
// Array to assign the current board state to
int CurrentSensorBoardState[8][8];

int LastSensorBoardState[8][8] =
{ {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1}
};



/* Classes */
// Data to be received
class InData {
    // Class that houses the functions to interpret data sent to the arduino from the python side
    public:
    InData(){
    }
};

// Data to be sent
class OutData {
    // Class that houses the functions to send data to the python side
  public:
    OutData() {
    }
    void SendData(String opCode, String value) {
      String Data = opCode + ":" + value;
      Serial.println(Data);
    }
};

// Internal Settings
class Settings {
    // Class that houses all the internal settings 
  private:
    void UpdateClock(int Mins, int Secs, bool IsWhitesTurn) {
        // Function to update the clock in the corresponding position
      if (IsWhitesTurn) {
        lcd.setCursor(9, 2);
      }
      else {
        lcd.setCursor(15, 2);
      }
      // Add 0 infront of the time if it is less than 10 ex. 4 -> 04
      String mins = Mins >= 10 ? String(Mins) : "0" + String(Mins);
      String secs = Secs >= 10 ? String(Secs) : "0" + String(Secs);
      lcd.print(mins + ":" + secs);
    }

  public:
    bool GameInProgress;
    
    // Timer variables
    bool InfLength;
    bool IsPaused;
    int TimerLength;
    int Mins_White;
    int Mins_Black;
    int Secs_White;
    int Secs_Black;
    unsigned long PrevTime;

    // AI Variables
    String AiDifficulty;
    int AiDifficultyValue;
    bool AgainstAI;

    // Turn Variables
    bool IsWhitesTurn;
    bool IsWhitePlayer;

    // Class Declerations
    InData dataIn;
    OutData dataOut;

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
        // Function to increase the timer length after changing it on the settings screen
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
        // Function to increase the difficulty after changing it on the settings screen
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
    // Displays the PauseScreen
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
    // Resets the screen to the in-game screen
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
        dataOut.SendData("VsHuman", "");
        dataOut.SendData("Difficulty", String(AiDifficultyValue));
      }
      else{
        dataOut.SendData("VsHuman", "True");
      }
      dataOut.SendData("StartGame", "True");
      
    }

    void Tick() {
        // Function to tick the clock every second. Delays are not used for efficiency
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
            dataOut.SendData("TimeOut", "True");
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
            dataOut.SendData("TimeOut", "True");
          }
        }
        else {
          Secs_Black--;
          UpdateClock(Mins_Black, Secs_Black, false);
        }
      }
    }
};

/* Class Declerations */
Settings settings;
OutData outdata;



/*
 * 
 * Microcontroller Pin Setup and Initialization
 * 
 */


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
  for (int sensorPin = 0; sensorPin < 8; sensorPin++) {
    // initialize the output/input pins for sensor matrix:                            // set sensor output pins
    pinMode(sensorColPins[sensorPin], INPUT);                                 // columns will be read for high signal
    pinMode(sensorRowPins[sensorPin], OUTPUT);                                // a row will be set to HIGH one at a time for reading
    // sensor setup done
  }
  for (int LEDPin = 0; LEDPin < 8; LEDPin++) {
    // initialize the output pins for LED matrix:
    pinMode(LEDColPins_R[LEDPin], OUTPUT);                                 // set LED output pins
    pinMode(LEDColPins_G[LEDPin], OUTPUT);
    pinMode(LEDRowPins[LEDPin], OUTPUT);
    // take the col pins (i.e. the cathodes) high to ensure that
    // the LEDS are off:
    digitalWrite(LEDRowPins[LEDPin], HIGH);    // turns off LEDs
  }

  /* Variable Initialization */
  menuIndex = 1;
  maxIndex = 2;


  /* Display Inital Screen */
  WriteToScreen("Classical Chess", mainMenu);
  currentMenu = 0;
}



/*
 * 
 * Main Loop
 * 
 */

void loop() {
    /*
    *   Multiple Paths to Take:
    *   1. If the game is in progress and is not paused
    *       a. Check for the pause button being pressed (denyButton)
    *       b. If the timer is not inf length tick it down if a second has passed
    *       c. Check if a end move button has been pressed (whiteButton, BlackButton)
    *       d. Check for serial data, then process it depending on what it is
    *       e. Check the board state, if something changed let the python side know
    *       f. Important: When the user is happy with their move, they must press 
               the end move button to inform that their turn has ended
    *   2. If the game is in progress and is paused
    *       a. Check for user inputs, the user can either resume or quit
    *       b. If the user resumes, unpause the game and display the in-game screen
    *       c. If the user quits, tell the python why the game is done
    *   3. If the game is not in progress
    *       a. Check for user inputs, depending on the screen a corresponding action will be completed
    *          As the user changes settings update the variables in the settings class.
    *          This way, we can send these values to the game engine on the python side
    */

/*
 * 
 * LCD Start-up 
 * 
 */
 
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


/*
 * 
 * Row/Col Scanning Processes Called Here
 * 
 */

    ReadCurrentBoardState();       // Read the current board state
    delay(10);
    
    CompareBoardStates();       // Find out how the board state changed
    delay(10);

    ReceiveData();     // Read serial port for data from python
    delay(10);


  }
  
/*
 * 
 * LCD Menu Screen and Button Control
 * 
 */
  
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



/*
 * 
 * Board State Functions
 * 
 */

void ChangeState(int *Array, bool state, int count = 8) {
  for (int i = 0; i < count; i++) {
    digitalWrite(Array[i], state);
  }
}

void ReadCurrentBoardState() {
  // iterate over the rows:
  for (int rowNow = 0; rowNow < 8; rowNow++) {
    ChangeState(sensorRowPins, false);                          // set all rows to low
    digitalWrite(sensorRowPins[rowNow], HIGH);                  // set rows HIGH one at a time to start scanning

    for (int colNow = 0; colNow < 8; colNow++) {
      CurrentSensorBoardState[rowNow][colNow] = digitalRead(sensorColPins[colNow]); // reads sensor value and assigns to position in array
      //Serial.print(CurrentSensorBoardState[rowNow][colNow]);               // use this to access the values in serial monitor
      if (CurrentSensorBoardState[rowNow][colNow] == 1) {
        TurnOnLEDs(rowNow,colNow,true,false); 
        }
    }
    //Serial.println();
  }
    //Serial.println();
}


void CompareBoardStates() {
  String data;
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      int difference = CurrentSensorBoardState[row][col] - LastSensorBoardState[row][col];
      LastSensorBoardState[row][col] = CurrentSensorBoardState[row][col];
      if(difference == -1){
        data = "(" + String(row) + "," + String(col) + ")";             // Piece Picked Up
        outdata.SendData("InitialTile", data);
      }
      else if(difference == 1){
        data = "(" + String(row) + "," + String(col) + ")";             // Piece Moved
        outdata.SendData("NextTile", data);
      }
    }
  }
}

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
  WipeArray();
  int maxMoves = ParseData();
  // 5,3 4,2
  for (int i = 0; i < maxMoves; i++) {
    TurnOnLEDs(rowMoves[i], colMoves[i], isGreen, isRed);
  }
}

void ReceiveData() {                                     // if statements for boolean data
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
      // LEDScan(false, false);
    }
  }
}

void TurnOnLEDs(int row, int col, bool isGreen, bool isRed) {
  ChangeState(LEDRowPins, true);
  ChangeState(LEDColPins_G, false);
  ChangeState(LEDColPins_R, false);
  digitalWrite(LEDRowPins[row], false);
  digitalWrite(LEDColPins_G[col], isGreen);
  digitalWrite(LEDColPins_R[col], isRed);
  ChangeState(LEDRowPins, true);
  ChangeState(LEDColPins_G, false);
  ChangeState(LEDColPins_R, false);
}


/*
 * 
 * Debugging Functions
 * 
 */
 
void PrintLog(String log) {
  if (DEBUGGING) {
    Serial.println(log);
  }
}
