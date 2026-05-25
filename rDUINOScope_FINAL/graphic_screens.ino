//
//    rDUINOScope - Arduino based telescope control system (GOTO).
//    Copyright (C) 2016 Dessislav Gouzgounov (Desso)
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    PROJECT Website: http://rduinoscope.tk/
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

// ......................................................................
//  This part of the code take care of:
//  - Drawing interface screens;
//  - Day / Night mode set;
//  - Draws symbols on certain screens (e.g. XX/XX/XXXX in day/time screen)
//  - Draws OnScreen Messages
//
//  Screens are separated like:
//    * CURRENT_SCREEN==0  - drawGPSScreen() Where the GPS coordinates are displayed
//    * CURRENT_SCREEN==1  - drawClockScreen() Captures updates on the time and date
//    * CURRENT_SCREEN==2  -                - not used
//    * CURRENT_SCREEN==3  - drawSelectAlignment() Select Alignment method (only have 3 buttons)
//    * CURRENT_SCREEN==4  - drawMainScreen() Captures all clicks on the MAIN Screen of the application
//    * CURRENT_SCREEN==5  - drawCoordinatesScreen() Only have "back" button
//    * CURRENT_SCREEN==6  - drawLoadScreen() Captures input on Load screen (all of them: Messier && Treasurres)
//    * CURRENT_SCREEN==7  - drawOptionsScreen();
//    * CURRENT_SCREEN==8  -                - not used
//    * CURRENT_SCREEN==9  - drawScreenCalibration() Touch Screen calibration              
//    * CURRENT_SCREEN==10  - drawSTATScreen()
//    * CURRENT_SCREEN==11  - drawStarMap();
//    * CURRENT_SCREEN==12  - drawStarSyncScreen() - To Select Alignment Star;
//    * CURRENT_SCREEN==13  - drawConstelationScreen(int indx) - to actually align on Star. Called few times per alignment procedure.
//    * CURRENT_SCREEN==14  - drawAutoGuidingScreen();
//

