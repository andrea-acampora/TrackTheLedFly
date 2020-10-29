/*
 * Definition of a class for the management of the 16 bit Timer1 on the ATmega328P mounted on an Arduino Uno.
 * Authors: Giorgia Rondinini, Matteo Castellucci
 * Date: 17-10-2019
 */

#ifndef __MINI_TIMER_1_H__
#define __MINI_TIMER_1_H__

#include "Arduino.h"

/*
 * This class represents an interface from which to manage the Timer 1 of an ATmega328P processor on a Arduino Uno. It
 * can be use to manage it as a timer in the strict sense of the word, with functionalities for only starting, stopping
 * and resetting it. Moreover, it can perform an action when the specified period of time has elapsed. It can count
 * up to four seconds, more or less, because of the size of the ATmega328P's registers. Every value greater than
 * that will be subsituted by the maximum value allowed into the counter register (2^16 - 1).
 */
class MiniTimerOne {
    public:
        /*
         * Returns a pointer to the only instance of this class.
         */
        static MiniTimerOne *getInstance();
        /* 
         * Initializes the timer so as to use the CTC (Clear Timer on Compare match) timer mode, stop the timer from
         * counting CPU clock cycles and reset its counter to zero.
         * It must be called one time before using the timer, otherwise it can bring to unpredictable results.
         */
        void init(void);
        /*
         * Sets the period (in microseconds) of the timer after which the counter is put to zero and an interrupt is generated.
         * Not calling this function before starting the timer can bring to unpredictable results.
         */
        void setPeriod(unsigned long int period);
        /* 
         * Sets the function called when the interrupt is fired and enables the firing of interrupts for the timer.
         * If this method is not called, the function is a routine which does nothing.
         */
        void attachInterrupt(void (*isr)(void));
        /* 
         * Disables the firing of interrupts, and set the function called when an interrupt is fired to a one that does nothing.
         */
        void detachInterrupt(void);
        /* 
         * Starts the timer.
         */
        void start(void);
        /* 
         * Stops the timer. It doesn't reset the value of the counter. That means that calling the start function after this one,
         * without calling the reset function, will cause the first interrupt to be called after the residual time period.
         */
        void stop(void);
        /*
         * Sets the timer counter to zero. It doesn't stop the timer from counting.
         */
        void reset(void);
        /*
         * Returns the callback routine currently called when the interrupt is fired.
         */
        void (*getCallback(void))(void);
    private:
        static MiniTimerOne *SINGLETON;

        MiniTimerOne(void);

        void (*_isrCallback)(void);
        char _clockSelectBits;
};

//Global variable accessible from the Arduino code for facilitating the use of this timer library
extern MiniTimerOne MiniTimer1;

#endif
