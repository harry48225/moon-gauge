# moon-gauge 🌔

![the device](/media/case-front.jpg)

## a pi pico powered display of the moon's current distance.

An analog voltmeter is used to show at what stage the moon is at in its orbit (between apogee and perigee) and an 8 digit seven segement display to display the current distance in km.
It implements an algorithm from *Astronomical algorithms by Jean Meeus*.

There is a python implementation of the algorithm and then the actual ardiuno code including a C implementation of the algorithm in `main.ino`

There are stl files for the case in `CAD/` along with an svg to stick over the orginal voltmeter markings. Heat set inserts should be pressed into the main part of the case using M3 screws to screw the back to the case

![schematic](/media/wiring_bb.png)