void removeTime_addXX() {
  if (dateEntryPos == 0) {
    tft.fillRect(50, 62, 130, 25, l_text);
    tft.fillRect(190, 62, 80, 25, l_text );
    tft.setTextColor(BLACK, l_text);
    tft.cursorToXY(55, 68);
    tft.print("XX/XX/XXXX");
    tft.cursorToXY(195, 68);
    tft.print("XX:XX");
  }
}
///////////////////////////////////////////////////// GPS Screen Graphics ///////////////////////////////////////////////////////
void drawGPSScreen()
{
  CURRENT_SCREEN = 0;
  tft.setFontMode(gTextFontModeTransparent);// Set font mode to transparent (No Highlight)
  tft.fillRect(0, 0, 320, 30, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  tft.setTextScale(3);
  tft.setTextColor(title_texts);
  tft.cursorToXY(15, 5);
  //tft.setTextColor(title_texts);
  tft.print("Reading GPS Data");

  if (IS_NIGHTMODE) {
    drawBin("UI/night/bg_gps.bin", 0, 30, 320, 450);
  } else {
    drawBin("UI/day/bg_gps.bin", 0, 30, 320, 450);
  }

  // CONTINUE BTN
  tft.setTextScale(2);
  tft.setTextColor(title_texts);
  tft.cursorToXY(120, 425);
  tft.println("CONTINUE");
}

///////////////////////////////////////////////////// Clock Screen Graphics ///////////////////////////////////////////////////////
void drawClockScreen()
{
  CURRENT_SCREEN = 1;
  tft.fillRect(0, 0, 320, 30, title_bg);
  tft.cursorToXY(63, 5);
  tft.setTextColor(title_texts);
  tft.setTextScale(3);
  tft.print("Date & Time");

  char obj_name[50];
  String OBJ_NAME = "";
  String OBJ_NAME_1 = "";
  if (!IS_NIGHTMODE) {
    OBJ_NAME = "UI/day/bg_time_1.bin";
    if (Summer_Time == 0) {
      OBJ_NAME_1 = "UI/day/bg_time_2_off.bin";
    } else {
      OBJ_NAME_1 = "UI/day/bg_time_2_on.bin";
    }
  } else {
    OBJ_NAME = "UI/night/bg_time_1.bin";
    if (Summer_Time == 0) {
      OBJ_NAME_1 = "UI/night/bg_time_2_off.bin";
    } else {
      OBJ_NAME_1 = "UI/night/bg_time_2_on.bin";
    }
  }
  if (SD.open(OBJ_NAME)) {
    OBJ_NAME.toCharArray(obj_name, 50);
    drawBin(obj_name, 0, 30, 320, 450); // Drawing selected Object Tumbnail 140*140 Pixels
  }
  if (SD.open(OBJ_NAME_1)) {
    OBJ_NAME_1.toCharArray(obj_name, 50);
    drawBin(obj_name, 0, 415, 320, 65); // Drawing selected Object Tumbnail 140*140 Pixels
  }

  tft.drawRect(15, 224, 80, 50, messie_btn); //1 x 15>95   y 224>274
  tft.drawRect(120, 224, 80, 50, messie_btn);//2 x 120>200 y 224>274
  tft.drawRect(230, 224, 80, 50, messie_btn);//3 x 230>280 y 224>274
  tft.drawRect(15, 285, 80, 50, messie_btn); //4 x 15>95   y 285> 335
  tft.drawRect(120, 285, 80, 50, messie_btn);//5 x 120>200 y 285>335
  tft.drawRect(230, 285, 80, 50, messie_btn);//6 x 230>280 y 285>335
  tft.drawRect(15, 346, 80, 50, messie_btn); //7 x 15>95   y 346>396
  tft.drawRect(120, 346, 80, 50, messie_btn);//8 x 120>200 y 346>396
  tft.drawRect(230, 346, 80, 50, messie_btn);//9 x 230>280 y 346>396
  tft.drawRect(120, 408, 80, 50, messie_btn);//0 x 120>200 y 408>478

  tft.setTextScale(2);
  tft.setTextColor(title_texts);
  tft.cursorToXY(55, 68);
  tft.print(rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/'));
  tft.cursorToXY(195, 68);
  tft.print(rtc.getTimeStr(FORMAT_SHORT));
}

///////////////////////////////////////////////////// Select Alignment Screen Graphics ///////////////////////////////////////////////////////
void drawSelectAlignment() {
  CURRENT_SCREEN = 3;
  tft.fillRect(0, 0, 320, 30, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  tft.cursorToXY(85, 7);
  tft.setTextColor(title_texts); // black
  tft.setTextScale(3);
  tft.print("ALIGNMENT");

  char obj_name[50];
  String OBJ_NAME = "";
  if (!IS_NIGHTMODE) {
    if (OBSERVATION_LONGITUDE > 0) {
      OBJ_NAME = "UI/day/bg_allign_2.bin";
    } else {
      OBJ_NAME = "UI/day/bg_allign_1.bin";
    }
  } else {
    if (OBSERVATION_LONGITUDE > 0) {
      OBJ_NAME = "UI/night/bg_allign_2.bin";
    } else {
      OBJ_NAME = "UI/night/bg_allign_1.bin";
    }
  }
  if (SD.open(OBJ_NAME))
  {
    OBJ_NAME.toCharArray(obj_name, 50);
    drawBin(obj_name, 0, 30, 320, 450); // Drawing selected Object Tumbnail 140*140 Pixels
  }
  tft.setTextColor(title_texts);
  tft.cursorToXY(100, 430);
  tft.println("CONTINUE");
}

///////////////////////////////////////////////////// Main Screen Graphics ///////////////////////////////////////////////////////
void drawMainScreen()
{
  tft.setTextScale(1);
  CURRENT_SCREEN = 4;
  drawStatusBar();
  tft.fillScreen(BLACK);
  w_DateTime[0] = 0;
  calculateLST_HA();
  update_time = millis();

  char obj_name[50];
  String OBJ_NAME = "objects/";
  String OBJ_NAME_1 = "UI/";
  String OBJ_NAME_2 = "UI/";
  if (IS_NIGHTMODE == true) {
    OBJ_NAME += "night/";
    OBJ_NAME_1 += "night/status_bar_back.bin";
    OBJ_NAME_2 += "night/menu_background.bin";
  } else {
    OBJ_NAME += "day/";
    OBJ_NAME_1 += "day/status_bar_back.bin";
    OBJ_NAME_2 += "day/menu_background.bin";
  }

  if (SD.open(OBJ_NAME_1))
  {
    OBJ_NAME_1.toCharArray(obj_name, 50);
    drawBin(obj_name, 0, 89, 320, 27); // Drawing selected Object Tumbnail 140*140 Pixels
    tft.drawLine(0, 90, 320, 90, BLACK );//Time_area_back
  }

  tft.fillRect(0, 27, 320, 63, Time_area_back );//ILI9341_INDIGO // title_bg / ILI9341_SLATEBLUE /ILI9341_DARKBLUE/ ILI9341_DODGERBLUE
  drawStatusBar();
  tft.setTextColor(Time_area_font); // title_texts // ILI9341_INDIGO ILI9341_DARKBLUE
  tft.cursorToXY(1, 35);
  tft.setTextScale(3);
  tft.print("TIME:");
  tft.cursorToXY(88, 35);
  tft.print(String(rtc.getTimeStr()).substring(0, 5));

  tft.cursorToXY(7, 65);
  tft.print("LST");
  tft.cursorToXY(73, 65);
  tft.print(":");
  tft.cursorToXY(88, 65);
  if ((int)LST < 10) {
    tft.print("0");
    tft.setTextScale(1);
    tft.print(" ");
    tft.setTextScale(3);
    tft.print((int)LST);
  } else {
    tft.print((int)LST);
  }
  tft.setTextScale(3);
  tft.print(":");
  if ((LST - (int)LST) * 60 < 10) {
    tft.print("0");
    tft.setTextScale(1);
    tft.print(" ");
    tft.setTextScale(3);
    tft.print((LST - (int)LST) * 60, 0);
  } else {
    tft.print((LST - (int)LST) * 60, 0);
  }


// Julien Modif add Dew Point

  tft.setTextScale(2);
  tft.setTextColor(Time_area_font);
  tft.cursorToXY(201, 30);
  tft.print("T C:");
  tft.setTextScale(1);
  tft.cursorToXY(218, 28);
  tft.print("o");
  tft.setTextScale(2);
  tft.cursorToXY(201, 52);
  tft.print("HUM");
  tft.cursorToXY(237, 52);
  tft.print(":");
  tft.cursorToXY(251, 30);
  tft.print(_temp, 1);
  tft.cursorToXY(251, 52);
  tft.print(_humid, 1);
  tft.cursorToXY(201, 74);
  tft.print("DWP:");
  tft.cursorToXY(251, 74);
  tft.print(0);
  tft.cursorToXY(295, 30);
  tft.print(" C");
  tft.cursorToXY(295, 52);
  tft.print(" %");
  tft.cursorToXY(295, 74);
  tft.print(" C");
  tft.setTextScale(1);
  tft.cursorToXY(300, 28);
  tft.print("o");
  tft.cursorToXY(300, 72);
  tft.print("o");

// Julien Modif add Dew Point




  tft.setTextScale(1);
  tft.setTextColor(l_text );
  tft.cursorToXY(1, 100);
  tft.print("Date:");

  tft.setTextColor(btn_l_border);
  tft.cursorToXY(35, 100);
  tft.print(String(rtc.getDateStr()).substring(0, 2));
  tft.print(" ");
  tft.print(rtc.getMonthStr(FORMAT_SHORT));
  tft.print(" ");
  tft.print(String(rtc.getDateStr()).substring(6));
  tft.cursorToXY(110, 100);
  tft.print("@");
  tft.setTextColor(l_text);
  tft.cursorToXY(120, 100);
  tft.print("LAT:");
  tft.setTextColor(btn_l_border);
  tft.print(OBSERVATION_LATTITUDE, 4);
  tft.setTextColor(l_text);
  tft.print(" LONG:");
  tft.setTextColor(btn_l_border);
  tft.print(OBSERVATION_LONGITUDE, 4);
  tft.setTextColor(l_text );
  tft.print(" ALT:");
  tft.setTextColor(btn_l_border);
  tft.print(OBSERVATION_ALTITUDE, 0);

  tft.setTextScale(3);
  tft.setTextColor(l_text);
  tft.cursorToXY(1, 119);
  if (OBJECT_NAME.length() > 7) {
    tft.setTextScale(2);
    tft.cursorToXY(1, 129);
    tft.print("OBS:");
  } else {
    tft.setTextScale(3);
    tft.print("OBSERVING:");
  }

  // Data for the observed object....
  if (OBJECT_NAME != "") {
    tft.setTextColor(title_bg);
    tft.print(OBJECT_NAME);

    if ((OBJECT_NAME != "") && (ALT < 0)) {
      tft.setTextScale(2);
      tft.cursorToXY(0, 290);
      tft.setTextColor(RED);
      tft.println("OBJECT NOT VISIBLE!");
      tft.fillRect(250, 235, 70, 70, Maroon);
      tft.setTextScale(5);
      tft.cursorToXY(272, 252);
      tft.print("!");
    }
    else
    {
      OBJ_NAME += String(OBJECT_NAME) + ".bin";
#ifdef serial_debug
      Serial.print("file name: ");
      Serial.println(OBJ_NAME);
      Serial.print("SD.open(OBJ_NAME) = ");
      Serial.println(SD.open(OBJ_NAME));
#endif
      if (SD.open(OBJ_NAME))
      {
        OBJ_NAME.toCharArray(obj_name, 50);
        drawBin(obj_name, 0, 143, 320, 142); // Drawing selected Object Tumbnail 320*133 Pixels // Make it 145
      }
      if (SD.open(OBJ_NAME_2) /*&& (IS_TRACKING)*/)
      {
        OBJ_NAME_2.toCharArray(obj_name, 50);
        drawBin(obj_name, 0, 286, 320, 194); // Drawing Object Details Background Image
        tft.cursorToXY(0, 295);
        tft.setTextColor(l_text);
        tft.setTextScale(1);
        tft.println(OBJECT_DETAILS);
        tft.drawLine(0, 285, 320, 285, title_bg);
      } else {
        tft.drawLine(0, 285, 320, 285, title_bg);
        OnScreenMsg(1);
      }
      tft.setTextScale(1);
      tft.setTextColor(title_bg);
      tft.cursorToXY(0, 148);
      tft.print(OBJECT_DESCR);
    }

    tft.setTextScale(2);
    tft.setTextColor(l_text);
    tft.cursorToXY(0, 165);
    tft.print("RA :");
    tft.setTextColor(title_bg);
    tft.print(OBJECT_RA_H, 0);
    tft.print("h ");
    tft.print(OBJECT_RA_M, 2);
    tft.println("'");
    tft.cursorToXY(0, 190);
    tft.setTextColor(l_text);
    tft.print("DEC:");
    tft.setTextColor(title_bg);
    tft.print(OBJECT_DEC_D, 0);
    tft.setTextScale(1);
    tft.print("o ");
    tft.setTextScale(2);
    tft.print(abs(OBJECT_DEC_M), 2);
    tft.println("'");

    if ((OBJECT_NAME != "CP") && (IS_BT_MODE_ON == false)) {
      tft.setTextColor(l_text);
      tft.cursorToXY(0, 215);
      tft.println("HA :");
      tft.cursorToXY(48, 215);
      tft.setTextColor(title_bg);
      tft.print(HAHour, 0);
      tft.print("h ");
      tft.print(HAMin, 1);
      tft.print("m");
      tft.cursorToXY(0, 240);
      tft.setTextColor(l_text);
      tft.println("ALT:");
      tft.cursorToXY(0, 265);
      tft.println("AZ :");
    }
    // End data for the observed object...
  } else {
    tft.setTextScale(2);
    tft.setTextColor(texts);
    tft.cursorToXY(0, 150);
    tft.println("");
    tft.println("No object is selected!");
    tft.println("");
    tft.setTextScale(1);
    tft.println("Use LOAD button below to select objects from Solar System, Messier, NGC or custom.csv catalogues");
  }

#ifdef use_battery_level
  drawBatteryLevel(260, 335, calculateBatteryLevel());
#endif
  //  Draw Main Screen Menu:
  drawMainScreen_Menu(0);
}

///////////////////////////////////////////////////// Coordintaes Screen Graphics ///////////////////////////////////////////////////////
void drawCoordinatesScreen() {
  CURRENT_SCREEN = 5;
  tft.fillRect(0, 0, 320, 30, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  tft.setTextColor(title_texts);
  tft.setTextScale(3);
  tft.cursorToXY(5, 5);
  tft.print("Coordinates");

  char obj_name[50];
  String OBJ_NAME = "";
  String OBJ_NAME_3 = "";
  if (!IS_NIGHTMODE) {
    OBJ_NAME = "UI/day/bg_coord.bin";
    OBJ_NAME_3 = "UI/day/btn_back.bin";
  } else {
    OBJ_NAME = "UI/night/bg_coord.bin";
    OBJ_NAME_3 = "UI/night/btn_back.bin";
  }

  if (SD.open(OBJ_NAME_3)/* && (IS_TRACKING)*/)
  {
    OBJ_NAME_3.toCharArray(obj_name, 50);
    drawBin(obj_name, 245, 0, 75, 30); // Drawing selected Object Tumbnail 140*140 Pixels
  }

  if (SD.open(OBJ_NAME)/* && (IS_TRACKING)*/)
  {
    OBJ_NAME.toCharArray(obj_name, 50);
    drawBin(obj_name, 0, 30, 320, 450); // Drawing selected Object Tumbnail 140*140 Pixels
  }
}

///////////////////////////////////////////////////// Load Screen Graphics ///////////////////////////////////////////////////////
void drawLoadScreen() {
  CURRENT_SCREEN = 6;
  //  MESS_PAGER = 0;
  char obj_name[50];
  String OBJ_NAME = "";
  String OBJ_NAME_2 = "";
  String OBJ_NAME_3 = "";
  String OBJ_NAME_4 = "";
  if (!IS_NIGHTMODE) {
    OBJ_NAME = "UI/day/";
    OBJ_NAME_2 = "UI/day/btn_next_prev.bin";
    OBJ_NAME_3 = "UI/day/btn_back.bin";
    OBJ_NAME_4 = "UI/day/load_background.bin";
  } else {
    OBJ_NAME = "UI/night/";
    OBJ_NAME_2 = "UI/night/btn_next_prev.bin";
    OBJ_NAME_3 = "UI/night/btn_back.bin";
    OBJ_NAME_4 = "UI/night/load_background.bin";
  }
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 400, 30, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  // tft.setTextColor(title_texts);
  // tft.setTextScale(3); Altered by JG (replaced by prev and next page)
  // tft.cursorToXY(5, 5); Altered by JG (replaced by prev and next page)
  // tft.print("Load..."); Altered by JG (replaced by prev and next page)

  // Add JG
  if (!IS_NIGHTMODE) {
  drawBin("UI/day/btn_prev.bin", 0, 0, 75, 30); // Draw prev cat Button Thumbnail
  drawBin("UI/day/btn_next.bin", 122, 0, 75, 30); // Draw next cat Button Thumbnail
   } else {
  drawBin("UI/night/btn_prev.bin", 0, 0, 75, 30); // Draw prev cat Button Thumbnail
  drawBin("UI/night/btn_next.bin", 122, 0, 75, 30); // Draw next cat Button Thumbnail 
   }
  // End add JG
  
  if (SD.open(OBJ_NAME_4))
  {
    OBJ_NAME_4.toCharArray(obj_name, 50);
    drawBin(obj_name, 0, 30, 320, 450); // Draw Back Button Thumbnail
  }

  // Draw buttons to load CSVs
  if (LOAD_SELECTOR == 0) {
    OBJ_NAME += "btn_load_0.bin";
  }
  if (LOAD_SELECTOR == 1) {
    OBJ_NAME += "btn_load_1.bin";
  }
  if (LOAD_SELECTOR == 2) {
    OBJ_NAME += "btn_load_2.bin";
  }
  if (LOAD_SELECTOR == 3) {
    OBJ_NAME += "btn_load_3.bin";
  }
  if (LOAD_SELECTOR == 4) {
    OBJ_NAME += "btn_load_4.bin";
  }
// Add JG

if (LOAD_SELECTOR == 5) {
    OBJ_NAME += "btn_load_0b.bin";
  }
if (LOAD_SELECTOR == 6) {
   OBJ_NAME += "btn_load_1b.bin";
  }
if (LOAD_SELECTOR == 7) {
    OBJ_NAME += "btn_load_2b.bin";
  }
if (LOAD_SELECTOR == 8) {
    OBJ_NAME += "btn_load_3b.bin";
  }
if (LOAD_SELECTOR == 9) {
    OBJ_NAME += "btn_load_4b.bin";
  }
if (LOAD_SELECTOR == 10) {
    OBJ_NAME += "btn_load_0c.bin";
  }
if (LOAD_SELECTOR == 11) {
   OBJ_NAME += "btn_load_1c.bin";
  }
if (LOAD_SELECTOR == 12) {
    OBJ_NAME += "btn_load_2c.bin";
  }
if (LOAD_SELECTOR == 13) {
    OBJ_NAME += "btn_load_3c.bin";
  }
if (LOAD_SELECTOR == 14) {
    OBJ_NAME += "btn_load_4c.bin";
  }
// End add JG

  if (SD.open(OBJ_NAME_3))
  {
    OBJ_NAME_3.toCharArray(obj_name, 50);
    drawBin(obj_name, 245, 0, 75, 30); // Draw Back Button Thumbnail
  }

  if (SD.open(OBJ_NAME))
  {
    OBJ_NAME.toCharArray(obj_name, 50);
    drawBin(obj_name, 0, 60, 320, 80); // Draw Selection Button Thumbnail
  }

  if (SD.open(OBJ_NAME_2))
  {
    OBJ_NAME_2.toCharArray(obj_name, 50);
    drawBin(obj_name, 0, 434, 320, 46); // Draw Next/Prev Button Thumbnail
  }
  drawLoadObjects();
}

///////////////////////////////////////////////////// Options Screen Graphics ///////////////////////////////////////////////////////
void drawOptionsScreen()
{
  CURRENT_SCREEN = 7;
  tft.fillRect(0, 0, 320, 30, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  if (!IS_NIGHTMODE) {
    drawBin("UI/day/btn_back.bin", 245, 0, 75, 30); // Draw Back Button Thumbnail
  } else {
    drawBin("UI/night/btn_back.bin", 245, 0, 75, 30); // Draw Back Button Thumbnail
  }
  tft.cursorToXY(5, 5);
  tft.setTextColor(title_texts);
  tft.setTextScale(3);
  tft.print("OPTIONS");
  if (!IS_NIGHTMODE) {
    drawBin("UI/day/bg_options_buttons_off.bin", 0, 30, 320, 450); // All Page Buttons off - Day
  } else {
    drawBin("UI/night/bg_options_buttons_off.bin", 0, 30, 320, 450); // All Page Buttons off - Night
  }
  updateTrackingMode_opt();
  updateTriangleBrightness_opt();
  updateScreenTimeout_opt();
  updateMeridianFlip_opt();
  updateSound_opt();
  updateStepper_opt();
  
  //updateFocus_stepper_opt();
  
}

///////////////////////////////////////////////////// STAT Screen Graphics ///////////////////////////////////////////////////////
void drawSTATScreen() {
  CURRENT_SCREEN = 10;
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 320, 30, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  if (!IS_NIGHTMODE) {
    drawBin("UI/day/btn_back.bin", 245, 0, 75, 30); // Draw Back Button Thumbnail
    drawBin("UI/day/stats_background.bin", 0, 30, 320, 450); // Draw Back Button Thumbnail

  } else {
    drawBin("UI/night/btn_back.bin", 245, 0, 75, 30); // Draw Back Button Thumbnail
    drawBin("UI/night/stats_background.bin", 0, 30, 320, 450); // Draw Back Button Thumbnail
  }
  tft.drawLine(0, 32, 320, 32, title_bg); // yellow line
  tft.cursorToXY(5, 5);
  tft.setTextColor(title_texts);
  tft.setTextScale(3);
  tft.print("Stats");

  // Draw staistics...
  tft.setTextColor(l_text);
  tft.setTextScale(2);
  tft.cursorToXY(0, 45);
  tft.print("Observation on:\n");
  tft.setTextColor(btn_l_border);
  tft.print(Start_date);
  tft.setTextColor(l_text);
  tft.print(" @ ");
  tft.setTextColor(btn_l_border);
  tft.print(START_TIME);
  tft.println("h");
  tft.setTextColor(l_text);
  tft.print("Report generated at:");
  tft.setTextColor(btn_l_border);
  tft.print(String(rtc.getTimeStr()).substring(0, 5));
  tft.println(" ");
  tft.cursorToXY(100, 105);
  tft.setTextColor(l_text);
  tft.println("LOCATION:");
  tft.cursorToXY(70, 125);
  tft.print("LAT : ");
  tft.setTextColor(btn_l_border);
  tft.println(OBSERVATION_LATTITUDE, 4);
  tft.cursorToXY(70, 145);
  tft.setTextColor(l_text);
  tft.print("LONG: ");
  tft.setTextColor(btn_l_border);
  tft.println(OBSERVATION_LONGITUDE, 4);
  tft.cursorToXY(70, 165);
  tft.setTextColor(l_text);
  tft.print("ALT : ");
  tft.setTextColor(btn_l_border);
  tft.println(OBSERVATION_ALTITUDE, 0);
  tft.println("");
  tft.cursorToXY(5, 195);
  tft.setTextColor(l_text);
  tft.print(" OBSERVATION: ");
  tft.setTextColor(btn_l_border);
  double st;
  int st_h;
  int st_m;
  st = (String(rtc.getTimeStr()).substring(0, 2).toInt() * 60) + String(rtc.getTimeStr()).substring(3, 5).toInt();
  st -= (START_TIME.substring(0, 2).toInt() * 60) + START_TIME.substring(3, 5).toInt();
  if (st < 0) {
    st += 1440;
  }
  st_h = int(st / 60);
  st_m = ((st / 60) - st_h) * 60;
  tft.print(st_h);
  tft.print("h ");
  tft.print(st_m);
  tft.println("m");
  tft.setTextScale(1);
  tft.cursorToXY(0, 220);
  tft.setTextColor(l_text);
  tft.print("Observation started at ");
  tft.setTextColor(btn_l_border);
  tft.print(START_TIME);
  tft.setTextColor(l_text);
  tft.print("h where environment\ntempersture was ");
  tft.setTextColor(btn_l_border);
  tft.print(_Stemp, 0);
  tft.print("C and ");
  tft.print(_Shumid, 0);
  tft.println("% humidity\n");
  tft.setTextColor(l_text);
  tft.print("OBJECTS: ");
  tft.setTextColor(btn_l_border);
  tft.print(Observed_Obj_Count);
  tft.setTextColor(l_text);
  tft.println(" object(s) observed:");
  tft.setTextColor(btn_l_border);
  tft.println("");
  for (int i = 0; i < Observed_Obj_Count; i++) {
    int i1 = ObservedObjects[i].indexOf(';');
    int i2 = ObservedObjects[i].indexOf(';', i1 + 1);
    int i3 = ObservedObjects[i].indexOf(';', i2 + 1);
    int i4 = ObservedObjects[i].indexOf(';', i3 + 1);
    int i5 = ObservedObjects[i].indexOf(';', i4 + 1);
    int i6 = ObservedObjects[i].indexOf(';', i5 + 1);
    int i7 = ObservedObjects[i].indexOf(';', i6 + 1);
    int tt;
    String ha_;
    String degs_;
    if (i == (Observed_Obj_Count - 1)) {
      tt = (((String(rtc.getTimeStr()).substring(0, 2).toInt()) * 60)  + (String(rtc.getTimeStr()).substring(3, 5).toInt())) - ((ObservedObjects[i].substring(i2 + 1, i2 + 3).toInt() * 60) + ObservedObjects[i].substring(i2 + 4, i3).toInt());
      if (tt < 0) {
        tt += 1440;
      }
    } else {
      tt = ObservedObjects[i].substring(i7 + 1, ObservedObjects[i].length()).toInt();
    }
    degs_ = ObservedObjects[i].substring(i6 + 1, i7);
    ha_ = ObservedObjects[i].substring(i5 + 1, i6);
    String Composed = ObservedObjects[i].substring(0, i1) + " @" + ObservedObjects[i].substring(i2 + 1, i3) + " for " + String(tt) + "min | Alt: " + degs_ + "deg\n";
    //Composed += "\nAt the time of observation the object was "+degs_+"deg. above horizon, with HA:"+ha_+" Environment was: " + ObservedObjects[i].substring(i3+1, i4) + " C and " + ObservedObjects[i].substring(i4+1, i5) + "% humidity. ";
    tft.print(Composed);
  }
}

///////////////////////////////////////////////////// Star Sync Screen Graphics ///////////////////////////////////////////////////////
void drawStarSyncScreen() {
  CURRENT_SCREEN = 12;
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 320, 30, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  if (!IS_NIGHTMODE) {
    drawBin("UI/day/btn_done.bin", 245, 0, 75, 30); // Draw Back Button Thumbnail
  } else {
    drawBin("UI/night/btn_done.bin", 245, 0, 75, 30); // Draw Back Button Thumbnail
  }
  tft.cursorToXY(5, 5);
  tft.setTextColor(title_texts);
  tft.setTextScale(3);
  tft.print("Sync...");

  if (!IS_NIGHTMODE) {
    drawBin("UI/day/btn_next_prev.bin", 0, 434, 320, 46); // Draw Next/Prev Button Thumbnail
  } else {
    drawBin("UI/night/btn_next_prev.bin", 0, 434, 320, 46); // Draw Next/Prev Button Thumbnail
  }

  // I'll draw 24 objects per page, thus "(pager*24)" will give me the start of the [index_]
  tft.setTextColor(l_text);
  drawAlignObjects_ali();
}

///////////////////////////////////// Constelation Screen Graphics ///////////////////////////////////////////////
void drawConstelationScreen(int indx)
{
  CURRENT_SCREEN = 13;
  tft.fillRect(0, 0, 320, 30, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  if (!IS_NIGHTMODE) {
    drawBin("UI/day/manual_align_background.bin", 0, 30, 320, 450); // Draw Back Button Thumbnail
  } else {
    drawBin("UI/night/manual_align_background.bin", 0, 30, 320, 450); // Draw Back Button Thumbnail
  }
  tft.cursorToXY(16, 5);
  tft.setTextColor(title_texts);
  tft.setTextScale(3);
  tft.print("MANUAL ALIGNMENT");
  tft.setTextColor(title_bg);
  tft.setTextScale(2);

  if (ALLIGN_TYPE != 3)
  {
    //DrawButton(0, 420, 100, 60, "<Repeat", 0, btn_l_border, btn_l_text, 2, false);
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_sync_repeat.bin", 0, 434, 320, 46); // Draw REPEAT/SYNC! Button Thumbnail
    } else {
      drawBin("UI/night/btn_sync_repeat.bin", 0, 434, 320, 46); // Draw REPEAT/SYNC! Button Thumbnail
    }
  }
  else
  {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_sync_exit.bin", 0, 434, 320, 46); // Draw <EXIT/SYNC! Button Thumbnail
    } else {
      drawBin("UI/night/btn_sync_exit.bin", 0, 434, 320, 46); // Draw <EXIT/SYNC! Button Thumbnail
    }
  }
  if (ALLIGN_STEP == 1)
  {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_sync_exit.bin", 0, 434, 320, 46); // Draw REPEAT/SYNC! Button Thumbnail
    } else {
      drawBin("UI/night/btn_sync_exit.bin", 0, 434, 320, 46); // Draw REPEAT/SYNC! Button Thumbnail
    }

    tft.cursorToXY(0, 80);
    tft.println("Use manual motor\nmovement to center\nthe star!\n");
    tft.println("Once centered, hit\n(SYNC!) button\n");

    tft.print("Selected Object:");
    tft.cursorToXY(1, 250);
    tft.setTextColor(l_text);
    tft.println(OBJECT_NAME);
  }
  else if (ALLIGN_STEP == 2)
  {
    if (ALLIGN_TYPE == 3)
    {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/btn_next_exit.bin", 0, 434, 320, 46); // Draw <EXIT/NEXT> Button Thumbnail
      } else {
        drawBin("UI/night/btn_next_exit.bin", 0, 434, 320, 46); // Draw <EXIT/NEXT> Button Thumbnail
      }
      tft.cursorToXY(0, 80);
      tft.println("Use Alt/Az (physical)\nknobs on your mount\nto center on Polaris!\n");
      tft.println("Once centered, hit\n(ALIGN) button.");
      tft.println("");
      tft.println("NB! Only correct\nhalfway to center!:");
      tft.cursorToXY(1, 275);
      tft.print("Now Observing:");
      tft.setTextColor(l_text);
      tft.cursorToXY(1, 300);
      tft.println(OBJECT_NAME);
    }
    else
    {
      DrawButton(220, 420, 100, 60, "CENTER", 0, btn_l_border, btn_l_text, 2, false);
      tft.cursorToXY(0, 80);
      tft.println("Use manual motor movement\nto center the star!");
      tft.println("");
      tft.println("Once centered, hit (SYNC!\nbutton.");
    }
  }
}

///////////////////////////////////////////////////// On-Screen Messages ///////////////////////////////////////////////////////
void OnScreenMsg(int Msg) {
  // Msg = 1 -> Moving;
  // Msg = 2 -> Tracking Off;
  // Msg = 3 -> Object Not visible;
  // Msg = 5 -> Stepper motors OFF
  // Msg = 6 -> Backing UP ... to Home Pos;
  // Msg = 7 -> Funny message when selecting the Earth;

  char obj_name[50];
  String OBJ_NAME = "";
  if (!IS_NIGHTMODE) {
    OBJ_NAME = "UI/day/";
  } else {
    OBJ_NAME = "UI/night/";
  }

  if (Msg == 1) {

    OBJ_NAME += "msg_1.bin";

  } else if (Msg == 3) {

    OBJ_NAME += "msg_3.bin";

  } else if (Msg == 5) {
    OBJ_NAME += "msg_5.bin";
    if (IS_SOUND_ON) {
      SoundOn(note_C, 32);
      delay(200);
      SoundOn(note_C, 32);
      delay(200);
      SoundOn(note_C, 32);
      delay(1000);
    }

  } else if (Msg == 6) {

    OBJ_NAME += "msg_6.bin";
  }
  else if (Msg == 7) {

    OBJ_NAME += "msg_7.bin";

  }

  if (SD.open(OBJ_NAME)/* && (IS_TRACKING)*/) {
    OBJ_NAME.toCharArray(obj_name, 50);
    drawBin(obj_name, 66.5, 190, 187, 100); // Drawing selected Object Tumbnail 140*140 Pixels
  }

  String m1, m2, m3;
  //tft.fillRect(80, 215, 160, 100, MsgBox_bg );
  //tft.drawRect(80, 215, 160, 100, MsgBox_t);
  tft.setTextColor(MsgBox_t);
  if (Msg == 2) {
    m1 = "WARNING";
    m2 = "TRACKING was";
    m3 = "Turned Off!";
    tft.cursorToXY(98, 230);
    tft.setTextScale(3);
    tft.println(m1);
    tft.cursorToXY(88, 265);
    tft.setTextScale(2);
    tft.print(m2);
    tft.cursorToXY(98, 290);
    tft.print(m3);
    delay(1000);
    drawMainScreen();
  } else if (Msg == 4) {
    m1 = "PICK AGAIN";
    m2 = "ALT=" + String(ALT, 0);
    m3 = "RA= " + String(OBJECT_RA_H, 0);
    //Timer3.stop(); //
    //IS_TRACKING = false;
    tft.cursorToXY(95, 230);
    tft.setTextScale(3);
    tft.println(m1);
    tft.cursorToXY(95, 265);
    tft.setTextScale(2);
    tft.print(m2);
    tft.cursorToXY(95, 290);
    tft.print(m3);
  } else if (Msg == 8) {
    m1 = "Connected";
    m2 = "TO";
    m3 = "PHD2:)";
    tft.cursorToXY(105, 230);
    tft.setTextScale(2);
    tft.println(m1);
    tft.cursorToXY(145, 260);
    tft.setTextScale(2);
    tft.print(m2);
    tft.cursorToXY(125, 290);
    tft.print(m3);
    delay(1000);
  } else if (Msg == 9) {
    m1 = "Disconnected";
    m2 = "FROM";
    m3 = "PHD2:)";
    tft.cursorToXY(87, 230);
    tft.setTextScale(2);
    tft.println(m1);
    tft.cursorToXY(135, 260);
    tft.setTextScale(2);
    tft.print(m2);
    tft.cursorToXY(125, 290);
    tft.print(m3);
    delay(1000);
  }
}

////////////////////////////////////////////////// Day/Night Mode Graphics Changes ///////////////////////////////////////////////////////
void considerDayNightMode() {
  boolean prev_night_mode = IS_NIGHTMODE;
  if (analogRead(DAY_NIGHT_PIN) < 800) {
    IS_NIGHTMODE = true;
  } else {
    IS_NIGHTMODE = false;
  }
  // ora make sure it works for all screen - redraws them when mode switched
  if (prev_night_mode != IS_NIGHTMODE) {
    if (IS_NIGHTMODE == true) {
      // Night Mode
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
    if (CURRENT_SCREEN == 0)
    {
      drawGPSScreen();
    }
    else if (CURRENT_SCREEN == 1)
    {
      drawClockScreen();
    } else if (CURRENT_SCREEN == 3) {
      drawSelectAlignment();
    } else if (CURRENT_SCREEN == 4) {
      drawMainScreen();
    } else if (CURRENT_SCREEN == 5) {
      drawCoordinatesScreen();
    } else if (CURRENT_SCREEN == 6) {
      drawLoadScreen();
    } else if (CURRENT_SCREEN == 7) {
      drawOptionsScreen();
    } else if (CURRENT_SCREEN == 10) {
      drawSTATScreen();
    } else if (CURRENT_SCREEN == 12) {
      drawStarSyncScreen();
    } else if (CURRENT_SCREEN == 13) {
      drawConstelationScreen(0);
    } else if ( CURRENT_SCREEN == 14) {
      drawAutoGuidingScreen();
    }
  }
}

///////////////////////////////////////////////////// Main Screen Menu Graphics ///////////////////////////////////////////////////////
void drawMainScreen_Menu(int fill) {
  if (MAIN_SCREEN_MENU == 0) {
    if (IS_BT_MODE_ON == false) {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/btn_load.bin", 9, 375, 90, 44);
        drawBin("UI/day/btn_home.bin", 115, 375, 90, 44);
        drawBin("UI/day/btn_stats.bin", 9, 428, 90, 44);
        drawBin("UI/day/btn_up_down.bin", 115, 428, 90, 44);
        drawBin("UI/day/btn_BT_off.bin", 222, 428, 90, 44);
      } else {
        drawBin("UI/night/btn_load.bin", 9, 375, 90, 44);
        drawBin("UI/night/btn_home.bin", 115, 375, 90, 44);
        drawBin("UI/night/btn_stats.bin", 9, 428, 90, 44);
        drawBin("UI/night/btn_up_down.bin", 115, 428, 90, 44);
        drawBin("UI/night/btn_BT_off.bin", 222, 428, 90, 44);
      }
    } else {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/btn_up_down.bin", 115, 428, 90, 44);
        drawBin("UI/day/btn_BT_on.bin", 222, 428, 90, 44);
      } else {
        drawBin("UI/night/btn_up_down.bin", 115, 428, 90, 44);
        drawBin("UI/night/btn_BT_on.bin", 222, 428, 90, 44);
      }
    }

    if (IS_TRACKING == true) {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/btn_track_on.bin", 222, 375, 90, 44);
      } else {
        drawBin("UI/night/btn_track_on.bin", 222, 375, 90, 44);
      }
      tft.setTextScale(3);
      tft.fillRect(0, 116, 320, 27, BLACK); // Modif JG, graphic bug
      tft.cursorToXY(1, 119);
      tft.setTextColor(btn_l_border);
      if (OBJECT_NAME.length() > 7) {
        tft.setTextScale(2);
        tft.cursorToXY(1, 129);
        tft.print("TRK:");
      } else {
        tft.setTextScale(3);
        tft.print("TRACKING:");
      }
      tft.setTextColor(title_bg);
      tft.print(OBJECT_NAME);
    } else {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/btn_track_off.bin", 222, 375, 90, 44);
      } else {
        drawBin("UI/night/btn_track_off.bin", 222, 375, 90, 44);
      }
      tft.fillRect(0, 116, 320, 25, BLACK);
      tft.setTextColor(l_text);
      tft.setTextScale(3);
      tft.cursorToXY(1, 119);
      if (OBJECT_NAME.length() > 7) {
        tft.setTextScale(2);
        tft.cursorToXY(1, 129);
        tft.print("OBS:");
      } else {
        tft.setTextScale(3);
        tft.print("OBSERVING:");
      }
      tft.setTextColor(title_bg);
      tft.print(OBJECT_NAME);
    }
    MAIN_SCREEN_MENU = 0;
  } else {

    if (IS_BT_MODE_ON == false) {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/btn_coord.bin", 9, 375, 90, 44);
        drawBin("UI/day/btn_map.bin", 115, 375, 90, 44);
        drawBin("UI/day/btn_option.bin", 9, 428, 90, 44);
        drawBin("UI/day/btn_up_down.bin", 115, 428, 90, 44);
      } else {
        drawBin("UI/night/btn_coord.bin", 9, 375, 90, 44);
        drawBin("UI/night/btn_map.bin", 115, 375, 90, 44);
        drawBin("UI/night/btn_option.bin", 9, 428, 90, 44);
        drawBin("UI/night/btn_up_down.bin", 115, 428, 90, 44);
      }
    } else {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/btn_up_down.bin", 115, 428, 90, 44);
      } else {
        drawBin("UI/night/btn_up_down.bin", 115, 428, 90, 44);
      }
    }

    if (IS_FAN1_ON) {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/btn_fan1_on.bin", 222, 375, 90, 44);
      } else {
        drawBin("UI/night/btn_fan1_on.bin", 222, 375, 90, 44);
      }
    } else {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/btn_fan1_off.bin", 222, 375, 90, 44);
      } else {
        drawBin("UI/night/btn_fan1_off.bin", 222, 375, 90, 44);
      }
    }
    if (IS_GTMEM_ON) {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/btn_gotoLast_on_gt.bin", 222, 428, 90, 44);
      } else {
        drawBin("UI/night/btn_gotoLast_on_gt.bin", 222, 428, 90, 44);
      }
    } else {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/btn_gotoLast_off_st.bin", 222, 428, 90, 44);
      } else {
        drawBin("UI/night/btn_gotoLast_off_st.bin", 222, 428, 90, 44);
      }
    }
    MAIN_SCREEN_MENU = 1;
  }
}

