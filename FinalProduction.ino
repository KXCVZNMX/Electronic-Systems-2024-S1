#include <SPI.h>
#include <TFT.h>
#define white ST7735_WHITE
#define black ST7735_BLACK
int CS = 6; 
int DC = 5;
int RES = 4;
int SCLK = 13;
long gameStart;
bool playing;
bool lost;
int playerPadPosMin;
int playerPadPosMax;
int cpuPadPosMin;
int cpuPadPosMax;
double cpuPadPos;
double cpuPadSpeed = 0.7;
int CPU = 0;
int PLAYER = 0;
int delaynum = 30;

TFT screen(CS, DC, RES);

class Pong {
  public:
  struct Vec2 {
    double x;
    double y;
  };

  struct Vec2 ballDir;
  struct Vec2 ballPos;

  void ResetBall() {
    cpuPadPos = 42;
    ballPos.x = screen.width() / 2;
    ballPos.y = screen.height() / 2;
    double randomDir = -0.5 + (random(1000) / 1000.0);
    ballDir.x = 3 * cos(randomDir);
    ballDir.y = 3 * sin(-randomDir);
  }

  void UpdateBall() {
    screen.noStroke();
    screen.fill(0, 0, 0);
    screen.rect(ballPos.x, ballPos.y, 4, 4);
    ballPos.x += ballDir.x;
    ballPos.y += ballDir.y;

    if (ballPos.y <= 1 || ballPos.y >= (screen.height() - 5)) { //upper boundary bounce back
      ballDir.y *= -1;
    }

    if (ballPos.x > screen.width() - 14 && !lost) {
      if (ballPos.y < playerPadPosMin || ballPos.y > playerPadPosMax) { //ball out of bounds
        lost = true;
      }
      else { 
        double paddleHit = -0.5 + (ballPos.y - playerPadPosMin) / 28;
        paddleHit *= 1.5;
        ballDir.x = -3 * cos(paddleHit);
        ballDir.y = -3 * sin(-paddleHit);
      }
    }
    if (ballPos.x < 12 && !lost) {
      if (ballPos.y < cpuPadPosMin || ballPos.y > cpuPadPosMax) {
        lost = true;
      }
      else {
        double paddleHit = -0.5 + (ballPos.y - cpuPadPosMin) / 28;
        paddleHit *= 1.5;
        ballDir.x = 3 * cos(paddleHit);
        ballDir.y = -3 * sin(-paddleHit);
      }
    }
    if (lost && ballPos.x > screen.width() || lost && ballPos.x < -2 ) {
      if (ballPos.x > screen.width()) CPU++;
      if (ballPos.x < 0) PLAYER++;
      lost = false;
      playing = false;
      ResetBall();
      gameStart = millis() + 1000;
    }
    else {
      screen.fill(220, 220, 255);
      screen.rect(ballPos.x, ballPos.y, 4, 4);
    }
  }
  
  void UpdatePlayerPad() {
    int val = map(constrain(analogRead(A1), 0, 800), 0, 800, 0, 110);

    screen.noStroke();
    screen.fill(0, 0, 0);
    screen.rect(screen.width() - 8, 0, 8, screen.height());
    screen.fill(255, 255, 255);
    screen.rect(screen.width() - 8, val, 4, 28);
    playerPadPosMin = val;
    playerPadPosMax = val + 28;
    Serial.print(val);
    Serial.print(" ");
    Serial.println(analogRead(A1));
  }

  void UpdateCPUPad() {
    if (ballPos.y > cpuPadPos + 14) {
      cpuPadPos += cpuPadSpeed;
    }
    if (ballPos.y < cpuPadPos + 14) {
      cpuPadPos -= cpuPadSpeed;
    }

    cpuPadPos = constrain(cpuPadPos, 0, 84);

    screen.noStroke();
    screen.fill(0, 0, 0);
    screen.rect(0, 0, 8, screen.height());
    screen.fill(255, 255, 255);
    screen.rect(4, cpuPadPos, 4, 28);

    cpuPadPosMin = cpuPadPos;
    cpuPadPosMax = cpuPadPos + 28;
  }
};

Pong pong;

void setup() {
  Serial.begin(9600);
  digitalWrite(RES, HIGH);
  screen.begin();
  screen.background(0,0,0);
  screen.stroke(255,255,255);

  CPU = 0;
  PLAYER = 0;

  playing = false;
  pong.ResetBall();
  int gameStart = millis() + 1000;
}

void loop() {
  delay(delaynum);
  pong.UpdatePlayerPad();
  pong.UpdateCPUPad();
  if (millis() > gameStart) {
    playing = true;
  }
  if (playing) {
    pong.UpdateBall();
  }
}