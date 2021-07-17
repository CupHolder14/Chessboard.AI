/* Libraries */
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

/* Debugging Variables */
bool DEBUGGING = true;

/* Local Variables */
// UNO pin setup
int upButton = 7;
int downButton = 8;
int acceptButton = 12;
int denyButton = 13;
int WhiteButton = 7;
int BlackButton = 8;

// menu variables
int currentMenu;
int menuIndex;
int maxIndex;

// menu options
String mainMenu[] = {"Play", "Settings"};                    // 0
String playMenu[] = {"Vs. AI", "Vs. Player"};               // 1
String settingsMenu[] = {"Timer Length", "AI Difficulty"}; // 2
String ChangeSideMenu[] = {"White", "Black"};             // 3
String pauseMenu[] = {"Resume", "Quit"};                 // 4
String AcceptMenu[] = {"Yes","No"};                     // 5
String TimerLengthMenu[] = {"    < 10 mins >", "Confirm"};        // 6
String AiDifficultyMenu[] = {"    < Easy >","Confirm"};         // 7



/* Classes */
// Data to be received
class InData{
  public:
    bool GAMEOVER; // True = game is over, false = game is active
    int Winner; // 0 = white, 1 = black, 2 = stalemate
    // operation

  InData(){
    this->GAMEOVER = false;
    this->Winner = 0;
  }

  void SendData(String s){
    Serial.print(s);
  }
};

// Data to be sent
class OutData{
  public:
    bool BlackPlayer; // false = vs. AI, true = vs. Human
    bool GAMEOVER; // True = game is over, false = game is active

  OutData(){
      this->BlackPlayer = false;
      this->GAMEOVER = 0;
  }
};

// Internal Settings
class Settings{
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
    
    Settings(){
      this->GameInProgress = false;
      this->InfLength = false;
      this->TimerLength = 10;
      this->AiDifficulty = "Easy";
      this->PrevTime = 0;
      this->IsWhitesTurn = true;
      this->IsPaused = false;
    }

    void IncreaseTimer(String* TimerLengthMenu){
      if(TimerLength == 20 && InfLength == false){
        InfLength = true;
        TimerLengthMenu[0] ="    < unlimited >";
      }
      else if(TimerLength == 20 && InfLength){
        TimerLength = 5;
        InfLength = false;
        TimerLengthMenu[0] = "    < " + String(TimerLength) + " mins >";
      }
      else{
        TimerLength = TimerLength + 5;
        TimerLengthMenu[0] = "    < " + String(TimerLength) + " mins >";
      }
    }

    void IncreaseDifficulty(String* AiDifficultyMenu){
      if(AiDifficulty == "Easy"){
        AiDifficulty = "Medium";
      }
      else if(AiDifficulty == "Medium"){
        AiDifficulty = "Hard";
      }
      else if(AiDifficulty == "Hard"){
        AiDifficulty = "Easy";
      }
      AiDifficultyMenu[0] = "    < " + AiDifficulty + " >";
    }

    void PauseScreen(){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Game Paused");
      lcd.setCursor(1,1);
      lcd.print("Resume");
      lcd.setCursor(1,2);
      lcd.print("Quit");
      lcd.setCursor(0,1);
      lcd.print(">");
    }

    void ResumeScreen(){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("In Game");
      lcd.setCursor(9,1);
      lcd.print("White Black");
      lcd.setCursor(0,2);
      lcd.print("Time");
      lcd.setCursor(9,2);
      if(InfLength){
        lcd.print("--:-- --:--");
      }
      else{
        UpdateClock(Mins_White, Secs_White, true);
        if(AgainstAI){
          lcd.setCursor(15,2);
          lcd.print("--:--");
        }
        else{
          UpdateClock(Mins_Black, Secs_Black, false);
        }
      }
    }

    void StartGame(bool againstAI){
      // To start the game, we need to display the in game screen (timer, last move, etc.)
      // We need to initalize the clock by calculating the minutes, and seconds
      // We need to then tell the python to start the AI code, if we are versing an AI

      GameInProgress = true;

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("In Game");
      lcd.setCursor(9,1);
      lcd.print("White Black");
      lcd.setCursor(0,2);
      lcd.print("Time");
      lcd.setCursor(9,2);
      if(InfLength){
        lcd.print("--:-- --:--");
      }
      else{
        Mins_White = TimerLength;
        Mins_Black = TimerLength;
        Secs_White = 0;
        Secs_Black = 0;
        String mins = TimerLength >= 10 ? String(TimerLength) : "0" + String(TimerLength);
        if(AgainstAI){
          lcd.print(mins + ":" + "00 " + "--" + ":" + "--");
        }
        else{
          lcd.print(mins + ":" + "00 " + mins + ":" + "00");
        }
      }
      lcd.setCursor(0,3);
      lcd.print("Last Move");
      
      if(againstAI){
        // Tell python to start ai engine
        Data_IN.SendData("Start vs AI");
      }
    }
    
