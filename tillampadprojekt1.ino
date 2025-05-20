#include "U8glib.h"
#include <Wire.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

const int BUTTONG = 8;
const int BUTTONR = 6;
const int BUTTONB = 5;
const int BUTTONY = 7;

unsigned long startMillis = 0;
int score = 0;
unsigned long finalElapsedSeconds = 0; 

struct TriviaQuestion {
  String question;
  String choices[4];
  int correctChoice;
};

TriviaQuestion questions[] = {
  {
    "Square root of 144?",
    {"10", "11", "12", "13"},
    2
  },
  {
    "Who wrote 'Hamlet'?",
    {"Charles Dickens", "William Shakespeare", "Mark Twain", "Jane Austen"},
    1
  },
  {
    "Capital of Mongolia?",
    {"Ulaanbaatar", "Astana", "Bishkek", "Tashkent"},
    0
  },
  {
    "Major component of air?",
    {"Oxygen", "CO2", "Nitrogen", "Helium"},
    2
  },
  {
    "Hardest natural substance?",
    {"Gold", "Iron", "Diamond", "Quartz"},
    2
  },
  {
    "Derivative of sin(x)?",
    {"cos(x)", "-cos(x)", "sin(x)", "-sin(x)"},
    0
  }
};

int totalQuestions = sizeof(questions) / sizeof(questions[0]);
int currentQuestionIndex = 0;

enum GameMode { MODE_QUESTION, MODE_FEEDBACK, MODE_GAMEOVER };
GameMode gameMode = MODE_QUESTION;
String feedbackMessage = "";
unsigned long feedbackStartMillis = 0;
const unsigned long feedbackDuration = 1000;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(BUTTONG, INPUT);
  pinMode(BUTTONR, INPUT);
  pinMode(BUTTONB, INPUT);
  pinMode(BUTTONY, INPUT);
  
  startMillis = millis();
}

void loop() {
  updateDisplay();
  
  if (gameMode == MODE_QUESTION) {
    if (digitalRead(BUTTONG) == HIGH) {
      checkAnswer(0);
      Serial.println("Green pressed");
      delay(200);
    } 
    else if (digitalRead(BUTTONR) == HIGH) {
      checkAnswer(1);
      Serial.println("Red pressed");
      delay(200);
    } 
    else if (digitalRead(BUTTONB) == HIGH) {
      checkAnswer(2);
      Serial.println("Blue pressed");
      delay(200);
    } 
    else if (digitalRead(BUTTONY) == HIGH) {
      checkAnswer(3);
      Serial.println("Yellow pressed");
      delay(200);
    }
  } 
  else if (gameMode == MODE_FEEDBACK) {
    if (millis() - feedbackStartMillis >= feedbackDuration) {
      if (currentQuestionIndex < totalQuestions) {
        gameMode = MODE_QUESTION;
      } else {
        finalElapsedSeconds = (millis() - startMillis) / 1000;
        gameMode = MODE_GAMEOVER;
      }
    }
  }
  else if (gameMode == MODE_GAMEOVER) {
    if (digitalRead(BUTTONG) == HIGH || digitalRead(BUTTONR) == HIGH ||
        digitalRead(BUTTONB) == HIGH || digitalRead(BUTTONY) == HIGH) {
      restartGame();
      delay(200);
    }
  }
}

void checkAnswer(int userChoice) {
  if (currentQuestionIndex >= totalQuestions)
    return;
  
  int correct = questions[currentQuestionIndex].correctChoice;
  if (userChoice == correct) {
    score++;
    feedbackMessage = "Correct!";
  } else {
    feedbackMessage = "Wrong!";
  }
  
  feedbackStartMillis = millis();
  currentQuestionIndex++;
  gameMode = MODE_FEEDBACK;
}

void updateDisplay() {

  String timerStr;
  if (gameMode == MODE_GAMEOVER) {
    timerStr = "T: " + String(finalElapsedSeconds);
  } else {
    unsigned long elapsedSeconds = (millis() - startMillis) / 1000;
    timerStr = "T: " + String(elapsedSeconds);
  }
  String scoreStr = "S: " + String(score);

  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_6x10);
    u8g.drawStr(0, 10, timerStr.c_str());
    u8g.drawStr(60, 10, scoreStr.c_str());

    if (gameMode == MODE_QUESTION && currentQuestionIndex < totalQuestions) {
      String qStr  = questions[currentQuestionIndex].question;
      String ansG  = "G: " + questions[currentQuestionIndex].choices[0];
      String ansR  = "R: " + questions[currentQuestionIndex].choices[1];
      String ansB  = "B: " + questions[currentQuestionIndex].choices[2];
      String ansY  = "Y: " + questions[currentQuestionIndex].choices[3];
      
      u8g.drawStr(0, 25, qStr.c_str());
      u8g.drawStr(0, 40, ansG.c_str());
      u8g.drawStr(0, 50, ansR.c_str());
      u8g.drawStr(64, 40, ansB.c_str());
      u8g.drawStr(64, 50, ansY.c_str());
    } 
    else if (gameMode == MODE_FEEDBACK) {
      u8g.drawStr(0, 30, feedbackMessage.c_str());
    } 
    else if (gameMode == MODE_GAMEOVER) {
      u8g.drawStr(0, 30, "Game Over!");
      u8g.drawStr(0, 45, "Press any button");
      u8g.drawStr(0, 60, "to restart.");

    }
  } while (u8g.nextPage());
}

void restartGame() {
  score = 0;
  currentQuestionIndex = 0;
  startMillis = millis();
  gameMode = MODE_QUESTION;
}