//////////////////////////////////////////////// Time Screen Time Change Graphics ///////////////////////////////////////////////////////
void TimerUpdateDraw(int z) {
  w_DateTime[dateEntryPos] = z;
  if (dateEntryPos >= 0 && dateEntryPos < 2) {
    tft.fillRect((dateEntryPos * 16) + 55, 68, 16, 20, l_text); // 15 was 18
    tft.cursorToXY((dateEntryPos * 16) + 55, 68);
  } else if (dateEntryPos > 1 && dateEntryPos < 4) {
    tft.fillRect((dateEntryPos * 16) + 55, 68, 16, 20, l_text);
    tft.cursorToXY((dateEntryPos * 16) + 55, 68);
  } else if (dateEntryPos > 3 && dateEntryPos < 8) {
    tft.fillRect((dateEntryPos * 16) + 55, 68, 16, 20, l_text);
    tft.cursorToXY((dateEntryPos * 16) + 55, 68);
  } else if (dateEntryPos > 7 && dateEntryPos < 10) {
    tft.fillRect(((dateEntryPos - 8) * 16) + 195, 68, 16, 20, l_text);
    tft.cursorToXY(((dateEntryPos - 8) * 16) + 195, 68);
  } else if (dateEntryPos > 9) {
    tft.fillRect(((dateEntryPos - 8) * 16) + 195, 68, 16, 20, l_text);
    tft.cursorToXY(((dateEntryPos - 8) * 16) + 195, 68);
  }
  tft.print(w_DateTime[dateEntryPos]);
  if (dateEntryPos > 10) {
    dateEntryPos = 0;
  } else {
    dateEntryPos += 1;
  }
}

