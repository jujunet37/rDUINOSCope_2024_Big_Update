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

  char OBJ_NAME[50];
  char OBJ_NAME_1[50];
  
// Determine the file paths
  if (!IS_NIGHTMODE) {
    safeStringCopy(OBJ_NAME, "UI/day/bg_time_1.bin", sizeof(OBJ_NAME));
    if (Summer_Time == 0) {
      safeStringCopy(OBJ_NAME_1, "UI/day/bg_time_2_off.bin", sizeof(OBJ_NAME_1));
    } else {
      safeStringCopy(OBJ_NAME_1, "UI/day/bg_time_2_on.bin", sizeof(OBJ_NAME_1));
    }
  } else {
    safeStringCopy(OBJ_NAME, "UI/night/bg_time_1.bin", sizeof(OBJ_NAME));
    if (Summer_Time == 0) {
      safeStringCopy(OBJ_NAME_1, "UI/night/bg_time_2_off.bin", sizeof(OBJ_NAME_1));
    } else {
      safeStringCopy(OBJ_NAME_1, "UI/night/bg_time_2_on.bin", sizeof(OBJ_NAME_1));
    }
  }
  
// Open and draw files directly with existing buffers
  if (SD.open(OBJ_NAME)) {
    drawBin(OBJ_NAME, 0, 30, 320, 450);
  }
  if (SD.open(OBJ_NAME_1)) {
    drawBin(OBJ_NAME_1, 0, 415, 320, 65);
  }

  tft.drawRect(15, 224, 80, 50, messie_btn);
  tft.drawRect(120, 224, 80, 50, messie_btn);
  tft.drawRect(230, 224, 80, 50, messie_btn);
  tft.drawRect(15, 285, 80, 50, messie_btn);
  tft.drawRect(120, 285, 80, 50, messie_btn);
  tft.drawRect(230, 285, 80, 50, messie_btn);
  tft.drawRect(15, 346, 80, 50, messie_btn);
  tft.drawRect(120, 346, 80, 50, messie_btn);
  tft.drawRect(230, 346, 80, 50, messie_btn);
  tft.drawRect(120, 408, 80, 50, messie_btn);

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

  char OBJ_NAME[50];
  
// Determine the file path directly in the buffer
  if (!IS_NIGHTMODE) {
    if (OBSERVATION_LONGITUDE > 0) {
      safeStringCopy(OBJ_NAME, "UI/day/bg_allign_2.bin", sizeof(OBJ_NAME));
    } else {
      safeStringCopy(OBJ_NAME, "UI/day/bg_allign_1.bin", sizeof(OBJ_NAME));
    }
  } else {
    if (OBSERVATION_LONGITUDE > 0) {
      safeStringCopy(OBJ_NAME, "UI/night/bg_allign_2.bin", sizeof(OBJ_NAME));
    } else {
      safeStringCopy(OBJ_NAME, "UI/night/bg_allign_1.bin", sizeof(OBJ_NAME));
    }
  }
  
