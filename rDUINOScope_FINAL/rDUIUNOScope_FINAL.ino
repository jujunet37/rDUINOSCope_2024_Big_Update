

// Alignment , Coordinates
//    rDUINOScope - Arduino based telescope control system (GOTO).
//    Copyright (C) 2016 Dessislav Gouzgounov (Desso)
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    PROJECT Website: http://rduinoscope.tk/"motors spin in the wrong way"
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    ALLIGNMENT Method: 1 Star Alignment - The method I have implemented is part of Mr. Ralph Pass alignment procedure described on http://rppass.com/
//                       Mr. Pass helped rDUINOScope by providing the calculations needed to implement the method. http://rppass.com/align.pdf - the actual PDF
//
//                       Iterative Alignment - The method is based on article from "New" Hamilton Astronomy.com website: http://astro.hcadvantage.com
//                       Actual PDF document: http://astro.hcadvantage.com/ASTRO_ARTICLES/Polar_Alignment_Part_II.pdf
// Note: the Adafruit version has been built upon a modified version of the ILI9341_due by https://github.com/marekburiak.
// Coding and GUI Designs have been developed by Khairey Nouhe as a contribution to the rDUINOScope Support community!
//
////////////////////////////////////////////////
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include "gfxfont.h"
#include "DHT.h"
#include <TinyGPS++.h>
#include <Time.h>
#include "TouchScreen.h"
#include <SdFat.h>
#include <DueTimer.h> // interruptions library
#include <DS3231.h>
#include <math.h>
#include <TimeLib.h>
#include "defines.h"
#include "DFRobotDFPlayerMini.h"

DFRobotDFPlayerMini mp3;
char textBuff[20];
#define serial_debug       // comment out to deactivate the serial debug mode
#define use_battery_level // comment out to deactivate battery level option

// Firmware & Hardware Versions
const String FirmwareDate = "9/4/2020";
const String FirmwareNumber = "v2.3.1 Boiana-Adafruit";
const String FirmwareName = "rDUINOScope Boiana-EQ V2.3.1";
const String FirmwareTime = "12:00:00";
const String HardwareVersion = "AstrogatinesPCB V.1.0";

// Default values to load when CANCEL button is hit on the GPS screen ( Change by your Home Location!)

float OBSERVATION_LONGITUDE = 0.5716; // (Semblancay)
float OBSERVATION_LATTITUDE = 47.4952; // (Semblancay)
float OBSERVATION_ALTITUDE = 127.00; // (Semblancay)
int TIME_ZONE = 1;                   // France
// .............................................................
unsigned long startTime;
unsigned int  duration;
// HERE GOES THE Mount, Gears and Drive information.
// ... used to calculate the HourAngle to microSteps ratio
// UPDATE THIS PART according to your SET-UP
// ---------------------------------------------
// NB: RA and DEC uses the same gear ratio (144 tooth in my case)!
//----------------------------------------------
#ifdef serial_debug
int WORM = 144;
#else
int WORM = 144;
#endif
int REDUCTOR = 3;      // 1:4 gear reduction
int DRIVE_STP = 400;   // Stepper drive have 200 steps per revolution
int MICROSteps = 16;   // I'll use 1/16 microsteps mode to drive sidereal - also determines the LOWEST speed.

// below variables are used to calculate the paramters where the drive works
#define ARCSEC_F_ROTAT 1296000   // ArcSeconds in a Full earth rotation;
#define SIDEREAL_DAY 86164.0905   // Sidereal day in seconds
float ArcSECstep;
int MicroSteps_360;
int RA_90;  // How much in microSteps the RA motor have to turn in order to make 6h = 90 degrees;
int DEC_90;   // How mich in microSteps the DEC motor have to turn in order to make 6h = 90 degrees;
int HA_H_CONST;
int HA_M_CONST;
int DEC_D_CONST;
int DEC_M_CONST;
int MIN_TO_MERIDIAN_FLIP = 2;   // This constant tells the system when to do the Meridian Flip. "= 2" means 2 minutes before 24:00h (e.g. 23:58h)
int MIN_SOUND_BEFORE_FLIP = 3;   // This constant tells the system to start Sound before it makes Meridian Flip
float mer_flp;                   // The calculateLST_HA() function depending on this timer will convert the HA and DEC to the propper ones to do the flip.
boolean MERIDIAN_FLIP_DO = false;
int Tracking_type = 1;  // 1: Sidereal, 2: Solar, 0: Lunar;
int Clock_Sidereal;  // Variable for the Interruptions. nterruption is initialized depending on the DATA above -in miliseconds
int Clock_Solar;  // Variable for the Interruptions. nterruption is initialized depending on the DATA above -in miliseconds
int Clock_Lunar;  // Variable for the Interruptions. nterruption is initialized depending on the DATA above -in miliseconds

