#include "MiniTimerOne.h"

#define FIRST_PERIOD 500000UL
#define SECOND_PERIOD 750000UL
#define FIRST_COUNT 10
#define SECOND_COUNT 5

volatile unsigned long lastTime;
volatile int firstInterruptCount;
volatile int secondInterruptCount;

void setup() {
    Serial.begin(9600);
    firstInterruptCount = 0;
    secondInterruptCount = 0;
    lastTime = 0;
    /* initializes the timer for the test */
    MiniTimer1.init();
    MiniTimer1.setPeriod(FIRST_PERIOD);
    MiniTimer1.attachInterrupt(firstInterrupt);
    MiniTimer1.start();
}

void loop() {
}

void firstInterrupt(void) {
    unsigned long actualTime = millis();
    if (firstInterruptCount < FIRST_COUNT) {
        /* if the first type of interrupt was fired less than FIRST_COUNT, 
         * print the time elapsed since the last interrupt */
        Serial.println("N. " + String(firstInterruptCount) 
                      + " interrupt1 - Elapsed time: " 
                      + String(actualTime - lastTime) 
                      + "; should be " + String(FIRST_PERIOD / 1000));
        lastTime = actualTime;
        firstInterruptCount++;
    } else {
        /* stops the timer, reset the count, sets the new period and prepare for the 
         * second type of interrupt */
        MiniTimer1.stop();
        MiniTimer1.reset();
        MiniTimer1.setPeriod(SECOND_PERIOD);
        MiniTimer1.attachInterrupt(secondInterrupt);
        lastTime = millis();
        /* starts the second phase of the test */
        MiniTimer1.start();
    }
}

void secondInterrupt(void) {
    unsigned long actualTime = millis();
    if (secondInterruptCount < SECOND_COUNT) {
        /* if the second type of interrupt was fired less than SECOND_COUNT, 
         * print the time elapsed since the last interrupt */
        Serial.println("N. " + String(secondInterruptCount) 
                      + " interrupt2 - Elapsed time: " 
                      + String(actualTime - lastTime) 
                      + "; should be " + String(SECOND_PERIOD / 1000));
        lastTime = actualTime;
        secondInterruptCount++;
    } else {
        /* stops the test */
        MiniTimer1.stop();
        Serial.println("The test has ended, please check the results!");
    }
}
