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
//  - Handles ALL TOUCH SCREEN Inputs;
//  - Screens are separated like:
//    * CURRENT_SCREEN==0  - drawGPSScreen() Where the GPS coordinates are displayed
//    * CURRENT_SCREEN==1  - drawClockScreen() Captures updates on the time and date
//    * CURRENT_SCREEN==2  -                - not used
//    * CURRENT_SCREEN==3  - Select Alignment method (only have 3 buttons)
//    * CURRENT_SCREEN==4  - drawMainScreen() Captures all clicks on the MAIN Screen of the application
//    * CURRENT_SCREEN==5  - drawCoordinatesScreen() Only have "back" button
//    * CURRENT_SCREEN==6  - drawLoadScreen() Captures input on Load screen (all of them: Messier && Treasurres)
//    * CURRENT_SCREEN==7  -                - not used
//    * CURRENT_SCREEN==8  -                - not used
//    * CURRENT_SCREEN==9  - drawScreenCalibration() Touch Screen calibration
//    * CURRENT_SCREEN==10  - drawSTATScreen()
//    * CURRENT_SCREEN==11  -               - not used
//    * CURRENT_SCREEN==12  - drawStarSyncScreen() - To Select Alignment Star;
//    * CURRENT_SCREEN==13  - drawConstelationScreen(int indx) - to actually align on Star. Called few times per alignment procedure.
//    * CURRENT_SCREEN==14  - drawAutoGuidingScreen();
//    * CURRENT_SCREEN==15  - confirm_sun_track()
//
//  - Please note, that Touches are separated in 2 section to capture OnPress && OnRelease!
//    You will notice that "if (lx > 0 && ly > 0 )" this part defines OnPress activities.

      int counttl = 0; // count number of touch on top left corner, stop when 5 touch are completed
      int counttr = 0; // same for top right
      int countbl = 0; // bottom left
      int countbr = 0; // bottom right
      float topleftcalx = 0; // Store touch value for 5 touch, then do the mean. (x value for top left corner)
      float toprightcalx = 0; // same for top right
      float botleftcalx = 0; // bottom left
      float botrightcalx = 0; // bottom right
      float topleftcaly = 0; // Store touch value for 5 touch, then do the mean. (y value for top left corner)
      float toprightcaly = 0; // same for top right
      float botleftcaly = 0; // bottom left
      float botrightcaly = 0; // bottom righ
      
      