    void UpdateClock(int Mins, int Secs, bool IsWhitesTurn){
      if(IsWhitesTurn){
        lcd.setCursor(9,2);
      }
      else{
        lcd.setCursor(15,2);
      }
      String mins = Mins >= 10 ? String(Mins) : "0" + String(Mins);
      String secs = Secs >= 10 ? String(Secs) : "0" + String(Secs);
      lcd.print(mins + ":" + secs);
    }

    void Tick(){
      if(IsWhitesTurn){
        if(Secs_White == 0){
          if(Mins_White != 0){
            Mins_White--;
            Secs_White = 59;
            UpdateClock(Mins_White, Secs_White, true);
          }
          else{
            // Timer Depleted for white
            GameInProgress = false;
            lcd.print("                    ");
            lcd.print("Game Over - Black Wins");
            Data_IN.SendData("White Lost");
          }
        }
        else{
          Secs_White--;
          UpdateClock(Mins_White, Secs_White, true);
        }
      }
      else{
        if(AgainstAI){
          return;
        }
        if(Secs_Black == 0){
          if(Mins_Black != 0){
            Mins_Black--;
            Secs_Black = 59;
            UpdateClock(Mins_Black, Secs_Black, false);
          }
          else{
            // Timer Depleted for black
            GameInProgress = false;
            lcd.setCursor(0,0);
            lcd.print("                    ");
            lcd.setCursor(0,0);
            lcd.print("Game Over - White Wins");
            Data_IN.SendData("Black Lost");
          }
        }
        else{
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
  if(DEBUGGING){
    Serial.begin(9600);
  }

  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(acceptButton, INPUT_PULLUP);
  pinMode(denyButton, INPUT_PULLUP);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  
  menuIndex = 1;
  maxIndex = 2;

  WriteToScreen("Classical Chess", mainMenu);
  currentMenu = 0;
}

/*
 * Wait for user input via buttons (accept, deny, ...)
 * On user input, redirect to corresponding screen (main menu, settings menu, ...)
 * 
 * If accept button is pressed, check current menu, check index, then pan to the corresponding screen
 * 
 */
void loop() {
  if(settings.GameInProgress && !settings.IsPaused){
    if(!digitalRead(denyButton)){
        PrintLog("Paused");
        settings.IsPaused = true;
        menuIndex = 1;
        settings.PauseScreen();
        while(!digitalRead(denyButton));
    }
    if(!settings.InfLength){
      unsigned long currentTime = millis();
      if(currentTime - settings.PrevTime >= 1000){
        settings.PrevTime = currentTime;
        settings.Tick();
      }
    }
    if(!digitalRead(WhiteButton) && settings.IsWhitesTurn){
      settings.IsWhitesTurn = !settings.IsWhitesTurn;
      while(!digitalRead(WhiteButton));
    }
    if(!digitalRead(BlackButton) && !settings.IsWhitesTurn){
      settings.IsWhitesTurn = !settings.IsWhitesTurn;
      while(!digitalRead(WhiteButton));
    }
    // HELEN CODE
  }
  else if(settings.GameInProgress && settings.IsPaused){
    if(!digitalRead(upButton)){
      if(menuIndex != 1){
        menuIndex--;
        RefreshScreen("up");
      }
      while(!digitalRead(upButton)); 
    }
    if(!digitalRead(downButton)){
      if(menuIndex != 3 && menuIndex != maxIndex){
        menuIndex++;
        RefreshScreen("down"); 
      }
      while(!digitalRead(downButton));
    }
    if(!digitalRead(acceptButton)){
      switch(menuIndex){
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
  else{
    if(!digitalRead(upButton)){
      PrintLog("upButton");
      if(menuIndex != 1){
        menuIndex--;
        RefreshScreen("up");
      }
      while(!digitalRead(upButton)); 
    }
  if(!digitalRead(downButton)){
    PrintLog("downButton");
    if(menuIndex != 3 && menuIndex != maxIndex){
      menuIndex++;
      RefreshScreen("down"); 
    }
    while(!digitalRead(downButton));
  }
  if(!digitalRead(acceptButton)){
    PrintLog("acceptButton");
    switch(currentMenu){
      // Main Menu
      case 0:
        switch(menuIndex){
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
        switch(menuIndex){
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
        switch(menuIndex){
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
        switch(menuIndex){
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
        switch(menuIndex){
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
    while(!digitalRead(acceptButton));
  }
  if(!digitalRead(denyButton)){
    PrintLog("denyButton");
    switch(currentMenu){
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
  while(!digitalRead(denyButton));
  }
}

void RefreshScreen(String op){
  if(op == "up"){
    lcd.setCursor(0,menuIndex + 1);
    lcd.print(" ");
    lcd.setCursor(0,menuIndex);
    lcd.print(">");
  }
  if(op == "down"){
    lcd.setCursor(0,menuIndex - 1);
    lcd.print(" ");
    lcd.setCursor(0,menuIndex);
    lcd.print(">");
  }
}

void WriteToScreen(String title, String options[]){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(title);
  for(int i = 1; i <= sizeof(options) && i <= 3; i++){
    lcd.setCursor(1,i);
    lcd.print(options[i-1]);
  }
  lcd.setCursor(0,menuIndex);
  lcd.print(">");
}

void PrintLog(String log){
  if(DEBUGGING){
    Serial.println(log);
  }
}