// ACAMPORA ANDREA - 0000873699
// ACCURSI GIACOMO - 0000883208
#include <MiniTimerOne.h>
#include <TrueRandom.h>
#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>

#define LED_START 11
#define POT A5
#define BUTTON_0 2
#define BUTTON_1 3
#define BUTTON_2 4
#define BUTTON_3 5
#define LED_0 6
#define LED_1 7
#define LED_2 8
#define LED_3 9
#define TMIN 1500000
#define LEVEL_GAP_PERCENTAGE 5 //PERCENTUALE DI DIMINUZIONE DI TEMPO DI OGNI LIVELLO 
#define K 2.5 //FATTORE PER IL QUALE VIENE MOLTIPLICATO TMIN PER OTTENERE TMAX (TMAX-TIM = INTERVALLO GENERALE DI TEMPO A DISPOSIZIONE DEL PLAYER)
#define WAITING_PLAYER 0
#define SETUP_GAME 1
#define IN_GAME 2
#define GAME_OVER 3
#define LED_ON 1
#define LED_TRACKED 0
#define BOUNCE_DELAY 160 //TEMPO NECESSARIO INDIVIDUATO DOPO NUMEROSE PROVE PER EVITARE BOUNCING CON I PULSANTI A DISPOSIZIONE

uint32_t last_interrupt_time = 0;
volatile int score;
int currIntensity = 0;
int fadeAmount = 5;
int level;
int oldButtonPressed = BUTTON_0;
volatile int indexLedOn;
unsigned long int currentTmin;
unsigned long int currentTmax;
volatile int gameState;
volatile int led_state;
unsigned char leds[4];
unsigned char buttons[4];

void setup() {
  Serial.begin(9600);
  Serial.println("Welcome to the Track The Led Fly Game !");
  Serial.println("Press key T1 to start");
  initializePins();
  gameState = WAITING_PLAYER;
  enableInterrupt(buttons[0],setupGame,FALLING);//INTERRUPT ASSOCIATA AL BOTTONE T1 PER INIZIARE LA PARTITA
  MiniTimer1.init();
  MiniTimer1.attachInterrupt(setGameOver);
}

void loop() {
  noInterrupts();
  int currentGameState = gameState;
  interrupts();
  if(currentGameState == WAITING_PLAYER){
    fadeStep();
  }else if(currentGameState == SETUP_GAME){
    switchOffLed(LED_START);
    score = 0;
    level = map(analogRead(POT),0,1023,0,7);
    currentTmin = TMIN -(TMIN * LEVEL_GAP_PERCENTAGE * level / 100);
    currentTmax = currentTmin * K;
    indexLedOn = TrueRandom.random(0,4);
    gameState = IN_GAME;
    Serial.println("\nGo!");
  }else if(currentGameState == IN_GAME) {
    noInterrupts();
    int currentLedState = led_state;
    interrupts();
    if(currentLedState == LED_TRACKED){// CICLO PER NON ACCENDERE IL LED DI CONTINUO
      noInterrupts();
      int currentIndexLedOn = indexLedOn;
      interrupts();
      digitalWrite(leds[currentIndexLedOn],HIGH);
      led_state = LED_ON;
      manageInterrupts();
      setTimeoutTimer();// FUNZIONE PER FARE PARTIRE IL TIMER DI SCADENZA
      Serial.println(String("Tracking the fly pos: ")+(currentIndexLedOn+1));
    }
  }else if(currentGameState == GAME_OVER){
    noInterrupts();/*Salvo il punteggio finale con le interrupts disabilitate perchè sono ancora attive sui bottoni e potrebbero incrementare il punteggio a tempo scaduto*/
    int finalScore = score;
    interrupts();
    switchOffLed(leds[indexLedOn]);
    Serial.println(String("GAME OVER! - SCORE : ") +finalScore);
    digitalWrite(LED_START,HIGH);
    delay(2000);
    gameState = WAITING_PLAYER;
    reinitializeInterrupts();
  }
}

void setupGame(){
    if(!bouncing()){
      gameState = SETUP_GAME;
    }
}

unsigned long int getAvailableTime(){
      if (score>0){
        currentTmin = currentTmin * 7/8;
        currentTmax = currentTmin * K;
        return TrueRandom.random(currentTmin,currentTmax);
      }else{
        currentTmax = currentTmin * K;      
        return TrueRandom.random(currentTmin,currentTmax);
      }
}

void fadeStep(){
  analogWrite(LED_START, currIntensity);   
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
       enableInterrupt(buttons[i],ledTracked,FALLING);
    }else{
       enableInterrupt(buttons[i],setGameOver,FALLING);
    }
  }
}


void ledTracked(){
  if(!bouncing()){
      stopAndResetTimer();
      updateScore();
      switchOffLed(leds[indexLedOn]);
      indexLedOn = getAdjacentLed();
  }
}

int getAdjacentLed(){
    int random_vec[2];
    switch(indexLedOn){
      case 0 :
        random_vec[0] = 3;
        random_vec[1] = 1;
        break;
      case 1:
        random_vec[0] = 0;
        random_vec[1] = 2;
        break;
      case 2 :   
        random_vec[0] = 1;
        random_vec[1] = 3;
        break;
      case 3 : 
        random_vec[0] = 2;
        random_vec[1] = 1;
        break;
    }
    return random_vec[TrueRandom.random(0,2)];
}

void switchOffLed(int LED_PIN){
     digitalWrite(LED_PIN,LOW);
     led_state = LED_TRACKED;
}

void setTimeoutTimer(){ //FUNZIONE PER SETTARE IL TEMPO OGNI VOLTA CHE SI ACCENDE UN NUOVO LED DOPO CHE IL GIOCATORE PREME IL BOTTONE
    MiniTimer1.setPeriod(getAvailableTime());
    MiniTimer1.start();
}

void updateScore(){
    score = score +1;
}

void stopAndResetTimer(){
    MiniTimer1.stop();
    MiniTimer1.reset(); 
}

void setGameOver(){
  if(!bouncing()){
    MiniTimer1.stop();
    MiniTimer1.reset();
    gameState = GAME_OVER;
  }
}

void reinitializeInterrupts(){
  for(int i = 0; i<=3; i++){
    disableInterrupt(buttons[i]);
  }
   enableInterrupt(buttons[0],setupGame,FALLING);//INTERRUPT ASSOCIATA AL BOTTONE T1 PER INIZIARE LA PARTITA  
}

bool bouncing(){
  uint32_t interrupt_time = millis();
  if(interrupt_time - last_interrupt_time < BOUNCE_DELAY && oldButtonPressed==arduinoInterruptedPin ){
    last_interrupt_time = interrupt_time;
    return true;
  }
  last_interrupt_time = interrupt_time;
  oldButtonPressed = arduinoInterruptedPin;
  return false;
}
