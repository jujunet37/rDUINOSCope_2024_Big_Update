# rDUINOScope_2024_Big_Update

# I worked on the 10 April 2020 Khairey Nouhe's rDUINOSCOPE version with Adafruit 3.5" screen and this is my first release.
## This is a modified version of rDUINOScope by Khairey Nouhe that works with Adafruit 3.5" 320X480 TFT SPI mode, Updated to the latest firmware version V_2.3.1.

### The improvements I made are here :

### Update Tuesday 16/07/2024 with corrected bugs and many new features and catalogs :

### corrected bugs :

- Previously corrected autoguiding : now works.
- Status bar : Auto and OFF superposition message for meridian flip : corrected.
- Coordinates Page : RA, DEC and HA inversion, DEC format error (missing a ":", and was in HA box : Workaround found because correct it directly make bluetooth no longer works. Now DEC in right format and BT works)
- LST and AZ/H error due to bad summer time parameters for France. Corrected for France, check the code to adjust for other countries. (same change days as for Italy (by default), but GMT + 1 for winter and GMT + 2 for summer).
- Corrected an error in dayno formula that causes a shift in RA for planets.
- Corrected an error in Moon computing formulas and add precise information data.
- Corrected errors in Moon computing formulas
- RA went false or negative on Moon Goto : Corrected

### Added :

### Hardware :

- Same wiring than Khairey Nouhe design with all functionalities but focuser and fan connectors.
- Now PCB is provided, gerber files are in the package.
- Internal cooling fan, controlled by the UI.

### Software :

- Previously add dew point information.
- Previously double goto speed.
- Previously center joystick tracks and orthogonality of both axes in software (for my component, look at the previous video, 1h05m)
- Joystick directions more intuitive : left : RA return, right : RA forward.
- Add fine manual move for BlueTooth app, works great with Skysafari, works with stellarium plus but very long latency.
- Bluetooth works, tested with Skysafari 6, 7, stellarium mobile plus and Stellarium desktop.
- Add Store position utility : To memorize any position and goto back function to recall this position. Useful for keeping a frame camera when needed to go elsewhere (ex goto bright star for Bahtinov focusing and then go back to the target)
- Touchscreen calibration tool added (button was in option page but not activated). New calibration tool was created to help unshift touchzones with display. The tool can display the new correction parameters, and store it in SD card. Parameters can be fine tuned by editing "OPTION.TXT" in SD card. Four last values are : Slope in x, Slope in y, Intercept in x and intercept in y. A function can restore to default and erase SD card calibration data in case of bad calibration. Look at code comment for further informations.
- New load menu with cool JWST's creation pillars background :-)
- Checked Messier Catalog and update informations abouts objects, such as name and distance.
- Move M111 and M112, which don't exist in Messier catalog, now in NGC catalog.
- Now around 3000 objets in database !
- New NGC catalog with 2400 objects :
	- All objects in Northern hemishere down to -35° DEC, Magnitude < 13. Then filtering objects by a selection by taking into account the atmospheric extinction : ojects with Magnitude < 10 at -35° to -25° DEC, Magnitude < 11 at -25 to -20° DEC. Magnitude < 12.5 at -20 to -15° DEC. Magnitude 13 and lower for DEC > -15°.
	- NGC catalog divided in 5 parts with 480 item each. Each part with a magnitude range, for a variety of apertures.
