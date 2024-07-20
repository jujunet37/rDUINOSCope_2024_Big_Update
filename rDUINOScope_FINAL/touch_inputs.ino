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

        String Date_q = String(rtc.getDateStr());
        String Time_q = String(rtc.getTimeStr());
        int D = Date_q.substring(0, 2).toInt();
        int M = Date_q.substring(3, 5).toInt();
        int Y = Date_q.substring(6).toInt();
        int H = Time_q.substring(0, 2).toInt();
        int MN = Time_q.substring(3, 5).toInt();
        int S = Time_q.substring(6).toInt();
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

        Start_date = String(rtc.getDateStr()).substring(0, 2) + " " + rtc.getMonthStr(FORMAT_SHORT) + " " + String(rtc.getDateStr()).substring(6);
        old_d = rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/');
        START_TIME = rtc.getTimeStr(FORMAT_SHORT);
        _Stemp = dht.readTemperature();;
        _Shumid = dht.readHumidity();

        drawSelectAlignment();
        //drawStarSyncScreen();
        //drawMainScreen();
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
        tft.drawRect(40, 89, 241, 69, btn_l_border);   //x 40>281 y 89>158
        ALLIGN_TYPE = 1;
        drawStarSyncScreen();
      }
      else if (lx > 40 && lx < 281 && ly > 194 && ly < 262)
      {
        // BTN "Iterative Alignment" pressed
        tft.drawRect(40, 194, 241, 68, btn_l_border);  //x 40>281 y 194>262
        ALLIGN_TYPE = 3;

        // I'll take some time to Initialize the Sub ARRAY with suitable stars (only for Northern Hemi)
        tft.cursorToXY(0, 5);
        int cc = 0;
        for (int t = 0; t < 203; t++)
        {
          int i1 = Stars[t].indexOf(';');
          int i2 = Stars[t].indexOf(';', i1 + 1);
          int i3 = Stars[t].indexOf(';', i2 + 1);
          OBJECT_NAME = Stars[t].substring(i1 + 1, i2) + " from " + Stars[t].substring(0, i1);
          String OBJ_RA = Stars[t].substring(i2 + 1, i3);
          OBJECT_RA_H = OBJ_RA.substring(0, OBJ_RA.indexOf('h')).toFloat();
          OBJECT_RA_M = OBJ_RA.substring(OBJ_RA.indexOf('h') + 1, OBJ_RA.length() - 1).toFloat();
          float ts_RA = OBJECT_RA_H + OBJECT_RA_M / 100;
          String OBJ_DEC = Stars[t].substring(i3, Stars[t].length());
          String sign = OBJ_DEC.substring(0, 1);
          OBJECT_DEC_D = OBJ_DEC.substring(1, OBJ_DEC.indexOf('°')).toFloat();
          if (sign == "-")
          {
            OBJECT_DEC_D *= (-1);
          }
          OBJECT_DEC_M = 0;
          if (ts_RA > 5.3 && ts_RA < 23.3)
          {
            calculateLST_HA();
            if (AZ > 90 && AZ < 270 && ALT > 20)
            {
              Iter_Stars[cc] = Stars[t];
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
      } else if (lx > 67 && lx < 262 && ly > 414 && ly < 480) {
        // BTN "Skip Alignment" pressed
        tft.drawRect(67, 414, 195, 50, btn_l_border);  //x 67>262 y 414>480
        IS_IN_OPERATION = true;
        drawMainScreen();
      }
    }

    ////////////////////////////////////////////////// Main Screen Touch Actions ///////////////////////////////////////////////////////
    else if (CURRENT_SCREEN == 4)
    { // captures touches on drawMainScreen()
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
        // BTN 1 pressed
        if (MAIN_SCREEN_MENU == 0) {
          last_button = 1;
        } else {
          last_button = 7;
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
      if (lx > 220 && lx < 320 && ly > 420 && ly < 480) {
        // BTN next> pressed  TREAS_PAGER
        if (LOAD_SELECTOR == 2) { Bright_PAGER += 1; CAT_NAME = "Bright_CAT.csv"; if (Bright_PAGER <= 8) { OBJ_PAGER = Bright_PAGER; drawLoadObjects(); } else { Bright_PAGER = 8;  OBJ_PAGER = Bright_PAGER; }} // JG modif for independant pagers that store last catalog position. But one general pager (OBJ_PAGER) that launch goto.
        else if (LOAD_SELECTOR == 3) { Double_PAGER += 1; CAT_NAME = "Double_CAT.csv"; if (Double_PAGER <= 7) { OBJ_PAGER = Double_PAGER; drawLoadObjects(); } else { Double_PAGER = 7; OBJ_PAGER = Double_PAGER;}}
        else if (LOAD_SELECTOR == 4) { Variable_PAGER += 1; CAT_NAME = "Variable_CAT.csv"; if (Variable_PAGER <= 6) {  OBJ_PAGER = Variable_PAGER; drawLoadObjects(); } else { Variable_PAGER = 6; OBJ_PAGER = Variable_PAGER;}}
        else if (LOAD_SELECTOR == 6) { MESS_PAGER += 1; CAT_NAME = "messier.csv"; if (MESS_PAGER <= 5) { OBJ_PAGER = MESS_PAGER; drawLoadObjects(); } else { MESS_PAGER = 5; OBJ_PAGER = MESS_PAGER;}}
        else if (LOAD_SELECTOR == 7) { NGC_PAGER1 += 1; if (NGC_PAGER1 <= 11) { CAT_NAME = "NGC_CAT1_10.csv"; } else { CAT_NAME = "NGC_CAT2_10.csv"; } if (NGC_PAGER1 <= 23) { OBJ_PAGER = NGC_PAGER1; drawLoadObjects(); } else { NGC_PAGER1 = 23; OBJ_PAGER = NGC_PAGER1;}}
        else if (LOAD_SELECTOR == 8) { NGC_PAGER2 += 1; if (NGC_PAGER2 <= 11) { CAT_NAME = "NGC_CAT3_10.csv"; } else { CAT_NAME = "NGC_CAT4_10.csv"; } if (NGC_PAGER2 <= 23) { OBJ_PAGER = NGC_PAGER2; drawLoadObjects(); } else { NGC_PAGER2 = 23; OBJ_PAGER = NGC_PAGER2;}}
        else if (LOAD_SELECTOR == 9) { NGC_PAGER3 += 1; if (NGC_PAGER3 <= 11) { CAT_NAME = "NGC_CAT5_10.csv"; } else { CAT_NAME = "NGC_CAT6_10.csv"; } if (NGC_PAGER3 <= 23) { OBJ_PAGER = NGC_PAGER3; drawLoadObjects(); } else { NGC_PAGER3 = 23; OBJ_PAGER = NGC_PAGER3;}}
        else if (LOAD_SELECTOR == 11) { NGC_PAGER4 += 1; if (NGC_PAGER4 <= 11) { CAT_NAME = "NGC_CAT7_10.csv"; } else { CAT_NAME = "NGC_CAT8_10.csv"; } if (NGC_PAGER4 <= 23) { OBJ_PAGER = NGC_PAGER4; drawLoadObjects(); } else { NGC_PAGER4 = 23; OBJ_PAGER = NGC_PAGER4;}}
        else if (LOAD_SELECTOR == 12) { NGC_PAGER5 += 1; if (NGC_PAGER5 <= 11) { CAT_NAME = "NGC_CAT9_10.csv"; } else { CAT_NAME = "NGC_CAT10_10.csv"; } if (NGC_PAGER5 <= 23) { OBJ_PAGER = NGC_PAGER5; drawLoadObjects(); } else { NGC_PAGER5 = 23; OBJ_PAGER = NGC_PAGER5;}}
        else if (LOAD_SELECTOR == 13) { IC_PAGER += 1; CAT_NAME = "IC_CAT.csv"; if (IC_PAGER <= 9) { OBJ_PAGER = IC_PAGER; drawLoadObjects(); } else { IC_PAGER = 9; OBJ_PAGER = IC_PAGER; }}
        else if (LOAD_SELECTOR == 14) { CUSTOM_PAGER += 1; CAT_NAME = "custom_CAT.csv"; if (CUSTOM_PAGER <= 11) { OBJ_PAGER = CUSTOM_PAGER; drawLoadObjects(); } else { CUSTOM_PAGER = 11; OBJ_PAGER = CUSTOM_PAGER;}}
      }
      if (lx > 0 && lx < 100 && ly > 420 && ly < 480) {
        // BTN <prev pressed
        if (LOAD_SELECTOR == 2) { Bright_PAGER -= 1; CAT_NAME = "Bright_CAT.csv"; if (Bright_PAGER >= 0) { OBJ_PAGER = Bright_PAGER; drawLoadObjects(); } else {Bright_PAGER = 0; OBJ_PAGER = Bright_PAGER;}}
        else if (LOAD_SELECTOR == 3) { Double_PAGER -= 1; CAT_NAME = "Double_CAT.csv"; if (Double_PAGER >= 0) { OBJ_PAGER = Double_PAGER; drawLoadObjects(); } else {Double_PAGER = 0; OBJ_PAGER = Double_PAGER;}}
        else if (LOAD_SELECTOR == 4) { Variable_PAGER -= 1; CAT_NAME = "Variable_CAT.csv"; if (Variable_PAGER >= 0) { OBJ_PAGER = Variable_PAGER; drawLoadObjects(); } else {Variable_PAGER = 0; OBJ_PAGER = Variable_PAGER;}}
        else if (LOAD_SELECTOR == 6) { MESS_PAGER -= 1; CAT_NAME = "messier.csv"; if (MESS_PAGER >= 0) { OBJ_PAGER = MESS_PAGER; drawLoadObjects(); } else {MESS_PAGER = 0; OBJ_PAGER = MESS_PAGER;}}
        else if (LOAD_SELECTOR == 7) { NGC_PAGER1 -= 1; if (NGC_PAGER1 <= 11) { CAT_NAME = "NGC_CAT1_10.csv"; } else { CAT_NAME = "NGC_CAT2_10.csv"; } if (NGC_PAGER1 >= 0) { OBJ_PAGER = NGC_PAGER1; drawLoadObjects(); } else { NGC_PAGER1 = 0; OBJ_PAGER = NGC_PAGER1;}}
        else if (LOAD_SELECTOR == 8) { NGC_PAGER2 -= 1; if (NGC_PAGER2 <= 11) { CAT_NAME = "NGC_CAT3_10.csv"; } else { CAT_NAME = "NGC_CAT4_10.csv"; } if (NGC_PAGER2 >= 0) { OBJ_PAGER = NGC_PAGER2; drawLoadObjects(); } else { NGC_PAGER2 = 0; OBJ_PAGER = NGC_PAGER2;}}
        else if (LOAD_SELECTOR == 9) { NGC_PAGER3 -= 1; if (NGC_PAGER3 <= 11) { CAT_NAME = "NGC_CAT5_10.csv"; } else { CAT_NAME = "NGC_CAT6_10.csv"; } if (NGC_PAGER3 >= 0) { OBJ_PAGER = NGC_PAGER3; drawLoadObjects(); } else { NGC_PAGER3 = 0; OBJ_PAGER = NGC_PAGER3;}}
        else if (LOAD_SELECTOR == 11) { NGC_PAGER4 -= 1; if (NGC_PAGER4 <= 11) { CAT_NAME = "NGC_CAT7_10.csv"; } else { CAT_NAME = "NGC_CAT8_10.csv"; } if (NGC_PAGER4 >= 0) { OBJ_PAGER = NGC_PAGER4; drawLoadObjects(); } else { NGC_PAGER4 = 0; OBJ_PAGER = NGC_PAGER4;}}
        else if (LOAD_SELECTOR == 12) { NGC_PAGER5 -= 1; if (NGC_PAGER5 <= 11) { CAT_NAME = "NGC_CAT9_10.csv"; } else { CAT_NAME = "NGC_CAT10_10.csv"; } if (NGC_PAGER5 >= 0) { OBJ_PAGER = NGC_PAGER5; drawLoadObjects(); } else { NGC_PAGER5 = 0; OBJ_PAGER = NGC_PAGER5;}}
        else if (LOAD_SELECTOR == 13) { IC_PAGER -= 1; CAT_NAME = "IC_CAT.csv"; if (IC_PAGER >= 0) { OBJ_PAGER = IC_PAGER; drawLoadObjects(); } else {IC_PAGER = 0; OBJ_PAGER = IC_PAGER;}}
        else if (LOAD_SELECTOR == 14) { CUSTOM_PAGER -= 1;  CAT_NAME = "custom_CAT.csv"; if (CUSTOM_PAGER >= 0) { OBJ_PAGER = CUSTOM_PAGER; drawLoadObjects(); } else {CUSTOM_PAGER = 0; OBJ_PAGER = CUSTOM_PAGER;}}
      }

  // Add JG

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

  // End add JG
      
      char obj_name[50];
      String OBJ_NAME = "";
      if (!IS_NIGHTMODE) {
        OBJ_NAME = "UI/day/";
      } else {
        OBJ_NAME = "UI/night/";
      }

      if (lx > 0 && lx < 80 && ly > 60 && ly < 130)
      {
        // Modif JG : 1st of 4 buttons pressed
        // Add JG : modification to load three menus and corresponding .csv file

        if (CAT_Page == 1) { 
        LOAD_SELECTOR = 1;
        OBJ_NAME += "btn_load_1.bin";
        
        } if (CAT_Page == 2) { 
        LOAD_SELECTOR = 6;
        CAT_NAME = "messier.csv";
        OBJ_NAME += "btn_load_1b.bin";
        OBJ_PAGER = MESS_PAGER; 
        } if (CAT_Page == 3) {
        LOAD_SELECTOR = 11;  
        if (NGC_PAGER4 <= 11) { CAT_NAME = "NGC_CAT7_10.csv"; } else {CAT_NAME = "NGC_CAT8_10.csv"; } // Add JG for 2 x 240 items catalogs on one objects menu.
        OBJ_NAME += "btn_load_1c.bin";
        OBJ_PAGER = NGC_PAGER4; 
        }
        
        if (SD.open(OBJ_NAME)/* && (IS_TRACKING)*/)
        {
          OBJ_NAME.toCharArray(obj_name, 50);
          drawBin(obj_name, 0, 60, 320, 80); // Drawing selected Object Tumbnail 140*140 Pixels
        }
        drawLoadObjects();
    } // Modif JG : 2nd of 4 buttons pressed
      if (lx > 81 && lx < 161 && ly > 60 && ly < 130)
      {
        if (CAT_Page == 1) { 
        LOAD_SELECTOR = 2;
        CAT_NAME = "Bright_CAT.csv";
        OBJ_NAME += "btn_load_2.bin";
        OBJ_PAGER = Bright_PAGER;
        } if (CAT_Page == 2) { 
        LOAD_SELECTOR = 7; 
        if (NGC_PAGER1 <= 11) { CAT_NAME = "NGC_CAT1_10.csv"; } else {CAT_NAME = "NGC_CAT2_10.csv"; } // Add JG for 2 x 240 items catalogs on one objects menu.
        OBJ_NAME += "btn_load_2b.bin"; 
        OBJ_PAGER = NGC_PAGER1;
        } if (CAT_Page == 3) {
        LOAD_SELECTOR = 12; 
        if (NGC_PAGER5 <= 11) { CAT_NAME = "NGC_CAT9_10.csv"; } else {CAT_NAME = "NGC_CAT10_10.csv"; } // Add JG for 2 x 240 items catalogs on one objects menu.
        OBJ_NAME += "btn_load_2c.bin"; 
        OBJ_PAGER = NGC_PAGER5;
        }
        
        if (SD.open(OBJ_NAME)/* && (IS_TRACKING)*/)
        {
          OBJ_NAME.toCharArray(obj_name, 50);
          drawBin(obj_name, 0, 60, 320, 80); // Drawing selected Object Tumbnail 140*140 Pixels
        }
        drawLoadObjects();
      } // Modif JG : 3rd of 4 buttons pressed
      if (lx > 162 && lx < 237 && ly > 60 && ly < 130)
      {
        // BTN Solar System pressed
        if (CAT_Page == 1) { 
        LOAD_SELECTOR = 3;
        CAT_NAME = "Double_CAT.csv";
        OBJ_NAME += "btn_load_3.bin";
        OBJ_PAGER = Double_PAGER;
        } if (CAT_Page == 2){
        LOAD_SELECTOR = 8; 
        if (NGC_PAGER2 <= 11) { CAT_NAME = "NGC_CAT3_10.csv"; } else {CAT_NAME = "NGC_CAT4_10.csv"; } // Add JG for 2 x 240 items catalogs on one objects menu.
        OBJ_NAME += "btn_load_3b.bin"; 
        OBJ_PAGER = NGC_PAGER2;
        } if (CAT_Page == 3) {
        LOAD_SELECTOR = 13; 
        CAT_NAME = "IC_CAT.csv";
        OBJ_NAME += "btn_load_3c.bin";  
        OBJ_PAGER = IC_PAGER; 
        }
        
        if (SD.open(OBJ_NAME)/* && (IS_TRACKING)*/)
        {
          OBJ_NAME.toCharArray(obj_name, 50);
          drawBin(obj_name, 0, 60, 320, 80); // Drawing selected Object Tumbnail 140*140 Pixels
        }
        drawLoadObjects();
      } // Modif JG : 4rd of 4 buttons pressed
      if (lx > 238 && lx < 318 && ly > 60 && ly < 130)
      {
        // BTN custom.csv pressed
        if (CAT_Page == 1) { 
        LOAD_SELECTOR = 4;
        CAT_NAME = "Variable_CAT.csv";
        OBJ_NAME += "btn_load_4.bin";
        OBJ_PAGER = Variable_PAGER;
        } if (CAT_Page == 2 ) {
        LOAD_SELECTOR = 9; 
        if (NGC_PAGER3 <= 11) { CAT_NAME = "NGC_CAT5_10.csv"; } else {CAT_NAME = "NGC_CAT6_10.csv"; } // Add JG for 2 x 240 items catalogs on one objects menu.
        OBJ_NAME += "btn_load_4b.bin";
        OBJ_PAGER = NGC_PAGER3;
        } if (CAT_Page == 3) {
        LOAD_SELECTOR = 14; 
        CAT_NAME = "Custom_CAT.csv";
        OBJ_NAME += "btn_load_4c.bin";
        OBJ_PAGER = CUSTOM_PAGER;
        }
        
        if (SD.open(OBJ_NAME)/* && (IS_TRACKING)*/)
        {
          OBJ_NAME.toCharArray(obj_name, 50);
          drawBin(obj_name, 0, 60, 320, 80); // Draw Selection Button Thumbnail
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
              if (OBJ_Array[zz] != "")
              {
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
                  Tracking_Mode = "Celest";
                  Slew_timer = millis();
                  Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
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
                        Tracking_Mode = "Solar";
                        break;
                      case 10:
                        Tracking_type = 0; // Preparar el seguimiento que se usara, 1: Sidereal, 2: Solar, 0: Lunar;
                        Tracking_Mode = "Lunar";
                        break;
                      default:
                        Tracking_type = 1; // Preparar el seguimiento que se usara, 1: Sidereal, 2: Solar, 0: Lunar;
                        Tracking_Mode = "Celest";
                        break;
                    }
                    Slew_timer = millis();
                    Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
                  }
                  UpdateObservedObjects();
                  //CUSTOM_PAGER == 0;
                  sun_confirm = false;
                  drawMainScreen();
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
        Tracking_Mode = "Celest";
        updateTrackingMode_opt();
      }
      if (lx > 211 && lx < 308 && ly > 370 && ly < 419) {
        // Solar Tracking Selected
        Tracking_type = 2;
        Tracking_Mode = "Solar";
        updateTrackingMode_opt();
      }
      if (lx > 211 && lx < 308 && ly > 418 && ly < 467) {
        // Lunar Tracking Selected
        Tracking_type = 0;
        Tracking_Mode = "Lunar";
        updateTrackingMode_opt();
      }
      if (lx > 259 && lx < 310 && ly > 54 && ly < 99 )
      {
        // Screen MAX Brightness
        analogWrite(TFTBright, 255);
        TFT_Brightness = 255;
        updateTriangleBrightness_opt();
      }
      if (lx > 5 && lx < 255 && ly > 50 && ly < 100) {
        // Screen REDUCE Brightness   "Triangle"
        TFT_Brightness = 190 + ((lx - 5) * 0.25);
        analogWrite(TFTBright, TFT_Brightness);
        updateTriangleBrightness_opt();
      }
      
//add JG      
      
      
      if (lx > 3 && lx < 108 && ly > 275 && ly < 355) {
        // Touch Screen Calibration
      drawScreenCalibration();
      if (interx != 0.00 && intery != 0.00) {
      DrawButton(60, 270, 220, 50, "Reset Values", 0, btn_d_border, btn_d_border, 2, false);
      }        
      }

//fin add JG
      
      if (lx > 3 && lx < 56 && ly > 221 && ly < 269) {
        // ECO Mode - Timeout in seconds  Never
        TFT_timeout = 0;
        TFT_Time = "AL-ON";
        updateScreenTimeout_opt();
      }
      if (lx > 56 && lx < 108 && ly > 221 && ly < 269) {
        // ECO Mode - Timeout in seconds  30 Seconds
        TFT_timeout = 30000;
        TFT_Time = "30 s";
        updateScreenTimeout_opt();
      }
      if (lx > 108 && lx < 161 && ly > 221 && ly < 269) {
        // ECO Mode - Timeout in seconds   60 Seconds
        TFT_timeout = 60000;
        TFT_Time = "60 s";
        updateScreenTimeout_opt();
      }
      if (lx > 160 && lx < 210 && ly > 221 && ly < 269) {
        // ECO Mode - Timeout in seconds   2 Minutes
        TFT_timeout = 120000;
        TFT_Time = "2 min";
        updateScreenTimeout_opt();
      }
      if (lx > 212 && lx < 263 && ly > 221 && ly < 269) {
        // ECO Mode - Timeout in seconds  5 Minutes
        TFT_timeout = 300000;
        TFT_Time = "5 min";
        updateScreenTimeout_opt();
      }
      if (lx > 265 && lx < 318 && ly > 221 && ly < 269) {
        // ECO Mode - Timeout in seconds  10 Minutes
        TFT_timeout = 600000;
        TFT_Time = "10 min";
        updateScreenTimeout_opt();
      }
      if (lx > 111 && lx < 204 && ly > 323 && ly < 371) {
        // ON Meridian Flip
        IS_MERIDIAN_FLIP_AUTOMATIC = true;
        Mer_Flip_State = "AUTO";
        updateMeridianFlip_opt();
      }
      if (lx > 111 && lx < 204 && ly > 371 && ly < 419) {
        // OFF Meridian Flip
        IS_MERIDIAN_FLIP_AUTOMATIC = false;
        Mer_Flip_State = "OFF";
        updateMeridianFlip_opt();
      }
      if (lx > 53 && lx < 96 && ly > 162 && ly < 190) {
        // ON Sound
        IS_SOUND_ON = true;
        Sound_State = "ON";
        updateSound_opt();
      }
      if (lx > 1 && lx < 52 && ly > 162 && ly < 190) {
        // OFF Sound
        IS_SOUND_ON = false;
        Sound_State = "OFF";
        updateSound_opt();
      }
      if (lx > 160 && lx < 204 && ly > 162 && ly < 190)
      {
        // ON Stepper Motors
        IS_STEPPERS_ON = true;
        digitalWrite(RA_EN, LOW);
        digitalWrite(DEC_EN, LOW);
        Stepper_State = "ON";
        updateStepper_opt();
      }
      if (lx > 115 && lx < 159 && ly > 162 && ly < 190)
      {
        // OFF Stepper Motors
        IS_STEPPERS_ON = false;
        digitalWrite(RA_EN, HIGH);
        digitalWrite(DEC_EN, HIGH);
        Stepper_State = "OFF";
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
              if (Stars[zz] != "")
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
                  Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
                  STARS_PAGER == 0;
                  SELECTED_STAR = zz;
                  ALLIGN_STEP += 1;
                  //drawConstelationScreen(zz);
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
              int zz = (STARS_PAGER * 24) + (i * 3) + j;
              if (Iter_Stars[zz] != "")
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
                Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
                STARS_PAGER == 0;
                SELECTED_STAR = zz;
                ALLIGN_STEP += 1;
                // drawConstelationScreen(zz);
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
            Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
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
            Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
            //drawConstelationScreen(0);
            ALLIGN_STEP = 1;
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
            Tracking_Mode = "Solar";
            updateTrackingMode_opt();
            Slew_timer = millis();
            Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
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
        String OBJ_NAME_1 = "";
        char obj_name[50];
        if (Summer_Time == 1) {
          Summer_Time = 0;
          if (!IS_NIGHTMODE) {
            OBJ_NAME_1 = "UI/day/bg_time_2_off.bin";
          } else {
            OBJ_NAME_1 = "UI/night/bg_time_2_off.bin";
          }
        } else {
          Summer_Time = 1;
          if (!IS_NIGHTMODE) {
            OBJ_NAME_1 = "UI/day/bg_time_2_on.bin";
          } else {
            OBJ_NAME_1 = "UI/night/bg_time_2_on.bin";
          }
        }
        if (SD.open(OBJ_NAME_1)) {
          OBJ_NAME_1.toCharArray(obj_name, 50);
          drawBin(obj_name, 0, 415, 320, 65); // Drawing selected Object Tumbnail 140*140 Pixels
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
          //tft.fillRect(1, 325, 100, 70, BLACK);
          //DrawButton( 1, 325, 100, 70, "LOAD", btn_d_border, btn_l_border, btn_l_text, 2, false);
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
        OBJECT_NAME = "CP";
        OBJECT_DESCR = "Celestial pole";
        OBJECT_RA_H = 12;
        OBJECT_RA_M = 0;
        OBJECT_DEC_D = 90;
        OBJECT_DEC_M = 0;
        IS_OBJ_FOUND = false;
        IS_OBJECT_RA_FOUND = false;
        IS_OBJECT_DEC_FOUND = false;
        Slew_timer = millis();
        Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
        OBJECT_DETAILS = "The north and south celestial poles are the two imaginary points in the sky where the Earth's axis of rotation, intersects the celestial sphere";
      }

      if (last_button == 2 && IS_BT_MODE_ON == false && IS_STEPPERS_ON == false) {
        last_button = 0;
        IS_TRACKING = false;
        Timer3.stop();
        OnScreenMsg(5);
        delay(1000);
        drawOptionsScreen();
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
            tft.fillRect(0, 116, 320, 25, BLACK);
            tft.setTextColor(btn_l_border);
            tft.cursorToXY(1, 119);
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
            setmStepsMode("R", MICROSteps);
            if (Tracking_type == 1) { // 1: Sidereal, 2: Solar, 0: Lunar;
              Timer3.start(Clock_Sidereal);
            } else if (Tracking_type == 2) {
              Timer3.start(Clock_Solar);
            } else if (Tracking_type == 0) {
              Timer3.start(Clock_Lunar);
            }
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
            //OnScreenMsg(2);
            setmStepsMode("R", 1);
            Timer3.stop(); //
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
            OBJECT_NAME = "BT";
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
          Fan1_State = "OFF";
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
          Fan1_State = "ON";
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
  OBJECT_NAME = "GOTO back";
  OBJECT_DESCR = "";
  OBJECT_DETAILS = "Telescope return to the last stored position";
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
  Tracking_Mode = "Celest";
  Slew_timer = millis();
  Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
  }
  sun_confirm = false;
  drawMainScreen();
          GTMEM_State = "Free";
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
          GTMEM_State = "ARM";
          drawStatusBar();

// end add JG
          
        }
        //storeOptions_SD();
      }
    }
  }
}