////////////////////////////////////////////////// Star Maps Screen Graphics ///////////////////////////////////////////////////////
void drawStarMap() {
  CURRENT_SCREEN = 11;
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 320, 30, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  if (!IS_NIGHTMODE) {
    drawBin("UI/day/btn_back.bin", 245, 0, 75, 30); // Draw Back Button Thumbnail
  } else {
    drawBin("UI/night/btn_back.bin", 245, 0, 75, 30); // Draw Back Button Thumbnail
  }
  tft.cursorToXY(10, 6);
  tft.setTextColor(title_texts);
  tft.setTextScale(3);
  tft.print("StarMap");

  // Draw Star Map...
  String PIC_StarMap = "starmap/";
  if (IS_NIGHTMODE) {
    PIC_StarMap += "night/";
  } else {
    PIC_StarMap += "day/";
  }

  // Need to calculate which image to show
  // Images are named as in a matrix
  // (1,8 1,7 1,6 .... 1,1)
  // (2,8 2,7 2,6 .... 2,1)
  // (3,8 3,7 3,6 .... 3,1)
  // 1,1  = DEC > 30 deg and 3h > RA > 0h;
  // 1,2  = DEC > 30 deg and 6h> RA > 3h;
  // 2,1 = -30 deg > DEC > 30 deg and 3h > RA > 0h;
  // ......

  Current_RA_DEC();
  float DEC_dd = curr_DEC_D + curr_DEC_M / 60;
  int telescope_X = 0;
  int telescope_Y = 0;
  int tmp_map_r = 0;
  int tmp_map_c = 0;
  if (DEC_dd > 30) {
    tmp_map_r = 1;
  } else if (DEC_dd < -30) {
    tmp_map_r = 3;
  } else {
    tmp_map_r = 2;
  }
  tmp_map_c = int((curr_RA_H + curr_RA_M / 60) / 3) + 1;

  if ((tmp_map_c == map_c) && (tmp_map_r == map_r)) {
    IS_CUSTOM_MAP_SELECTED = false;
  }

  // First time only after you Start the rDUINOScope map, it needs this routine!
  if ((map_c == 0) && (map_r == 0)) {
    map_c = tmp_map_c;
    map_r = tmp_map_r;
  }

  if (!IS_CUSTOM_MAP_SELECTED) {
    map_c = tmp_map_c;
    map_r = tmp_map_r;
    // ora decide where is the Telescope Pointing...
    // Only calculate Telescope IF: -70 < DEC < 70 deg.
    if ((DEC_dd < 70) && (DEC_dd > -70)) {
      telescope_X = 320 - (106 * ((curr_RA_H + curr_RA_M / 60) - (tmp_map_c - 1) * 3));

      if (tmp_map_r == 2) {

        if ((DEC_dd > -20) && (DEC_dd < 20)) {
          telescope_Y = 255 + DEC_dd * -7.35;  //  Was   -5.35  Done
          // Example Targets M 15,M53    //M5, M77, M61 > 0 //M2, M14, M73 <0
        } else if (DEC_dd > 20) {
          telescope_Y = 130 - (DEC_dd - 20) * 9.1; //     Done
          // Example Targets  M1,M64,M3
        } else if (DEC_dd < -20) {
          telescope_Y = 390 + (abs(DEC_dd) - 20) * 9.1; //   Done
          // Example Targets  M4, M30, M75,M83
        }
      } else {
        if ((abs(DEC_dd) > 30) && (abs(DEC_dd) < 40)) {
          telescope_Y = 480 - (DEC_dd - 30) * 7.2;        // Done
          // Example Targets  M13,M36, M37, M38,M57
        } else if ((abs(DEC_dd) > 40) && (abs(DEC_dd) < 60)) {
          telescope_Y = 412 - (DEC_dd - 40) * 10.4;              // 438         // Done
          // Example Targets   M110 , M92, M39, M102,M101, M51,M63, M76
        } else if (DEC_dd > 60) {
          telescope_Y = 243 - (DEC_dd - 60) * 18.7;   // Done
          // Example Targets  M82, M81
        }
        if (DEC_dd < 0) {
          telescope_Y = 450 - telescope_Y; // Was 480
        }
      }
    }
  }

  char My_Map[50];
  PIC_StarMap += String(map_r) + "-" + String(map_c) + ".bin";
  PIC_StarMap.toCharArray(My_Map, 50);
  drawBin(My_Map, 0, 30, 320, 450);

  if (!IS_CUSTOM_MAP_SELECTED) {
    tft.drawCircle(telescope_X, telescope_Y, 20, btn_l_border);
    tft.drawLine(0, telescope_Y, 320, telescope_Y, btn_l_border);
    tft.drawLine(telescope_X, 30, telescope_X, 480, btn_l_border);
  }
  IS_CUSTOM_MAP_SELECTED = false;
}

