# Track the Led Fly

### Assignment n° 1 - Sistemi Embedded & IoT

###### Autori: Andrea Acampora - Accursi Giacomo

---

Il progetto è stato realizzato avvalendosi delle seguenti librerie: 

+ TrueRandom : utilizzata per l'estrazione di numeri casuali. Sostituisce la libreria Random di arduino la quale restituisce ad ogni avvio del sistema gli stessi valori.

+ EnableInterrupt : utilizzata per associare le interrupts ai bottoni. Indispensabile data la necessità di avere 4 pin che possono generare interruzioni. La funzione *attachInterrupt()* consentiva di associare interruzioni solo ai pin 2 e 3.
+ MiniTimerOne : utilizzata per la gestione del timer1. Il timer viene fatto partire al momento dell'accensione del led da tracciare e genera una interruzione che porta alla fine del gioco qualora il giocatore non riuscisse a spingere il bottone corrispondente in tempo. Consigliata dal prof in alternativa alla libreria TimerOne in quanto quest'ultima provoca un'interruzione indesiderata quando viene chiamata la funzione *start()*.

Il led rosso è stato collegato al pin 11 il quale supporta pwm (necessario per il fading) gestito da timer2. In questo modo timer1 viene utilizzato solo durante la fase di gioco.

Ogni volta che cambia il led da tracciare, vengono riassegnate le interruzioni ai bottoni: 

+ La funzione *ledTracked()* verrà chiamata se il giocatore preme il pulsante corretto entro i tempi stabiliti e si consentirà il proseguimento del gioco. 
+ La funzione *setGameOver()* verrà chiamata se il giocatore preme un pulsante sbagliato oppure dal timer in caso di tempo scaduto.

I diversi momenti del gioco sono definiti attraverso 4 stati: 

+ WaitingPlayer : il led rosso pulsa in attesa di iniziare la partita.
+ SetupGame: viene inizializzato l'ambiente di gioco. 
+ InGame: attivo durante tutta la fase di gioco.
+ GameOver: fine della partita. Viene stampato il punteggio e disabilitate le interruzioni

I livelli di gioco sono 8 e di difficoltà crescente. Il tempo a disposizione per premere il bottone è casuale nell'intervallo (Tmin, Tmax).
$Tmin = 1.5 - (1.5 * 5 * livello scelto / 100)$ secondi
$Tmax = Tmin * k$ secondi,  $k = 2.5$ costante per tutti i livelli