// Open and draw directly with the OBJ_NAME buffer
  if (SD.open(OBJ_NAME)) {
    drawBin(OBJ_NAME, 0, 30, 320, 450); // Drawing selected Object Tumbnail 140*140 Pixels
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
  char OBJ_NAME[100];
  char OBJ_NAME_1[50];
  char OBJ_NAME_2[50];
  
  if (IS_NIGHTMODE == true) {
    safeStringCopy(OBJ_NAME, "objects/night/", sizeof(OBJ_NAME));
    safeStringCopy(OBJ_NAME_1, "UI/night/status_bar_back.bin", sizeof(OBJ_NAME_1));
    safeStringCopy(OBJ_NAME_2, "UI/night/menu_background.bin", sizeof(OBJ_NAME_2));
  } else {
    safeStringCopy(OBJ_NAME, "objects/day/", sizeof(OBJ_NAME));
    safeStringCopy(OBJ_NAME_1, "UI/day/status_bar_back.bin", sizeof(OBJ_NAME_1));
    safeStringCopy(OBJ_NAME_2, "UI/day/menu_background.bin", sizeof(OBJ_NAME_2));
  }

  if (SD.open(OBJ_NAME_1))
  {
    drawBin(OBJ_NAME_1, 0, 89, 320, 27);
    tft.drawLine(0, 90, 320, 90, BLACK); // Time_area_back
  }

  tft.fillRect(0, 27, 320, 63, Time_area_back); // ILI9341_INDIGO // title_bg / ILI9341_SLATEBLUE /ILI9341_DARKBLUE/ ILI9341_DODGERBLUE
  drawStatusBar();
  tft.setTextColor(Time_area_font); // title_texts // ILI9341_INDIGO ILI9341_DARKBLUE
  tft.cursorToXY(1, 35);
  tft.setTextScale(3);
  tft.print("TIME:");
  tft.cursorToXY(88, 35);
  
  char timeStr[20];
  const char* fullTime = rtc.getTimeStr();
  strncpy(timeStr, fullTime, 5);
  timeStr[5] = '\0';
  tft.print(timeStr);

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
  tft.setTextColor(l_text);
  tft.cursorToXY(1, 100);
  tft.print("Date:");

  tft.setTextColor(btn_l_border);
  tft.cursorToXY(35, 100);
  
  char dateStr[20];
  const char* fullDate = rtc.getDateStr();
  // Extract day (2 first types)
  strncpy(dateStr, fullDate, 2);
  dateStr[2] = '\0';
  tft.print(dateStr);
  tft.print(" ");
  tft.print(rtc.getMonthStr(FORMAT_SHORT));
  tft.print(" ");
  // Extract year (last 4 characters)
  int dateLen = strlen(fullDate);
  if (dateLen >= 4) {
    tft.print(fullDate + dateLen - 4);
  }
  
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
  tft.setTextColor(l_text);
  tft.print(" ALT:");
  tft.setTextColor(btn_l_border);
  tft.print(OBSERVATION_ALTITUDE, 0);

  tft.setTextScale(3);
  tft.setTextColor(l_text);
  tft.cursorToXY(1, 119);
  if (strlen(OBJECT_NAME) > 7) {
    tft.setTextScale(2);
    tft.cursorToXY(1, 129);
    tft.print("OBS:");
  } else {
    tft.setTextScale(3);
    tft.print("OBSERVING:");
  }

  // Data for the observed object....
  if (strlen(OBJECT_NAME) > 0) {
    tft.setTextColor(title_bg);
    tft.print(OBJECT_NAME);

    if ((strlen(OBJECT_NAME) > 0) && (ALT < 0)) {
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
      // ASCOM part
      if ((ascomConnected) && (slewascomsent)) {
        if (IS_NIGHTMODE) {
            safeStringCopy(obj_name, "objects/night/ASCOM.bin", sizeof(obj_name));
        } else {
            safeStringCopy(obj_name, "objects/day/ASCOM.bin", sizeof(obj_name));
        }
        drawBin(obj_name, 0, 143, 320, 142);
      } else {
        // build path
        snprintf(obj_name, sizeof(obj_name), "%s%s.bin", OBJ_NAME, OBJECT_NAME);
        if (SD.open(obj_name)) {
          drawBin(obj_name, 0, 143, 320, 142);
        }
      }

      if (SD.open(OBJ_NAME_2) /*&& (IS_TRACKING)*/)
      {
        drawBin(OBJ_NAME_2, 0, 286, 320, 194);
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

  if ((strcmp(OBJECT_NAME, "CP") != 0) && (IS_BT_MODE_ON == false)) {
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
  // Draw Main Screen Menu:
  drawMainScreen_Menu(0);

  if (ascomConnected == true) {
    tft.cursorToXY(15, 340);
    tft.setTextScale(3);
    tft.setTextColor(GREEN);
    tft.print("ASCOM REMOTE");
  }
}


///////////////////////////////////////////////////// Coordintaes Screen Graphics ///////////////////////////////////////////////////////

void drawCoordinatesScreen() {
  CURRENT_SCREEN = 5;
  tft.fillRect(0, 0, 320, 30, title_bg); // yellow Rectangle  Was (1, 1, 239, 35, title_bg);
  tft.setTextColor(title_texts);
  tft.setTextScale(3);
  tft.cursorToXY(5, 5);
  tft.print("Coordinates");

  char OBJ_NAME[50];
  char OBJ_NAME_3[50];
  
  // path
  if (!IS_NIGHTMODE) {
    safeStringCopy(OBJ_NAME, "UI/day/bg_coord.bin", sizeof(OBJ_NAME));
    safeStringCopy(OBJ_NAME_3, "UI/day/btn_back.bin", sizeof(OBJ_NAME_3));
  } else {
    safeStringCopy(OBJ_NAME, "UI/night/bg_coord.bin", sizeof(OBJ_NAME));
    safeStringCopy(OBJ_NAME_3, "UI/night/btn_back.bin", sizeof(OBJ_NAME_3));
  }

  if (SD.open(OBJ_NAME_3)/* && (IS_TRACKING)*/)
  {
    drawBin(OBJ_NAME_3, 245, 0, 75, 30);
  }

  if (SD.open(OBJ_NAME)/* && (IS_TRACKING)*/)
  {
    drawBin(OBJ_NAME, 0, 30, 320, 450);
  }
}


///////////////////////////////////////////////////// Load Screen Graphics ///////////////////////////////////////////////////////

void drawLoadScreen() {
  CURRENT_SCREEN = 6;
  char obj_name[50];
  char OBJ_NAME[50];
  char OBJ_NAME_2[50];
  char OBJ_NAME_3[50];
  char OBJ_NAME_4[50];
  
  if (!IS_NIGHTMODE) {
    safeStringCopy(OBJ_NAME, "UI/day/", sizeof(OBJ_NAME));
    safeStringCopy(OBJ_NAME_2, "UI/day/btn_next_prev.bin", sizeof(OBJ_NAME_2));
    safeStringCopy(OBJ_NAME_3, "UI/day/btn_back.bin", sizeof(OBJ_NAME_3));
    safeStringCopy(OBJ_NAME_4, "UI/day/load_background.bin", sizeof(OBJ_NAME_4));
  } else {
    safeStringCopy(OBJ_NAME, "UI/night/", sizeof(OBJ_NAME));
    safeStringCopy(OBJ_NAME_2, "UI/night/btn_next_prev.bin", sizeof(OBJ_NAME_2));
    safeStringCopy(OBJ_NAME_3, "UI/night/btn_back.bin", sizeof(OBJ_NAME_3));
    safeStringCopy(OBJ_NAME_4, "UI/night/load_background.bin", sizeof(OBJ_NAME_4));
  }
  
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 400, 30, title_bg);

  // Add JG
  if (!IS_NIGHTMODE) {
    drawBin("UI/day/btn_prev.bin", 0, 0, 75, 30);
    drawBin("UI/day/btn_next.bin", 122, 0, 75, 30);
  } else {
    drawBin("UI/night/btn_prev.bin", 0, 0, 75, 30);
    drawBin("UI/night/btn_next.bin", 122, 0, 75, 30);
  }
  // End add JG
  
  if (SD.open(OBJ_NAME_4)) {
    drawBin(OBJ_NAME_4, 0, 30, 320, 450);
  }

  // build path load button
  char load_button_path[50];
  safeStringCopy(load_button_path, OBJ_NAME, sizeof(load_button_path));
  
  // add file name in LOAD_SELECTOR
  const char* button_files[] = {
    "btn_load_0.bin", "btn_load_1.bin", "btn_load_2.bin", "btn_load_3.bin", "btn_load_4.bin",
    "btn_load_0b.bin", "btn_load_1b.bin", "btn_load_2b.bin", "btn_load_3b.bin", "btn_load_4b.bin",
    "btn_load_0c.bin", "btn_load_1c.bin", "btn_load_2c.bin", "btn_load_3c.bin", "btn_load_4c.bin"
  };
  
  if (LOAD_SELECTOR >= 0 && LOAD_SELECTOR < 15) {
    strncat(load_button_path, button_files[LOAD_SELECTOR], 
            sizeof(load_button_path) - strlen(load_button_path) - 1);
  }

  if (SD.open(OBJ_NAME_3)) {
    drawBin(OBJ_NAME_3, 245, 0, 75, 30);
  }

  if (SD.open(load_button_path)) {
    drawBin(load_button_path, 0, 60, 320, 80);
  }

  if (SD.open(OBJ_NAME_2)) {
    drawBin(OBJ_NAME_2, 0, 434, 320, 46);
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

char currentTime[10], startTime[10];
safeStringCopy(currentTime, rtc.getTimeStr(), sizeof(currentTime));
safeStringCopy(startTime, START_TIME, sizeof(startTime));

char* currPtr = strtok(currentTime, ":");
char* startPtr = strtok(startTime, ":");

int currMin = (atoi(currPtr) * 60) + atoi(strtok(NULL, ":"));
int startMin = (atoi(startPtr) * 60) + atoi(strtok(NULL, ":"));

st = currMin - startMin;
if (st < 0) {
    st += 1440;
}

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
    char* tokens[8];
    char* ptr = strtok(ObservedObjects[i], ";");
    int tokenCount = 0;
    
    while (ptr != NULL && tokenCount < 8) {
        tokens[tokenCount++] = ptr;
        ptr = strtok(NULL, ";");
    }
    
    if (tokenCount >= 8) {
        int tt;
        if (i == (Observed_Obj_Count - 1)) {
            char timePart[6];
            safeStringCopy(timePart, tokens[2], sizeof(timePart));
            char* timePtr = strtok(timePart, ":");
            int objMin = (atoi(timePtr) * 60) + atoi(strtok(NULL, ":"));
            
            char currentTime[10];
            safeStringCopy(currentTime, rtc.getTimeStr(), sizeof(currentTime));
            char* currPtr = strtok(currentTime, ":");
            int currMin = (atoi(currPtr) * 60) + atoi(strtok(NULL, ":"));
            
            tt = currMin - objMin;
            if (tt < 0) tt += 1440;
        } else {
            tt = atoi(tokens[7]);
        }
        
        char composed[500];
        snprintf(composed, sizeof(composed), "%s @%s for %dmin | Alt: %sdeg\n",
                tokens[0], tokens[2], tt, tokens[6]);
        tft.print(composed);
    }
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
  char obj_name[50];
  char OBJ_NAME[50];
  
  if (!IS_NIGHTMODE) {
    safeStringCopy(OBJ_NAME, "UI/day/", sizeof(OBJ_NAME));
  } else {
    safeStringCopy(OBJ_NAME, "UI/night/", sizeof(OBJ_NAME));
  }

  if (Msg == 1) {
    strncat(OBJ_NAME, "msg_1.bin", sizeof(OBJ_NAME) - strlen(OBJ_NAME) - 1);
  } else if (Msg == 3) {
    strncat(OBJ_NAME, "msg_3.bin", sizeof(OBJ_NAME) - strlen(OBJ_NAME) - 1);
  } else if (Msg == 5) {
    strncat(OBJ_NAME, "msg_5.bin", sizeof(OBJ_NAME) - strlen(OBJ_NAME) - 1);
    if (IS_SOUND_ON) {
      SoundOn(note_C, 32);
      delay(200);
      SoundOn(note_C, 32);
      delay(200);
      SoundOn(note_C, 32);
      delay(1000);
    }
  } else if (Msg == 6) {
    strncat(OBJ_NAME, "msg_6.bin", sizeof(OBJ_NAME) - strlen(OBJ_NAME) - 1);
  } else if (Msg == 7) {
    strncat(OBJ_NAME, "msg_7.bin", sizeof(OBJ_NAME) - strlen(OBJ_NAME) - 1);
  }

  if (SD.open(OBJ_NAME)) {
    drawBin(OBJ_NAME, 66.5, 190, 187, 100);
  }

  char m1[20], m2[30], m3[20];
  tft.setTextColor(MsgBox_t);

  if (Msg == 2) {
    safeStringCopy(m1, "WARNING", sizeof(m1));
    safeStringCopy(m2, "TRACKING was", sizeof(m2));
    safeStringCopy(m3, "Turned Off!", sizeof(m3));
    
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
    safeStringCopy(m1, "PICK AGAIN", sizeof(m1));
    
    char alt_str[10], ra_str[10];
    snprintf(alt_str, sizeof(alt_str), "%d", (int)ALT);
    snprintf(ra_str, sizeof(ra_str), "%d", (int)OBJECT_RA_H);
    
    snprintf(m2, sizeof(m2), "ALT=%s", alt_str);
    snprintf(m3, sizeof(m3), "RA= %s", ra_str);
    
    tft.cursorToXY(95, 230);
    tft.setTextScale(3);
    tft.println(m1);
    tft.cursorToXY(95, 265);
    tft.setTextScale(2);
    tft.print(m2);
    tft.cursorToXY(95, 290);
    tft.print(m3);
  } else if (Msg == 8) {
    safeStringCopy(m1, "Connected", sizeof(m1));
    safeStringCopy(m2, "TO", sizeof(m2));
    safeStringCopy(m3, "PHD2:)", sizeof(m3));
    
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
    safeStringCopy(m1, "Disconnected", sizeof(m1));
    safeStringCopy(m2, "FROM", sizeof(m2));
    safeStringCopy(m3, "PHD2:)", sizeof(m3));
    
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
    } else if (CURRENT_SCREEN == 14) {
      drawAutoGuidingScreen();
    } else if (CURRENT_SCREEN == SCREEN_LOAD_MENU) {
      drawLoadMenu();
    } else if (CURRENT_SCREEN == SCREEN_SEARCH_OBJECT) {
      drawSearchScreen();
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
    } else {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/btn_track_off.bin", 222, 375, 90, 44);
      } else {
        drawBin("UI/night/btn_track_off.bin", 222, 375, 90, 44);
      }
    }
    
// Display tracking status for menu 0
    displayTrackingStatus();
    
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
    
// Display tracking status for menu 1
    displayTrackingStatus();
    
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

char PIC_StarMap[50];

if (IS_NIGHTMODE) {
    safeStringCopy(PIC_StarMap, "starmap/night/", 50);
} else {
    safeStringCopy(PIC_StarMap, "starmap/day/", 50);
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

// Build the complete filename
snprintf(My_Map, sizeof(My_Map), "%s%d-%d.bin", PIC_StarMap, map_r, map_c);
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

if (strcmp(TFT_Time, "AL-ON") == 0) {
    tft.cursorToXY(47, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("AL-ON");
}

if (strcmp(TFT_Time, "30 s") == 0) {
    tft.cursorToXY(49, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("30 S");
}

if (strcmp(TFT_Time, "60 s") == 0) {
    tft.cursorToXY(49, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("60 S");
}

if (strcmp(TFT_Time, "2 min") == 0) {
    tft.cursorToXY(49, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("2 M");
}

if (strcmp(TFT_Time, "5 min") == 0) {
    tft.cursorToXY(49, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("5 M");
}

if (strcmp(TFT_Time, "10 min") == 0) {
    tft.cursorToXY(48, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("10 M");
}

if (strcmp(Tracking_Mode, "Celest") == 0) {
    tft.cursorToXY(88, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("Celest");
  }

if (strcmp(Tracking_Mode, "Lunar") == 0) {  
    tft.cursorToXY(88, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("Lunar");
  }

if (strcmp(Tracking_Mode, "Solar") == 0) {  
    tft.cursorToXY(88, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("Solar");
  } // modif JG was copied paste line here that make a graphic bug on mer flip status

if (strcmp(Mer_Flip_State, "AUTO") == 0) {
    tft.cursorToXY(141, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("AUTO");
  }

if (strcmp(Mer_Flip_State, "OFF") == 0) {    
    //tft.fillRect(140, 14, 30, 10, title_bg);
    tft.cursorToXY(141, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("OFF");
  }

if (strcmp(Fan1_State, "ON") == 0) {
    tft.cursorToXY(180, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("ON");
}

if (strcmp(Fan1_State, "OFF") == 0) {
    tft.cursorToXY(180, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("OFF");
}

if (strcmp(GTMEM_State, "ARM") == 0) { // Add JG mem position ARM / Free
    tft.cursorToXY(214, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("ARM");
}

if (strcmp(GTMEM_State, "Free") == 0) {
    tft.cursorToXY(214, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("Free");
}

  if (strcmp(Sound_State, "ON") == 0) {
    tft.cursorToXY(252, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("ON");
  }

  if (strcmp(Sound_State, "OFF") == 0) {  
    tft.cursorToXY(252, 18);
    tft.setTextColor(Button_State_OFF);
    tft.print("OFF");
  }

  if (strcmp(Stepper_State, "ON") == 0) {  
    tft.cursorToXY(292, 18);
    tft.setTextColor(Button_State_ON);
    tft.print("ON");
  }

  if (strcmp(Stepper_State, "OFF") == 0) {  
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

  // delete all triangle and percentage zone
  tft.fillRect(5, 50, 250, 60, BLACK); // delete a large zone

  // draw triangle
  tft.fillTriangle(5, 100, 253, 100, 253, 50, BLACK);

  // draw brightness triangle
  if (TFT_Brightness > 0) {
    int x_br = 5 + (TFT_Brightness * 250 / 255);
    int tria_h = 50 + (50 * (255 - TFT_Brightness) / 255);
    tft.fillTriangle(5, 100, x_br, 100, x_br, tria_h, Bright_triangle);
  }

  // display percentage
  int bright_perc = (TFT_Brightness * 100 / 255);
  
  #ifdef serial_debug
  Serial.print("Brightness: ");
  Serial.print(TFT_Brightness);
  Serial.print(" (");
  Serial.print(bright_perc);
  Serial.println("%)");
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
if (ALLIGN_TYPE == 3) {
    int kk = STARS_PAGER * 24;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            // Check if star exists
            if (Iter_Stars[kk][0] == '\0') {
                break;
            }
            
            // find separators
            char* starData = Iter_Stars[kk];
            char* i1 = strchr(starData, ';');
            char* i2 = strchr(i1 + 1, ';');
            
            if (i1 == NULL || i2 == NULL) {
                kk += 1;
                continue;
            }
            
            // extract S_NAME and C_NAME
            char S_NAME[50];
            char C_NAME[50];
            
            // C_NAME (from start to the first ;)
            size_t c_name_len = i1 - starData;
            strncpy(C_NAME, starData, c_name_len);
            C_NAME[c_name_len] = '\0';
            
            // S_NAME (between the first and second ;)
            size_t s_name_len = i2 - i1 - 1;
            strncpy(S_NAME, i1 + 1, s_name_len);
            S_NAME[s_name_len] = '\0';
            
            // check if S_NAME is empty
            if (S_NAME[0] == '\0') {
                break;
            }
            
            // display
            tft.fillRect(((j * 75) + 12), ((i * 50) + 85), 71, 45, messie_btn);
            
            // central position computing
            int l = (strlen(S_NAME) / 2) * 6;
            tft.cursorToXY(((j * 75) + (44 - l)), ((i * 50) + 93));
            tft.setTextScale(1);
            tft.print(S_NAME);
            
            tft.setTextScale(2);
            tft.cursorToXY(((j * 75) + 29), ((i * 50) + 110));
            tft.print(C_NAME);
            
            kk += 1;
            
            // check if we overflow the array
            if (kk >= 50) {
                break;
            }
        }
    }
}
else
{
    int kk = STARS_PAGER * 24;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            const char* tempStar = Stars[kk];
            char starData[100];
            safeStringCopy(starData, tempStar, sizeof(starData));
            
            // find separators
            char* i1 = strchr(starData, ';');
            if (!i1) break;
            
            char* i2 = strchr(i1 + 1, ';');
            if (!i2) break;
            
            // Extract S_NAME et C_NAME
            char S_NAME[50], C_NAME[50];
            
            // S_NAME (between i1 and i2)
            strncpy(S_NAME, i1 + 1, i2 - i1 - 1);
            S_NAME[i2 - i1 - 1] = '\0';
            
            // C_NAME (start until i1)
            strncpy(C_NAME, starData, i1 - starData);
            C_NAME[i1 - starData] = '\0';
            
            if (strlen(S_NAME) == 0) {
                break;
            }
            
            tft.fillRect(((j * 75) + 12), ((i * 50) + 85), 71, 45, messie_btn);
            int l = (strlen(S_NAME) / 2) * 6;
            tft.cursorToXY(((j * 75) + (44 - l)), ((i * 50) + 93));
            tft.setTextScale(1);
            tft.print(S_NAME);
            tft.setTextScale(2);
            tft.cursorToXY(((j * 75) + 29), ((i * 50) + 110));
            tft.print(C_NAME);
            kk += 1;
            
            if (kk >= 203) break; // overflow protection
        }
    }
}
}

///////////////////////////////////////////////////// Draw Load Objects Function ///////////////////////////////////////////////////////

void drawLoadObjects() {
  if (LOAD_SELECTOR >= 2 && LOAD_SELECTOR <= 4 || LOAD_SELECTOR >= 6 && LOAD_SELECTOR <= 9 || LOAD_SELECTOR >= 11 && LOAD_SELECTOR <= 14) {
    // Add JG local load csv file
    char in_char;
    char items[256] = "";  // Buffer for a line
    int items_index = 0;
    int k = 0;
    int l = 0;
    
    File dataFile = SD.open(CAT_NAME);
    
    while (dataFile.available()) {
        in_char = dataFile.read();
        
        if (items_index < sizeof(items) - 1) {
            items[items_index++] = in_char;
        }
        
        l = l + 1;
        if (in_char == '\n') {
            items[items_index] = '\0';
            safeStringCopy(OBJ_Array[k], items, sizeof(OBJ_Array[k]));
            k = k + 1;
            items_index = 0;
            items[0] = '\0';
        }
    }

    if (k < 240) { 
        for (int i = 0; i < (240 - k); i++) { 
            OBJ_Array[k + i][0] = '\0';
        }
    }

    dataFile.close();
    items_index = 0;
    items[0] = '\0';
    l = 0;
    // End add JG local load csv file
    
    tft.setTextScale(1);
    int ll = OBJ_PAGER * 20;
    if (OBJ_PAGER >= 12) {ll = (OBJ_PAGER - 12) * 20;} // Add JG for second 240 item NGC catalog

for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 4; j++) {
        if (OBJ_Array[ll][0] == '\0') {
            tft.fillRect(((j * 75) + 12), ((i * 50) + 160), 71, 45, BLACK);
            ll++;
            continue;
        }
        
        // Find separators
        char* i1 = strchr(OBJ_Array[ll], ';');
        char* i2 = (i1 != NULL) ? strchr(i1 + 1, ';') : NULL;
        char* i3 = (i2 != NULL) ? strchr(i2 + 1, ';') : NULL;
        char* i4 = (i3 != NULL) ? strchr(i3 + 1, ';') : NULL;
        char* i5 = (i4 != NULL) ? strchr(i4 + 1, ';') : NULL;
        char* i6 = (i5 != NULL) ? strchr(i5 + 1, ';') : NULL;
        
        // Extract parts
        char M_NAME[50] = "", C_NAME[50] = "", MAG_NAME[50] = "";
        
        // M_NAME (from start until the first ;) - Column A
        if (i1 != NULL) {
            strncpy(M_NAME, OBJ_Array[ll], i1 - OBJ_Array[ll]);
            M_NAME[i1 - OBJ_Array[ll]] = '\0';
        }
        
        // C_NAME (constellation) - Column D (after 3rd separator)
        if (i3 != NULL) {
            if (i4 != NULL) {
                // complete format : between i3 and i4
                strncpy(C_NAME, i3 + 1, i4 - i3 - 1);
                C_NAME[i4 - i3 - 1] = '\0';
            } else {
                // bright star format : after i3 until the end
                safeStringCopy(C_NAME, i3 + 1, sizeof(C_NAME));
            }
        }
        
        // MAG_NAME (magnitude) - Column F (between i5 and i6) - for LOAD_SELECTOR > 2
        if (LOAD_SELECTOR > 2 && i5 != NULL && i6 != NULL) {
            strncpy(MAG_NAME, i5 + 1, i6 - i5 - 1);
            MAG_NAME[i6 - i5 - 1] = '\0';
        }
        
        DrawButton(((j * 75) + 12), ((i * 50) + 160), 71, 45, M_NAME, messie_btn, 0, l_text, 1, true);
        
        tft.setTextScale(1);
        tft.cursorToXY(((j * 75) + 15), ((i * 50) + 162));
        tft.print(C_NAME); // Constellation
        
        if (LOAD_SELECTOR > 2 && MAG_NAME[0] != '\0') {
            tft.cursorToXY(((j * 75) + 15), ((i * 50) + 192));
            tft.print(MAG_NAME); // Magnitude
        }
        
        ll++;
    }
}
  }
  else if (LOAD_SELECTOR == 1)  // solar system planets
  {
    tft.setTextScale(1);
    int ll = 0;

    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 4; j++) {
        if (ll >= 11) {
          tft.fillRect(((j * 75) + 12), ((i * 50) + 160), 71, 45, BLACK);
        } else {
          const char* M_NAME = ss_planet_names[ll];
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
}

  /////////////////////////////////// Touch Calibration Graphics ////////////////////////////////

void drawScreenCalibration()
  {
  CURRENT_SCREEN = 9;
  drawBin("UI/day/ScreenCalib.bin", 0, 0, 320, 480); 
  } 

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