///////////////////////////////////////////////////// Status Bar Graphics ///////////////////////////////////////////////////////
void drawStatusBar()
{
  if (!IS_NIGHTMODE) {
    drawBin("UI/day/status_bar_back.bin", 0, 0, 320, 27); // Drawing selected Object Tumbnail 140*140 Pixels
  } else {
    drawBin("UI/night/status_bar_back.bin", 0, 0, 320, 27); // Drawing selected Object Tumbnail 140*140 Pixels
  }

  tft.drawLine(0, 26, 320, 26, BLACK );//
  tft.setTextScale(1);
  tft.setTextColor(l_text); //Button_Title

  tft.cursorToXY(3, 4);
  tft.print("Bright");

  tft.cursorToXY(47, 4);
  tft.print("T-out");

  tft.cursorToXY(85, 4);
  tft.print("Track-M");

  tft.cursorToXY(138, 4);
  tft.print("Mer-F");

  tft.cursorToXY(177, 4);
  tft.print("Fan");

  tft.cursorToXY(208, 4); // Add JG mem position status
  tft.print("Mem-P");

  tft.cursorToXY(244, 4);
  tft.print("Sound");

  tft.cursorToXY(283, 4);
  tft.print("Motors");

  if (TFT_Brightness == 255)
  {
    tft.setTextColor(Button_State_ON);
    tft.cursorToXY(5, 18);
    tft.print(TFT_Brightness * 100 / 255);
  }

  if (TFT_Brightness <= 254)
  {
    tft.setTextColor(Button_State_OFF);
    tft.cursorToXY(7, 18);
    tft.print(TFT_Brightness * 100 / 255);
  }

  tft.cursorToXY(29, 18);
  tft.print("%");

  if (TFT_Time == "AL-ON") {
    tft.cursorToXY(47, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("AL-ON");
  }

  if (TFT_Time == "30 s") {
    tft.cursorToXY(49, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("30 S");
  }

  if (TFT_Time == "60 s") {
    tft.cursorToXY(49, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("60 S");
  }

  if (TFT_Time == "2 min") {
    tft.cursorToXY(49, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("2 M");
  }

  if (TFT_Time == "5 min") {
    tft.cursorToXY(49, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("5 M");
  }

  if (TFT_Time == "10 min") {
    tft.cursorToXY(48, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("10 M");
  }

  if (Tracking_Mode == "Celest") {
    tft.cursorToXY(88, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("Celest");
  }

  if (Tracking_Mode == "Lunar") {
    tft.cursorToXY(88, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("Lunar");
  }

  if (Tracking_Mode == "Solar") {
    tft.cursorToXY(88, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("Solar");
  } // modif JG was copied paste line here that make a graphic bug on mer flip status

  if (Mer_Flip_State == "AUTO") {
    //tft.fillRect(140, 14, 30, 10, title_bg);
    tft.cursorToXY(141, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("AUTO");
  }

  if (Mer_Flip_State == "OFF") {
    //tft.fillRect(140, 14, 30, 10, title_bg);
    tft.cursorToXY(141, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("OFF");
  }

  if (Fan1_State == "ON") {
    tft.cursorToXY(180, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("ON");
  }

  if (Fan1_State == "OFF") {
    tft.cursorToXY(180, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("OFF");
  }

  if (GTMEM_State == "ARM") { // Add JG mem position ARM / Free
    tft.cursorToXY(214, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("ARM");
  }

  if (GTMEM_State == "Free") {
    tft.cursorToXY(214, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("Free");
  }

  if (Sound_State == "ON") {
    tft.cursorToXY(252, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("ON");
  }

  if (Sound_State == "OFF") {
    tft.cursorToXY(252, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("OFF");
  }

  if (Stepper_State == "ON") {
    tft.cursorToXY(292, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("ON");
  }

  if (Stepper_State == "OFF") {
    tft.cursorToXY(292, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("OFF");
  }
}

///////////////////////////////////////////////////// Graphics Update Function ///////////////////////////////////////////////////////

void updateTrackingMode_opt() 
{
  if (Tracking_type == 0) {  // 1: Sidereal, 2: Solar, 0: Lunar;

    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_lunar_on.bin", 211, 418, 97, 49);       // Lunar Button on
      drawBin("UI/day/btn_celestial_off.bin", 211, 321, 97, 49);  //Celestial Button off
      drawBin("UI/day/btn_solar_off.bin", 211, 370, 97, 49);      // Solar Button off
    } else {
      drawBin("UI/night/btn_lunar_on.bin", 211, 418, 97, 49);       // Lunar Button on
      drawBin("UI/night/btn_celestial_off.bin", 211, 322, 97, 49);  //Celestial Button off
      drawBin("UI/night/btn_solar_off.bin", 211, 370, 97, 49);      // Solar Button off
    }
  } else if (Tracking_type == 1) {  // 1: Sidereal, 2: Solar, 0: Lunar;
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_celestial_on.bin", 211, 321, 97, 49);  //Celestial Button on
      drawBin("UI/day/btn_solar_off.bin", 211, 370, 97, 49);     // Solar Button off
      drawBin("UI/day/btn_lunar_off.bin", 211, 418, 97, 49);     // Lunar Button off
    } else {
      drawBin("UI/night/btn_celestial_on.bin", 211, 322, 97, 49);  //Celestial Button on
      drawBin("UI/night/btn_solar_off.bin", 211, 370, 97, 49);     // Solar Button off
      drawBin("UI/night/btn_lunar_off.bin", 211, 418, 97, 49);     // Lunar Button off
    }
  } else if (Tracking_type == 2) {  //  1: Sidereal, 2: Solar, 0: Lunar;
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_solar_on.bin", 211, 370, 97, 49);       // Solar Button on
      drawBin("UI/day/btn_celestial_off.bin", 211, 321, 97, 49);  //Celestial Button off
      drawBin("UI/day/btn_lunar_off.bin", 211, 418, 97, 49);      // Lunar Button off
    } else {
      drawBin("UI/night/btn_solar_on.bin", 211, 370, 97, 49);       // Solar Button on
      drawBin("UI/night/btn_celestial_off.bin", 211, 322, 97, 49);  //Celestial Button off
      drawBin("UI/night/btn_lunar_off.bin", 211, 418, 97, 49);      // Lunar Button off
    }
  }
}

void updateScreenTimeout_opt() 
{
  if (TFT_timeout == 0) {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_never_on.bin", 3, 221, 54, 48);  // NEVER Button on
    } else {
      drawBin("UI/night/btn_never_on.bin", 3, 221, 54, 48);  // NEVER Button on
    }
  } else {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_never_off.bin", 3, 221, 54, 48);  // NEVER Button off
    } else {
      drawBin("UI/night/btn_never_off.bin", 3, 221, 54, 48);  // NEVER Button off
    }
  }
  if (TFT_timeout == 30000) {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_30s_on.bin", 56, 221, 53, 48);  // 30S Button on
    } else {
      drawBin("UI/night/btn_30s_on.bin", 56, 221, 53, 48);  // 30S Button on
    }
  } else {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_30s_off.bin", 56, 221, 53, 48);  // 30S Button off
    } else {
      drawBin("UI/night/btn_30s_off.bin", 56, 221, 53, 48);  // 30S Button off
    }
  }
  if (TFT_timeout == 60000) {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_60s_on.bin", 108, 221, 53, 48);  // 60S Button on
    } else {
      drawBin("UI/night/btn_60s_on.bin", 108, 221, 53, 48);  // 60S Button on
    }
  } else {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_60s_off.bin", 108, 221, 53, 48);  // 60S Button off
    } else {
      drawBin("UI/night/btn_60s_off.bin", 108, 221, 53, 48);  // 60S Button off
    }
  }
  if (TFT_timeout == 120000) {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_2m_on.bin", 160, 221, 53, 48);  // 2M Button on
    } else {
      drawBin("UI/night/btn_2m_on.bin", 160, 221, 53, 48);  // 2M Button on
    }
  } else {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_2m_off.bin", 160, 221, 53, 48);  // 2M Button off
    } else {
      drawBin("UI/night/btn_2m_off.bin", 160, 221, 53, 48);  // 2M Button off
    }
  }
  if (TFT_timeout == 300000) {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_5m_on.bin", 212, 221, 53, 48);  // 5M Button on
    } else {
      drawBin("UI/night/btn_5m_on.bin", 212, 221, 53, 48);  // 5M Button on
    }
  } else {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_5m_off.bin", 212, 221, 53, 48);  // 5M Button off
    } else {
      drawBin("UI/night/btn_5m_off.bin", 212, 221, 53, 48);  // 5M Button off
    }
  }
  if (TFT_timeout == 600000) {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_10m_on.bin", 265, 221, 53, 48);  // 10M Button on
    } else {
      drawBin("UI/night/btn_10m_on.bin", 264, 221, 53, 48);  // 10M Button on
    }
  } else {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_10m_off.bin", 265, 221, 53, 48);  // 10M Button off
    } else {
      drawBin("UI/night/btn_10m_off.bin", 264, 221, 53, 48);  // 10M Button off
    }
  }
}

void updateMeridianFlip_opt() 
{
  if (IS_MERIDIAN_FLIP_AUTOMATIC) {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_auto_on.bin", 111, 323, 93, 48);      // Mer F Auto Button on
      drawBin("UI/day/btn_mer_off_off.bin", 111, 370, 93, 49);  // Mer F off Button off
    } else {
      drawBin("UI/night/btn_auto_on.bin", 111, 323, 93, 48);      // Mer F Auto Button on
      drawBin("UI/night/btn_mer_off_off.bin", 111, 371, 93, 48);  // Mer F off Button off
    }
  } else {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_auto_off.bin", 111, 323, 93, 48);    // Mer F Auto Button off
      drawBin("UI/day/btn_mer_off_on.bin", 111, 370, 93, 49);  // Mer F off Button on
    } else {
      drawBin("UI/night/btn_auto_off.bin", 111, 323, 93, 48);    // Mer F Auto Button off
      drawBin("UI/night/btn_mer_off_on.bin", 111, 370, 93, 48);  // Mer F off Button on
    }
  }
}

void updateSound_opt() 
{
  if (IS_SOUND_ON) {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_sound_on.bin", 8, 162, 88, 26);  // Sound Button on
    } else {
      drawBin("UI/night/btn_sound_on.bin", 8, 162, 88, 26);  // Sound Button on
    }
  } else {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_sound_off.bin", 8, 162, 88, 26);  // Sound Button off
    } else {
      drawBin("UI/night/btn_sound_off.bin", 8, 162, 88, 26);  // Sound Button off
    }
  }
}

