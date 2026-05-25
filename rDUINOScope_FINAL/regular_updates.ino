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
//  - Measures Temperature and Humidity;
//  - On each TimeInterval (~10 sec.) - Updates screens accordingly (e.g. Time, LST, Temperature, Humidity and etc.)
//

///////////////////////////////////////////////////// Temperature Updates ///////////////////////////////////////////////////////
void considerTempUpdates() // Temperature && Humidity Updates && battery voltage
{
  if (CURRENT_SCREEN == 4 && (millis() - Tupdate_time) > 30000)
  {
    float tHum = dht.readHumidity();
    float tTemp = dht.readTemperature();
    if (isnan(tHum) || isnan(tTemp))
    {
      return;
    }
    else
    {
      _temp = tTemp-(-0.0866*tTemp+0.8264); // Add JG Julien GICQUIAUD (correction for my DHT22 vs laboratory thermometer)
      _humid = tHum-1.74; // Add Julien GICQUIAUD (correction for my DHT22 vs laboratory thermohygrometer)
    }
    tft.setTextScale(2);
    if (_temp > -75 && _temp < 75 && _humid < 100 && _humid > 0)
    {
      tft.setFontMode(gTextFontModeSolid);// Set font mode to Solid (Highlight Eraser Color)
      tft.setTextColor(Time_area_font, Time_area_back); //Button_Title
     
// Add JG modif Add Dew point
      
      tft.cursorToXY(251, 30);
      tft.print(_temp, 1);
      tft.cursorToXY(251, 52);
      tft.print(_humid, 1);
      tft.cursorToXY(201, 74);
      tft.print("DWP:");
      tft.cursorToXY(251, 74);
      tft.print((237.7*(log(_humid/100)+(17.27*_temp)/(237.7+_temp)))/(17.27-(log((_humid/100))+(17.27*_temp)/(237.7+_temp))), 1);

// End add JG Julien modif Add Dew point  
    
    }
    Tupdate_time = millis();

#ifdef use_battery_level
    drawBatteryLevel(260, 335, calculateBatteryLevel());
#endif
  }
  tft.setFontMode(gTextFontModeTransparent);// Set font mode to Solid (Highlight Eraser Color)
}

