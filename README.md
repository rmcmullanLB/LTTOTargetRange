# LTTOTargetRange

Description

Lazer Tag Host
Copyright (c) 2019 Ryan McMullan

Lazer Tag Host is compatible with Lazer Tag Team Ops taggers from Tiger, Nerf, and Hasbro. However, the authors have no affiliation with those companies and this software is not endorsed or condoned by them.  This includes the LTTO Deluxe, Drone, and Tag Master Blaster, as well as the Phoenix LTX and Lazer Tag Augmented Realtiy (LTAR).


Hardware
You need both the Mouser shopping list and the Amazon list.

Shopping list on Mouser:
https://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=17103A9430 

Additional components from Amazon:
- 400 pin breadboards (2 needed):  https://smile.amazon.com/gp/product/B07LFD4LT6/ (this is a better price than Mouser)
- Arduino Nano (1 needed):  https://smile.amazon.com/gp/product/B07G99NNXL/ (this is a better price than Mouser)
- Energizer Tap Light (1 needed):  https://smile.amazon.com/gp/product/B01MU8TYJU/


Arduino Wiring:
The core circuits can be found in this schematic by Astrospark:
https://github.com/astrospark/LazerTagHost/blob/master/Hardware/Schematic.pdf

Then create these additional circuits:
- piezo speaker to pin 9
- RGB LED with 80R to green and blue, and 150R to red (the type in the order is common cathode, so they share the same ground return pin)

Arduino Programming:

1) Download the .ino file and put in your sketchbook
2) Launch the Arduino IDE
3) Open Arduino\Sketches\TargetRange\TargetRange.ino
5) Under Tools->Board, select Arduino Nano (or appropriate)
6) Connect your Arduino
7) Under Tools->Serial Port, select the USB serial port to which your Arduino is connected
8) Click Upload