void updateStepper_opt() 
{
  if (IS_STEPPERS_ON) {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_steppers_on.bin", 115, 162, 88, 26);  // Steppers Button on
    } else {
      drawBin("UI/night/btn_steppers_on.bin", 115, 162, 88, 26);  // steppers Button on
    }
  } else {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_steppers_off.bin", 115, 162, 88, 26);  // Steppers Button off
    } else {
      drawBin("UI/night/btn_steppers_off.bin", 115, 162, 88, 26);  // steppers Button off
    }
  }
}

void updateTriangleBrightness_opt() 
{
  if (TFT_Brightness >= 255) {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_max_on.bin", 259, 54, 51, 45);  // Max Button on
    } else {
      drawBin("UI/night/btn_max_on.bin", 259, 54, 51, 45);  // Max Button on
    }
  } else {
    if (!IS_NIGHTMODE) {
      drawBin("UI/day/btn_max_off.bin", 259, 54, 51, 45);  // Max Button off
    } else {
      drawBin("UI/night/btn_max_off.bin", 259, 54, 51, 45);  // Max Button off
    }
  }

  //Dark Triangle
  tft.fillTriangle(5, 100, 253, 100, 253, 50, BLACK);

  //Clear Triangle
  int x_br = (TFT_Brightness - 192) * 4 + 1;
  int tria_h = floor((255 - x_br) * 0.2000);
  tft.fillTriangle(5, 100, x_br, 100, x_br, tria_h + 51, Bright_triangle);
  int bright_perc = (TFT_Brightness * 100 / 255);