void considerTouchInput(int lx, int ly)
{
  //**************************************************************
  //
  //      BUTTON DOWN Events start here
  //
  //      - only executed when the user touches the screen - PRESS
  //**************************************************************

// add JG

lx = (lx-interx)/slopex; // lx and ly correction after calibration
ly = (ly-intery)/slopey;

// End add JG 

  if (lx > 0 && ly > 0 )
  {
    if (lx >= 0 && ly >= 0 ) // Sound on any Screen touch
    {
      if (IS_SOUND_ON) {
        SoundOn(800, 8);
      }
    }
    // Make sure you WakeUp the TFT
    // in case the lid is OFF

    if (IS_TFT_ON == false)
    {
      analogWrite(TFTBright, TFT_Brightness);
      IS_TFT_ON = true;
      TFT_Timer = millis();
      delay(200);
      return;
    }
    TFT_Timer = millis();
    if (CURRENT_SCREEN == 0)
    { // captures touches on drawGPSScreen()
      if (lx > 60 && lx < 260 && ly > 400 && ly < 475)
      {
        last_button = 1;
      }
    }


// add JG
   
 ////////////////////////////////////////////////// Touch Screen calibration Touch Actions ///////////////////////////////////////////////////////

 // Calibration tool for touchscreen. The touch area didn't match the display area for my screen. Display is x=0, y=0 to x=320, y=480. Touch area was x=18, y=58 to x=305, y=478. There was a bigger shift on the top of the screen for y.
 // By this fact, touch area is smaller in term of points than the display area, so we have to make a linear regression to make the correction. 
 // For some buttons on center or top of the screen, the touch area was shifted to the top, and the very top button "back" was very difficult to press, because looking for ly 10 to 60 and the value for first top pixels was 58.
 // In this code, I made loops to sampling the 4 corners several times to get the minimum and maximum recorded values for each corner. These values are recorded and used to make a linear regression.
 // This linear regression correct the lx and ly values to return new lx and ly that match with screen resolution like those already defined in this code page. After correction, in my case ly=58 become 0 and ly=478 become 480. Lx = 18 become 0 and lx = 305 become 320.
 // I ignore the shifting on other adafruit screens so I made a 150*150 windows on each corner to be sure to catch right values, but user should touch closest to the edge of the screen, using a touchscreen stylus. Program will automatically record min and max values and ignore other ones.
 // You can "tickle" each corner until a green square appear, meaning that values are recorded for this corner.
 // Calibrate your screen once for all times and write values, use of sharp object as pencils on resistive touchscreen can damage resistive layer. 
 // Calibration lines values are displayed at the end of calibration and stored in the 4 last values of OPTIONS.TXT in the SD card. So you can write it to restore later by altering the .txt file.
 // If you want to start the calibration again, and so as not to overcorrect, I put a button to reset the values ​​to zero. 
 // If after calibration the touch part of the screen no longuer work, something has gone wrong. Don't panic, in this case, edit the Options.txt file and replace the last 4 values ​​with 1,1,0,0, This resets the values ​​to zero.

    else if (CURRENT_SCREEN == 9) {
      if (lx > 0 && lx < 150 && ly > 0 && ly < 150 && counttl < 6) { // detect touch on top left corner and create min-max sampling value
      if (counttl == 0) {topleftcalx = lx; topleftcaly = ly;}
      if (topleftcalx > lx) {topleftcalx = lx;}
      if (topleftcaly > ly) {topleftcaly = ly;}
      counttl += 1;
      if (counttl >5) {
      tft.fillRect(0, 0, 15, 15, GREEN);
      }}
      
      if (lx > 170 && lx < 320 && ly > 0 && ly < 150 && counttr < 6) { // detect touch on top right corner and create min-max  sampling value
      if (counttr == 0) {toprightcalx = lx; toprightcaly = ly;}
      if (toprightcalx < lx) {toprightcalx = lx;}
      if (toprightcaly > ly) {toprightcaly = ly;}
      counttr += 1;
      if (counttr >5) {
      tft.fillRect(305, 0, 320, 15, GREEN);
      }}
    
      if (lx > 0 && lx < 150 && ly > 330 && ly < 480 && countbl < 6) { // detect touch on bottom left corner and create min-max  sampling value
      if (countbl == 0) {botleftcalx = lx; botleftcaly = ly;}
      if (botleftcalx > lx) {botleftcalx = lx;}
      if (botleftcaly < ly) {botleftcaly = ly;}
      countbl += 1;
      if (countbl >5) {
      tft.fillRect(0, 465, 15, 480, GREEN);
      }}
      
      if (lx > 170 && lx < 320 && ly > 330 && ly < 480 && countbr < 6) { // detect touch on bottom right corner and create min-max  sampling value
      if (countbr == 0) {botrightcalx = lx; botrightcaly = ly;}
      if (botrightcalx < lx) {botrightcalx = lx;}
      if (botrightcaly < ly) {botrightcaly = ly;}
      countbr += 1;
      if (countbr >5) {
      tft.fillRect(305, 465, 320, 480, GREEN);
      }}
      
      if (counttl >5 && counttr >5 && countbl >5 && countbr >5) { // the four corners are sampled
      tft.setTextScale(2);
      tft.cursorToXY(30, 380);
      tft.print("CALIBRATION COMPLETED");
      tft.cursorToXY(27, 405);
      tft.setTextScale(2);
      tft.print("Data stored in SD card");
      
      // Linear regression : calculate slope
      // x values :
      
      slopex = (((toprightcalx + botrightcalx) / 2 ) - ((topleftcalx + botleftcalx) / 2 )) / 320;

      // y values :
      
      slopey = (((botleftcaly + botrightcaly) / 2 ) - ((topleftcaly + toprightcaly) / 2 )) / 480;
      
      // caclulate intercept
      // x values :

      interx = ((toprightcalx + botrightcalx) / 2 ) - (slopex*320);
      
      // y values :

      intery = ((botleftcaly + botrightcaly) / 2 ) - (slopey*480);

      tft.cursorToXY(27, 430);
      tft.setTextScale(1);
      tft.print("X Slope : "+String(slopex)+"   X Shift : "+String(interx));
      tft.cursorToXY(27, 440);
      tft.setTextScale(1);
      tft.print("Y Slope : "+String(slopey)+"   Y Shift : "+String(intery));
      storeOptions_SD();
      }
      if (interx != 0.00 && intery != 0.00) { // calibration are already done
      DrawButton(60, 270, 220, 50, "Reset Values", 0, btn_d_border, btn_d_border, 2, false); // display button to reset values to zero
        if (lx > 60 && lx < 280 && ly > 270 && ly < 320) { // button pressed, reset values
        slopex = 1; // reset values to no correction
        slopey = 1; 
        interx = 0; 
        intery = 0; 
        counttl = 0; // count number of touch on top left corner, stop when 5 touch are completed
        counttr = 0; // same for top right
        countbl = 0; // bottom left
        countbr = 0; // bottom right
        topleftcalx = 0; // Store touch value for 5 touch, then do the mean. (x value for top left corner)
        toprightcalx = 0; // same for top right
        botleftcalx = 0; // bottom left
        botrightcalx = 0; // bottom right
        topleftcaly = 0; // Store touch value for 5 touch, then do the mean. (y value for top left corner)
        toprightcaly = 0; // same for top right
        botleftcaly = 0; // bottom left
        botrightcaly = 0; // bottom right
        storeOptions_SD();
        drawBin("UI/day/ScreenCalib.bin", 0, 0, 320, 480);
        tft.cursorToXY(27, 405);
        tft.setTextScale(2);
        tft.print("Data stored in SD card");
        tft.cursorToXY(27, 430);
        tft.setTextScale(1);
        tft.print("X Slope : "+String(slopex)+"   X Shift : "+String(interx));
        tft.cursorToXY(27, 440);
        tft.setTextScale(1);
        tft.print("Y Slope : "+String(slopey)+"   Y Shift : "+String(intery));  
      }}
      if (lx > 100 && lx < 205 && ly > 40 && ly < 90) { 
      // press Back button
      tft.fillRect(0, 0, 320, 480, BLACK);
      drawMainScreen();
      }     
    }
    
// fin add JG    
    
    ////////////////////////////////////////////////// Clock Screen Touch Actions ///////////////////////////////////////////////////////
    else if (CURRENT_SCREEN == 1)
    { // captures touches on drawClockScreen()
      if (lx > 220 && lx < 320 && ly > 405 && ly < 480)
      {
        // BTN OK pressed
        int changes = 0;
        for (int y = 0; y < 12; y++)
        {
          if (w_DateTime[y] != 0)
          {
            changes = 1;
          }
        }
        if (changes == 1)
        {
          // Do the magic as the date and time has been updated... Update the RTC accordingly
          int hh = (w_DateTime[8] * 10)   + w_DateTime[9];
          int mm = (w_DateTime[10] * 10)  + w_DateTime[11];
          int dd = (w_DateTime[0] * 10)   + w_DateTime[1];
          int mo = (w_DateTime[2] * 10)   + w_DateTime[3];
          int yy = (w_DateTime[4] * 1000) + (w_DateTime[5] * 100) + (w_DateTime[6] * 10) + w_DateTime[7];
          rtc.setTime(hh, mm, 00);
          rtc.setDate(dd, mo, yy);
        }
        int ora, date_delay = 0; //, month_delay = 0, year_delay = 0;
        int time_delay = TIME_ZONE;
        time_delay += Summer_Time;

        char touch_dateBuffer[20], touch_timeBuffer[20];
        safeStringCopy(touch_dateBuffer, rtc.getDateStr(), sizeof(touch_dateBuffer));
        safeStringCopy(touch_timeBuffer, rtc.getTimeStr(), sizeof(touch_timeBuffer));
        
        // Extract date parts
        char dayPart[3], monthPart[3], yearPart[5];
        strncpy(dayPart, touch_dateBuffer, 2);
        dayPart[2] = '\0';
        strncpy(monthPart, touch_dateBuffer + 3, 2);
        monthPart[2] = '\0';
        strcpy(yearPart, touch_dateBuffer + 6);
        
        // Extract hour parts
        char hourPart[3], minutePart[3], secondPart[3];
        strncpy(hourPart, touch_timeBuffer, 2);
        hourPart[2] = '\0';
        strncpy(minutePart, touch_timeBuffer + 3, 2);
        minutePart[2] = '\0';
        strcpy(secondPart, touch_timeBuffer + 6);
        
        int D = atoi(dayPart);
        int M = atoi(monthPart);
        int Y = atoi(yearPart);
        int H = atoi(hourPart);
        int MN = atoi(minutePart);
        int S = atoi(secondPart);

        //update the value of the variable ora
        //ora = hh - time_delay;
        ora = H - time_delay;
        //to update the real time
        if (ora < 0) // In all these calculations it is not necessary to correct the month or year (although it may work)
        {
          ora += 24;
          date_delay = -1;
          /*if (dd + date_delay < 1)
            {
            dd = (mo==2 || mo==4 || mo==6 || mo==8 || mo==9 || mo==11 || mo==1? 31: (mo!=3? 30: (yy%4==0 and yy%100!=0 or yy%400==0? 29: 28)));
            date_delay = 0;
            month_delay = -1;
            if (mo + month_delay < 1)
            {
              mo = 12;
              month_delay = 0;
              year_delay = -1;
            }
            }*/
        }
        // convert to epoch
        //setTime(ora, mm, 00, dd + date_delay, mo + month_delay, yy + year_delay); // <-- too mantain sync with UTC
        //setTime(ora, mm, 00, dd + date_delay, mo, yy); // <-- too mantain sync with UTC
        setTime(ora, MN, S, D + date_delay, M, Y); // <-- too mantain sync with UTC

        //Start_date = String(rtc.getDateStr()).substring(0, 2) + " " + rtc.getMonthStr(FORMAT_SHORT) + " " + String(rtc.getDateStr()).substring(6);

        // Retrieve the parts of the date
        const char* dateStr = rtc.getDateStr();
        const char* monthStr = rtc.getMonthStr(FORMAT_SHORT);

        // build date
        snprintf(Start_date, sizeof(Start_date), "%.2s %s %.2s", 
        dateStr, monthStr, dateStr + 6);

        safeStringCopy(old_d, rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/'), sizeof(old_d));

        char timeBuffer[10];
        safeStringCopy(timeBuffer, rtc.getTimeStr(FORMAT_SHORT), sizeof(timeBuffer));
        safeStringCopy(START_TIME, timeBuffer, sizeof(START_TIME));

        _Stemp = dht.readTemperature();;
        _Shumid = dht.readHumidity();

        drawSelectAlignment();

      }
      else if (lx > 15 && lx < 95 && ly > 224 && ly < 274)
      {
        // BTN 1 pressed
        tft.drawRect(15, 224, 80, 50, title_bg);
        last_button = 1;
        removeTime_addXX();

      }
      else if (lx > 120 && lx < 200 && ly > 224 && ly < 274)
      {
        // BTN 2 pressed
        tft.drawRect(120, 224, 80, 50, title_bg);
        last_button = 2;
        removeTime_addXX();

      }
      else if (lx > 230 && lx < 280 && ly > 224 && ly < 274)
      {
        // BTN 3 pressed
        tft.drawRect(230, 224, 80, 50, title_bg);
        last_button = 3;
        removeTime_addXX();

      }
      else if (lx > 15 && lx < 95 && ly > 285 && ly < 335)
      {
        // BTN 4 pressed
        tft.drawRect(15, 285, 80, 50, title_bg);
        last_button = 4;
        removeTime_addXX();

      }
      else if (lx > 120 && lx < 200 && ly > 285 && ly < 335)
      {
        // BTN 5 pressed
        tft.drawRect(120, 285, 80, 50, title_bg);
        last_button = 5;
        removeTime_addXX();

      }
      else if (lx > 230 && lx < 280 && ly > 285 && ly < 335)
      {
        // BTN 6 pressed
        tft.drawRect(230, 285, 80, 50, title_bg);
        last_button = 6;
        removeTime_addXX();

      }
      else if (lx > 15 && lx < 95 && ly > 346 && ly < 396)
      {
        // BTN 7 pressed
        tft.drawRect(15, 346, 80, 50, title_bg);
        last_button = 7;
        removeTime_addXX();

      }
      else if (lx > 120 && lx < 200 && ly > 346 && ly < 396)
      {
        // BTN 8 pressed
        tft.drawRect(120, 346, 80, 50, title_bg);
        last_button = 8;
        removeTime_addXX();

      }
      else if (lx > 230 && lx < 280 && ly > 346 && ly < 396)
      {
        // BTN 9 pressed
        tft.drawRect(230, 346, 80, 50, title_bg);
        last_button = 9;
        removeTime_addXX();

      }
      else if (lx > 120 && lx < 200 && ly > 408 && ly < 478)
      {
        // BTN 0 pressed
        tft.drawRect(120, 408, 80, 50, title_bg);
        last_button = 10;
        removeTime_addXX();

      }
      else if (lx > 30 && lx < 100 && ly > 405 && ly < 460)
      {
        // BTN SummerTime pressed
        last_button = 22;
      }
    }

    ////////////////////////////////////////////////// Select Alignment Screen Touch Actions ///////////////////////////////////////////////////////

else if (CURRENT_SCREEN == 3)
{
    if (lx > 40 && lx < 281 && ly > 89 && ly < 158)
    {
        // BTN "1 Star Alignment" pressed
        tft.drawRect(40, 89, 241, 69, btn_l_border);
        ALLIGN_TYPE = 1;
        drawStarSyncScreen();
    }
    else if (lx > 40 && lx < 281 && ly > 194 && ly < 262)
    {
        // BTN "Iterative Alignment" pressed
        tft.drawRect(40, 194, 241, 68, btn_l_border);
        ALLIGN_TYPE = 3;

        // I'll take some time to Initialize the Sub ARRAY with suitable stars (only for Northern Hemi)
        tft.cursorToXY(0, 5);
        int cc = 0;
        for (int t = 0; t < 203; t++)
        {
            const char* starData = Stars[t];
            
            // Find separators
            const char* i1 = strchr(starData, ';');
            if (!i1) continue;
            
            const char* i2 = strchr(i1 + 1, ';');
            if (!i2) continue;
            
            const char* i3 = strchr(i2 + 1, ';');
            if (!i3) continue;

            // Extract name and catalog
            char namePart[50], catalogPart[50];
            strncpy(namePart, i1 + 1, i2 - i1 - 1);
            namePart[i2 - i1 - 1] = '\0';
            strncpy(catalogPart, starData, i1 - starData);
            catalogPart[i1 - starData] = '\0';
            
            // build OBJECT_NAME
            snprintf(OBJECT_NAME, sizeof(OBJECT_NAME), "%s from %s", namePart, catalogPart);
            
            // Extract RA
            char OBJ_RA[20];
            strncpy(OBJ_RA, i2 + 1, i3 - i2 - 1);
            OBJ_RA[i3 - i2 - 1] = '\0';
            
            char* hPos = strchr(OBJ_RA, 'h');
            if (hPos) {
                *hPos = '\0';
                OBJECT_RA_H = atof(OBJ_RA);
                OBJECT_RA_M = atof(hPos + 1);
            }
            
            float ts_RA = OBJECT_RA_H + OBJECT_RA_M / 100;
            
            // Extract DEC
            char OBJ_DEC[20];
            strcpy(OBJ_DEC, i3 + 1);
            
            char sign = OBJ_DEC[0];
            char* degPos = strchr(OBJ_DEC, '°');
            
            if (degPos) {
                *degPos = '\0';
                OBJECT_DEC_D = atof(OBJ_DEC + 1);
                if (sign == '-') {
                    OBJECT_DEC_D *= (-1);
                }
            }
            OBJECT_DEC_M = 0;
            
            if (ts_RA > 5.3 && ts_RA < 23.3)
            {
                calculateLST_HA();
                if (AZ > 90 && AZ < 270 && ALT > 20)
                {
                    safeStringCopy(Iter_Stars[cc], Stars[t], sizeof(Iter_Stars[cc]));
                    cc += 1;
                    if (cc > 49)
                    {
                        break;
                    }
                }
            }
            int_star_count = floor(cc / 15) + 1;
        }
        delay(500);
        drawStarSyncScreen();
    }
    else if (lx > 67 && lx < 262 && ly > 414 && ly < 480)
    {
        // BTN "Skip Alignment" pressed
        tft.drawRect(67, 414, 195, 50, btn_l_border);
        IS_IN_OPERATION = true;
        drawMainScreen();
    }
}

    ////////////////////////////////////////////////// Main Screen Touch Actions ///////////////////////////////////////////////////////

else if (CURRENT_SCREEN == 4)
{ // capture touches on drawMainScreen()
  if (lx > 0 && lx < 90 && ly > 0 && ly < 100) {
    // Left Side Touched
    // Load the GPS screen to capture new data && correct time if needed on the next screen (Time Screen)
    CURRENT_SCREEN = 0;
    GPS_iterrations = 0;
    IS_IN_OPERATION = false;
    ALLIGN_STEP = 0;
    ALLIGN_TYPE = 0;
    drawGPSScreen();
  }

  if (lx > 250 && lx < 320 && ly > 0 && ly < 100)
  {
    // Right Side Touched
    drawOptionsScreen();
  }

  if (lx > 9 && lx < 99 && ly > 375 && ly < 419 && IS_BT_MODE_ON == false) {
    // BTN LOAD pressed - go directly to the new menu
    if (MAIN_SCREEN_MENU == 0) {
        // instead of putting last_button = 1, go directly to the new menu
        drawLoadMenu();
        last_button = 0; // Reset
    } else {
        last_button = 7; // Keep the behavior for the other menu mode
    }
  } else if (lx > 9 && lx < 99 && ly > 428 && ly <480  && IS_BT_MODE_ON == false) {
    // BTN 4 pressed
    if (MAIN_SCREEN_MENU == 0) {
      last_button = 4;
    } else {
      last_button = 10;
    }
  } else if (lx > 115 && lx < 205 && ly > 375 && ly < 419 && IS_BT_MODE_ON == false) {
    // BTN 2 pressed
    if (MAIN_SCREEN_MENU == 0) {
      last_button = 2;
    } else {
      last_button = 8;
    }
  } else if (lx > 115 && lx < 205 && ly > 428 && ly < 480) {
    // BTN 5 pressed
    if (MAIN_SCREEN_MENU == 0) {
      last_button = 5;
    } else {
      last_button = 11;
    }
  } else if (lx > 222 && lx < 320 && ly > 375 && ly < 420) {
    // BTN 3 pressed
    if (MAIN_SCREEN_MENU == 0) {
      last_button = 3;
    } else {
      last_button = 9;
    }
  } else if (lx > 222 && lx < 320 && ly > 428 && ly <480) {
    // BTN 6 pressed
    if (MAIN_SCREEN_MENU == 0) {
      last_button = 6;
    } else {
      last_button = 12;
    }
  }
}

/////////////////////////////////////////////// Load Menu Screen Touch Actions /////////////////////////////////////////////////////////
else if (CURRENT_SCREEN == SCREEN_LOAD_MENU) 
{
    // Load/Search/Back menu management
    if (lx > 60 && lx < 260) {
        if (ly > 150 && ly < 210) {
            // "Load Object" - go to the old loading screen
            LOAD_SELECTOR = 0;
            CAT_Page = 1;
            drawLoadScreen();
        }
        else if (ly > 230 && ly < 290) {
            // "Search Object" - go to the search screen
            safeStringCopy(searchQuery, "", sizeof(searchQuery));
            searchResultCount = 0;
            selectedResult = -1;
            currentSearchPage = 0;
            drawSearchScreen();
        }
        else if (ly > 310 && ly < 370) {
            // "Back" - return to main screen
            drawMainScreen();
        }
    }
}

/////////////////////////////////////////////// Search Object Screen Touch Actions /////////////////////////////////////////////////////////
else if (CURRENT_SCREEN == SCREEN_SEARCH_OBJECT) 
{
    // Button Back
    if (lx > 265 && lx < 320 && ly > 0 && ly < 45) {
        drawLoadMenu();
    }
    
    // Button Search
    else if (lx > 10 && lx < 130 && ly > 90 && ly < 125) {
        scrollOffset = 0;
        searchObject(searchQuery);
        drawSearchScreen();
    }
    
    // Button Clear
    else if (lx > 140 && lx < 260 && ly > 90 && ly < 125) {
        safeStringCopy(searchQuery, "", sizeof(searchQuery));
        searchResultCount = 0;
        selectedResult = -1;
        scrollOffset = 0;
        drawSearchScreen();
    }
    
    // Button GOTO
    else if (selectedResult >= 0 && lx > 200 && lx < 310 && ly > 430 && ly < 470) {
        gotoSelectedObject();
    }
    
    // Results area (avoid the scrollbar area)
    else if (lx > 10 && lx < 250 && ly >= 130 && ly <= 280) {
        int resultIndex = scrollOffset + (ly - 130) / 30;
        if (resultIndex < searchResultCount) {
            selectedResult = resultIndex;
            updateSearchResultsOnly();
        }
    }
    
// Scrollbar and navigation buttons - ZONE 260-320
    else if (lx > 280 && lx < 300) { // Scrollbar centered on 290 (280-300)
    if (ly >= 130 && ly <= 280) { // scrollbar cursor
        int scrollbarHeight = 150;
        int clickPos = ly - 130;
        scrollOffset = (clickPos * max(1, searchResultCount - 5)) / scrollbarHeight;
        scrollOffset = constrain(scrollOffset, 0, max(0, searchResultCount - 5));
        updateSearchResultsOnly();
    }
    else if (ly > 115 && ly < 130) { // up arrow
        if (scrollOffset > 0) {
            scrollOffset--;
            updateSearchResultsOnly();
        }
    }
    else if (ly > 280 && ly < 295) { // dwn arrow
        if (scrollOffset < max(0, searchResultCount - 5)) {
            scrollOffset++;
            updateSearchResultsOnly();
        }
    }
}
    
// Virtual keyboard
else if (ly >= 320 && ly <= 480) {
    int keyWidth = 32;
    int keyHeight = 40;
    int startX = 0;
    int startY = 320;
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 10; col++) {
            int x = startX + col * keyWidth;
            int y = startY + row * keyHeight;
            
            if (lx > x && lx < x + keyWidth &&        
                ly > y && ly < y + keyHeight - 2) {   
                
                char keyChar = keyboardLayout[row][col];
                
                if (keyChar == '<') {
                    int len = strlen(searchQuery);
                    if (len > 0) searchQuery[len - 1] = '\0';
                } else if (keyChar == '_') {
                    int len = strlen(searchQuery);
                    if (len < sizeof(searchQuery) - 1) {
                        searchQuery[len] = ' ';
                        searchQuery[len + 1] = '\0';
                    }
                } else if (keyChar != ' ') {
                    int len = strlen(searchQuery);
                    if (len < sizeof(searchQuery) - 1) {
                        searchQuery[len] = keyChar;
                        searchQuery[len + 1] = '\0';
                    }
                }
                
                updateSearchField();
                
                delay(200); // avoid bouncing when typing
                
                return;
            }
        }
    }
}
}
    
    ////////////////////////////////////////////////// Coordinates Screen Touch Actions ///////////////////////////////////////////////////////
    else if (CURRENT_SCREEN == 5)
    { // captures touches on drawCoordinatesScreen()
      if (lx > 210 && lx < 320 && ly > 10 && ly < 60) {
        // BTN back pressed
        drawMainScreen();
      }

    }

    ////////////////////////////////////////////////// Load Screen Touch Actions ///////////////////////////////////////////////////////
    else if (CURRENT_SCREEN == 6)
    { // captures touches on drawLoadScreen() .. the one that loads objects from DB
      if (lx > 210 && lx < 320 && ly > 10 && ly < 60)
      {
        // BTN Back pressed
        drawMainScreen();
      }
      
// First condition (BTN next) simplified:
if (lx > 220 && lx < 320 && ly > 420 && ly < 480) {
    switch (LOAD_SELECTOR) {
        case 2: Bright_PAGER += 1; handlePager(Bright_PAGER, 8, "Bright_CAT.csv", OBJ_PAGER); break;
        case 3: Double_PAGER += 1; handlePager(Double_PAGER, 7, "Double_CAT.csv", OBJ_PAGER); break;
        case 4: Variable_PAGER += 1; handlePager(Variable_PAGER, 6, "Variable_CAT.csv", OBJ_PAGER); break;
        case 6: MESS_PAGER += 1; handlePager(MESS_PAGER, 5, "messier.csv", OBJ_PAGER); break;
        case 7: NGC_PAGER1 += 1; handlePagerWithTwoFiles(NGC_PAGER1, 11, "NGC_CAT1_10.csv", "NGC_CAT2_10.csv", 23, OBJ_PAGER); break;
        case 8: NGC_PAGER2 += 1; handlePagerWithTwoFiles(NGC_PAGER2, 11, "NGC_CAT3_10.csv", "NGC_CAT4_10.csv", 23, OBJ_PAGER); break;
        case 9: NGC_PAGER3 += 1; handlePagerWithTwoFiles(NGC_PAGER3, 11, "NGC_CAT5_10.csv", "NGC_CAT6_10.csv", 23, OBJ_PAGER); break;
        case 11: NGC_PAGER4 += 1; handlePagerWithTwoFiles(NGC_PAGER4, 11, "NGC_CAT7_10.csv", "NGC_CAT8_10.csv", 23, OBJ_PAGER); break;
        case 12: NGC_PAGER5 += 1; handlePagerWithTwoFiles(NGC_PAGER5, 11, "NGC_CAT9_10.csv", "NGC_CAT10_10.csv", 23, OBJ_PAGER); break;
        case 13: IC_PAGER += 1; handlePager(IC_PAGER, 9, "IC_CAT.csv", OBJ_PAGER); break;
        case 14: CUSTOM_PAGER += 1; handlePager(CUSTOM_PAGER, 11, "custom_CAT.csv", OBJ_PAGER); break;
    }
}

if (lx > 0 && lx < 100 && ly > 420 && ly < 480) {
    switch (LOAD_SELECTOR) {
        case 2: Bright_PAGER -= 1; handlePager(Bright_PAGER, 8, "Bright_CAT.csv", OBJ_PAGER); break;
        case 3: Double_PAGER -= 1; handlePager(Double_PAGER, 7, "Double_CAT.csv", OBJ_PAGER); break;
        case 4: Variable_PAGER -= 1; handlePager(Variable_PAGER, 6, "Variable_CAT.csv", OBJ_PAGER); break;
        case 6: MESS_PAGER -= 1; handlePager(MESS_PAGER, 5, "messier.csv", OBJ_PAGER); break;
        case 7: NGC_PAGER1 -= 1; handlePagerWithTwoFiles(NGC_PAGER1, 11, "NGC_CAT1_10.csv", "NGC_CAT2_10.csv", 23, OBJ_PAGER); break;
        case 8: NGC_PAGER2 -= 1; handlePagerWithTwoFiles(NGC_PAGER2, 11, "NGC_CAT3_10.csv", "NGC_CAT4_10.csv", 23, OBJ_PAGER); break;
        case 9: NGC_PAGER3 -= 1; handlePagerWithTwoFiles(NGC_PAGER3, 11, "NGC_CAT5_10.csv", "NGC_CAT6_10.csv", 23, OBJ_PAGER); break;
        case 11: NGC_PAGER4 -= 1; handlePagerWithTwoFiles(NGC_PAGER4, 11, "NGC_CAT7_10.csv", "NGC_CAT8_10.csv", 23, OBJ_PAGER); break;
        case 12: NGC_PAGER5 -= 1; handlePagerWithTwoFiles(NGC_PAGER5, 11, "NGC_CAT9_10.csv", "NGC_CAT10_10.csv", 23, OBJ_PAGER); break;
        case 13: IC_PAGER -= 1; handlePager(IC_PAGER, 9, "IC_CAT.csv", OBJ_PAGER); break;
        case 14: CUSTOM_PAGER -= 1; handlePager(CUSTOM_PAGER, 11, "custom_CAT.csv", OBJ_PAGER); break;
    }
}

      if (lx > 0 && lx < 110 && ly > 10 && ly < 60) { // button prev cat pressed
      if (CAT_Page == 1 ) {LOAD_SELECTOR = 0; CAT_Page = 1;}
      else if (CAT_Page == 2 ) {LOAD_SELECTOR = 0; CAT_Page = 1;} 
      else if (CAT_Page == 3 ) {LOAD_SELECTOR = 5; CAT_Page = 2;}
      
      drawLoadScreen();     
      }
      if (lx > 105 && lx < 215 && ly > 10 && ly < 60) { // button next cat pressed
      if (CAT_Page == 1 ) {LOAD_SELECTOR = 5; CAT_Page = 2;}
      else if (CAT_Page == 2 ) {LOAD_SELECTOR = 10; CAT_Page = 3;} 
      else if (CAT_Page == 3 ) {LOAD_SELECTOR = 10; CAT_Page = 3;}
      drawLoadScreen();
      }

    slewascomsent = false;
    char obj_name[50];
    char OBJ_NAME[50];
    
    if (!IS_NIGHTMODE) {
        safeStringCopy(OBJ_NAME, "UI/day/", sizeof(OBJ_NAME));
    } else {
        safeStringCopy(OBJ_NAME, "UI/night/", sizeof(OBJ_NAME));
    }

if (lx > 0 && lx < 80 && ly > 60 && ly < 130) {
    // Modif JG : 1st of 4 buttons pressed
    char temp_path[50];
    safeStringCopy(temp_path, OBJ_NAME, sizeof(temp_path));
    
    if (CAT_Page == 1) { 
        LOAD_SELECTOR = 1;
        strncat(temp_path, "btn_load_1.bin", sizeof(temp_path) - strlen(temp_path) - 1);
    } 
    else if (CAT_Page == 2) { 
        LOAD_SELECTOR = 6;
        safeStringCopy(CAT_NAME, "messier.csv", sizeof(CAT_NAME));
        strncat(temp_path, "btn_load_1b.bin", sizeof(temp_path) - strlen(temp_path) - 1);
        OBJ_PAGER = MESS_PAGER; 
    } 
    else if (CAT_Page == 3) {
        LOAD_SELECTOR = 11;  
        if (NGC_PAGER4 <= 11) { 
            safeStringCopy(CAT_NAME, "NGC_CAT7_10.csv", sizeof(CAT_NAME));
        } else {
            safeStringCopy(CAT_NAME, "NGC_CAT8_10.csv", sizeof(CAT_NAME));
        }
        strncat(temp_path, "btn_load_1c.bin", sizeof(temp_path) - strlen(temp_path) - 1);
        OBJ_PAGER = NGC_PAGER4; 
    }
    
    if (SD.open(temp_path)) {
        drawBin(temp_path, 0, 60, 320, 80);
    }
    drawLoadObjects();
}

// Modif JG : 2nd of 4 buttons pressed
if (lx > 81 && lx < 161 && ly > 60 && ly < 130) {
    char temp_path[50];
    safeStringCopy(temp_path, OBJ_NAME, sizeof(temp_path));
    
    if (CAT_Page == 1) { 
        LOAD_SELECTOR = 2;
        safeStringCopy(CAT_NAME, "Bright_CAT.csv", sizeof(CAT_NAME));
        strncat(temp_path, "btn_load_2.bin", sizeof(temp_path) - strlen(temp_path) - 1);
        OBJ_PAGER = Bright_PAGER;
    } 
    else if (CAT_Page == 2) { 
        LOAD_SELECTOR = 7; 
        if (NGC_PAGER1 <= 11) { 
            safeStringCopy(CAT_NAME, "NGC_CAT1_10.csv", sizeof(CAT_NAME));
        } else {
            safeStringCopy(CAT_NAME, "NGC_CAT2_10.csv", sizeof(CAT_NAME));
        }
        strncat(temp_path, "btn_load_2b.bin", sizeof(temp_path) - strlen(temp_path) - 1);
        OBJ_PAGER = NGC_PAGER1;
    } 
    else if (CAT_Page == 3) {
        LOAD_SELECTOR = 12; 
        if (NGC_PAGER5 <= 11) { 
            safeStringCopy(CAT_NAME, "NGC_CAT9_10.csv", sizeof(CAT_NAME));
        } else {
            safeStringCopy(CAT_NAME, "NGC_CAT10_10.csv", sizeof(CAT_NAME));
        }
        strncat(temp_path, "btn_load_2c.bin", sizeof(temp_path) - strlen(temp_path) - 1);
        OBJ_PAGER = NGC_PAGER5;
    }
    
    if (SD.open(temp_path)) {
        drawBin(temp_path, 0, 60, 320, 80);
    }
    drawLoadObjects();
}

// Modif JG : 3rd of 4 buttons pressed
if (lx > 162 && lx < 237 && ly > 60 && ly < 130) {
    // BTN Solar System pressed
    char temp_path[50];
    safeStringCopy(temp_path, OBJ_NAME, sizeof(temp_path));
    
    if (CAT_Page == 1) { 
        LOAD_SELECTOR = 3;
        safeStringCopy(CAT_NAME, "Double_CAT.csv", sizeof(CAT_NAME));
        strncat(temp_path, "btn_load_3.bin", sizeof(temp_path) - strlen(temp_path) - 1);
        OBJ_PAGER = Double_PAGER;
    } 
    else if (CAT_Page == 2){
        LOAD_SELECTOR = 8; 
        if (NGC_PAGER2 <= 11) { 
            safeStringCopy(CAT_NAME, "NGC_CAT3_10.csv", sizeof(CAT_NAME));
        } else {
            safeStringCopy(CAT_NAME, "NGC_CAT4_10.csv", sizeof(CAT_NAME));
        }
        strncat(temp_path, "btn_load_3b.bin", sizeof(temp_path) - strlen(temp_path) - 1);
        OBJ_PAGER = NGC_PAGER2;
    } 
    else if (CAT_Page == 3) {
        LOAD_SELECTOR = 13; 
        safeStringCopy(CAT_NAME, "IC_CAT.csv", sizeof(CAT_NAME));
        strncat(temp_path, "btn_load_3c.bin", sizeof(temp_path) - strlen(temp_path) - 1);
        OBJ_PAGER = IC_PAGER; 
    }
    
    if (SD.open(temp_path)) {
        drawBin(temp_path, 0, 60, 320, 80);
    }
    drawLoadObjects();
}

// Modif JG : 4th of 4 buttons pressed
if (lx > 238 && lx < 318 && ly > 60 && ly < 130) {
    // BTN custom.csv pressed
    char temp_path[50];
    safeStringCopy(temp_path, OBJ_NAME, sizeof(temp_path));
    
    if (CAT_Page == 1) { 
        LOAD_SELECTOR = 4;
        safeStringCopy(CAT_NAME, "Variable_CAT.csv", sizeof(CAT_NAME));
        strncat(temp_path, "btn_load_4.bin", sizeof(temp_path) - strlen(temp_path) - 1);
        OBJ_PAGER = Variable_PAGER;
    } 
    else if (CAT_Page == 2 ) {
        LOAD_SELECTOR = 9; 
        if (NGC_PAGER3 <= 11) { 
            safeStringCopy(CAT_NAME, "NGC_CAT5_10.csv", sizeof(CAT_NAME));
        } else {
            safeStringCopy(CAT_NAME, "NGC_CAT6_10.csv", sizeof(CAT_NAME));
        }
        strncat(temp_path, "btn_load_4b.bin", sizeof(temp_path) - strlen(temp_path) - 1);
        OBJ_PAGER = NGC_PAGER3;
    } 
    else if (CAT_Page == 3) {
        LOAD_SELECTOR = 14; 
        safeStringCopy(CAT_NAME, "Custom_CAT.csv", sizeof(CAT_NAME));
        strncat(temp_path, "btn_load_4c.bin", sizeof(temp_path) - strlen(temp_path) - 1);
        OBJ_PAGER = CUSTOM_PAGER;
    }
    
    if (SD.open(temp_path)) {
        drawBin(temp_path, 0, 60, 320, 80);
    }
        drawLoadObjects();
      }

      if (LOAD_SELECTOR >= 2 && LOAD_SELECTOR <= 4 or LOAD_SELECTOR >= 6 && LOAD_SELECTOR <= 9 or LOAD_SELECTOR >= 11 && LOAD_SELECTOR <= 14) // Add JG, deleted all messier, treasure, custom conditions, now one condition for all catalogs and another one for solar system.
      {
        for (int i = 0; i < 5; i++)
        {
          for (int j = 0; j < 4; j++)
          {
            if (lx > ((j * 75) + 12) && lx < ((j * 75) + 81) && ly > ((i * 50) + 160) && ly < ((i * 50) + 203))
            {
              //75 box width ,12start from left , 50 box hight , 110 start from up

              // found button pressed.... ora I need to get his ID and link to the ARRAY;
              int zz = (OBJ_PAGER * 20) + (i * 4) + j;
              if (OBJ_PAGER >= 12) {zz = ((OBJ_PAGER - 12) * 20) + (i * 4) + j;} // ADD JG for second 240 items NGC Catalog 
                if (OBJ_Array[zz][0] != '\0') {
                selectOBJECT_M(zz, 1);
              
                calculateLST_HA();
                if (ALT > 0)
                {
                  OnScreenMsg(1);
                  if (IS_SOUND_ON)
                  {
                    SoundOn(note_C, 32);
                    delay(200);
                    SoundOn(note_C, 32);
                    delay(200);
                    SoundOn(note_C, 32);
                    delay(1000);
                  }
                  // Stop Interrupt procedure for tracking.
                  Timer3.stop(); //
                  IS_TRACKING = false;
                  IS_OBJ_FOUND = false;
                  IS_OBJECT_RA_FOUND = false;
                  IS_OBJECT_DEC_FOUND = false;
                  Tracking_type = 1; // Preparar el seguimiento que se usara, 1: Sidereal, 2: Solar, 0: Lunar;
                  //Tracking_Mode = "Celest";
                  safeStringCopy(Tracking_Mode, "Celest", sizeof(Tracking_Mode));
                  sendTrackingStatus();
                  Slew_timer = millis();
                  Slew_RA_timer = Slew_timer + 5000;   // Give 20 sec. advance to the DEC. We will revise later.
                }
                UpdateObservedObjects();
                OBJ_PAGER == 0;
                sun_confirm = false;
                drawMainScreen();
              }
            }
          }
        }
      }
      else if (LOAD_SELECTOR == 1)
      {
        // I'm in Solar System selector and need to check which Solar System object is pressed
        for (int i = 0; i < 5; i++)
        {
          for (int j = 0; j < 4; j++)
          {
            if (lx > ((j * 75) + 12) && lx < ((j * 75) + 81) && ly > ((i * 50) + 160) && ly < ((i * 50) + 203))
            {
              // found button pressed.... ora I need to get his ID and link to the ARRAY;
              int zz = (i * 4) + j;
#ifdef serial_debug
              Serial.print("zz = ");
              Serial.println(zz);
#endif
              if (zz == 3)
              {
                OnScreenMsg(7);
                delay(2000);
                drawMainScreen();
              }
              else if (zz <= 10)
              {
                if (zz == 0) // I chose the Sun
                {
                  drawConfirmSunTrack();
                }
                else
                {
                  planet_pos(zz);
                }
                if (sun_confirm || zz != 0)
                {
                  calculateLST_HA();
                  if (ALT > 0)
                  {
                    OnScreenMsg(1);
                    if (IS_SOUND_ON)
                    {
                      SoundOn(note_C, 32);
                      delay(200);
                      SoundOn(note_C, 32);
                      delay(200);
                      SoundOn(note_C, 32);
                      delay(1000);
                    }
                    // Stop Interrupt procedure for tracking.
                    Timer3.stop(); //
                    IS_TRACKING = false;

                    IS_OBJ_FOUND = false;
                    IS_OBJECT_RA_FOUND = false;
                    IS_OBJECT_DEC_FOUND = false;
                    switch (zz)
                    {
                      case 0:
                        Tracking_type = 2; // Preparar el seguimiento que se usara, 1: Sidereal, 2: Solar, 0: Lunar;
                        //Tracking_Mode = "Solar";
                        safeStringCopy(Tracking_Mode, "Solar", sizeof(Tracking_Mode));
                        break;
                      case 10:
                        Tracking_type = 0; // Preparar el seguimiento que se usara, 1: Sidereal, 2: Solar, 0: Lunar;
                        //Tracking_Mode = "Lunar";
                        safeStringCopy(Tracking_Mode, "Lunar", sizeof(Tracking_Mode));
                        break;
                      default:
                        Tracking_type = 1; // Preparar el seguimiento que se usara, 1: Sidereal, 2: Solar, 0: Lunar;
                        //Tracking_Mode = "Celest";
                        safeStringCopy(Tracking_Mode, "Celest", sizeof(Tracking_Mode));
                        break;
                    }
                    Slew_timer = millis();
                    Slew_RA_timer = Slew_timer + 5000;   // Give 20 sec. advance to the DEC. We will revise later.
                  }
                  UpdateObservedObjects();
                  //CUSTOM_PAGER == 0;
                  sun_confirm = false;
                  drawMainScreen();
                  sendTrackingStatus();
                }
              }
            }
          }
        }
      }
    }

    ////////////////////////////////////////////////// Options Screen Touch Actions ///////////////////////////////////////////////////////
    else if (CURRENT_SCREEN == 7)
    {
      // captures touches on drawOptionsScreen()
      if (lx > 210 && lx < 320 && ly > 10 && ly < 60) {
        // BTN <Back pressed
        storeOptions_SD();
        drawMainScreen();
      }
      if (lx > 211 && lx < 308 && ly > 321 && ly < 370) {
        // Celestial Tracking Selected
        Tracking_type = 1;
        //Tracking_Mode = "Celest";
        safeStringCopy(Tracking_Mode, "Celest", sizeof(Tracking_Mode));
        updateTrackingMode_opt();
      }
      if (lx > 211 && lx < 308 && ly > 370 && ly < 419) {
        // Solar Tracking Selected
        Tracking_type = 2;
        //Tracking_Mode = "Solar";
        safeStringCopy(Tracking_Mode, "Solar", sizeof(Tracking_Mode));
        updateTrackingMode_opt();
      }
      if (lx > 211 && lx < 308 && ly > 418 && ly < 467) {
        // Lunar Tracking Selected
        Tracking_type = 0;
        //Tracking_Mode = "Lunar";
        safeStringCopy(Tracking_Mode, "Lunar", sizeof(Tracking_Mode));
        updateTrackingMode_opt();
      }

      if (lx > 259 && lx < 310 && ly > 54 && ly < 99 )
      {
          // Screen MAX Brightness
          setDisplayBrightness(1.0); // 100%
          TFT_Brightness = 255;
          updateTriangleBrightness_opt();
      }
      
      if (lx > 5 && lx < 255 && ly > 50 && ly < 100) {
          // extended range : 5 to 255 pixels → 0% à 100%
          float position = constrain(lx, 5, 255);
          float brightness_value = (position - 5) / 250.0; // 0.0 to 1.0
          
          setDisplayBrightness(brightness_value);
          updateTriangleBrightness_opt();
      }

//add JG      
      
      if (lx > 3 && lx < 108 && ly > 275 && ly < 355) {
        // Touch Screen Calibration
      drawScreenCalibration();
      if (interx != 0.00 && intery != 0.00) {
      DrawButton(60, 270, 220, 50, "Reset Values", 0, btn_d_border, btn_d_border, 2, false);
      }}

//fin add JG

if (lx > 3 && lx < 56 && ly > 221 && ly < 269) {
    TFT_timeout = 0;
    strcpy(TFT_Time, "AL-ON");
    updateScreenTimeout_opt();
}
if (lx > 56 && lx < 108 && ly > 221 && ly < 269) {
    TFT_timeout = 30000;
    strcpy(TFT_Time, "30 s");
    updateScreenTimeout_opt();
}
if (lx > 108 && lx < 161 && ly > 221 && ly < 269) {
    TFT_timeout = 60000;
    strcpy(TFT_Time, "60 s");
    updateScreenTimeout_opt();
}
if (lx > 160 && lx < 210 && ly > 221 && ly < 269) {
    TFT_timeout = 120000;
    strcpy(TFT_Time, "2 min");
    updateScreenTimeout_opt();
}
if (lx > 212 && lx < 263 && ly > 221 && ly < 269) {
    TFT_timeout = 300000;
    strcpy(TFT_Time, "5 min");
    updateScreenTimeout_opt();
}
if (lx > 265 && lx < 318 && ly > 221 && ly < 269) {
    TFT_timeout = 600000;
    strcpy(TFT_Time, "10 min");
    updateScreenTimeout_opt();
}

      if (lx > 111 && lx < 204 && ly > 323 && ly < 371) {
        // ON Meridian Flip
        IS_MERIDIAN_FLIP_AUTOMATIC = true;
        //Mer_Flip_State = "AUTO";
        safeStringCopy(Mer_Flip_State, "AUTO", sizeof(Mer_Flip_State));
        updateMeridianFlip_opt();
      }
      if (lx > 111 && lx < 204 && ly > 371 && ly < 419) {
        // OFF Meridian Flip
        IS_MERIDIAN_FLIP_AUTOMATIC = false;
        //Mer_Flip_State = "OFF";
        safeStringCopy(Mer_Flip_State, "OFF", sizeof(Mer_Flip_State));
        updateMeridianFlip_opt();
      }
      if (lx > 53 && lx < 96 && ly > 162 && ly < 190) {
        // ON Sound
        IS_SOUND_ON = true;
        //Sound_State = "ON";
        safeStringCopy(Sound_State, "ON", sizeof(Sound_State));        
        updateSound_opt();
      }
      if (lx > 1 && lx < 52 && ly > 162 && ly < 190) {
        // OFF Sound
        IS_SOUND_ON = false;
        //Sound_State = "OFF";
        safeStringCopy(Sound_State, "OFF", sizeof(Sound_State));
        updateSound_opt();
      }
      if (lx > 160 && lx < 204 && ly > 162 && ly < 190)
      {
        // ON Stepper Motors
        IS_STEPPERS_ON = true;
        digitalWrite(RA_EN, LOW);
        digitalWrite(DEC_EN, LOW);
        //Stepper_State = "ON";
        safeStringCopy(Stepper_State, "ON", sizeof(Stepper_State));
        updateStepper_opt();
      }
      if (lx > 115 && lx < 159 && ly > 162 && ly < 190)
      {
        // OFF Stepper Motors
        IS_STEPPERS_ON = false;
        digitalWrite(RA_EN, HIGH);
        digitalWrite(DEC_EN, HIGH);
        //Stepper_State = "OFF";
        safeStringCopy(Stepper_State, "OFF", sizeof(Stepper_State));
        updateStepper_opt();
      }
      //Touched GPS configuration
      if (lx > 10 && lx < 100 && ly > 360 && ly < 480)
      {
        storeOptions_SD();
        CURRENT_SCREEN = 0;
        drawGPSScreen();
      }
      //Touched TFT Calibration
    }

    ////////////////////////////////////////////////// STAT Screen Touch Actions ///////////////////////////////////////////////////////
    else if (CURRENT_SCREEN == 10) // captures touches on drawSTATScreen()
    {
      if (lx > 210 && lx < 320 && ly > 10 && ly < 60)
      {
        // BTN Back pressed
        IS_IN_OPERATION = true;
        drawMainScreen();
        IS_CONNECTED = true;
        IS_PulseGuiding = false;
      }
    }

    //////////////////////////////////////////////////Star Maps Screen Touch Actions ///////////////////////////////////////////////////////
    else if (CURRENT_SCREEN == 11) // captures touches on drawStarMap()
    {
      if (lx > 200 && lx < 320 && ly > 0 && ly < 50)
      {
        // BTN Back pressed
        IS_IN_OPERATION = true;
        drawMainScreen();
      }
      // Take care of Map move...
      // When user touches left, right, top and bottom part of the image,
      // the system loads the corresponding Star Map - moving left, right, top and bottom.
      if (lx > 0 && lx < 50 && ly > 90 && ly < 420) // Left side touched... show next StarMap image
      {
        if (map_c < 8)
        {
          map_c += 1;
          IS_CUSTOM_MAP_SELECTED = true;
          drawStarMap();
        }
      }
      if (lx > 190 && lx < 320 && ly > 90 && ly < 420) {
        // Right side touched... show next StarMap image
        if (map_c > 1) {
          map_c -= 1;
          IS_CUSTOM_MAP_SELECTED = true;
          drawStarMap();
        }
      }
      if (lx > 0 && lx < 320 && ly > 40 && ly < 90) {
        // Top side touched... show next StarMap image
        if (map_r > 1) {
          map_r -= 1;
          IS_CUSTOM_MAP_SELECTED = true;
          drawStarMap();
        }
      }
      if (lx > 0 && lx < 320 && ly > 440 && ly < 480) {
        // Bottom side touched... show next StarMap image
        if (map_r < 3) {
          map_r += 1;
          IS_CUSTOM_MAP_SELECTED = true;
          drawStarMap();
        }
      }
    }

    ////////////////////////////////////////////////// Star Sync Screen Touch Actions ///////////////////////////////////////////////////////
    else if (CURRENT_SCREEN == 12) // captures touches on drawStarSyncScreen()
    {
      if (lx > 210 && lx < 320 && ly > 10 && ly < 60)
      {
        // BTN Done pressed
        IS_IN_OPERATION = true;
        drawMainScreen();
      }
      int do_kolko = 0;
      if (ALLIGN_TYPE == 3)
      {
        // Chage the 14 to represent the real count of screens.
        // They need to be dynamically calculated... not fixed
        do_kolko = int_star_count;
      }
      else
      {
        do_kolko = 14;
      }
      if (lx > 220 && lx < 320 && ly > 420 && ly < 480) {
        // BTN next> pressed
        STARS_PAGER += 1;
        if (STARS_PAGER < do_kolko) {
          //drawStarSyncScreen();
          drawAlignObjects_ali();
        } else {
          STARS_PAGER = do_kolko - 1;
        }
      }
      if (lx > 0 && lx < 100 && ly > 420 && ly < 480) {
        // BTN <prev pressed
        STARS_PAGER -= 1;
        if (STARS_PAGER >= 0) {
          //drawStarSyncScreen();
          drawAlignObjects_ali();
        } else {
          STARS_PAGER = 0;
        }
      }

      if (ALLIGN_TYPE < 3)
      {
        // I'm in STARS selector and need to check which Star object is pressed
        for (int i = 0; i < 6; i++)
        {
          for (int j = 0; j < 4; j++)
          {
            if (lx > ((j * 75) + 12) && lx < ((j * 75) + 81) && ly > ((i * 50) + 85) && ly < ((i * 50) + 128))
            {
              // found button pressed.... ora I need to get his ID and link to the ARRAY;
              int zz = (STARS_PAGER * 24) + (i * 4) + j;

                if (strlen(Stars[zz]) > 0)


              {
                selectOBJECT_M(zz, 2);
                calculateLST_HA();
                if (ALT < 0)
                {
                  OnScreenMsg(3);
                  if (IS_SOUND_ON)
                  {
                    SoundOn(note_C, 96);
                    delay(2000);
                  }
                  drawStarSyncScreen();
                }
                else
                {
                  OnScreenMsg(1);
                  if (IS_SOUND_ON)
                  {
                    SoundOn(note_C, 32);
                    delay(200);
                    SoundOn(note_C, 32);
                    delay(200);
                    SoundOn(note_C, 32);
                    delay(200);
                  }
                  // Stop Interrupt procedure for tracking.
                  Timer3.stop(); //
                  IS_TRACKING = false;

                  IS_OBJ_FOUND = false;
                  IS_OBJECT_RA_FOUND = false;
                  IS_OBJECT_DEC_FOUND = false;
                  Slew_timer = millis();
                  Slew_RA_timer = Slew_timer + 5000;   // Give 20 sec. advance to the DEC. We will revise later.
                  STARS_PAGER == 0;
                  SELECTED_STAR = zz;
                  ALLIGN_STEP += 1;
                  sendTrackingStatus();
                }
              }
            }
          }
        }
      }
      else
      {
        // I'm in STARS selector and need to check which Star object is pressed
        for (int i = 0; i < 6; i++)
        {
          for (int j = 0; j < 4; j++)
          {
            if (lx > ((j * 75) + 10) && lx < ((j * 75) + 79) && ly > ((i * 50) + 86) && ly < ((i * 50) + 129))
            {
              // found button pressed.... ora I need to get his ID and link to the ARRAY;
              int zz = (STARS_PAGER * 24) + (i * 4) + j;
              //if (Iter_Stars[zz] != "")
              //if (strcmp(Iter_Stars[zz], "") != 0) {


              if (strlen(Iter_Stars[zz]) > 0) {

                
              {
                // selectOBJECT_M(zz,3);
                Iterative_Star_Index = zz;
                // replaced the above with the below row...
                // to make sure each press selects the same star (inital or on Step 2)
                // change made after Test on 17th March.
                selectOBJECT_M(Iterative_Star_Index, 3);
                calculateLST_HA();
                OnScreenMsg(1);
                if (IS_SOUND_ON)
                {
                  SoundOn(note_C, 32);
                  delay(200);
                  SoundOn(note_C, 32);
                  delay(200);
                  SoundOn(note_C, 32);
                  delay(200);
                }
                // Stop Interrupt procedure for tracking.
                Timer3.stop(); //
                IS_TRACKING = false;
                IS_OBJ_FOUND = false;
                IS_OBJECT_RA_FOUND = false;
                IS_OBJECT_DEC_FOUND = false;
                Slew_timer = millis();
                Slew_RA_timer = Slew_timer + 5000;   // Give 20 sec. advance to the DEC. We will revise later.
                STARS_PAGER == 0;
                SELECTED_STAR = zz;
                ALLIGN_STEP += 1;
                sendTrackingStatus();
                // drawConstelationScreen(zz);
              }
            }
          }
        }
      }
    }
    }
    ////////////////////////////////////////////////// Constellations Screen Touch Actions ///////////////////////////////////////////////////////
    else if (CURRENT_SCREEN == 13)
    { // captures touches on drawConstelationScreen(int indx)
      if (lx > 0 && lx < 100 && ly > 420 && ly < 480)
      {
        // BTN "<Repeat" or "<EXIT" pressed
        if (ALLIGN_TYPE == 3)
        {
          // delta_a_RA = 0;
          // delta_a_DEC = 0;
          STARS_PAGER = 0;
          IS_TRACKING = false;
          IS_IN_OPERATION = true;
          drawMainScreen();
        }
        else
        {
          ALLIGN_STEP -= 1;
          drawStarSyncScreen();
        }
      }
      if (lx > 220 && lx < 320 && ly > 420 && ly < 480)
      {
        // BTN "ALIGN!" pressed
        // Here we need to kora which Star is this - 1st, 2nd, 3rd... etc ?
        // In order to use Ralph Pass alignment procedure described on http://rppass.com/
        // http://rppass.com/align.pdf - the actual PDF

        if (ALLIGN_STEP == 1)
        {
          if (ALLIGN_TYPE == 1)
          {
            IS_TRACKING = false;
            IS_IN_OPERATION = true;
            drawMainScreen();
          }
          else if (ALLIGN_TYPE == 3)
          {
            // Select Polaris and SlewTo...
            IS_TRACKING = false; // Select Polaris and SlewTo...
            IS_TRACKING = false;
            //
            //
            // ora I have to SYNC on that STAR!
            // Assign Values calculated for
            // SLEW_RA_microsteps and SLEW_DEC_microsteps
            //
            selectOBJECT_M(166, 2); // Polaris in on Index 192 in the Stars Array // Modif JG Deleted south hemisphere stars, so Polaris is the 166th star in the array
            calculateLST_HA();
            OnScreenMsg(1);
            if (IS_SOUND_ON)
            {
              SoundOn(note_C, 32);
              delay(200);
              SoundOn(note_C, 32);
              delay(200);
              SoundOn(note_C, 32);
              delay(200);
            }
            // Stop Interrupt procedure for tracking.
            Timer3.stop(); //
            IS_TRACKING = false;
            IS_OBJ_FOUND = false;
            IS_OBJECT_RA_FOUND = false;
            IS_OBJECT_DEC_FOUND = false;
            Slew_timer = millis();
            Slew_RA_timer = Slew_timer + 5000;   // Give 20 sec. advance to the DEC. We will revise later.
            sendTrackingStatus();
            //drawConstelationScreen(0);
            ALLIGN_STEP = 2;
          }
        }
        else if (ALLIGN_STEP == 2)
        {
          if (ALLIGN_TYPE == 3)
          {
            // Select First Star Again and SlewTo...
            IS_TRACKING = false;
            selectOBJECT_M(Iterative_Star_Index, 3); // Load First Star from the Stars Array
            calculateLST_HA();
            OnScreenMsg(1);
            if (IS_SOUND_ON)
            {
              SoundOn(note_C, 32);
              delay(200);
              SoundOn(note_C, 32);
              delay(200);
              SoundOn(note_C, 32);
              delay(200);
            }
            // Stop Interrupt procedure for tracking.
            Timer3.stop(); //
            IS_TRACKING = false;
            IS_OBJ_FOUND = false;
            IS_OBJECT_RA_FOUND = false;
            IS_OBJECT_DEC_FOUND = false;
            Slew_timer = millis();
            Slew_RA_timer = Slew_timer + 5000;   // Give 20 sec. advance to the DEC. We will revise later.
            //drawConstelationScreen(0);
            ALLIGN_STEP = 1;
            sendTrackingStatus();
          }
        }
      }
    }
    ////////////////////////////////////////////////// Auto-Guiding Screen Touch Actions ///////////////////////////////////////////////////////
    else if (CURRENT_SCREEN == 14) // captures touches on drawAutoGuidingScreen()
    {
      if (lx > 110 && lx < 210 && ly > 430 && ly < 480)
      {
        // BTN Back pressed
        IS_IN_OPERATION = true;
        CURRENT_SCREEN = 4;
        drawMainScreen();
      }
    }
    /////////////////////////////////////////////// Confirm Sun Tracking Screen Touch Actions /////////////////////////////////////////////////////////
    else if (CURRENT_SCREEN == 15)
    {
      // Capture touch inputs on drawConfirmSunTrack()
      if (lx > 10 && lx < 150 && ly > 380 && ly < 460) //BTN YES
      {
        DrawButton(10, 380, 140, 80, "YES", 0, btn_l_selection, btn_l_text, 3, false);
        planet_pos(0);
        sun_confirm = true;

        if (sun_confirm)
        {
          calculateLST_HA();
          if (ALT > 0)
          {
            OnScreenMsg(1);
            if (IS_SOUND_ON)
            {
              SoundOn(note_C, 32);
              delay(200);
              SoundOn(note_C, 32);
              delay(200);
              SoundOn(note_C, 32);
              delay(1000);
            }
            // Stop Interrupt procedure for tracking.
            Timer3.stop(); //
            IS_TRACKING = false;
            IS_OBJ_FOUND = false;
            IS_OBJECT_RA_FOUND = false;
            IS_OBJECT_DEC_FOUND = false;
            Tracking_type = 2; // Change the tracking type Automatically. 1: Sidereal, 2: Solar, 0: Lunar;
            safeStringCopy(Tracking_Mode, "Solar", sizeof(Tracking_Mode));
            updateTrackingMode_opt();
            Slew_timer = millis();
            Slew_RA_timer = Slew_timer + 5000;   // Give 20 sec. advance to the DEC. We will revise later.
            sendTrackingStatus();
          }
          UpdateObservedObjects();
          sun_confirm = false;
          drawMainScreen();
        }
      }
      else if (lx > 170 && lx < 310 && ly > 380 && ly < 460) //BTN NO
      {
        DrawButton(170, 380, 140, 80, "NO", 0, btn_l_selection, btn_l_text, 3, false);
        drawLoadScreen();
        sun_confirm = false;
      }
    }
  }
  else
  {
    //**************************************************************
    //
    //      BUTTON UP Events start here
    //
    //      - only executed when the user touches the screen - RELEASE
    //**************************************************************

    if (CURRENT_SCREEN == 0) {
      if (last_button == 1) {
        last_button = 0;
        // Set the earth rotation direction depending on the Hemisphere...
        // HIGH and LOW are substituted
        if (OBSERVATION_LATTITUDE > 0) {
          STP_FWD = LOW;
          STP_BACK = HIGH;
        } else {
          STP_FWD = HIGH;
          STP_BACK = LOW;
        }
#ifdef serial_debug
        Serial.println(OBSERVATION_LATTITUDE);
        Serial.print("STP_FWD = ");
        Serial.println(STP_FWD);
        Serial.print("STP_BACK = ");
        Serial.println(STP_BACK);
#endif
        CURRENT_SCREEN = 1;
        drawClockScreen();
      }
    } else if (CURRENT_SCREEN == 1) {
      if (last_button == 1) {
        last_button = 0;
        tft.drawRect(15, 224, 80, 50, messie_btn);
        TimerUpdateDraw(1);
      }
      if (last_button == 2) {
        last_button = 0;
        tft.drawRect(120, 224, 80, 50, messie_btn);
        TimerUpdateDraw(2);
      }
      if (last_button == 3) {
        last_button = 0;
        tft.drawRect(230, 224, 80, 50, messie_btn);
        TimerUpdateDraw(3);
      }
      if (last_button == 4) {
        last_button = 0;
        tft.drawRect(15, 285, 80, 50, messie_btn);
        TimerUpdateDraw(4);
      }
      if (last_button == 5) {
        last_button = 0;
        tft.drawRect(120, 285, 80, 50, messie_btn);
        TimerUpdateDraw(5);
      }
      if (last_button == 6) {
        last_button = 0;
        tft.drawRect(230, 285, 80, 50, messie_btn);
        TimerUpdateDraw(6);
      }
      if (last_button == 7) {
        last_button = 0;
        tft.drawRect(15, 346, 80, 50, messie_btn);
        TimerUpdateDraw(7);
      }
      if (last_button == 8) {
        last_button = 0;
        tft.drawRect(120, 346, 80, 50, messie_btn);
        TimerUpdateDraw(8);
      }
      if (last_button == 9) {
        last_button = 0;
        tft.drawRect(230, 346, 80, 50, messie_btn);
        TimerUpdateDraw(9);
      }
      if (last_button == 10) {
        last_button = 0;
        tft.drawRect(120, 408, 80, 50, messie_btn);
        TimerUpdateDraw(0);
      }
if (last_button == 22) {
    char OBJ_NAME_1[50];
    char obj_name[50];
    
    if (Summer_Time == 1) {
        Summer_Time = 0;
        if (!IS_NIGHTMODE) {
            safeStringCopy(OBJ_NAME_1, "UI/day/bg_time_2_off.bin", sizeof(OBJ_NAME_1));
        } else {
            safeStringCopy(OBJ_NAME_1, "UI/night/bg_time_2_off.bin", sizeof(OBJ_NAME_1));
        }
    } else {
        Summer_Time = 1;
        if (!IS_NIGHTMODE) {
            safeStringCopy(OBJ_NAME_1, "UI/day/bg_time_2_on.bin", sizeof(OBJ_NAME_1));
        } else {
            safeStringCopy(OBJ_NAME_1, "UI/night/bg_time_2_on.bin", sizeof(OBJ_NAME_1));
        }
    }
    
    if (SD.open(OBJ_NAME_1)) {
        drawBin(OBJ_NAME_1, 0, 415, 320, 65);
    }
    
    tft.setTextColor(btn_l_text);
    tft.setTextScale(3);
    last_button = 0;
}
    } else if (CURRENT_SCREEN == 4) {
      tft.setTextColor(btn_l_text);

      if (last_button == 1 && IS_BT_MODE_ON == false) {
        last_button = 0;

        if (IS_STEPPERS_ON == true) {
          drawLoadScreen();
        } else if (IS_STEPPERS_ON == false) {
          OnScreenMsg(5);
          delay(1000);
          CURRENT_SCREEN = 7;
          drawOptionsScreen();
        }
      }
      if (last_button == 4 && IS_BT_MODE_ON == false) {
        last_button = 0;
        drawSTATScreen();
      }

      if (last_button == 2 && IS_BT_MODE_ON == false && IS_STEPPERS_ON == true) {
        last_button = 0;
        IS_TRACKING = false;
        Timer3.stop();
        OnScreenMsg(6);
        safeStringCopy(OBJECT_NAME, "CP", sizeof(OBJECT_NAME));
        safeStringCopy(OBJECT_DESCR, "Celestial pole", sizeof(OBJECT_DESCR));
        OBJECT_RA_H = 12;
        OBJECT_RA_M = 0;
        OBJECT_DEC_D = 90;
        OBJECT_DEC_M = 0;
        IS_OBJ_FOUND = false;
        IS_OBJECT_RA_FOUND = false;
        IS_OBJECT_DEC_FOUND = false;
        Slew_timer = millis();
        Slew_RA_timer = Slew_timer + 5000;   // Give 20 sec. advance to the DEC. We will revise later.
        safeStringCopy(OBJECT_DETAILS, "The north and south celestial poles are the two imaginary points in the sky where the Earth's axis of rotation, intersects the celestial sphere", sizeof(OBJECT_DETAILS));
        sendTrackingStatus();
      }

      if (last_button == 2 && IS_BT_MODE_ON == false && IS_STEPPERS_ON == false) {
        last_button = 0;
        IS_TRACKING = false;
        Timer3.stop();
        OnScreenMsg(5);
        delay(1000);
        drawOptionsScreen();
        sendTrackingStatus();
      }

      if ((last_button == 5) || (last_button == 11)) {
        last_button = 0;
        if (MAIN_SCREEN_MENU == 0) {
          MAIN_SCREEN_MENU = 1;
        } else {
          MAIN_SCREEN_MENU = 0;
        }
        drawMainScreen_Menu(1);
      }
      if (last_button == 3) {
        last_button = 0;
        if (IS_STEPPERS_ON == true) {
          if (IS_TRACKING == false) {
            IS_TRACKING = true;
            if (!IS_NIGHTMODE) {
              drawBin("UI/day/btn_track_on.bin", 222, 375, 90, 44);
            } else {
              drawBin("UI/night/btn_track_on.bin", 222, 375, 90, 44);
            }
            tft.setTextScale(3);
            tft.fillRect(0, 116, 320, 27, BLACK);
            tft.setTextColor(btn_l_border);
            tft.cursorToXY(1, 119);
            if (strlen(OBJECT_NAME) > 7) {  
              tft.setTextScale(2);
              tft.cursorToXY(1, 129);
              tft.print("TRK:");
            } else {
              tft.setTextScale(3);
              tft.print("TRACKING:");
            }
            tft.setTextColor(title_bg);
            tft.print(OBJECT_NAME);
            setmStepsMode("R", MICROSteps);
            if (Tracking_type == 1) { // 1: Sidereal, 2: Solar, 0: Lunar;
              Timer3.start(Clock_Sidereal);
            } else if (Tracking_type == 2) {
              Timer3.start(Clock_Solar);
            } else if (Tracking_type == 0) {
              Timer3.start(Clock_Lunar);
            } sendTrackingStatus();
          } else {
            IS_TRACKING = false;
            if (!IS_NIGHTMODE) {
              drawBin("UI/day/btn_track_off.bin", 222, 375, 90, 44);
            } else {
              drawBin("UI/night/btn_track_off.bin", 222, 375, 90, 44);
            }
            tft.setTextScale(3);
            tft.fillRect(0, 116, 320, 25, BLACK);
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
            tft.setTextColor(title_bg);
            tft.print(OBJECT_NAME);
            //OnScreenMsg(2);
            setmStepsMode("R", 1);
            Timer3.stop(); //
            sendTrackingStatus();
          }
        }
        else if (IS_STEPPERS_ON == false) {
          if (!IS_NIGHTMODE) {
            drawBin("UI/day/btn_track_off.bin", 222, 375, 90, 44);
          } else {
            drawBin("UI/night/btn_track_off.bin", 222, 375, 90, 44);
          }
          OnScreenMsg(5);
          delay(1000);
          CURRENT_SCREEN = 7;
          drawOptionsScreen();
        }
      }

      if (last_button == 6) {   // BlueTooth Mode
        if (IS_STEPPERS_ON == true) {

          if (IS_BT_MODE_ON == true) {
            IS_BT_MODE_ON = false;
          } else {
            IS_BT_MODE_ON = true;
            // Initialize Bluetooth communication on PINs: 15 (RX) and 14 (TX)
            safeStringCopy(OBJECT_NAME, "BT", sizeof(OBJECT_NAME));
            drawBin("objects/BT.bin", 0, 143, 320, 142);
          }
          last_button = 0;
          drawMainScreen();
        }
        else if (IS_STEPPERS_ON == false) {
          last_button = 0;
          OnScreenMsg(5);
          delay(1000);
          CURRENT_SCREEN = 7;
          drawOptionsScreen();
        }
      }

      if (last_button == 7 && IS_BT_MODE_ON == false) {
        last_button = 0;
        if ((IS_TRACKING == true) && (IS_SOUND_ON)) {
          SoundOn(note_C, 32);
        }
        drawCoordinatesScreen();
      }
      if (last_button == 8 && IS_BT_MODE_ON == false) {
        last_button = 0;
        drawStarMap();
      }
      if (last_button == 9)
      {
        last_button = 0;
        if (IS_FAN1_ON)
        {
          IS_FAN1_ON = false;

          if (!IS_NIGHTMODE) {
            drawBin("UI/day/btn_fan1_off.bin", 222, 375, 90, 44);
          } else {
            drawBin("UI/night/btn_fan1_off.bin", 222, 375, 90, 44);
          }
          digitalWrite(FAN1, LOW);
          safeStringCopy(Fan1_State, "OFF", sizeof(Fan1_State));
          drawStatusBar();
        }
        else
        {
          IS_FAN1_ON = true;
          if (!IS_NIGHTMODE) {
            drawBin("UI/day/btn_fan1_on.bin", 222, 375, 90, 44);
          } else {
            drawBin("UI/night/btn_fan1_on.bin", 222, 375, 90, 44);
          }
          digitalWrite(FAN1, HIGH);
          safeStringCopy(Fan1_State, "ON", sizeof(Fan1_State));
          drawStatusBar();
        }
        storeOptions_SD();
      }
      if (last_button == 10)
      {
        last_button = 0;
        drawOptionsScreen();
      }
      if (last_button == 12)
      {
        last_button = 0;
        if (IS_GTMEM_ON)
        {
          IS_GTMEM_ON = false;
          if (!IS_NIGHTMODE) {
            drawBin("UI/day/btn_gotoLast_off_st.bin", 222, 428, 90, 44);
          } else {
            drawBin("UI/night/btn_gotoLast_off_st.bin", 222, 428, 90, 44);
          }

//add JG goto last mem pos

  OBJECT_DEC_D = mem_DEC_D;
  OBJECT_DEC_M = mem_DEC_M;
  OBJECT_RA_H = mem_RA_H;
  OBJECT_RA_M = mem_RA_M;
  tft.fillRect(0, 116, 320, 25, BLACK);
  safeStringCopy(OBJECT_NAME, "GOTO back", sizeof(OBJECT_NAME));
  safeStringCopy(OBJECT_DESCR, "", sizeof(OBJECT_DESCR));
  safeStringCopy(OBJECT_DETAILS, "Telescope return to the last stored position", sizeof(OBJECT_DETAILS));
  calculateLST_HA();
  if (ALT > 0) {
  OnScreenMsg(1);
  if (IS_SOUND_ON) {
  SoundOn(note_C, 32);
  delay(200);
  SoundOn(note_C, 32);
  delay(200);
  SoundOn(note_C, 32);
  delay(1000);
  }
// Stop Interrupt procedure for tracking.
  Timer3.stop(); //
  IS_TRACKING = false;
  IS_OBJ_FOUND = false;
  IS_OBJECT_RA_FOUND = false;
  IS_OBJECT_DEC_FOUND = false;
  Tracking_type = 1; // Preparar el seguimiento que se usara, 1: Sidereal, 2: Solar, 0: Lunar;
  safeStringCopy(Tracking_Mode, "Celest", sizeof(Tracking_Mode));
  sendTrackingStatus();
  Slew_timer = millis();
  Slew_RA_timer = Slew_timer + 5000;   // Give 20 sec. advance to the DEC. We will revise later.
  }
  sun_confirm = false;
  drawMainScreen();
  safeStringCopy(GTMEM_State, "Free", sizeof(GTMEM_State));
          drawStatusBar();
        }
        else
        {
          IS_GTMEM_ON = true;
          if (!IS_NIGHTMODE) {
            drawBin("UI/day/btn_gotoLast_on_gt.bin", 222, 428, 90, 44);
          } else {
            drawBin("UI/night/btn_gotoLast_on_gt.bin", 222, 428, 90, 44);
          }
  Current_RA_DEC();
  mem_DEC_D = curr_DEC_D;
  mem_DEC_M = curr_DEC_M;
  mem_RA_H = curr_RA_H;
  mem_RA_M = curr_RA_M;
  safeStringCopy(GTMEM_State, "ARM", sizeof(GTMEM_State));
  drawStatusBar();
        }
      }

if (last_button == 1 && IS_BT_MODE_ON == false) {
  last_button = 0;
  
  if (IS_STEPPERS_ON == true) {
    drawLoadMenu();
  } else if (IS_STEPPERS_ON == false) {
    OnScreenMsg(5);
    delay(1000);
    CURRENT_SCREEN = 7;
    drawOptionsScreen();
  }
}
    }
  }
}