- 186 IC catalog objetcs with same magnitude references as NGC catalog above, with no cross references with NGC.
- 172 Bright stars catalog for focusing purpose.
- 160 Double stars catalog
- 138 Variable stars catalog
- Custom catalog with up to 240 entries : completed with Collinder catalog and special objects : 84 entries.
- Deleted double references in NGC, IC catalogs and cross references with other catalogs. Priority : Messier, NGC, IC.
- All new catalogs sorted by constellation : less time to move from one to the other ! Buttons now show name, constellation and magnitude.
- Bunch of new information data (distance, separation and color for double stars, period, type for variable stars, etc...)
- All catalogs loaded in the same array : Same SRAM usage but now more than 3000 objects !
- New Autoguiding background : deleted circle RA, DEC, Focus. New landscape.
- Add new planet info from J2000 (1st January 2000 @ 12h GMT), previous information was approximate computed info for 2013 from J2000 data. Now starting computing from J2000 original information with better precision : On a test date, error values for internal planets was RA/DEC 4-5m/20-30'. External planets was 0-3m/2-5' (except 15-20' DEC for Mars and Neptune). Now internal planets at this test date are in range RA/DEC error : 1-2m/8-13'. External planets : 0-2m/2-7'. Tested on random dates between 2024-2044, for all planets, error are within -5m to +2m RA / -13' to 13' DEC, most values <10'.

### miscellaneous :

- Complete tutorial construction on YouTube.
-  French :
-  English :

### All changes in code are tagged by // Add JG, // End add JG, // Modif JG, for easily look at the modifications.
===========================================================

#  Previous Updates From Khairey Nouhe, Giacommo Mammarella, Dessislav Gouzgounov and contributors :

### Update Friday 10/4/2020 (Fayrouz Update With ILI9341):
- The new version is built on the SdFat Library.
- It uses a modified version of marekburiak-ILI9341_due library that has been modified specially for this project.
- New Graphical User Interface has been developed,Inspired by Dessislav Gouzgounov.
- Corrected Solar System Objects bug from last update. Thanks to (Aitor Plazaola Alcerreca) From the rDUINOScope Support Group.
- Support for Stellarium Mobile Plus has been added.
- Supports the rDUINOScope Companion Android App (will be launched soon on Google Play)!
### Hardware Changes:
- Support for PHD2 Autoguiding has been added. Thanks to the project by https://github.com/kevinferrare/arduino-st4
- A third stepper motor has been added for focus control.
- An Optional DFRobot Mp3 Player has been added for future upgrade.
- Consider the new Wiring Diagram for Hardware Changes!
===========================================================

### Update Saturday 4/5/2019 (Fayrouz Update With Solar System!):
- Removed Nextion Commands to avoid errors in compile.
### Update Tuesday 16/3/2019 (Fayrouz Update With Solar System!):

### Use IDE 1.8.5 to avoid compile errors.

Includes Giacomo Mammarellas's updated features!

- No hardware changes.
- Memory card contents are changed.
- Added Solar System objects tracking to Load Screen.
- Added Battery Power indicator to main screen, currently showing USB only, just for future upgrade.
- Added images for Solar and stellar objects to show on Main Screen. ( Memory Car items changed).
===========================================================

### Update Tuesday 3/7/2018 (Fayrouz Update!):
- Includes Giacomo Mammarellas's updated features!

- No hardware changes.
- Memory card contents are changed.
- Changed supporters logos at startup page.
- Full Hardware check and confirmation at initilize.
- More Accurate Joystick calibration at startup.
- Options' data are saved to a text file on the memory card, and will be restored and initilized during next boot.
- A new screen brightness control triangle has been added.
- New on screen messages has been added.
- Prevent (Load), (Home), (Bluetooth), and (Track) buttons to function while steppers are off. Avoiding Wrong Telescope Position coordinates while steppers are off.
- Corrected Steppers' speed during " Slew to" process . very slow>slow>fast>faster>>>>>>>>>>>fast>slow>very slow>stop>track.
- Faster browsing for Messier and treasures catalogs.
- Automatic date and time set on clockScreen() derived from GPS data: the software is able to calculate the location's time zone and to auto update from summer or winter time.
- Added an new button in Options screen to update date and time from GPS data.
- New debug mode. can be used to debug touchscreen actions.
===========================================================

### Update Saturday 23/9/2017:
### Added Giacomo Mammarellas's new features Like:
- Joypad calibration at startup. No more panic to find right values for the joypad.
- Empirial March sound function moved to confirm good initialization of the device. If no sound then you're having problems! :'(
- Automatic data and time set on clockScreen() derived from GPS data: the software is able to calculate the location's time zone and to auto update to summer or winter time.
- Custom line of text can be appended to any .csv file to display custom "Description" strings. Example: I added "rich cluster with more than 100 stars" to M11 in messier.csv
- Orange color for Name and Description when tracking objects.
- Added custom.csv wich can be used to add more custom sky objects (still to be implemented) ===========================================================

Repairs:

- Re-position Summer button at Clock Screen.
- added Green and Red colors to Screen-Timeout at the status bar.
===========================================================

## rDUINOScope

- THE WORLD'S FIRST STAND ALONE ARDUINO BASED TELESCOPE CONTROL GOTO (handheld controller).
rDUINOScope is an Open Source, Arduino Due based Telescope Control System (GOTO). Drafted as stand alone system, rDUINOScope does not need PC, Tablet or Cell Phone, nor Internet connection in order to operate and deliver stunning views! It does however supports Bluetooth and LX200 protocol to connect and be controlled by smart devices!
rDUINOScope uses database with THE BEST ~250 stellar objects ( Messier and Hidden Treasures Catalogues) and 200 stars, calculates their position on the sky and points the telescope. Implemented Alignment procedures allow for "Always in the eyepiece" experience when slewing to selected objects and once the object is in the eyepiece, the system keeps tracking it.
The best part of rDUINOScope is that it is an OPEN SOURCE SOFTWARE and HARDWARE! Few custom telescope makers have announced that they are adding or about to add it to their product line. In addition, few months ago a new HW shield board was born as a spin-off project to spare you the hassle of building your own rDUINOScope!
Published under GNU General Public License
Instructions on how to build hardware can be found on the project's website: http://rduinoscope.byethost24.com/ HACKADAY.IO Project: https://hackaday.io/project/21024-rduinoscope
Facebook: https://www.facebook.com/rDUINOScope/

### rDUINOScope v2.3.1 Boiana EQ
- Release date: 03 August 2017 Status: Stable Version Author: Dessislav Gouzgounov / Десислав Гузгнов /(deskog@gmail.com)

- v2.3.1 aims to bring all existing hardware platforms into one code base. Additionally this version introduces new features to the rDUINOScope like Screensaver, TFT brightness, MosFET to control power to DRV8825 and etc.

- Current release will also be used as a base to add the Alt/Az changes.

### New features added in v2.3.1:
- Brings all existing flavours of the hardware to use same code!
- Hardware Changes!!! - Plese reffer to the HACKADAY instructions!
- Screen Brightness control;
- Screen Auto OFF function with timeout;
- Implemented FET transistor to control power to the stepper motors;
- More BlueTooth commands added to prepare for a screenless version (or operation);
- It is now possible to control rDUINOScope without the TFT screen - so called "BLIND" version.
- BUG fixes:
	- GPS fixes;
	- SlewTo speed issues for low amperage;
### New features added in v2.3:
- Code Optimization for Performance (SlewTo now capable of speeds > 3 deg/sec);
- Added Celestial, Lunar and Solar tracking speeds;
- Added new Main Menu system for more functionality;
- Added control on 2 external devices (DEV 1 & 2 - you can hook your Heater, Fan and etc);
- Added support for SkySafari 5 (smartphone software);
- Added build in Star Atlas showing current telescope position;
- Shows current Firmware version on Loading screen and on Bluetooth request using LX200 protocol;
- Bug fixes: o Wrong DEC movement for negative DEC values passed over Bluetooth; o LST revised in the format XX:XX;
### New features added in v2.1:
- Added “1 Star Alignment” Method;
- Added “Iterative Alignment” Method;
- Added 200 brightest stars to be used in alignment procedures;
- Added Bluetooth control from Stellarium (cloned LX200 communication protocols);
- Added Bluetooth connection with PC;
- Code has been refactored for easy reading/editing;
- Bug fixes: o Wrong DEC movement for negative DEC values; o Revised RA movement formula; o OnScreen messages have been fixed;
- Buttons move after press event was fixed;
