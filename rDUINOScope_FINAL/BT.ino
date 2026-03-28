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
//  - Bluetooth communication (both ways);
//
void considerBTCommands() {
if ((strcmp(BT_COMMAND_STR, ":GR") != 0) && (strcmp(BT_COMMAND_STR, ":GD") != 0) && (strcmp(BT_COMMAND_STR, "") != 0)) {
    if (IS_SOUND_ON) {
      SoundOn(note_c, 8);
    }
    strcat(BTs, "\r\n");
    strcat(BTs, rtc.getTimeStr(FORMAT_LONG));
    strcat(BTs, ": ");
    strcat(BTs, BT_COMMAND_STR);
  }

  if (strstr(BT_COMMAND_STR, "synClock") != NULL) {
    char date_time[50];
    char* synPos = strstr(BT_COMMAND_STR, "synClock");
    strcpy(date_time, synPos + 9);
    
    char* k0 = strchr(date_time, ':');
    char* k1 = strchr(k0 + 1, ':');
    char* k2 = strchr(k1 + 1, ':');
    
    char hh_str[3], mm_str[3], ss_str[3];
    strncpy(hh_str, date_time, k0 - date_time);
    hh_str[k0 - date_time] = '\0';
    strncpy(mm_str, k0 + 1, k1 - k0 - 1);
    mm_str[k1 - k0 - 1] = '\0';
    strncpy(ss_str, k1 + 1, k2 - k1 - 1);
    ss_str[k2 - k1 - 1] = '\0';
    
    int hh = atoi(hh_str);
    int mm = atoi(mm_str);
    int ss = atoi(ss_str) + 1;
    rtc.setTime(hh, mm, ss);
    
    char* k3 = strchr(k2 + 1, ':');
    char* k4 = strchr(k3 + 1, ':');
    char* k5 = strchr(k4 + 1, ':');
    
    char yy_str[5], mo_str[3], dd_str[3], st_str[3];
    strncpy(yy_str, k2 + 1, k3 - k2 - 1);
    yy_str[k3 - k2 - 1] = '\0';
    strncpy(mo_str, k3 + 1, k4 - k3 - 1);
    mo_str[k4 - k3 - 1] = '\0';
    strncpy(dd_str, k4 + 1, k5 - k4 - 1);
    dd_str[k5 - k4 - 1] = '\0';
    strcpy(st_str, k5 + 1);
    
    int yy = atoi(yy_str);
    int mo = atoi(mo_str);
    int dd = atoi(dd_str);
    Summer_Time = atoi(st_str);
    rtc.setDate(dd, mo, yy);
    
    Serial3.print("synClock: ");
    Serial3.print(rtc.getTimeStr(FORMAT_LONG));
    Serial3.print(" ");
    Serial3.print(rtc.getDateStr(FORMAT_LITTLEENDIAN));
    Serial3.print(" summer_time=");
    Serial3.print(Summer_Time);
    Serial3.println(" .....(OK)");
    
    BT_COMMAND_STR[0] = '\0';
  }

if (strstr(BT_COMMAND_STR, "synGPS") != NULL) {
    // :synGPS LAT:LONG:ALT:TZ# (TZ = time zone)
    char* synPos = strstr(BT_COMMAND_STR, "synGPS");
    char gps_str[100];
    strcpy(gps_str, synPos + 7);
    
    // Remove final # end if present
    char* hashPos = strchr(gps_str, '#');
    if (hashPos != NULL) *hashPos = '\0';
    
    // Find separators
    char* k0 = strchr(gps_str, ':');
    char* k1 = strchr(k0 + 1, ':');
    char* k2 = strchr(k1 + 1, ':');
    
    if (k0 != NULL && k1 != NULL && k2 != NULL) {
        // Extract les values
        *k0 = '\0'; // End LAT part
        *k1 = '\0'; // End LONG part
        *k2 = '\0'; // End ALT part
        
        OBSERVATION_LATTITUDE = atof(gps_str);
        OBSERVATION_LONGITUDE = atof(k0 + 1);
        OBSERVATION_ALTITUDE = atof(k1 + 1);
        TIME_ZONE = atoi(k2 + 1);
        
        Serial3.print("synGPS: ");
        Serial3.print(OBSERVATION_LATTITUDE, 6);
        Serial3.print(" | ");
        Serial3.print(OBSERVATION_LONGITUDE, 6);
        Serial3.print(" | ");
        Serial3.print(OBSERVATION_ALTITUDE, 2);
        Serial3.println(" .....(OK)");
    }
    BT_COMMAND_STR[0] = '\0';
}

if (strstr(BT_COMMAND_STR, "gotoHome") != NULL) {
    IS_TRACKING = false;
    Timer3.stop();
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
    Slew_RA_timer = Slew_timer + 20000;
    safeStringCopy(OBJECT_DETAILS, "The north and south celestial poles are the two imaginary points in the sky where the Earth's axis of rotation, intersects the celestial sphere", sizeof(OBJECT_DETAILS));
    sendTrackingStatus();
    BT_COMMAND_STR[0] = '\0';
}

if (strcmp(BT_COMMAND_STR, "RD_priv") == 0) {
    Current_RA_DEC();
    Serial3.print("Right Ascension: ");
    Serial3.println(curr_RA_lz);
    Serial3.print("Declination: ");
    Serial3.print(curr_DEC_lz);
    BT_COMMAND_STR[0] = '\0';
}

// :AP# - Start Tracking
if (strcmp(BT_COMMAND_STR, ":AP") == 0) {
    if (IS_TRACKING == false) {
        IS_TRACKING = true;

        if (Tracking_type == 1) { // 1: Sidereal, 2: Solar, 0: Lunar;
            Timer3.start(Clock_Sidereal);
        } else if (Tracking_type == 2) {
            Timer3.start(Clock_Solar);
        } else if (Tracking_type == 0) {
            Timer3.start(Clock_Lunar);
        }
        drawMainScreen();
        sendTrackingStatus();
    }
    BT_COMMAND_STR[0] = '\0';
}

// :AL#  - Stop Tracking
if (strcmp(BT_COMMAND_STR, ":AL") == 0) {
    if (IS_TRACKING == true) {
        IS_TRACKING = false;
        Timer3.stop();
        drawMainScreen();
        sendTrackingStatus();
    }
    BT_COMMAND_STR[0] = '\0';
}

// :CM# or :CMR# - Sync with selected Object
if ((strcmp(BT_COMMAND_STR, ":CM") == 0) || (strcmp(BT_COMMAND_STR, ":CMR") == 0)) {
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

    delta_a_RA = (double(RA_microSteps) - double(HA_deci * HA_H_CONST)) / double(HA_H_CONST);
    delta_a_DEC = (double(DEC_microSteps) - double(SLEW_DEC_microsteps)) / double(DEC_D_CONST);

    Serial3.print("Coordinates  matched #");
    BT_COMMAND_STR[0] = '\0';
}

// <0x06>  - Request Alignment Type
if (strcmp(BT_COMMAND_STR, "") == 0) {
    Serial3.print("P");   // A= if scope in Alt/Az Mode, L= if scope in Land mode, P= if scope in Polar Mode
    BT_COMMAND_STR[0] = '\0';
}

// :GR#  - Request RA coordinate
if (strcmp(BT_COMMAND_STR, ":GR") == 0) {
    Current_RA_DEC();
    Serial3.print(curr_RA_lz);
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

// :GD#  - Request DEC coordinate
if (strcmp(BT_COMMAND_STR, ":GD") == 0) {
    Current_RA_DEC();
    Serial3.print(curr_DEC_lz);
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

// Request product version
if (strcmp(BT_COMMAND_STR, ":GVP") == 0) {
    Serial3.print(FirmwareName);
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

// Request Firmware version
if (strcmp(BT_COMMAND_STR, ":GVN") == 0) {
    Serial3.print(FirmwareNumber);
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

// Request Firmware Date
if (strcmp(BT_COMMAND_STR, ":GVD") == 0) {
    Serial3.print(FirmwareDate);
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

// Request Firmware Time
if (strcmp(BT_COMMAND_STR, ":GVT") == 0) {
    Serial3.print(FirmwareTime);
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

// Request Current Site Longitude
if (strcmp(BT_COMMAND_STR, ":Gg") == 0) {
    Serial3.print(OBSERVATION_LONGITUDE, 6);
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

// Request tracking rate
if (strcmp(BT_COMMAND_STR, ":Gt") == 0) {
    Serial3.print("+43[223]28:00");
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

// Request Current Date
if (strcmp(BT_COMMAND_STR, ":GC") == 0) {
    Serial3.print(rtc.getDateStr(FORMAT_SHORT, FORMAT_LITTLEENDIAN, '/'));
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

// Request Current Time
if (strcmp(BT_COMMAND_STR, ":GL") == 0) {
    Serial3.print(rtc.getTimeStr(FORMAT_LONG));
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

// Request UTC offset time
if (strcmp(BT_COMMAND_STR, ":GG") == 0) {
    Serial3.print(TIME_ZONE);
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

// Request GW  ??? Unkoran
if (strcmp(BT_COMMAND_STR, ":GW") == 0) {
    Serial3.print("AT2");
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

// Request currently selected object/target declination
if (strcmp(BT_COMMAND_STR, ":Gd") == 0) {
    Serial3.print(curr_DEC_lz);
    Serial3.print("#");
    BT_COMMAND_STR[0] = '\0';
}

if (strcmp(BT_COMMAND_STR, ":Ms") == 0) {
    setmStepsMode("D", 8);
    for (int i = 0; i <= 200; i++) {
        digitalWrite(DEC_DIR, STP_BACK);
        PIOC->PIO_SODR = (1u << 24);
        delay(200 / i);
        PIOC->PIO_CODR = (1u << 24);
        DEC_microSteps += DEC_mode_steps;
    }
    for (int i = 0; i <= 20; i++) {
        digitalWrite(DEC_DIR, STP_BACK);
        PIOC->PIO_SODR = (1u << 24);
        delay(2 * i);
        PIOC->PIO_CODR = (1u << 24);
        DEC_microSteps += DEC_mode_steps;
    }
    BT_COMMAND_STR[0] = '\0';
}

if (strcmp(BT_COMMAND_STR, ":Mn") == 0) {
    setmStepsMode("D", 8);
    for (int i = 0; i <= 200; i++) {
        digitalWrite(DEC_DIR, STP_FWD);
        PIOC->PIO_SODR = (1u << 24);
        delay(200 / i);
        PIOC->PIO_CODR = (1u << 24);
        DEC_microSteps -= DEC_mode_steps;
    }
    for (int i = 0; i <= 20; i++) {
        digitalWrite(DEC_DIR, STP_FWD);
        PIOC->PIO_SODR = (1u << 24);
        delay(2 * i);
        PIOC->PIO_CODR = (1u << 24);
        DEC_microSteps -= DEC_mode_steps;
    }
    BT_COMMAND_STR[0] = '\0';
}

if (strcmp(BT_COMMAND_STR, ":Mw") == 0) {
    setmStepsMode("R", 8);
    if (IS_TRACKING == true) { Timer3.stop(); }
    
    for (int i = 0; i <= 200; i++) {
        digitalWrite(RA_DIR, STP_FWD);
        PIOC->PIO_SODR = (1u << 26);
        delay(200 / i);
        PIOC->PIO_CODR = (1u << 26);
        RA_microSteps -= RA_mode_steps;
    }
    for (int i = 0; i <= 15; i++) {
        digitalWrite(RA_DIR, STP_FWD);
        PIOC->PIO_SODR = (1u << 26);
        delay(2 * i);
        PIOC->PIO_CODR = (1u << 26);
        RA_microSteps -= RA_mode_steps;
        if (IS_TRACKING == true) {
            if (Tracking_type == 1) {
                Timer3.start(Clock_Sidereal);
            } else if (Tracking_type == 2) {
                Timer3.start(Clock_Solar);
            } else if (Tracking_type == 0) {
                Timer3.start(Clock_Lunar);
            }
        }
    }
    sendTrackingStatus();
    BT_COMMAND_STR[0] = '\0';
}

if (strcmp(BT_COMMAND_STR, ":Me") == 0) {
    setmStepsMode("R", 8);
    if (IS_TRACKING == true) { Timer3.stop(); }
    
    for (int i = 0; i <= 200; i++) {
        digitalWrite(RA_DIR, STP_BACK);
        PIOC->PIO_SODR = (1u << 26);
        delay(200 / i);
        PIOC->PIO_CODR = (1u << 26);
        RA_microSteps += RA_mode_steps;
    }
    for (int i = 0; i <= 15; i++) {
        digitalWrite(RA_DIR, STP_BACK);
        PIOC->PIO_SODR = (1u << 26);
        delay(2 * i);
        PIOC->PIO_CODR = (1u << 26);
        RA_microSteps += RA_mode_steps;
        if (IS_TRACKING == true) {
            if (Tracking_type == 1) {
                Timer3.start(Clock_Sidereal);
            } else if (Tracking_type == 2) {
                Timer3.start(Clock_Solar);
            } else if (Tracking_type == 0) {
                Timer3.start(Clock_Lunar);
            }
        }
    }
    sendTrackingStatus();
    BT_COMMAND_STR[0] = '\0';
}

// :Sr 07:08:52# - Set Target RA
if (strstr(BT_COMMAND_STR, "Sr") != NULL) {
    char* srPos = strstr(BT_COMMAND_STR, "Sr");
    char _RA[20];
    strncpy(_RA, srPos + 2, strlen(BT_COMMAND_STR) - (srPos - BT_COMMAND_STR) - 3);
    _RA[strlen(BT_COMMAND_STR) - (srPos - BT_COMMAND_STR) - 3] = '\0';
    
    char* k0 = strchr(_RA, ':');
    char* k1 = strchr(k0 + 1, ':');
    
    if (k0 != NULL && k1 != NULL) {
        *k0 = '\0';
        *k1 = '\0';
        
        OBJECT_RA_H = atof(_RA);
        float RAse = (strlen(k1 + 1) < 2) ? atof(k1 + 1) * 10 : atof(k1 + 1);
        OBJECT_RA_M = atof(k0 + 1) + RAse / 60;
        
        char details[100];
        snprintf(details, sizeof(details), "_RA:%s", _RA);
        safeStringCopy(OBJECT_DETAILS, details, sizeof(OBJECT_DETAILS));
        Serial3.print("1");
    }
}

// :Sd +18Я12:30#  - Set Target DEC
if (strstr(BT_COMMAND_STR, "Sd") != NULL) {
    char* sdPos = strstr(BT_COMMAND_STR, "Sd");
    char _DEC[20];
    strncpy(_DEC, sdPos + 2, strlen(BT_COMMAND_STR) - (sdPos - BT_COMMAND_STR) - 3);
    _DEC[strlen(BT_COMMAND_STR) - (sdPos - BT_COMMAND_STR) - 3] = '\0';
    
    char* k0 = strchr(_DEC, (char)223); // Search character °
    if (k0 == NULL) k0 = strchr(_DEC, '*');
    
    char* k1 = strchr(_DEC, ':');
    
    if (k0 != NULL && k1 != NULL) {
        *k0 = '\0';
        *k1 = '\0';
        
        OBJECT_DEC_D = atof(_DEC);
        float DECse = (strlen(k1 + 1) < 2) ? atof(k1 + 1) * 10 : atof(k1 + 1);
        OBJECT_DEC_M = atof(k0 + 1) + DECse / 60;
        
        if (OBJECT_DEC_D < 0) {
            OBJECT_DEC_M *= -1;
        }
        
        char details[200];
        snprintf(details, sizeof(details), "%s, _DEC:%s", OBJECT_DETAILS, _DEC);
        safeStringCopy(OBJECT_DETAILS, details, sizeof(OBJECT_DETAILS));
        Serial3.print("1");
    }
}

// :MS#  -- Slew To Target RA and DEC
if (strcmp(BT_COMMAND_STR, ":MS") == 0) {
    calculateLST_HA();
    if (ALT > 0) {
        Serial3.print("0");
        if (IS_SOUND_ON) {
            SoundOn(note_C, 32);
            delay(200);
            SoundOn(note_C, 32);
            delay(200);
            SoundOn(note_C, 32);
            delay(1500);
        }
        UpdateObservedObjects();
        safeStringCopy(OBJECT_NAME, "BT", sizeof(OBJECT_NAME));
        safeStringCopy(OBJECT_DESCR, "using LX200 Protocol", sizeof(OBJECT_DESCR));
        
        char details[500];
        snprintf(details, sizeof(details), "Missing information about the object! %s", OBJECT_DETAILS);
        safeStringCopy(OBJECT_DETAILS, details, sizeof(OBJECT_DETAILS));
        
        Timer3.stop();
        IS_TRACKING = false;
        IS_OBJ_FOUND = false;
        IS_OBJECT_RA_FOUND = false;
        IS_OBJECT_DEC_FOUND = false;
        sendTrackingStatus();
        Slew_timer = millis();
        Slew_RA_timer = Slew_timer + 20000;
    } else {
        Serial3.print("rDUINOScope: Object Below Horizon! #");
    }
    drawMainScreen();
    BT_COMMAND_STR[0] = '\0';
}

if (strcmp(BT_COMMAND_STR, "Current") == 0) {
    Serial3.println("MECHANICS DATA (Software Defined):");
    Serial3.println("==================================");
    Serial3.print("Firmware Version: ");
    Serial3.print(FirmwareName);
    Serial3.print(" ");
    Serial3.println(FirmwareNumber);
    Serial3.print("WORM GEAR Tooths = ");
    Serial3.println(WORM);
    Serial3.print("REDUCTOR = 1:");
    Serial3.println(REDUCTOR);
    Serial3.print("MOTOR STEPS / REV = ");
    Serial3.println(DRIVE_STP);
    Serial3.print("MICROSteps Mode: ");
    Serial3.println(MICROSteps);
    Serial3.println("\r\nCALCULATED VALUES IN THE SOFTWARE:");
    Serial3.print("MicroSteps for 360 rotation = ");
    Serial3.println(MicroSteps_360);
    Serial3.print("RA_90 = ");
    Serial3.println(RA_90);
    Serial3.print("DEC_90 = ");
    Serial3.println(DEC_90);
    Serial3.print("RA_H_CONST = ");
    Serial3.println(HA_H_CONST);
    Serial3.print("DEC_D_CONST = ");
    Serial3.println(DEC_D_CONST);
    Serial3.print("RA Clock Motor (Micro Seconds) = ");
    Serial3.print(Clock_Sidereal);
    Serial3.print(" (");
    Serial3.print(1000000 / Clock_Sidereal);
    Serial3.println(" MicroSteps / Second )");
    Serial3.print("Time to Meridian Flip = ");
    Serial3.print(mer_flp);
    Serial3.println(" hours");
    Serial3.println("\r\nCURRENT TELESCOPE POSITION COUNTER:");
    Serial3.print("RA_microSteps: ");
    Serial3.println(RA_microSteps);
    Serial3.print("DEC_microSteps: ");
    Serial3.println(DEC_microSteps);
    Serial3.print("delta_a_RA: ");
    Serial3.println(delta_a_RA);
    Serial3.print("delta_a_DEC: ");
    Serial3.println(delta_a_DEC);
    Serial3.println("\r\nCURRENT GPS POSITION & DATE:");
    Serial3.print("OBSERVATION LATTITUDE: ");
    Serial3.println(OBSERVATION_LATTITUDE, 6);
    Serial3.print("OBSERVATION LONGITUDE: ");
    Serial3.println(OBSERVATION_LONGITUDE, 6);
    Serial3.print("OBSERVATION ALTITUDE: ");
    Serial3.println(OBSERVATION_ALTITUDE, 6);
    Serial3.print("TIME ZONE: ");
    Serial3.println(TIME_ZONE);

    // For the formatted date:
    char dateBuffer[20];
    const char* dateStr = rtc.getDateStr();
    char monthBuffer[10];
    safeStringCopy(monthBuffer, rtc.getMonthStr(FORMAT_SHORT), sizeof(monthBuffer));
    
    snprintf(dateBuffer, sizeof(dateBuffer), "%.2s %s %.2s", dateStr, monthBuffer, dateStr + 6);
    Serial3.print("Mount Date: ");
    Serial3.println(dateBuffer);
    
    // For hour :
    char timeBuffer[6];
    strncpy(timeBuffer, rtc.getTimeStr(), 5);
    timeBuffer[5] = '\0';
    Serial3.print("Mount Time: ");
    Serial3.println(timeBuffer);


    Serial3.print("Summer Time: ");
    Serial3.println(Summer_Time);

    Serial3.println("\r\nOBJECT DATA (Selected Object):");
    Serial3.println("==================================");
    Serial3.print("IS_OBJ_FOUND = ");
    Serial3.println(IS_OBJ_FOUND);
    Serial3.print("OBJECT_NAME = ");
    Serial3.println(OBJECT_NAME);
    Serial3.print("RA = ");
    Serial3.print(OBJECT_RA_H, 0);
    Serial3.print("h ");
    Serial3.print(OBJECT_RA_M);
    Serial3.println("m");
    Serial3.print("DEC = ");
    Serial3.print(OBJECT_DEC_D, 0);
    Serial3.print("* ");
    Serial3.print(OBJECT_DEC_M);
    Serial3.println("m");
    Serial3.print("HA = ");
    Serial3.print(HAHour, 0);
    Serial3.print("h ");
    Serial3.print(HAMin);
    Serial3.println("m");
    Serial3.print("ALT = ");
    Serial3.println(ALT, 2);
    Serial3.print("AZ = ");
    Serial3.println(AZ, 2);
    Serial3.println("\r\nVARIABLE DATA (Software):");
    Serial3.println("==================================");
    Serial3.print("IS_OBJ_VISIBLE: ");
    Serial3.println(IS_OBJ_VISIBLE);
    Serial3.print("IS_IN_OPERATION: ");
    Serial3.println(IS_IN_OPERATION);
    Serial3.print("IS_TRACKING: ");
    Serial3.println(IS_TRACKING);
    Serial3.print("IS_NIGHTMODE: ");
    Serial3.println(IS_NIGHTMODE);
    Serial3.print("IS_MERIDIAN_PASSED: ");
    Serial3.println(IS_MERIDIAN_PASSED);
    Serial3.print("IS_BT_MODE_ON: ");
    Serial3.println(IS_BT_MODE_ON);
    Serial3.print("CURRENT_SCREEN: ");
    Serial3.println(CURRENT_SCREEN);
    Serial3.println("==================================");

    BT_COMMAND_STR[0] = '\0';
}

if (strstr(BT_COMMAND_STR, "SlewTo") != NULL) {
    char* tokens[12];
    char* ptr = strtok(BT_COMMAND_STR, ";");
    int tokenCount = 0;
    
    while (ptr != NULL && tokenCount < 12) {
        tokens[tokenCount++] = ptr;
        ptr = strtok(NULL, ";");
    }
    
    if (tokenCount >= 12) {
        safeStringCopy(OBJECT_NAME, tokens[1], sizeof(OBJECT_NAME));
        safeStringCopy(OBJECT_DESCR, "Pushed via BlueTooth", sizeof(OBJECT_DESCR));
        
        char details[500];
        snprintf(details, sizeof(details), 
            "The %s is a type %s object in constellation %s, with visible magnitude of %s and size of %s'. %s is %smil. l.y from Earth",
            tokens[1], tokens[11], tokens[10], tokens[8], tokens[7], tokens[1], tokens[9]);
        
        if (strcmp(tokens[12], "0") != 0 && strcmp(tokens[12], "") != 0) {
            strcat(details, " and is a.k.a ");
            strcat(details, tokens[12]);
            strcat(details, ".");
        }
        safeStringCopy(OBJECT_DETAILS, details, sizeof(OBJECT_DETAILS));
        
        OBJECT_RA_H = atof(tokens[2]);
        OBJECT_RA_M = atof(tokens[3]);
        OBJECT_DEC_D = atof(tokens[4]);
        OBJECT_DEC_M = atof(tokens[5]);
        
        if (OBJECT_DEC_D < 0) {
            OBJECT_DEC_M *= -1;
        }

    // ora SlewTo the selected object and draw information on mainScreen
    calculateLST_HA();
    if (ALT > 0) {
      if (IS_SOUND_ON) {
        SoundOn(note_C, 32);
        delay(200);
        SoundOn(note_C, 32);
        delay(200);
        SoundOn(note_C, 32);
        delay(1500);
      }
      UpdateObservedObjects();
      // Stop Interrupt procedure for tracking.
      Timer3.stop(); //
      IS_TRACKING = false;
      IS_OBJ_FOUND = false;
      IS_OBJECT_RA_FOUND = false;
      IS_OBJECT_DEC_FOUND = false;
      sendTrackingStatus();
      Slew_timer = millis();
      Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
    }
    drawMainScreen();
      BT_COMMAND_STR[0] = '\0';
  }
}

if (strcmp(BT_COMMAND_STR, "Status") == 0) {
    // Calculation of elapsed time
    char currentTime[10];
    safeStringCopy(currentTime, rtc.getTimeStr(), sizeof(currentTime));
    char startTime[10];
    safeStringCopy(startTime, START_TIME, sizeof(startTime));
    
    // Extract hours and minutes
    char* currPtr = strtok(currentTime, ":");
    int currHours = atoi(currPtr);
    int currMinutes = atoi(strtok(NULL, ":"));
    
    char* startPtr = strtok(startTime, ":");
    int startHours = atoi(startPtr);
    int startMinutes = atoi(strtok(NULL, ":"));
    
    int st = (currHours * 60 + currMinutes) - (startHours * 60 + startMinutes);
    if (st < 0) {
        st += 1440;
    }
    int st_h = st / 60;
    int st_m = st % 60;

    // Header
    Serial3.println("rDUINO SCOPE - TELESCOPE GOTO System");
    Serial3.println("Copyright (C) 2016 Dessislav Gouzgounov");
    Serial3.println("Download for free @ http://rduinoscope.byethost24.com\r\n");
    Serial3.print("Observing Sesssion started on ");
    Serial3.print(Start_date);
    Serial3.print(" @");
    Serial3.print(START_TIME);
    Serial3.println("h ");
    Serial3.print("Report generated at: ");
    Serial3.println(rtc.getTimeStr());
    Serial3.println("===============================================");
    Serial3.println("Location Information:");
    Serial3.print("      LATITUDE: ");
    Serial3.println(OBSERVATION_LATTITUDE, 4);
    Serial3.print("      LONGITUDE: ");
    Serial3.println(OBSERVATION_LONGITUDE, 4);
    Serial3.print("      ALTITUDE: ");
    Serial3.print(OBSERVATION_ALTITUDE, 0);
    Serial3.println();
    Serial3.print("Observation started at ");
    Serial3.print(START_TIME);
    Serial3.print("h, and continued for ");
    Serial3.print(st_h);
    Serial3.print("h ");
    Serial3.print(st_m);
    Serial3.print("m. Environmental temperature was ");
    Serial3.print(_temp, 0);
    Serial3.print("C and humidity was ");
    Serial3.print(_humid, 0);
    Serial3.println("%");
    Serial3.println();
    Serial3.print("Observed the following ");
    Serial3.print(Observed_Obj_Count);
    Serial3.println(" object(s):");
    Serial3.println("");
    
    for (int i = 0; i < Observed_Obj_Count; i++) {
        // Parse each input with strtok
        char entry[300];
        safeStringCopy(entry, ObservedObjects[i], sizeof(entry));
        
        char* tokens[8];
        char* ptr = strtok(entry, ";");
        int tokenCount = 0;
        
        while (ptr != NULL && tokenCount < 8) {
            tokens[tokenCount++] = ptr;
            ptr = strtok(NULL, ";");
        }
        
        if (tokenCount >= 7) {
            int tt;
            if (i == (Observed_Obj_Count - 1)) {
                // compute for last object
                char objTime[6];
                safeStringCopy(objTime, tokens[2], sizeof(objTime));
                char* timePtr = strtok(objTime, ":");
                int objHours = atoi(timePtr);
                int objMinutes = atoi(strtok(NULL, ":"));
                
                tt = (currHours * 60 + currMinutes) - (objHours * 60 + objMinutes);
                if (tt < 0) tt += 1440;
            } else {
                tt = atoi(tokens[7]);
            }
            
            // Build report
            char composed[500];
            snprintf(composed, sizeof(composed), 
                "@ %sh, %s was observed for %d min\n"
                "At the time of observation the object was %sdeg. above horizon, with HA:%s "
                "Environment wise: %s C and %s%% humidity. %s\n",
                tokens[2], tokens[0], tt, tokens[6], tokens[5], 
                tokens[3], tokens[4], tokens[1]);
            
            Serial3.println(composed);
        }
    BT_COMMAND_STR[0] = '\0';
}
}

BT_COMMAND_STR[0] = '\0';
Serial3.flush();
}