#ifdef serial_debug
  Serial.print("tria_h = ");
  Serial.println(tria_h);
  Serial.print("brightness: ");
  Serial.println(TFT_Brightness);
#endif

  tft.setTextScale(2);
  tft.setTextColor(btn_l_border);
  tft.cursorToXY(130, 85);
  tft.print(bright_perc);
  tft.print(" %");
  tft.setTextColor(btn_l_text);
}


///////////////////////////////////////////////////// Draw Alignment Objects Function ///////////////////////////////////////////////////////

void drawAlignObjects_ali()
{
  tft.fillRect(0, 80, 320, 340, BLACK);
  if (ALLIGN_TYPE == 3)
  {
    int kk = STARS_PAGER * 24;
    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 4; j++) {
        int i1 = Iter_Stars[kk].indexOf(';');
        int i2 = Iter_Stars[kk].indexOf(';', i1 + 1);
        String S_NAME = Iter_Stars[kk].substring(i1 + 1, i2);
        String C_NAME = Iter_Stars[kk].substring(0, i1);
        if (S_NAME == "") {
          break;
        }
        tft.fillRect(((j * 75) + 12), ((i * 50) + 85), 71, 45, messie_btn);
        int l = (S_NAME.length() / 2) * 6;
        tft.cursorToXY(((j * 75) + (44 - l)), ((i * 50) + 93));
        tft.setTextScale(1);
        tft.print(S_NAME);
        tft.setTextScale(2);
        tft.cursorToXY(((j * 75) + 29), ((i * 50) + 110));
        tft.print(C_NAME);
        kk += 1;
      }
    }
  }
  else
  {
    int kk = STARS_PAGER * 24;
    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 4; j++) {
        int i1 = Stars[kk].indexOf(';');
        int i2 = Stars[kk].indexOf(';', i1 + 1);
        String S_NAME = Stars[kk].substring(i1 + 1, i2);
        String C_NAME = Stars[kk].substring(0, i1);
        if (S_NAME == "") {
          break;
        }
        tft.fillRect(((j * 75) + 12), ((i * 50) + 85), 71, 45, messie_btn);
        int l = (S_NAME.length() / 2) * 6;
        tft.cursorToXY(((j * 75) + (44 - l)), ((i * 50) + 93));
        tft.setTextScale(1);
        tft.print(S_NAME);
        tft.setTextScale(2);
        tft.cursorToXY(((j * 75) + 29), ((i * 50) + 110));
        tft.print(C_NAME);
        kk += 1;
      }
    }
  }
}

