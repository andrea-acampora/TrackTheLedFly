# MiniTimerOne

## Preface

Inspired by the ![TimerOne](https://github.com/PaulStoffregen/TimerOne) library, this library aims to provide an easy and minimal
way to use the Timer1 of the ATMega328P on the Arduino Uno for generating periodic interrupts. It can be used if you need a timer
working as a traditional one, which counts the time from with a particular time period after which it restarts from zero, if not stopped.
Differently from the original TimerOne library, it doesn't have PWM functionalities, for a more minimal and coherent user interface.

## Acknowledgements

This library was made with love and rage by Matteo Castellucci and Giorgia Rondinini, but based on preexisting work made by the
creators of the TimerOne library, the Arduino playground and the Atmel manual for the ATMega328P.

## How to's

To use this library, simply use the object "MiniTimer1" and call on it the below methods:

* init(): initializes the timer. You need to call this method once before using it. You can call it again, but seriously, it
isn't required.
* setPeriod(period): sets the period after which the timer signals that the time is up. You need to call this method before starting the timer at least one time. You can call it again whenever you need to change the timer period.
  * "period": the time which elapses before the timer signals that the time is up. The value is in *microseconds*.
* start(): starts the timer.
* stop(): stops the timer. If you start it again after stopping it, the timer will restart from where it was.
* reset(): resets the timer count. It doesn't stop it, there is another method to do that.
* attachInterrupt(routine): sets the action to be performed when the time is up.
  * "routine": the function to be called when the time is up. Its signature must be in the form of "void func(void)".
* detachInterrupt(): unsets the action to be performed when the time is up.

## Warnings and general notes

Please, please, please (we can't stress this enough) **initialize your timer before using any method** listed above. We don't know
in which state you left your timer, so we can't make any guarantees on what is going to happen. For the same reasons, please
**set a period at least once during your use of the timer**. After all, what's a timer without its time to count? :)  
Due to technical limitations of the ATmega328P, this timer can have a period of *maximum 4.194304 seconds*. If you need more 
time, we can't help you.
