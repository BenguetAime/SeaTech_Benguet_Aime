There are two main versions of the Low Pin Count USB Development Kit board.

The older version of the board shipped only with the PIC18F14K50 device, which
came pre-programmed with some ADC example code that would blink the LEDs D1-D4 in a pattern
corresponding to the POT setting.  The out of box code did not include any example USB functionality.
The older board can be identified by the marking in the lower right corner of the PCB.  On the silk screen
there should be 02-02043-R (and on the top copper layer: 04-02043Rx - where x is a number, such as 5).
This board was sold as DM164127 or in the DV164139 kit.


The newer version of the board comes with the PIC16F1459 microcontroller, which is preprogrammed
with the demo firmware .hex image provided in this folder.  The new version of the board can be
identified by the lower right corner of the board, which should have a silk screen marking "02-10255-R".
This newer version of the board is sold as DM164127-2 and in the DV164139-2 kit.  This board has two 
pushbuttons on it (S1 and S2), while the older version of the demo board (the non-2 suffix boards) had
only one pushbutton.

In order to use the "PIC16F1459_LowPinCountUSBDevKit_HIDPnPDemo.hex" and for all functionality to
work normally, it is necessary to use the -2 suffix newer board, as the demo make use of both pushbuttons
(S1 for MCLR reset of the microcontroller, and S2 for generating data for the HID Custom PC application
demo).