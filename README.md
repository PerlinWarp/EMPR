# EMPR - Embedded Systems Project

***University of York, Computer Science BEng/MEng Stage 2 EMPR (Embedded Systems Project)***
#### Members are Lackshan Rama, Idris Al-Ghabra,Peter Walkington and Brian Camacho

## Useful Docs
[EMPR Lectures](https://vle.york.ac.uk/webapps/blackboard/content/listContent.jsp?course_id=_88743_1&content_id=_2848340_1&mode=reset)

[lpc17xx.h docs](https://www-users.cs.york.ac.uk/~pcc/MCP/drivers/html/files.html)

[VLE Driver Docs](https://vle.york.ac.uk/bbcswebdav/pid-2848390-dt-content-rid-7066727_2/courses/Y2018-006400/2015-16/CMSIS/drivers/html/modules.html)

[SPI Documentation](https://www.youtube.com/watch?v=dQw4w9WgXcQ)

[Pin and Port Mapping](https://www-users.cs.york.ac.uk/~pcc/MCP/MbedPins.html)

## Note when using Serial on uni computers. You need to use xfce terminal not gnome terminal.
## Basics
To run cd into ~/EMPR/MiniProject1
Then run:
make all
make install
and then restart your MBED board.


## Delays

## LEDs
### LED Pin numbers
* LED1 18
* LED2 20
* LED3 21
* LED4 23

## I2C
9 - sda (0b11) - Function 4 - Port 0 Pin 0

10 - scl (0b11) - Function 4 - Port 0 Pin 1

28 - sda - Function 3 - Port 0 Pin 10

27 - scl - Function 3 - Port 0 Pin 11

## Keypad
We use a one hot 8 bit buffer. E.g. 1111 1111
We call this buffer recv.

The lower four bits select which row of the keypad we want to read from.

The upper 4 bits give us which column of the keypad was pressed.
So 0111 1110 would mean we have pressed the * key on the keypad.

Therefore to read which key was pressed. We can just use AND to get the upper and lower nibbles and then use each as an index into a 2D Array which acts as a look up table for the keys.

`char KBD_Read_Blocking()` blocks execution until a character is pressed, and return the character
`char KBD_Read()` returns the currently pressed character or `null` if not character is pressed

### Software debouncer
A software debouncer stops one key press from being detected as multiple.
I
It uses `int debounce_cnt` to count registered successive presses of the same key.
When the same keypress is registered more than once, it increments `debounce_cnt`;
Once `debounce_cnt` reaches a threshold (20 works nice), it resets the counter and prints the character.
When a different keypress is detected, or the key is released, the counter is reset.



## LCD
The LCD has 2 rows of 16 characters.
Each char is an address we can write to, however the row addresses do not continue from one to the next.

char cmdBuff[2] = {A,B}
A signifies if B is a command or data.
If A is 0x00 then B is a command, if A is 0x40 then B is data to be written.

{0x00, 0x80} Says start writing the data on the top line.
{0x40, 0x64} Would then write d (0x64 in our char set) at address 0x80.

{0x00, 0xA8} writes on the second line.

## DAC
Output is to PinNum 26 which is actually physically pin number 18 on the breakout board. DAC_UpdateValue(LPC_DAC, x); Takes an int x, which represents a 10 bit number and therefore has a max of 1023.


## PWM

[PWM Tutorial](http://www.ocfreaks.com/lpc1768-pwm-programming-tutorial/)
[Embed not embed pinwise schematic](https://vle.york.ac.uk/bbcswebdav/pid-2848415-dt-content-rid-7065727_2/courses/Y2018-006400/2015-16/mbed-005.1.pdf)
[Other PWM guide](https://www.exploreembedded.com/wiki/LPC1768:_PWM)

The PWM pins on the board are 23 - 26.
