# Tri-timer
Pebble triathlon stopwatch
## Description
A stopwatch that times and stores the five phases of a triathlon (swim - transition 1 - bike - transition 2 - run), as well as the total time
## Use
* Press the DOWN button (marked with a ">" symbol) to start the timer, advance to the next phase, and stop the timer at the finish
* Use the UP button to look through the recorded times. If the watch is timing another phase, a flashing indicator appears in the bottom left of the screen indicating the phase being timed. After ten seconds, the display returns automatically to the phase currently being timed, unless the total time is being viewed
* Use the SET button to pause or restart the timer (only really needed in the event of a problem)
* When no timer is running, hold down the SET button for 3 seconds to reset all the timers
* Timers are held in persistent storage, so you can leave the app, even with timers running, and return later if you need to.

* If you accidentally advance the timer to the wrong phase, you can repeatedly press the DOWN button to roll through the timers to the correct phase
** TO DO: Sanity check on timers so resets SWIM/BIKE/RUN if less than 1 minute on advance to next to timer
