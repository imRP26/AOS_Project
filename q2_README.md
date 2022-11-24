# Q2 - Dish-Washing Problem
<b> Question Description </b>
- There are n taps and n-1 scrubs (one between each consecutive taps).
- There are m students. A student has to wash three utensils in order - his plate, glass and spoon.
- A student should acquire a free tap first and will  continue to occupy the same spot until he has washed all utensils. If a person is waiting for a tap, he continues to wait.
- Once a person has found a tap he will look for a free scrubber next to his tap. He must coordinate with his neighbors to finish washing his utensils as fast as possible.
- A plate takes 4 seconds to scrub and 5 seconds to wash.
- A glass takes 3 seconds to scrub and 3 seconds to wash.
- A spoon takes 2 seconds to scrub and 1 second to wash.
- While a person is washing his utensils, his neighbor can scrub his utensils.

***

<b> Solution </b>
- The values of n and m are passed as command line arguments.
- There is a semaphore for each scrubber and each tap.
- Each student is simulated as a different thread.
- A student first randomly goes to a tap and locks it if it is free or waits if it is locked.
- If the tap is free the student repeatedly check his left and right side to find an open scrubber (using ```sem_getvalue()``` function). Once an open scrubber is found the scrubber is also locked.
- The student scrubs his utensil for the required number of seconds and the unlocks the scrubber for use by other students.
- Next the student washes his utensils for the required number of seconds.
- The above steps are repeated for all three utensils after which the student unlocks the tap.
- There are logs showing which student is currently performing what action and the time when a student finishes washing all utensils is also printed.