///////////////////////////////////////////////////// Modules Setup /////////////////////////////////////////////////////

///////////////////////////////////////////////////// Adafruit 3.5" & ILI9488 TFT Setup /////////////////////////////////////////////////////
// The display uses hardware SPI
/** ILI9488 pin map */
#define TFT_CS  52
#define TFT_DC  3
#define TFT_RST -1
#define TFTBright DAC0
//ILI9488 tft(TFT_CS, TFT_DC, TFT_RST);
ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, TFT_RST);  //ILI9340
#define SD_CS 21    // Chip Select for SD card
#define SD_SPI_SPEED SPI_HALF_SPEED  // SD card SPI speed,SPI_HALF_SPEED, try SPI_FULL_SPEED >> doesn't work
SdFat SD; // set filesystem

// These are the four touchscreen analog pins
#define YP A10  // Y+ to A10   >>> must be an analog pin
#define XM A11  // X- to A11   >>> must be an analog pin
#define YM 44   // Y- to 44    >>> a digital pin
#define XP 42   // X+ to 42    >>> a digital pin

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 120  // touch Left margin  >>> Default value is 110 
#define TS_MINY 90   // touch up margin    >>> Default value is 80
#define TS_MAXX 850  // touch Right margin >>> Default value is 900  
#define TS_MAXY 930  // touch Down margin  >>> Default value is 940 

#define MINPRESSURE 5    // Minimum Pressure Touch Sensitivity >>> Default value is 10
#define MAXPRESSURE 1000 // Maximum Pressure Touch Sensitivity >>> Default value is 1000
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

///////////////////////////////////////////////////// Other Modules Setup /////////////////////////////////////////////////////

#define DHTPIN 43
#define DHTTYPE DHT22
DS3231 rtc(A4, A5);           // (SDA-A4, SCL-A5) from the RTC board
DHT dht(DHTPIN, DHTTYPE);
TinyGPSPlus gps;             // (Rx-16, Tx-17)

/** NIGHT MODE ANALOG pin */
#define DAY_NIGHT_PIN A6

//Joystick Pins//
int yPin = A0;
int xPin = A1;
int Joy_SW = A9;
int x_cal, y_cal = 0;

int FAN1 = 25;
//int FAN2 = A3;
int speakerOut = DAC1;         // Was 2
int BAT_PIN = A7;             //Analog channel A7 as used to measure BAT_PIN voltage

// Stepper Motors Setup
int RA_EN = 47;
int RA_STP = 4;
int RA_DIR = 5;
int RA_MODE0 = 11; // PD7
int RA_MODE1 = 12; // PD8
int RA_MODE2 = 13; // PB27

int DEC_EN = 49;
int DEC_STP = 6;
int DEC_DIR = 7;
int DEC_MODE0 = 10; // PC29
int DEC_MODE1 = 9; // PC21
int DEC_MODE2 = 8; // PC22

////////////////////////// Focus Motor Setup //////////////////////////////////
//int Focus_EN = 51;
//int focus_step_pin = 29;
//int focus_dir_pin = 27;

//int encoderCLK = 22;
//int encoderDT = 35;
//const int encoderSW = 33;  // Reading Push Button switch on the Rotary Encoder

//int Focus_Motor_position = 50; // The initial/home position of focus motor
//int encoderCLKLast = LOW;
//int encoderState = LOW;
//int Focus_StepsToTake = 50;    // Controls the resolution of the Focus Stepper

////////////////////////Auto Guiding Setup ////////////////////////////////////
int RA_PlusPin = 24;
int RA_MinusPin = 26;
int DEC_PlusPin = 28;
int DEC_MinusPin = 30;
int Guiding_Sense = 32;
bool IS_CONNECTED = false;
boolean IS_PulseGuiding = false;
#define CH0_OFFSET  (46 - LEVEL_HIGH/2)
#define CH1_OFFSET  (CH0_OFFSET + 62 * 1)
#define CH2_OFFSET  (CH0_OFFSET + 62 * 2)
#define CH3_OFFSET  (CH0_OFFSET + 62 * 3)
#define LEVEL_HIGH      (30)
#define LEVEL_LOW       (0)
static uint8_t ch0_data = LEVEL_LOW;
static uint8_t ch1_data = LEVEL_LOW;
static uint8_t ch2_data = LEVEL_LOW;
static uint8_t ch3_data = LEVEL_LOW;
////////////////////////////////////////////////////////////////////////////////////

// Variables:
String OBJ_Array[240]; // Add JG for new catalogues
String CAT_NAME = "";
// End JG