///////////////////////////////////////////////////// Draw Load Objects Function ///////////////////////////////////////////////////////
void drawLoadObjects() {
  if (LOAD_SELECTOR >= 2 && LOAD_SELECTOR <= 4 or LOAD_SELECTOR >= 6 && LOAD_SELECTOR <= 9 or LOAD_SELECTOR >= 11 && LOAD_SELECTOR <= 14) // Add JG, deleted all messier, treasure, custom conditions, now one condition for all catalogs and another one for solar system. 
  {
// Add JG local load csv file
  char in_char;
  String items = "";
  int k = 0;
  int l = 0;
  File dataFile = SD.open(CAT_NAME);
    while (dataFile.available())
    {
      in_char = dataFile.read();
      items += in_char;
      l = l + 1;
      if (in_char == '\n') {
      OBJ_Array[k] = items;
        k = k + 1;
        //          Serial.print(items);
        items = "";
      }}
      if (k < 240 ){ for (int i = 0; i < (240-k); i++) { OBJ_Array[k+i] = ""; }} // Add JG to fill space with empty data, after last object, in case of object number < 240, array size
  dataFile.close();
  items = "";
  k = 0;
  l = 0;
// End add JG local load csv file
    
    tft.setTextScale(1);
    int ll = OBJ_PAGER * 20;
    if (OBJ_PAGER >= 12) {ll = (OBJ_PAGER - 12) * 20;} // Add JG for second 240 item NGC catalog
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 4; j++) {

        int i1 = OBJ_Array[ll].indexOf(';');
        int i3 = OBJ_Array[ll].indexOf(';', OBJ_Array[ll].indexOf(';', i1 + 1) + 1);
        int i4 = OBJ_Array[ll].indexOf(';', i3 + 1);
        int i5 = OBJ_Array[ll].indexOf(';', i4 + 1);
        String M_NAME = OBJ_Array[ll].substring(0, i1);
        String C_NAME = OBJ_Array[ll].substring(i3 + 1, i4);
        String MAG_NAME = OBJ_Array[ll].substring(i5 + 1, OBJ_Array[ll].indexOf(';', i5 + 1));
        if (M_NAME == "")
        {
          tft.fillRect(((j * 75) + 12), ((i * 50) + 160), 71, 45, BLACK);
        }
        else
        {
          DrawButton(((j * 75) + 12), ((i * 50) + 160), 71, 45, M_NAME, messie_btn, 0, l_text, 1, true);
          ll += 1;
        tft.setTextScale(1);
        tft.cursorToXY(((j * 75) + 15), ((i * 50) + 162));
        tft.print(C_NAME);
        if (LOAD_SELECTOR > 2) {
        tft.cursorToXY(((j * 75) + 15), ((i * 50) + 192));
        tft.print(MAG_NAME);}
        }
      }
    }
  }
  else if (LOAD_SELECTOR == 1)  // solar system planets
  {
    tft.setTextScale(1);
    int ll = 0;

    for (int i = 0; i < 5; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        if (ll >= 11)
        {
          tft.fillRect(((j * 75) + 12), ((i * 50) + 160), 71, 45, BLACK);
        }
        else
        {
          String M_NAME = ss_planet_names[ll];
          DrawButton(((j * 75) + 12), ((i * 50) + 160), 71, 45, M_NAME, messie_btn, 0, l_text, 1, true);
        }
        ll += 1;
      }
    }
  }
  }

/////////////////////////////////////////////// Draw Sun Tracking Confirmation Screen ///////////////////////////////////////////////////////
void drawConfirmSunTrack()
{
  CURRENT_SCREEN = 15;
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 320, 100, RED);
  tft.drawRect(10, 10, 300, 80, YELLOW);
  tft.setTextScale(3);
  tft.setTextColor(YELLOW);
  tft.cursorToXY(42, 35);
  tft.println("!! WARNING !!");

  tft.setTextColor(WHITE);
  tft.cursorToXY(0, 120);
  tft.setTextScale(2);
  tft.print("Looking directly at Sun,\nwithout any protection\nfilter like the BAADER\nASTROSOLAR FILTER, may\nresult in\n");
  tft.setTextColor(RED);
  tft.print("permanent damage to your\neye or telescope!\n\n");
  tft.setTextColor(YELLOW);
  tft.setTextScale(1);
  tft.println("The programmer is not responsible for damages caused by the improper use of the rDUINOScope GoTo\ncontroller.\n");

  tft.setTextColor(WHITE);
  tft.setTextScale(2);
  tft.print("Do you want to continue?");

  DrawButton(10, 380, 140, 80, "YES", 0, btn_l_border, btn_l_text, 3, false);
  DrawButton(170, 380, 140, 80, "NO", 0, btn_l_border, btn_l_text, 3, false);


  /////////////////////////////////// Touch Calibration Graphics ////////////////////////////////
  /////////////////////// Unavailable for the Adafruit Version /////////////////////////////
}

  
  //Add JG
  
void drawScreenCalibration()
  {
  CURRENT_SCREEN = 9;
  drawBin("UI/day/ScreenCalib.bin", 0, 0, 320, 480); 
  } 

 // end Add JG
  /*
    while (!myTouch.touched()) {}

    int tx = 0;
    int ty = 0;

    while (ty < 10 || ty > 150 && ty < 170 || ty > 310 || tx < 380 || tx > 460)
    {
      if(myTouch.touched())
      {

        p = myTouch.getPoint();
        while (p.z < 600)
        {
          p = myTouch.getPoint(); //to remove noise
          delay(200);
        }

        tx = (p.x - 257) / calx;
        ty = (p.y - 445) / caly;

        //Useful to debug touch:
        #ifdef serial_debug
          Serial.print(" -> Touched: x = ");
          Serial.print(tx);
          Serial.print(", y = ");
          Serial.println(ty);
        #endif
      }
      else  delay(100);
    }
    considerTouchInput(ty, tx);
    }
  */

///////////////////////////////////////////////////// Auto-Guiding Screen Graphics ///////////////////////////////////////////////////////
void drawAutoGuidingScreen() {
  CURRENT_SCREEN = 14;
  if (IS_NIGHTMODE) {
    drawBin("UI/night/autoguiding.bin", 0, 0, 320, 481);
  } else {
    drawBin("UI/day/autoguiding.bin", 0, 0, 320, 481);
  }
  DrawButton(10, 80, 300, 40, "RA+ Pulse", 0, btn_d_border, btn_d_border, 3, false);
  DrawButton(10, 135, 300, 40, "RA- Pulse", 0, btn_d_border, btn_d_border, 3, false);
  DrawButton(10, 190, 300, 40, "DEC+ Pulse", 0, btn_d_border, btn_d_border, 3, false);
  DrawButton(10, 245, 300, 40, "DEC- Pulse", 0, btn_d_border, btn_d_border, 3, false);
}
