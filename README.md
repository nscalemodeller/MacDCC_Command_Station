# MacDCC_Command_Station
An extremely simple and incomplete DCC Command Station

## Repository Contents

- A simple Arduino sketch to send the DCC signal to an LMD18200 based booster
- A very simple 'booster tester' that just flips the port connected to the booster every 5 seconds ( basically a modified 'blink' sketch ) so that you can measure the booster output with a multimeter
- A very early version of my MacDCC Command Station.

The MacDCC Command Station requires 0SX 10.7+ and works on Mavericks up to at least MacOS Ventura. The software is ALPHA quality so will have many bugs. I have only tested it using a Bachmann Spectrum N Scale GE 44-Ton Switcher and it works, sort of. The Emergency Stop and Broadcast Emergency Stop don't seem to be recognised, and for some reason the Switcher will 'work' on 14, 28 and 128 speed step modes even though the decoder instructions say 28 speed step only. I also had to experiment to find which function switched on the headlight. The decoder instructions say function button 10 ( F10) on the E-Z Command system so I assumed that would be auxiliary function 10 in the DCC standard, but no, it's FL in Function Group 1 ( Extended Packet Formats For Digital Command Control, All Scales  RP 9.2.1 - line 240 ). Got to love standards!

**NOTE:** the MacDCC Command Station software is unsigned and you may get a warning that it is from an unidentified developer when you try to run it. If this is the case and you still want to run the software, you will have to ‘allow’ it from within the Security and Privacy tab of your System Preferences.

These programs are distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

## How to use

The software can be initially tested using the method shown in my [blog post], without using a booster.

Simply connect two Arduinos together as shown in the post, load the DDC monitor software mentioned in the post into one Arduino, the CommandTest.ino software in this repository into another Arduino, and then use the MacDCC software to send commands to the Arduino containing the CommandTest.ino software. The monitor program should show various DCC commands being sent.

You can then add the monitor and booster hardware to the respective arduinos as shown in [this post] to make sure everything is still working.

Finally, hook up the booster to a section of rail and run a loco.


[blog post]: https://nscalemodeller.com/arduino/dcc/2015/01/02/sending-dcc-with-arduino.html
[this post]: https://nscalemodeller.com/arduino/dcc/dcc%20command%20station/2015/01/29/command-station.html

