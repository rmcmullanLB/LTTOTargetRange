# LTTOTargetRange

Description

Lazer Tag Target Range
Copyright (c) 2019 Ryan McMullan

The Lazer Tag Target Range provides target shooting practice for your laser tagger.  It uses visual and audio cues to signal rest (red) time, followed by a target (green) period where you are to raise, aim, and fire.  If you hit in the allowed time (default to 2 sec), it flashes blue to show it, otherwise it returns to red.  After a number of rounds (default 5) it flashes your score, then returns to a pre-game mode.  Adjust game parameters in the code to adjust the length and difficulty of the game.

Lazer Tag Target Range is compatible with Lazer Tag Team Ops taggers from Tiger, Nerf, and Hasbro. However, the authors have no affiliation with those companies and this software is not endorsed or condoned by them.  This includes the LTTO Deluxe, Drone, and Tag Master Blaster, as well as the Phoenix LTX and Lazer Tag Augmented Realtiy (LTAR).


Hardware
You need both the Mouser shopping list and the Amazon list.

Shopping list on Mouser:
https://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=17103A9430 

Additional components from Amazon:
- 400 pin breadboards (2 needed):  https://smile.amazon.com/gp/product/B07LFD4LT6/ (this is a better price than Mouser)
- Arduino Nano (1 needed):  https://smile.amazon.com/gp/product/B07G99NNXL/ (this is a better price than Mouser)
- Energizer Tap Light (1 needed):  https://smile.amazon.com/gp/product/B01MU8TYJU/

Building the case:
- Unscrew the tap light case and lift off the lid and ring.
- Unscrew the LED PCB from the bottom of the case.
- Use a soldering iron to pull the red and black wires from the LED PCB (keep them connected to the battery packs).  Throw the PCB in the parts bin.
- In the bottom of the case is a post that the PCB was mounted on and bracket for mounting it on a screw.  
  Use a blade, hot knife or oscillating blade to cut these off and leave a flat bottom.
- Put electrical tape over the screw hole to keep the inside clean and dry.
- Prepare the breadboard base:
	- The breadboard has four sections to it:  left rail (with positive and ground), left bank (rows of 5 posts), right bank, and right rail.
	- Take one 400-pin breadboard and remove the left rail (scissors to cut the backing work fine)
	- Use a hack saw to cut that board at the 24th rows
	- Take a second breadboard and remove both rails with scissors
	- Use a hack saw to cut down the middle and separate the left bank from the right (the second board should now be in four pieces)
	- Use a hack saw to cut one of the banks at the 24th row
	- Line up the breadboard pieces in this order:  left bank-right bank-right rail-left bank
- Mount the breadboard
	- The breadboard sits too deep in the case to connect the USB cable to the Arduino so you need to prepare a shim about 5mm high.
	- I cut down a wooden yardstick to 6.5cm long to make this.
	- Peel the doublestick backing from the back of the breadboard pieces and mount them on the shim.
	- Add doublestick tape to the bottom of the case and mount the shim on it.
- The original case red and black wires become your Vin and Gnd feeds to your Arduino.

Arduino Wiring:
The core circuits can be found in this schematic by Astrospark:
https://github.com/astrospark/LazerTagHost/blob/master/Hardware/Schematic.pdf

Then create these additional circuits:
- piezo speaker to pin 9
- RGB LED with 80R to green and blue, and 150R to red (the type in the order is common cathode, so they share the same ground return pin).  These resistances are calculated based on the particular RGB LED in the Mouser Parts List -- if you use a different one you may need different resistors.

Arduino Programming:

1a) Download the .ino file and put in your sketchbook
1b) Download millisDelay Library: https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html#using (needed for easy timer management that doesn't delay the whole code)
1c) Download EnableInterrupt Library: https://github.com/GreyGnome/EnableInterrupt (needed for LTTO Library)
1d) Download LTTO Library:  https://github.com/rmick/LTTO_Library (needed for interpretting the LTTO IR protocol signals without manually counting bits)
2) Launch the Arduino IDE
3) Open Arduino\Sketches\TargetRange\TargetRange.ino
4) Update the pin values in the header based on your circuit build.  Customize game parameters if desired.
5) Under Tools->Board, select Arduino Nano (or appropriate)
6) Connect your Arduino
7) Under Tools->Serial Port, select the USB serial port to which your Arduino is connected
8) Click Upload

Thanks:
These are the giants whose shoulders I stood upon:
- Astrospark:  for the schematic and designing my first Arduino project of the LTTO Hosting Station
- RMick:  for the fantastic LTTO Library that makes working with LTTO IR protocol much, much easier
- Cole Stith:  for collaborating on the case design and my first custom coded Arduino project