///////////////////////////////////////////////////// Time Updates ///////////////////////////////////////////////////////
void considerTimeUpdates()
{ // UPDATEs time on Screen1 && Screen4 -  Clock Screen and Main Screen
  int changes = 0;
  for (int y = 0; y < 12; y++)
  {
    if (w_DateTime[y] != 0)
    {
      changes = 1;
    }
  }
  int mer_flp_sound = HAHour + ((HAMin + MIN_TO_MERIDIAN_FLIP + MIN_SOUND_BEFORE_FLIP) / 60);
  if ((millis() - update_time) > 10000 && mer_flp_sound >= 24)
  {
    if (IS_SOUND_ON)
    {
      SoundOn(3830, 8);
    }
  }
  if (CURRENT_SCREEN == 4 && (millis() - update_time) > 10000)
  {

    if (old_d != rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/'))
    {
      if (!IS_NIGHTMODE) {
        drawBin("UI/day/status_bar_new.bin", 0, 89, 320, 27); // Drawing selected Object Tumbnail 140*140 Pixels
      } else {
        drawBin("UI/night/status_bar_new.bin", 0, 89, 320, 27); // Drawing selected Object Tumbnail 140*140 Pixels
      }
      tft.setFontMode(gTextFontModeTransparent);// Set font mode to Transparent (NO Highlight Color)
      tft.setTextScale(1);
      tft.setTextColor(l_text);
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
      tft.setTextColor(l_text);
      tft.print(" ALT:");
      tft.setTextColor(btn_l_border);
      tft.print(OBSERVATION_ALTITUDE, 0);
      tft.setFontMode(gTextFontModeTransparent);// Set font mode to Solid (Highlight Eraser Color)
    }
    tft.setFontMode(gTextFontModeSolid);// Set font mode to Solid (Highlight Eraser Color)
    tft.setTextColor(Time_area_font, Time_area_back); //Button_Title
    tft.setTextScale(3);
    tft.cursorToXY(88, 35);
    tft.print(String(rtc.getTimeStr()).substring(0, 5));

    tft.fillRect(86, 64, 115, 22, Time_area_back);
    tft.cursorToXY(88, 65);
    if ((int)LST < 10)
    {
      tft.print("0");
      tft.setTextScale(1);
      tft.print(" ");
      tft.setTextScale(3);
      tft.print((int)LST);
    }
    else
    {
      tft.print((int)LST);
    }
    tft.setTextScale(3);
    tft.print(":");
    if ((LST - (int)LST) * 60 < 10)
    {
      tft.print("0");
      tft.setTextScale(1);
      tft.print(" ");
      tft.setTextScale(3);
      tft.print((LST - (int)LST) * 60, 0);
    }
    else
    {
      tft.print((LST - (int)LST) * 60, 0);
    }
    if ((OBJECT_NAME != "") && (OBJECT_NAME != "CP") && (IS_BT_MODE_ON == false))
    {
      /*
        if (TRACKING_MOON)
        {
        planet_pos(10);
        }
      */
      tft.setFontMode(gTextFontModeSolid);// Set font mode to Solid
      tft.setTextScale(2);
      tft.setTextColor(title_bg, BLACK);
      tft.cursorToXY(48, 215);
      tft.print(HAHour, 0);
      tft.print("h ");
      tft.print(HAMin, 1);
      tft.print("m");
      tft.cursorToXY(48, 240);
      tft.print(floor(ALT), 0);
      tft.print("^ ");
      tft.print((ALT - floor(ALT)) * 60, 0);
      tft.print("'");
      tft.cursorToXY(48, 265);
      tft.print(floor(AZ), 0);
      tft.print("^ ");
      tft.print((AZ - floor(AZ)) * 60, 0);
      tft.setFontMode(gTextFontModeTransparent);// Set font mode to transparent (No Highlight)
    }

    if ((IS_BT_MODE_ON == true) && (OBJECT_DESCR == "Pushed via BlueTooth")) {
      tft.setTextScale(2);
      tft.setFontMode(gTextFontModeSolid);// Set font mode to Solid (With Highlight)
      tft.setTextColor(l_text, BLACK);
      tft.cursorToXY(0, 215);
      tft.println("HA :");

      //tft.fillRect(5, 255, 230, 22, BLACK);
      tft.setTextColor(title_bg, BLACK);
      tft.cursorToXY(48, 215);
      tft.print(HAHour, 0);
      tft.print("h ");
      tft.print(HAMin, 1);
      tft.print("m");
      tft.cursorToXY(0, 240);
      tft.setTextColor(l_text, BLACK);
      tft.print("HS :");
      tft.setTextColor(title_bg, BLACK);
      tft.println(SLEW_RA_microsteps);
      tft.cursorToXY(0, 265);
      tft.setTextColor(l_text, BLACK);
      tft.print("CS :");
      tft.setTextColor(title_bg, BLACK);
      tft.print(RA_microSteps);
    }
    update_time = millis();
    tft.setFontMode(gTextFontModeTransparent);// Set font mode to Transparent (No Highlight)
  }
  else if (CURRENT_SCREEN == 5 && (millis() - update_time) > 2000)
  {
    Current_RA_DEC();
    tft.setTextScale(3);

    tft.fillRect(90, 60, 185, 28, l_text ); //l_text // modif JG there was inversions here that gives incorrect values on coord screen
    tft.cursorToXY(108, 65);
    tft.print(curr_HA_lz);

    tft.fillRect(90, 98, 185, 28, l_text);
    tft.cursorToXY(108, 102);
    tft.print(curr_RA_lz);

    tft.fillRect(90, 138, 185, 28, l_text);
    tft.cursorToXY(108, 143);
    tft.print(curr_DEC_lz_disp); // Modif JG, DEC displayed was in wrong format, 9000:00 instead of 90:00:00. Can't correct it when the value curr_DEC_lz was calculated (see comment in function page).
    //tft.print(curr_DEC_lz);

    update_time = millis();

  }
  else if (CURRENT_SCREEN == 1 && (millis() - update_time) > 10000 && changes == 0)
  {
    tft.setTextColor(BLACK);
    tft.fillRect(190, 62, 75, 25, l_text ); //
    tft.cursorToXY(195, 68);
    tft.print(rtc.getTimeStr(FORMAT_SHORT));
    if (old_d != rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/')) {

      tft.fillRect(50, 62, 130, 25, l_text); //l_text
      tft.cursorToXY(55, 68);
      tft.print(rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/'));
    }
    update_time = millis();
  }
  else if (CURRENT_SCREEN == 0 && (millis() - update_time) > 5000)
  {
    tft.setFontMode(gTextFontModeSolid);// Set font mode to transparent (No Highlight)
    tft.setTextColor(btn_l_text, BLACK);
    tft.setTextScale(3);
    tft.cursorToXY(65, 150);
    tft.print("LAT:");
    tft.print(gps.location.lat(), 4);
    tft.cursorToXY(65, 195);
    tft.print("LNG:");
    tft.print(gps.location.lng(), 4);
    tft.cursorToXY(85, 235);
    tft.setTextScale(2);
    tft.print("Satellites: ");
    tft.print(gps.satellites.value());
    tft.cursorToXY(115, 260);
    tft.print("GMT: ");
    tft.print(gps.time.hour());
    tft.print(":");
    tft.print(gps.time.minute());
    tft.cursorToXY(80, 285);
    tft.print("Altitude:");
    tft.print(gps.altitude.meters());
    tft.setFontMode(gTextFontModeTransparent);// Set font mode to transparent (No Highlight)
    if (gps.satellites.value() == 0)
    {
      smartDelay(1000);
    }
    else
    {
      GPS_iterrations += 1;
      smartDelay(1000);
    }

    if ((GPS_iterrations > 2) && (gps.location.lat() != 0))
    {
      OBSERVATION_LONGITUDE = gps.location.lng();
      OBSERVATION_LATTITUDE = gps.location.lat();
      OBSERVATION_ALTITUDE = gps.altitude.meters();
      // Set the earth rotation direction depending on the Hemisphere...
      // HIGH and LOW are substituted
      if (OBSERVATION_LATTITUDE > 0)
      {
        STP_FWD = LOW;
        STP_BACK = HIGH;
      }
      else
      {
        STP_FWD = HIGH;
        STP_BACK = LOW;
      }
      Serial.println(OBSERVATION_LATTITUDE);
      Serial.print("STP_FWD = ");
      Serial.println(STP_FWD);
      Serial.print("STP_BACK = ");
      Serial.println(STP_BACK);
      if (IS_SOUND_ON) {
        SoundOn(note_C, 32);
      }
      CURRENT_SCREEN = 1;
      // Serial2.end();

      int ora, date_delay = 0;
      //int time_delay = round(gps.location.lng() * 4 / 60); //rough calculation of the timezone delay
      int time_delay = TIME_ZONE; // Use the time zone delay  // Suggested by Aitor
      // convert to epoch
      setTime(gps.time.hour(), gps.time.minute(), gps.time.second(), gps.date.day(), gps.date.month(), gps.date.year());
#ifdef serial_debug
      Serial.print("epoch: ");
      Serial.println(now());
#endif

      if (isSummerTime())
      {
        //If in summer time sum 1h and put Summer_Time flag as 1
        time_delay += 1;
        Summer_Time = 1;
      }

      //update the value of the variable ora
      ora = gps.time.hour() + time_delay;

      //to update the real time
      if (ora >= 24)
      {
        ora -= 24;
        date_delay = 1;
      }

      //setTime(ora, gps.time.minute(), gps.time.second(), gps.date.day()+date_delay, gps.date.month(), gps.date.year()); <-- too mantain sync with UTC
      rtc.setDate(gps.date.day() + date_delay, gps.date.month(), gps.date.year());
      rtc.setTime(ora, gps.time.minute(), gps.time.second());

      drawClockScreen();
    }
    update_time = millis();
  }
  else if ((CURRENT_SCREEN == 13) && (IS_OBJ_FOUND == true) && ((millis() - update_time) > 2000))
  {
    tft.setTextColor(btn_l_text);
    tft.setTextScale(2);
    float HAHh;
    float HAMm;
    float HA_deci;

    if (HAHour >= 12) {
      HAHh = HAHour - 12;
    } else {
      HAHh = HAHour;
    }

    HAMm = HAMin;
    HA_deci = (HAHh + (HAMm / 60)) * 15; // In degrees - decimal

    if (ALLIGN_STEP == 1)
    {
      //tft.fillRect(0, 295, 320, 80, BLACK);
      tft.setFontMode(gTextFontModeSolid);// Set font mode to Solid (with Highlight)
      tft.setTextColor(title_bg, BLACK);
      tft.cursorToXY(0, 280);
      tft.setTextScale(2);
      delta_a_RA = (double(RA_microSteps) - double(HA_deci * HA_H_CONST)) / double(HA_H_CONST);
      delta_a_DEC = (double(DEC_microSteps) - double(SLEW_DEC_microsteps)) / double(DEC_D_CONST);
      tft.print("Delta_RA: ");
      tft.print(delta_a_RA * 60, 6);
      tft.println(" arcmin");
      tft.print("Delta_DEC: ");
      tft.print(delta_a_DEC * 60, 6);
      tft.println(" arcmin");
      tft.setFontMode(gTextFontModeTransparent);// Set font mode to Transparent (No Highlight)
    }
    tft.setTextScale(2);           // To make sure that when a button is Pressed, it will be re-drawn sith Size 2 text!
    update_time = millis();
  }

}
