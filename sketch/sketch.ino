#include <MiniTimerOne.h>
#include <TimerOne.h>
#define EI_ARDUINO_INTERRUPTED_PIN

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
#define TMIN 2000000
#define LEVEL_GAP_PERCENTAGE 5 //PERCENTUALE DI DIMINUZIONE DI TEMPO DI OGNI LIVELLO 
#define K 2

#define WAITING_PLAYER 0
#define SETUP_GAME 1
#define IN_GAME 2
#define GAME_OVER 3

#define LED_ON 1
#define LED_OFF 0

#define BOUNCE_DELAY 800

uint32_t last_interrupt_time = 0;


int score;
int currIntensity = 0;
int fadeAmount = 5;
int level;
volatile int indexLedOn;
unsigned long int currentTmin;
unsigned long int currentTmax;
volatile int currentGameState;
volatile int led_state = LED_OFF;
unsigned char currentLedOn;
volatile unsigned char leds[4];
volatile unsigned char buttons[4];

void setup() {
  Serial.begin(9600);
  Serial.println("Welcome to the Track The Led Fly Game !");
  Serial.println("Press key T1 to start");
  initializePins();
  currentGameState = WAITING_PLAYER;
  enableInterrupt(buttons[0],setupGame,FALLING);//INTERRUPT ASSOCIATA AL BOTTONE T1 PER INIZIARE LA PARTITA
}

void loop() {
  if(currentGameState == WAITING_PLAYER){
    fade(LED_START);
  }else if(currentGameState == SETUP_GAME){
    digitalWrite(LED_START,LOW);
    score = 0;
    level = map(analogRead(POT),0,1023,0,7);
    currentTmin = TMIN -(TMIN * LEVEL_GAP_PERCENTAGE * level / 100);
    currentTmax = currentTmin * K;
    indexLedOn = random(0,4);
    manageInterrupts(); //ASSEGNA LE INTERRUZIONI AI BOTTONI 
    currentGameState = IN_GAME;
    Serial.println("Go!");
  }else if(currentGameState == IN_GAME) {
    if(led_state == LED_OFF){// CICLO PER NON ACCENDERE IL LED DI CONTINUO
        digitalWrite(leds[indexLedOn],HIGH);
        led_state = LED_ON;
        setTimeoutTimer();// FUNZIONE PER FARE PARTIRE IL TIMER DI SCADENZA
    }
  }else if(currentGameState == GAME_OVER){
    // STAMPA SCORE
    // RICHIAMA SETUP
    //SPEGNI ULTIMO LED VERDE E CHIAMA SETUP()
    currentGameState = WAITING_PLAYER;
  }
  
}
void setupGame(){
    currentGameState = SETUP_GAME;
}

unsigned long int getAvailableTime(){
      if (score>0){
      currentTmin = currentTmin - (currentTmin *7/8);
      currentTmax = currentTmin * K;
      return random(currentTmin,currentTmax);
      }else{
        currentTmax = currentTmin * K;      
        return random(currentTmin,currentTmax);
      }
}

void fade(int LED_PIN){
  analogWrite(LED_PIN, currIntensity);   
  currIntensity = currIntensity + fadeAmount;
  if (currIntensity == 0 || currIntensity == 255) {
    fadeAmount = -fadeAmount ; 
  }
  delayMicroseconds(20000);
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
       enableInterrupt(buttons[i],setGameOverFromButton,FALLING);
    }
    
  }
}

void nextLed(){
  if(!bouncing()){
  score = score +1;
  digitalWrite(leds[indexLedOn],LOW);
  MiniTimer1.stop();
  MiniTimer1.reset(); 
  }
}

void setTimeoutTimer(){ //FUNZIONE PER SETTARE IL TEMPO OGNI VOLTA CHE SI ACCENDE UN NUOVO LED DOPO CHE IL GIOCATORE PREME IL BOTTONE
        MiniTimer1.attachInterrupt(setGameOver);
        MiniTimer1.setPeriod(getAvailableTime());
        MiniTimer1.start();
}

void setGameOver(){
  if(!bouncing()){
  Serial.println("GAME OVER!");
  MiniTimer1.stop();
  MiniTimer1.detachInterrupt();
  MiniTimer1.reset();
  currentGameState = GAME_OVER;
  }
}

void setGameOverFromButton(){
  if(!bouncing()){
  Serial.println(String("GAME OVER FROM BUTTON : ")+ arduinoInterruptedPin );
  MiniTimer1.stop();
  MiniTimer1.detachInterrupt();
  MiniTimer1.reset();
  currentGameState = GAME_OVER;
  }
}

bool bouncing(){
  uint32_t interrupt_time = millis();
  if(interrupt_time - last_interrupt_time > BOUNCE_DELAY){
    last_interrupt_time = interrupt_time;
    return false;
  }
  last_interrupt_time = interrupt_time;
  return true;
}
