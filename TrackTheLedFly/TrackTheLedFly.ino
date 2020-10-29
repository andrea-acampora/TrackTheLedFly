// ACAMPORA ANDREA - 0000873699
// ACCURSI GIACOMO - 0000883208
#include <MiniTimerOne.h>
#include <TrueRandom.h>
#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>

/*Assegnamento dei pin ai componenti*/
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

/*Costanti per la difficoltà e per i tempi di gioco*/
#define TMIN 1500000
#define LEVEL_GAP_PERCENTAGE 5 //Percentuale di diminunuzione di tempo ad ogni livello.
#define K 2.5 //Fattore per il quale viene moltiplicato TMIN per ottenere TMAX (TMAX-TMIN = intervallo generale di tempo a disposizione del player)

/*Stati del gioco*/
#define WAITING_PLAYER 0 
#define SETUP_GAME 1
#define IN_GAME 2
#define GAME_OVER 3 

/*Stati del led da tracciare*/
#define LED_STATE_ON 1
#define LED_STATE_TRACKED 0

/*Tempo minimo necessario per evitare bouncing dei pulsanti (individuato dopo diverse prove sui pulsanti a disposizione)*/
#define BOUNCE_DELAY 160 

uint32_t last_interrupt_time = 0; //istante dell'ultima interruzione
int oldButtonPressed = BUTTON_0;

/*Fading*/
int currIntensity = 0;
int fadeAmount = 5;

int level;
volatile int score;
unsigned long int currentTmin;
unsigned long int currentTmax;
volatile int gameState;//stato corrente del gioco

volatile int indexLedOn;//Indice del led da tracciare
volatile int last_led_tracked;//posizione del led tracciato
unsigned char leds[4];//vettore dei led
volatile int led_state;// stato del led da tracciare

unsigned char buttons[4];//vettore dei bottoni

void setup() {
  Serial.begin(9600);
  Serial.println("Welcome to the Track The Led Fly Game !");
  Serial.println("Press key T1 to start");
  initializePins();
  gameState = WAITING_PLAYER;
  enableInterrupt(buttons[0],setupGame,FALLING);//Interrupt associata al bottone T1 per iniziare la partita
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
    last_led_tracked = null;
    gameState = IN_GAME;
    Serial.println("\nGo!");
  }else if(currentGameState == IN_GAME) {
    noInterrupts();
    int currentLedState = led_state;
    interrupts();
    if(currentLedState == LED_STATE_TRACKED){//Entra solo quando viene cambiato il led da tracciare
      noInterrupts();
      int currentIndexLedOn = indexLedOn;
      interrupts();
      digitalWrite(leds[currentIndexLedOn],HIGH);
      led_state = LED_STATE_ON;
      manageInterrupts();
      setTimeoutTimer();
      if(score>0){
        Serial.println(String("Tracking the fly pos: ")+(last_led_tracked));
      }
    }
  }else if(currentGameState == GAME_OVER){
    noInterrupts();
    /*Salvo il punteggio finale con le interrupts disabilitate perchè sono ancora attive sui bottoni e potrebbero incrementare il punteggio a tempo scaduto*/
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


/*Calcola il nuovo tempo a disposizione per tracciare il led */
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

/*Varia l'intensità della luminosità del led*/
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

/*Assegna le interrupt ai bottoni */
void manageInterrupts(){
  for (int i = 0 ; i<=3 ; i++){
    if(i == indexLedOn){
       enableInterrupt(buttons[i],ledTracked,FALLING);//Interrupt per continuare a giocare
    }else{
       enableInterrupt(buttons[i],setGameOver,FALLING);//Interrupt di gameOver per pulsante sbagliato
    }
  }
}

/*Aumenta lo score e aggiorna il nuovo led da accendere*/
void ledTracked(){
  if(!bouncing()){
      stopAndResetTimer();
      updateScore();
      switchOffLed(leds[indexLedOn]);
      last_led_tracked = indexLedOn +1;
      indexLedOn = getAdjacentLed();
  }
}

/*Calcola il nuovo led da accendere adiacente a quello precedente*/
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

/*Spegne il led e cambia lo stato*/
void switchOffLed(int LED_PIN){
     digitalWrite(LED_PIN,LOW);
     led_state = LED_STATE_TRACKED;
}

/*Reinizializza il timer con il nuovo tempo a disposizione*/
void setTimeoutTimer(){ 
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

/*Disabilita le interruzioni a fine partita e riabilita l'interruzione di inizio gioco*/
void reinitializeInterrupts(){
  for(int i = 0; i<=3; i++){
    disableInterrupt(buttons[i]);
  }
   enableInterrupt(buttons[0],setupGame,FALLING);//INTERRUPT ASSOCIATA AL BOTTONE T1 PER INIZIARE LA PARTITA  
}

/*Comunica se il bottone ha fatto bouncing*/
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