String ObservedObjects[50];
String Iter_Stars[50];
int int_star_count = 0;       // Counter for how many stars are loaded into Iter_Stars[] array.... to be used with Pages, so that it does not show more pages than needed
int Observed_Obj_Count = 0;
int ALLIGN_STEP = 0;  // Using this variable to count the allignment steps - 1: Synchronize, 2: Allign and Center, 3:....
int ALLIGN_TYPE = 0;  // Variable to store the alignment type (0-Skip Alignment, 1-1 Star alignment, 2-2 Star alignment
float delta_a_RA = 0;
float delta_a_DEC = 0;
int Iterative_Star_Index = 0;
String Prev_Obj_Start;
int lastScreenUpdateTimer;
unsigned long Slew_timer, Slew_RA_timer = 0;
int OBJECT_Index;
String OBJECT_NAME;
String OBJECT_DESCR;
String OBJECT_DETAILS;
String BT_COMMAND_STR;
String START_TIME;
int STP_FWD = LOW;
int STP_BACK = HIGH;
float OBJECT_RA_H;
float OBJECT_RA_M;
float OBJECT_DEC_D;
float OBJECT_DEC_M;
float OBJECT_MAG;
float curr_RA_H, curr_RA_M, curr_RA_S, curr_DEC_D, curr_DEC_M, curr_DEC_S;    // Global variables to store Mount's current RA and DEC.

// add JG

float mem_RA_H, mem_RA_M, mem_DEC_D, mem_DEC_M = 0;  // Add mem position function
float slopex ; // slopes and intercept variables for screen calibration.
float slopey ; 
float interx ; 
float intery ; 
int CAT_Page = 1; // Add JG for menu page index
bool catstatus = true; //Add JG for catalog testing at bootup

// End add JG

char curr_RA_lz[8], curr_DEC_lz[9], curr_HA_lz[8], curr_DEC_lz_disp[9];                                               // Global variable to store current RA and DEC with Leading Zeroes and sign (RA: 00:00:00; DEC: +/-00*00:00) // Add JG variable for right format DEC display (see comments in Function and regular update sections). And resize chars array 8 characters for RA and HA and 9 for DEC, save memory.
int SELECTED_STAR = 0;
double DELTA_RA_ADJUST = 1; // cos RA
double DELTA_DEC_ADJUST = 1; // cos DEC
int GPS_iterrations = 0;
double LST, HAHour, HAMin, ALT, AZ;
double JD;
String BTs;
int last_button, STARS_PAGER, Bright_PAGER, Double_PAGER, Variable_PAGER, MESS_PAGER, NGC_PAGER1, NGC_PAGER2, NGC_PAGER3, NGC_PAGER4, NGC_PAGER5, IC_PAGER, CUSTOM_PAGER, OBJ_PAGER; // JG delete treas_pager and add for new catalogues
boolean IS_TFT_ON = true;
boolean IS_STEPPERS_ON = true;
//boolean IS_FOCUS_ON = true;
boolean IS_OBJ_VISIBLE = false;
boolean IS_IN_OPERATION = false;  // This variable becomes True when Main screen appears
boolean IS_TRACKING = false;
boolean IS_NIGHTMODE;
boolean IS_OBJ_FOUND = true;
boolean IS_OBJECT_RA_FOUND = true;
boolean IS_OBJECT_DEC_FOUND = true;
boolean IS_MERIDIAN_PASSED = false;
boolean IS_POSIBLE_MERIDIAN_FLIP = true;
boolean IS_MERIDIAN_FLIP_AUTOMATIC = true;
boolean IS_BT_MODE_ON = false;
boolean IS_MANUAL_MOVE = false;
boolean IS_FAN1_ON = true;
boolean IS_GTMEM_ON = true;
boolean IS_CUSTOM_MAP_SELECTED = false;
boolean IS_SOUND_ON = true;
int TFT_Brightness = 255;
int MAIN_SCREEN_MENU = 0;
int CURRENT_SCREEN = 0;
int LOAD_SELECTOR;   // selector to show which LOADING mechanism is used: 1 - Messier, 2 - File, 3 - NGCs
boolean TRACKING_MOON;
boolean sun_confirm = false;
String Fan1_State = "ON";
String GTMEM_State = "Free";
String TFT_Time = "AL-ON";
String Sound_State = "ON";
String Stepper_State = "ON";

//String Focus_Motor_State = "ON";

