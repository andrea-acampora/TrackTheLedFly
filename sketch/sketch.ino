#include <TimerOne.h>
#include <EnableInterrupt.h>

#define LED_START 10
#define POT A5
#define BUTTON_0 2
#define BUTTON_1 3
#define BUTTON_2 4
#define BUTTON_3 5
#define LED_0 6
#define LED_1 7
#define LED_2 8
#define LED_3 9
#define TMIN 1000000
#define LEVEL_GAP_PERCENTAGE 5 //PERCENTUALE DI DIMINUZIONE DI TEMPO DI OGNI LIVELLO 
#define K 1.75

#define WAITING_PLAYER 0
#define SETUP_GAME 1
#define IN_GAME 2
#define GAME_OVER 3

int score;
int currIntensity = 0;
int fadeAmount = 5;
int level;
int indexLedOn;
long currentTmin;
long currentTmax;
int currentGameState;
unsigned char currentLedOn;
unsigned char leds[4];
unsigned char buttons[4];

void setup() {
  Serial.begin(9600);
  Serial.println("Welcome to the Track The Led Fly Game !");
  Serial.println("Press key T1 to start");
  initializePins();
  currentGameState = WAITING_PLAYER;
  enableInterrupt(buttons[0],setupGame,FALLING);
}

void loop() {
  if(currentGameState == WAITING_PLAYER){
    fade(LED_START);
  }else if(currentGameState == SETUP_GAME){
    Serial.println("Game Started!");
    digitalWrite(LED_START,LOW);
    score = 0;
    level = map(analogRead(POT),0,1023,0,7);
    currentTmin = TMIN -(TMIN * LEVEL_GAP_PERCENTAGE * level / 100);
    currentTmax = currentTmin * K;
    indexLedOn = random(0,4);
    manageInterrupts();
    currentGameState = IN_GAME;
  }else if(currentGameState == IN_GAME) {
    Serial.println(indexLedOn);
    digitalWrite(leds[indexLedOn],HIGH);
    Timer1.initialize(getAvailableTime);
    Timer1.attachInterrupt(setGameOver);
  }else if(currentGameState == GAME_OVER){
    // STAMPA SCORE
    // RICHIAMA SETUP
    //SPEGNI ULTIMO LED VERDE E CHIAMA SETUP()
  }
  
}
void setupGame(){
    currentGameState = SETUP_GAME;
}

long getAvailableTime(){
      currentTmin = currentTmin - (currentTmin *7/8);
      currentTmax = currentTmin * K;
      return random(currentTmin,currentTmax);
}

void fade(int LED_PIN){
  analogWrite(LED_PIN, currIntensity);   
  currIntensity = currIntensity + fadeAmount;
  if (currIntensity == 0 || currIntensity == 255) {
    fadeAmount = -fadeAmount ; 
  }
  delay(20);
}

void initializePins(){
    pinMode(LED_START,OUTPUT);
    pinMode(POT,INPUT);
    leds[0] = LED_0;
    leds[1] = LED_1;
    leds[2] = LED_2;
    leds[3] = LED_3;
    buttons[0] = BUTTON_0;
    buttons[1] = BUTTON_1;
    buttons[2] = BUTTON_2;
    buttons[3] = BUTTON_3;
    for (int i = 0; i <= 3; i++){
       pinMode(leds[i], OUTPUT);  
       pinMode(buttons[i],INPUT_PULLUP);
    }
}

void manageInterrupts(){
  for (int i = 0 ; i<=3 ; i++){
    if(i == indexLedOn){
       enableInterrupt(buttons[i],nextLed,FALLING);
    }else{
       enableInterrupt(buttons[i],setGameOver,FALLING);
    }
    
  }
}

void nextLed(){
  //PRIMA SPEGNI LED POI ACCENDI NUOVO
}

void setGameOver(){
  currentGameState = GAME_OVER;
}