String Mer_Flip_State = "Auto";
String Tracking_Mode = "Celest";
int RA_microSteps, DEC_microSteps, rev_RA_microSteps, rev_DEC_microSteps;              // Current position of the motors in MicroSteps! - when movement occures, values are changed accordingly (manual, tracking or slew to);
int RA_mode_steps, DEC_mode_steps;
int SLEW_RA_microsteps, SLEW_DEC_microsteps;    // Where the mottors needs to go in order to point to the object
int RA_finish_last = 0;
int map_r = 0;    // Used to determine the StarMap Row ... image name (1-1.bmp; 1-2.bmp ....)
int map_c = 0;    // Ued to determine the StarMap Column .... image name
String old_t, old_d;
String Start_date;
int update_time, Tupdate_time, TFT_timeout;
unsigned long UPD_T, UPD_coord, DELAY_Slew, UPD_LST, TFT_Timer;
int RA_move_ending;
int w_DateTime[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // array to store date - as updated from updater screen - Wishing_Date
int dateEntryPos = 0;
int Summer_Time = 0;
int xPosition = 0;  // Joystick
int yPosition = 0;  // Joystick
float _temp, _Stemp;
float _humid, _Shumid;
int16_t Bright_triangle, Time_area_back, Time_area_font, texts, Button_State_ON, Button_State_OFF, Button_Title, l_text, d_text, btn_l_border, btn_d_border, btn_l_text, btn_d_text, btn_l_selection, title_bg, title_texts, messie_btn, file_btn, ngc_btn, MsgBox_bg, MsgBox_t; // defines string constants for the clor - Depending on the DAY/NIGHT modes
File roots;
File StarMaps;                    // bmp files

// Some variables used for Alignment procedure:
double Star_1_HA = 0;
double Star_1_DEC = 0;
double Star_2_HA = 0;
double Star_2_DEC = 0;
double err_RA = 0;
double err_DEC = 0;
double Eerr_RA = 0;
double Eerr_DEC = 0;
double err_ALT = 0;
double err_AZ = 0;
double det = 0;

///////////////////////////////////////////////////// Void Setup Section /////////////////////////////////////////////////////
void setup(void) {
#ifdef serial_debug
  Serial.begin(57600);
  while (!Serial) {}
#endif
  Serial1.begin(9600);
  mp3.begin(Serial1);
  mp3.volume(5);  //Set volume value. From 0 to 30

  Serial2.begin(9600);  // Initialize GPS communication on PINs: 17 (RX) and 16 (TX)
  Serial3.begin(9600); // Bluetooth communication on PINs:  15 (RX) and 14 (TX)
  pinMode(speakerOut, OUTPUT);

  // below variables are used to calculate the paramters where the drive works
  int ww = WORM * REDUCTOR;
  int www = DRIVE_STP * MICROSteps;

  MicroSteps_360 = ww * www;
  RA_90 = MicroSteps_360 / 4;  // How much in microSteps the RA motor have to turn in order to make 6h = 90 degrees;
  DEC_90 = RA_90;   // How mich in microSteps the DEC motor have to turn in order to make 6h = 90 degrees;
  HA_H_CONST = MicroSteps_360 / 360;
  DEC_D_CONST = HA_H_CONST;

  Clock_Sidereal = 1000000 / (MicroSteps_360 / SIDEREAL_DAY); // This way I make the interruption occuer 2wice faster than needed - REASON: allow max time for Pin-UP, Pin-DOWN action
  Clock_Solar = 1000000 / (MicroSteps_360 / (SIDEREAL_DAY - 235.9095));
  Clock_Lunar = 1000000 / (MicroSteps_360 / (SIDEREAL_DAY - 2089.2292));

#ifdef serial_debug
  Serial.print("Clock_Sidereal = ");
  Serial.println(Clock_Sidereal, 6);
#endif

  analogWrite(TFTBright, 255);  // LCD Backlight brightness 255 = ~100%)
  SD.begin(SD_CS, SD_SPI_SPEED);
  rtc.begin();
  dht.begin();
  tft.begin();
  tft.setRotation(iliRotation0);
  tft.setFont(gfxfont);
  tft.setFontMode(gTextFontModeTransparent);// Set font mode to transparent (No Highlight)
  tft.setTextWrap(true);
  tft.setTextLetterSpacing(1);
  tft.setTextLineSpacing(3);

  // DRB8825 - drive mode pins (determine Steppping Modes 1/8, 1/16 and etc.
  pinMode(RA_EN, OUTPUT);
  digitalWrite(RA_EN, HIGH);// Turn Off RA Motor at startup
  pinMode(RA_STP, OUTPUT); // RA Step
  pinMode(RA_DIR, OUTPUT); // RA Dir
  pinMode(RA_MODE0, OUTPUT);
  pinMode(RA_MODE1, OUTPUT);
  pinMode(RA_MODE2, OUTPUT);

  pinMode(DEC_EN, OUTPUT);
  digitalWrite(DEC_EN, HIGH);// Turn Off DEC Motor at startup
  pinMode(DEC_STP, OUTPUT); // DEC Step
  pinMode(DEC_DIR, OUTPUT); // DEC Dir
  pinMode(DEC_MODE0, OUTPUT);
  pinMode(DEC_MODE1, OUTPUT);
  pinMode(DEC_MODE2, OUTPUT);

  /////////////// Auto Guiding Pins Setup //////////////////
  pinMode (RA_PlusPin, INPUT);
  pinMode (RA_MinusPin, INPUT);
  pinMode (DEC_PlusPin, INPUT);
  pinMode (DEC_MinusPin, INPUT);

  ///////////////// Focus Motor Setup ///////////////////////
//  pinMode (encoderCLK, INPUT);
//  pinMode (encoderDT, INPUT);
//  pinMode(encoderSW, INPUT);
//  pinMode(Focus_EN, OUTPUT);
//  digitalWrite(Focus_EN, HIGH); // Turn Off Focus Motor at startup
//  pinMode(focus_dir_pin, OUTPUT);
//  pinMode(focus_step_pin, OUTPUT);
//  digitalWrite(encoderSW, HIGH); // Pull-Up resistor for switch

  //digitalWrite(RA_DIR,HIGH); // Set Dir high
  //digitalWrite(RA_STP,LOW);
  //digitalWrite(DEC_DIR,HIGH); // Set Dir high
  //digitalWrite(DEC_STP,LOW);

  // Joystick
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);

  // Device 1 & 2 command Pins
  pinMode(FAN1, OUTPUT);
//  pinMode(FAN2, OUTPUT);

  // Set RA and DEC microstep position
  RA_microSteps = RA_90; //  --> point to North Sudereal Pole = -180 deg (-12h)
  DEC_microSteps = 0; //  --> Point to North Sudereal Pole = 90 deg

  Timer3.attachInterrupt(Sidereal_rate);
  //  Timer3.start(Clock_Sidereal); // executes the code every 62.329 ms.

  if (analogRead(DAY_NIGHT_PIN) < 800)
  {
    IS_NIGHTMODE = true;
    texts = Maroon;
    l_text = RED;
    d_text = Maroon;
    title_bg = RED;
    title_texts = BLACK;
    messie_btn = Maroon;
    btn_l_text = RED;
    btn_d_text = Maroon;
    btn_l_border = RED;
    btn_d_border = Maroon;
    btn_l_selection = RED;
    MsgBox_bg = RED;
    MsgBox_t = BLACK;
    Button_State_ON = RED;
    Button_State_OFF = RED;
    Button_Title = BLACK;
    Time_area_back = BLACK;
    Time_area_font = RED;
    Bright_triangle = Maroon;
  } else {
    IS_NIGHTMODE = false;
    texts = LightGrey;
    l_text = WHITE;
    d_text = LightGrey;
    btn_l_text = GreenYellow;
    btn_d_text = DarkGreen;
    btn_l_border = GREEN;
    btn_d_border = DarkGreen;
    btn_l_selection = DarkGreen;
    title_bg = orange;
    title_texts = BLACK;
    messie_btn = DarkGrey;
    MsgBox_bg = Purple;
    MsgBox_t = GreenYellow;
    Button_State_ON = GREEN;
    Button_State_OFF = RED;
    Button_Title = Purple;
    Time_area_back = ILI9341_SLATEBLUE;
    Bright_triangle = ILI9341_SLATEBLUE;
    Time_area_font = BLACK;
  }


  ///////////////////////////////////////////////////// Boot-up Procedures /////////////////////////////////////////////////////
  // The below part cannot be removed form the code
  // You can add messages, but not remove!

  if (!IS_NIGHTMODE) {
    drawBin("UI/day/bg_boot.bin", 0, 0, 320, 481);
  } else {
    drawBin("UI/night/bg_boot.bin", 0, 0, 320, 481);
  }

  tft.cursorToXY(0, 190);
  tft.setTextColor(title_bg);
  tft.println(FirmwareNumber);

#ifdef serial_debug
  tft.setTextColor(btn_l_border);
  tft.cursorToXY(0, 200);
  tft.print("> DEBUG MODE");
#endif
  tft.setTextColor(l_text);
  tft.cursorToXY(0, 230);
  tft.setTextScale(1);

  // see if the card is present and can be initialized:
  char in_char;
  String items = "";
  int j = 0;
  int k = 0;
  Bright_PAGER = 0; // Add JG Add new catalogues
  Double_PAGER = 0;
  Variable_PAGER = 0;
  MESS_PAGER = 0;
  NGC_PAGER1 = 0;
  NGC_PAGER2 = 0;
  NGC_PAGER3 = 0;
  NGC_PAGER4 = 0;
  NGC_PAGER5 = 0;
  IC_PAGER = 0;
  STARS_PAGER = 0;
  CUSTOM_PAGER = 0;

  // End add JG


  volatile bool check = true;
  tft.print("--> Initializing touchscreen... ");
  tft.setTextColor(GREEN);
  tft.println("OK");
  tft.setTextColor(l_text);

  tft.print("--> Initializing DHT sensor... ");

  if (isnan(dht.readTemperature()) || isnan(dht.readHumidity()))
  {
    tft.setTextColor(RED);
    tft.println("FAIL");
    tft.setTextColor(l_text);
  }
  else
  {
    tft.setTextColor(GREEN);
    tft.println("OK");
    tft.setTextColor(l_text);
  }

  tft.print("--> Initializing RTC... ");
  int prev_mil = millis();
  if (isnan(rtc.getTemp()))
  {
    tft.setTextColor(RED);
    tft.println("FAIL");
    tft.setTextColor(l_text);
    check = false;
  }
  else
  {
    tft.setTextColor(GREEN);
    tft.print("OK   ");

#ifdef serial_debug
    tft.println(rtc.getTemp());
#else
    tft.println("");
#endif
    tft.setTextColor(l_text);
  }

  tft.print("--> Initializing SD card... ");

  for (int i = 0; i < 10 && !SD.begin(SD_CS, SD_SPI_SPEED); i++)
  {
    if (i == 9)
    {
      tft.setTextColor(RED);
      tft.println("ERROR: Card failed, or not present\n");
      tft.println("Try formatting the SD card to FAT32 and replace the files.");
      tft.setTextColor(l_text);
      check = false;
    }
    delay(50);
  }
  //Debug or card initialized:
  tft.setTextColor(GREEN);
  tft.println("OK");
  tft.setTextColor(l_text);

  tft.println("--> Testing .csv catalogs... ");// Modif JG, just testing presence and data inside. Now loading catalogs in load page.
  File dataFile = SD.open("Bright_CAT.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: Bright_CAT.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("Double_CAT.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: Double_CAT.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("Variable_CAT.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: Variable_CAT.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("Double_CAT.csv");
  // if the file is available, write to it:
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: Double_CAT.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("messier.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: messier.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("NGC_CAT1_10.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: NGC_CAT1_10.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("NGC_CAT2_10.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: NGC_CAT2_10.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("NGC_CAT3_10.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: NGC_CAT3_10.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("NGC_CAT4_10.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: NGC_CAT4_10.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("NGC_CAT5_10.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: NGC_CAT5_10.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("NGC_CAT6_10.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: NGC_CAT6_10.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("NGC_CAT7_10.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: NGC_CAT7_10.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("NGC_CAT8_10.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: NGC_CAT8_10.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("NGC_CAT9_10.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: NGC_CAT9_10.csv");
  tft.setTextColor(l_text); catstatus = false;}
  dataFile.close();
  dataFile = SD.open("NGC_CAT10_10.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: NGC_CAT10_10.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("IC_CAT.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: IC_CAT.csv");
  catstatus = false;}
  dataFile.close();
  dataFile = SD.open("custom_CAT.csv");
  if (dataFile){} else { tft.setTextColor(RED);
  tft.println("ERROR opening: custom_CAT.csv");
  catstatus = false;}
  dataFile.close();
  if (catstatus) {
  tft.setTextColor(GREEN);
  tft.println("All catalogs are OK");
  tft.setTextColor(l_text);
  } else { 
  tft.setTextColor(RED);
  tft.println("One or more catalogs are missing,");
  tft.println("corrupted, or empty of data");
  tft.setTextColor(l_text);}  // End Modif JG

  ////////////////////////////////////////////////////////////////
  tft.print("--> loading options... ");
  if (SD.exists("options.txt"))
  {
    loadOptions_SD();
    tft.setTextColor(GREEN);
    tft.println("OK");
    tft.setTextColor(l_text);
  }
  else
  {
    tft.setTextColor(RED);
    tft.println("FAIL");
    tft.setTextColor(l_text);
  }
  tft.println("--> initializing BlueTooth...");
  tft.println("--> initializing GPS...");
  tft.println("--> Powering Steppers up...");
#ifndef serial_debug
  if (check == false)  while (1); //don't do anything more
#endif

  calibrateJoypad(&x_cal, &y_cal);

  // Draw Supporters Logos
  if (!IS_NIGHTMODE) {
    drawBin("UI/day/hackaday_logo.bin", 0, 456, 120, 24);
  } else {
    drawBin("UI/night/hackaday_logo.bin", 0, 456, 120, 24);
  }

  delay(500);
  CURRENT_SCREEN = 0;
  drawGPSScreen();
  //CURRENT_SCREEN = 1;
  //drawClockScreen();
  //CURRENT_SCREEN = 3;
  //drawSelectAlignment();
  //CURRENT_SCREEN = 4;
  //drawMainScreen();
  //CURRENT_SCREEN = 5;
  //drawCoordinatesScreen() ;
  //CURRENT_SCREEN = 6;
  //drawLoadScreen();
  //CURRENT_SCREEN = 7;
  //drawOptionsScreen();
  //CURRENT_SCREEN=10;
  //drawSTATScreen();
  //CURRENT_SCREEN = 11;
  //drawStarMap();
  //CURRENT_SCREEN=12;
  //drawStarSyncScreen();
  //CURRENT_SCREEN = 14;
  //drawAutoGuidingScreen();
  //CURRENT_SCREEN = 15;
  //drawConfirmSunTrack();
  /////////////////////////////////////////////////////////////////////////////
  UPD_T = millis();
  UPD_LST = millis();
  DELAY_Slew = millis();
  TFT_Timer = millis();
  //TFT_timeout = 0;
  RA_move_ending = 0;
  considerTempUpdates();
  // Turn Telescope Steppers ON at Startup by default to enable ALignment and Manual motor control
  digitalWrite(RA_EN, LOW);
  digitalWrite(DEC_EN, LOW);
//  digitalWrite(Focus_EN, LOW);
  Stepper_State = "ON";
  IS_STEPPERS_ON = true;
//  Focus_Motor_State = "ON";
//  IS_FOCUS_ON = true; // Turn focus motor ON at Startup by default.
}
///////////////////////////////////////////////////// Void Loop Section /////////////////////////////////////////////////////
void loop(void) {

  // AutoGuiding control .... if Any!
  if (CURRENT_SCREEN == 14 && digitalRead (Guiding_Sense) == HIGH)
  {
    if (IS_CONNECTED == false)
    {
      OnScreenMsg(8);
      if (IS_SOUND_ON) {
        SoundOn(note_C, 32);
        delay(200);
        SoundOn(note_C, 32);
        delay(200);
        SoundOn(note_C, 32);
        delay(1000);
      }
      drawAutoGuidingScreen();
      IS_CONNECTED = true;
    }
    
    
    
    //Timer3.stop(); //
    //IS_TRACKING = false;
    //IS_IN_OPERATION = false;
    
    
    
    IS_PulseGuiding = true;
    CURRENT_SCREEN = 14;
    considerPulseGuiding();
  }
  else if (CURRENT_SCREEN == 4 && digitalRead (Guiding_Sense) == HIGH)
  {
    if (IS_CONNECTED == true)
    {
      OnScreenMsg(8);
      if (IS_SOUND_ON) {
        SoundOn(note_C, 32);
        delay(200);
        SoundOn(note_C, 32);
        delay(200);
        SoundOn(note_C, 32);
        delay(1000);
      }
      IS_CONNECTED = false;
    }
    CURRENT_SCREEN = 14;
    drawAutoGuidingScreen();
    
    
    
    //Timer3.stop(); //
    //IS_TRACKING = false;
    //IS_IN_OPERATION = false;
    
    
    IS_PulseGuiding = true;
    considerPulseGuiding();
  } else if (CURRENT_SCREEN == 14 && digitalRead (Guiding_Sense) == LOW && IS_CONNECTED == true && IS_PulseGuiding == true )
  {
    OnScreenMsg(9);
    if (IS_SOUND_ON) {
      SoundOn(note_C, 32);
      delay(200);
      SoundOn(note_C, 32);
      delay(200);
      SoundOn(note_C, 32);
      delay(1000);
    }
    CURRENT_SCREEN = 14;
    drawAutoGuidingScreen();
    IS_PulseGuiding = false;
    IS_CONNECTED = false;
  }

  // Focus Motor Control .... if Any!
//  if (IS_FOCUS_ON == true)
//  {
//    consider_focus_control();
//  }

  // This is done in order to prevent multiple calculations of LST_HA per second (especially while SlewTo) and only
  // do it once the DEC SlewTo slows down, but before stopping OR once every 10 seconds (in order to do the Meridian Flip)

  if (RA_move_ending == 1) {
    calculateLST_HA();
  }

  if ((IS_MERIDIAN_FLIP_AUTOMATIC) && ((UPD_LST + 10000) <= millis()) && (IS_OBJ_FOUND == true)) {
    calculateLST_HA();
    UPD_LST = millis();
  }

  // Adding this delay to SLOW DOWN the Arduino so that the motors can catch up!
  // The delay is only needed when in full speed.... otherways the CalculateLST_HA() takes over and
  // slows down the arduino enough. CalculateLST_HA() when slewing only fires when the motors slows down
  // after they are very close to the Object Position.
  if ((DELAY_Slew + 1 <= millis()) && (IS_OBJ_FOUND == false)) {

    // If you wonder how I get to this delay - 800 uS
    // When I optimised the code for speed, the main delay was coming from calculateLST_HA() which back then was calculated on every Loop();
    // Once I optimized it to only calculate when the SlewTo stops (to fine tune after DEC stops) it turned out that
    // the code is too fast and the motors only "screemed" but not rotating - due to the low voltage/current.
    // This variable depends on How You Limit the Current to your motors and the Voltage you use!
    // I use 12V and 1.6A (70% in full step = 1.10A) to drive my NEMA 17 SY42STH47-1684B Motors.
    // Please note that Potentiometer does not really give consistent results for current on every restart (it drifted between 1.12A - 0.9A).

    // HINT: you can try to play with the Current/Voltage that powers the motors to get faster speeds.
    if (IS_STEPPERS_ON) {
      cosiderSlewTo();
    } else {
      IS_OBJECT_RA_FOUND = true;
      IS_OBJECT_DEC_FOUND = true;
      IS_OBJ_FOUND = true;
      RA_move_ending = 0;
    }
    DELAY_Slew = millis();
    // delayMicroseconds(800);
  }

  // The below part of the code makes sure that the system does NOT process any other inputs while SlweingTo!
  // Since both motors need every STEP to come from Arduino board, it needs it's entire power to run the motors in fastest possible way
  // The fastes possible from this board in the current state of the software is approx 3 turns/sec (600 steps/sec)
  // IS_OBJ_FOUND == true --> Means that SLEW command have completed
  //
  if (IS_OBJ_FOUND == true)
  {
    // BLUETOOTH Considerations ? ... if any
    if ((IS_BT_MODE_ON == true) && (Serial3.available() > 0) && (IS_MANUAL_MOVE == false))
    {
      BT_COMMAND_STR = Serial3.readStringUntil('#');
#ifdef serial_debug
      Serial.println(BT_COMMAND_STR);
#endif
      considerBTCommands();
      drawBin("objects/BT.bin", 0, 143, 320, 135);
    }

    // JOYSTICK Movements ? ... if any
    xPosition = analogRead(xPin);
    yPosition = analogRead(yPin);

    if ((xPosition < x_cal - 100) || (xPosition > x_cal + 100) || (yPosition < y_cal - 100) || (yPosition > x_cal + 100)) // all were 100
    {
#ifdef serial_debug
      Serial.print("xPin = ");
      Serial.println(xPosition);
      Serial.print("yPin = ");
      Serial.println(yPosition);
#endif
      IS_MANUAL_MOVE = true;
      if (IS_STEPPERS_ON)
      {
        consider_Manual_Move(xPosition, yPosition);
      }
    }
    else
    {
      IS_MANUAL_MOVE = false;
    }

    // This will take care of turning OFF the TFT's background light if the device is not used
    // for XXX amont of seconds and IS_IN_OPERATION = TRUE
    if ((TFT_timeout > 0) && (millis() - TFT_Timer > TFT_timeout) && (IS_TFT_ON) && (IS_IN_OPERATION))
    {
      analogWrite(TFTBright, 0);
      IS_TFT_ON = false;
    }

    // TOUCH SCREEN Inputs ? ... if any

    TSPoint p = ts.getPoint();

    // Scale from ~0->1000 to tft.width using the calibration #'s

    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

    int tx, ty;

    tx = (p.x - 0);
    ty = (p.y - 0);

    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!

    if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
      return;
    }
    /*
           //Useful to debug touch:
         #ifdef serial_debug
           Serial.print(" -> Touched: ");
           Serial.print(tx);
           Serial.print(", y = ");
           Serial.println(ty);
         #endif
    */
    considerTouchInput(tx, ty);

    // OTHER UPDATES ?  ... if any
    // Happens every 2 seconds
    if (((millis() - UPD_T) > 2000) && (IS_MANUAL_MOVE == false))
    {
      calculateLST_HA();  // Make sure it Updates the LST! used on Main Screen and When Calculating current Coords.
      considerTimeUpdates();
      considerDayNightMode();
      considerTempUpdates();
      // I need to make sure the Drives are not moved to track the stars,
      // if Object is below horizon ALT < 0 - Stop tracking.
      if ((ALT <= 0) && (IS_TRACKING == true) && (IS_IN_OPERATION == true))
      {
        IS_TRACKING = false;
        Timer3.stop();
        drawMainScreen();
      }
      UPD_T = millis();
    }
  }
}
