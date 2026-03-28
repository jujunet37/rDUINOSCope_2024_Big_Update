///////////////////////////////////////////////////// Calculate LST Function /////////////////////////////////////////////////////
void calculateLST_HA() {

// HA = LST - RA
char dateBuffer[20], timeBuffer[20];
safeStringCopy(dateBuffer, rtc.getDateStr(), sizeof(dateBuffer));
safeStringCopy(timeBuffer, rtc.getTimeStr(), sizeof(timeBuffer));

// Extract values
char dayStr[3], monthStr[3], yearStr[5];
strncpy(dayStr, dateBuffer, 2);
dayStr[2] = '\0';
strncpy(monthStr, dateBuffer + 3, 2);
monthStr[2] = '\0';
strcpy(yearStr, dateBuffer + 6);

char hourStr[3], minuteStr[3], secondStr[3];
strncpy(hourStr, timeBuffer, 2);
hourStr[2] = '\0';
strncpy(minuteStr, timeBuffer + 3, 2);
minuteStr[2] = '\0';
strcpy(secondStr, timeBuffer + 6);

int D = atoi(dayStr);
int M = atoi(monthStr);
int Y = atoi(yearStr);
int S = atoi(secondStr);
int H = atoi(hourStr);

if (Summer_Time == 1) {
    H -= 2;
} else {
    H -= 1;
}
int MN = atoi(minuteStr);

if (M < 3) {
    M = M + 12;
    Y = Y - 1;
}
  float HH = H + ((float)MN / 60.00) + ((float)S / 3600.00);
  float AA = (int)(365.25 * (Y + 4716));
  float BB = (int)(30.6001 * (M + 1));
  double CurrentJDN = AA + BB + D - 1537.5 + (HH - TIME_ZONE) / 24;
  float current_day = CurrentJDN - 2451544.5; // Modif JG error 1 day, was 31 dec 1999 @ 00h00

  //calculate terms required for LST calcuation and calculate GMST using an approximation
  double MJD = CurrentJDN - 2400000.5;
  int MJD0 = (int)MJD;
  float ut = (MJD - MJD0) * 24.0;
  double t_eph  = (MJD0 - 51544.5) / 36525.0;
  double GMST = 6.697374558 + 1.0027379093 * ut + (8640184.812866 + (0.093104 - 0.0000062 * t_eph) * t_eph) * t_eph / 3600.0;

  LST = GMST + OBSERVATION_LONGITUDE / 15.0;

  //reduce it to 24 format
  int LSTint = (int)LST;
  LSTint /= 24;
  LST = LST - (double) LSTint * 24;

  // ora I'll use the global Variables OBJECT_RA_H and OBJECT_RA_M  To calculate the Hour angle of the selected object.

  double dec_RA = OBJECT_RA_M / 60 + OBJECT_RA_H;
  double HA_decimal = LST - dec_RA;

  HAHour = int(HA_decimal);
  HAMin = (HA_decimal - HAHour) * 60;

  if (HAMin < 0) {
    HAHour -= 1;
    HAMin += 60;
  }
  if (HAHour < 0) {
    HAHour += 24;
  }

  // Convert degrees into Decimal Radians
  double rDEC = 0;
  rDEC = OBJECT_DEC_D + (OBJECT_DEC_M / 60);
  //rDEC += delta_a_DEC;
  rDEC *= 0.0174532925199;
  double rHA =  HA_decimal * 0.26179938779915;   // 0.261799.. = 15 * 3.1415/180  (to convert to Deg. and * Pi) :)
  double rLAT = OBSERVATION_LATTITUDE * 0.0174532925199;

  IS_OBJ_VISIBLE = true;

  double sin_rDEC = sin(rDEC);
  double cos_rDEC = cos(rDEC);
  double sin_rLAT = sin(rLAT);
  double cos_rLAT = cos(rLAT);
  double cos_rHA = cos(rHA);
  double sin_rHA = sin(rHA);

  ALT = sin_rDEC * sin_rLAT;
  ALT += (cos_rDEC * cos_rLAT * cos_rHA);
  double sin_rALT = ALT;
  ALT =  asin(ALT);
  double cos_rALT = cos(ALT);
  ALT *= 57.2958;

  AZ = sin_rALT * sin_rLAT;
  AZ = sin_rDEC - AZ;
  AZ /= (cos_rALT * cos_rLAT);
  AZ = acos(AZ) * 57.2957795;
  if (sin_rHA > 0) {
    AZ = 360 - AZ;
  }

  if (ALT < 0) {
    IS_OBJ_VISIBLE = false;
    if ((IS_BT_MODE_ON == true) && (IS_OBJ_FOUND == false)) {
      Serial3.println("Object is out of sight! Telescope not moved.");
    }
    IS_OBJ_FOUND = true;
    IS_OBJECT_RA_FOUND = true;
    IS_OBJECT_DEC_FOUND = true;
    Slew_RA_timer = 0;
    RA_finish_last = 0;
  } else {
    IS_OBJ_VISIBLE = true;
  }

  ///////////////////////////////////////////////////// Decide Meridian Flip Function /////////////////////////////////////////////////////
  // Take care of the Meridian Flip coordinates
  // This will make the telescope do Meridian Flip... depending on the current HA and predefined parameter: MIN_TO_MERIDIAN_FLIP
  if (IS_MERIDIAN_FLIP_AUTOMATIC) {
    mer_flp = HAHour + ((HAMin + MIN_TO_MERIDIAN_FLIP) / 60);
    float old_HAMin = HAMin;
    float old_HAHour = HAHour;
    if (IS_POSIBLE_MERIDIAN_FLIP == true) {
      if (mer_flp >= 24) {
        HAMin = HAMin - 60;
        HAHour = 0;
        if (MERIDIAN_FLIP_DO == false) {
          IS_TRACKING = false;
          Timer3.stop();
          OnScreenMsg(1);
          if (IS_SOUND_ON) {
            SoundOn(note_C, 32);
            delay(200);
            SoundOn(note_C, 32);
            delay(200);
            SoundOn(note_C, 32);
            delay(1000);
          }
          IS_OBJ_FOUND = false;
          IS_OBJECT_RA_FOUND = false;
          IS_OBJECT_DEC_FOUND = false;
          Slew_timer = millis();
          //Slew_RA_timer = Slew_timer + 20000;   // Give 20 sec. advance to the DEC. We will revise later.
          Slew_RA_timer = Slew_timer + 5000;   // JG : Give 5 sec. advance to the DEC. GOTO speed increased by 4x
          MERIDIAN_FLIP_DO = true;
          MERFLIPRESET = true; // Add MF correction
          //Slew_Timer_flag = millis(); // Add MF Correction
          drawMainScreen();
        } else {
          if ((old_HAHour == HAHour) && (old_HAMin == HAMin)) {  // Meridian Flip is done so the code above will not execute
            MERIDIAN_FLIP_DO = false;
          }
        }
        //DEC is set as part of the SlewTo function
      }
    } else {
      if (mer_flp >= 24) {
        IS_TRACKING = false;
        Timer3.stop();
      }
    }
    sendTrackingStatus();
  }
}
///////////////////////////////////////////////////// Select Object Function /////////////////////////////////////////////////////

void selectOBJECT_M(int index_, int objects) {
  OBJECT_Index = index_;
  
if (objects == 1) {
    TRACKING_MOON = false;
    
    char* objData = OBJ_Array[index_];
    char* i1 = strchr(objData, ';');
    char* i2 = strchr(i1 + 1, ';');
    char* i3 = strchr(i2 + 1, ';');
    char* i4 = strchr(i3 + 1, ';');
    char* i5 = strchr(i4 + 1, ';');
    char* i6 = strchr(i5 + 1, ';');
    char* i7 = strchr(i6 + 1, ';');
    
    // OBJECT_NAME (from start to i1)
    if (i1 != NULL) {
        strncpy(OBJECT_NAME, objData, i1 - objData);
        OBJECT_NAME[i1 - objData] = '\0';
    }
    
    // OBJECT_DESCR (after i7)
    if (i7 != NULL) {
        safeStringCopy(OBJECT_DESCR, i7 + 1, sizeof(OBJECT_DESCR));
    }
    
    // RA (between i1 and i2)
    if (i1 != NULL && i2 != NULL) {
        char OBJ_RA[20];
        strncpy(OBJ_RA, i1 + 1, i2 - i1 - 1);
        OBJ_RA[i2 - i1 - 1] = '\0';
        
        char* hPos = strchr(OBJ_RA, 'h');
        if (hPos != NULL) {
            *hPos = '\0';
            OBJECT_RA_H = atof(OBJ_RA);
            OBJECT_RA_M = atof(hPos + 1);
        }
    }
    
    // DEC (between i2 and i3)
    if (i2 != NULL && i3 != NULL) {
        char OBJ_DEC[20];
        strncpy(OBJ_DEC, i2 + 1, i3 - i2 - 1);
        OBJ_DEC[i3 - i2 - 1] = '\0';
        
        char sign = OBJ_DEC[0];
        char* degPos = strchr(OBJ_DEC, '°');
        
        if (degPos != NULL) {
            *degPos = '\0';
            OBJECT_DEC_D = atof(OBJ_DEC + 1);
            OBJECT_DEC_M = atof(degPos + 1);
            
            if (sign == '-') {
                OBJECT_DEC_D *= -1;
                OBJECT_DEC_M *= -1;
            }
        }
    }
    
if (LOAD_SELECTOR >= 3) {
    char details[500];
    
    // Correctly extract the substrings
    char constellation[50] = "";
    char object_type[50] = "";
    char magnitude[20] = "";
    char size_period[50] = "";
    
    // Constellation (between i3 and i4)
    if (i3 != NULL && i4 != NULL) {
        strncpy(constellation, i3 + 1, i4 - i3 - 1);
        constellation[i4 - i3 - 1] = '\0';
    }
    
    // Object type (between i4 and i5)
    if (i4 != NULL && i5 != NULL) {
        strncpy(object_type, i4 + 1, i5 - i4 - 1);
        object_type[i5 - i4 - 1] = '\0';
    }
    
    // Magnitude (between i5 and i6)
    if (i5 != NULL && i6 != NULL) {
        strncpy(magnitude, i5 + 1, i6 - i5 - 1);
        magnitude[i6 - i5 - 1] = '\0';
    }
    
    // Size/Period/Separation (between i6 and i7)
    if (i6 != NULL && i7 != NULL) {
        strncpy(size_period, i6 + 1, i7 - i6 - 1);
        size_period[i7 - i6 - 1] = '\0';
    }
    
    // Build description
    if (LOAD_SELECTOR >= 6) {
        snprintf(details, sizeof(details), "%s is a %s in constellation %s, with visible magnitude of %s and a size of %s",
                 OBJECT_NAME, object_type, constellation, magnitude, size_period);
    } else if (LOAD_SELECTOR == 3) {
        snprintf(details, sizeof(details), "%s is a %s in constellation %s, with visible magnitude of %s and separation of %s arcsec",
                 OBJECT_NAME, object_type, constellation, magnitude, size_period);
    } else if (LOAD_SELECTOR == 4) {
        snprintf(details, sizeof(details), "%s is a %s in constellation %s, with visible magnitude of %s and a period of %s",
                 OBJECT_NAME, object_type, constellation, magnitude, size_period);
    }
    
    safeStringCopy(OBJECT_DETAILS, details, sizeof(OBJECT_DETAILS));
}
} else if (objects == 2) {
    TRACKING_MOON = false;
    
    // Use directly Stars array in RAM
    const char* starData = Stars[index_];
    
    if (strlen(starData) == 0) {
        return;
    }
    
    char* i1 = strchr(starData, ';');
    char* i2 = strchr(i1 + 1, ';');
    
    if (i1 && i2) {
        // part extract
        char namePart[20], catalogPart[20];
        strncpy(namePart, i1 + 1, i2 - i1 - 1);
        namePart[i2 - i1 - 1] = '\0';
        strncpy(catalogPart, starData, i1 - starData);
        catalogPart[i1 - starData] = '\0';
        
        snprintf(OBJECT_NAME, sizeof(OBJECT_NAME), "%s from %s", namePart, catalogPart);
        
        // Exctract RA - format : "13h24m"
        char OBJ_RA[20];
        strncpy(OBJ_RA, i2 + 1, strlen(i2 + 1));  // Take the rest
        OBJ_RA[strlen(i2 + 1)] = '\0';
        
        // Find the end of RA (next ';' or end of string)
        char* i3 = strchr(i2 + 1, ';');
        if (i3) {
            OBJ_RA[i3 - (i2 + 1)] = '\0';  // tuncate to DEC
        }
        
        // Parse RA
        char* hPos = strchr(OBJ_RA, 'h');
        if (hPos) {
            *hPos = '\0';
            OBJECT_RA_H = atof(OBJ_RA);
            char* mPos = strchr(hPos + 1, 'm');
            if (mPos) {
                *mPos = '\0';
                OBJECT_RA_M = atof(hPos + 1);
            }
        }
        
        // Extract DEC - start after RA
        char OBJ_DEC[20];
        if (i3) {
            // Format with separators : "54.9°"
            strcpy(OBJ_DEC, i3 + 1);
        } else {
            // Format without separator : take all after RA
            // We must find where DEC starts
            char* decStart = strchr(i2 + 1, ';');
            if (!decStart) {
                // No separator, DEC start after 'm' of RA
                char* mPos = strchr(i2 + 1, 'm');
                if (mPos) {
                    decStart = mPos + 1;
                } else {
                    decStart = i2 + strlen(OBJ_RA) + 1;
                }
            }
            strcpy(OBJ_DEC, decStart);
        }
        
        // Clean DEC - remove spaces etc.
        char cleanedDEC[20];
        int j = 0;
        for (int i = 0; i < strlen(OBJ_DEC) && j < 19; i++) {
            if (OBJ_DEC[i] != ' ' && OBJ_DEC[i] != '\t') {
                cleanedDEC[j++] = OBJ_DEC[i];
            }
        }
        cleanedDEC[j] = '\0';
        
        // Parse DEC
        char sign = '+';
        if (cleanedDEC[0] == '-') {
            sign = '-';
        }
        
        char* degPos = strchr(cleanedDEC, '°');
        if (degPos) {
            *degPos = '\0';
            if (sign == '-') {
                OBJECT_DEC_D = -atof(cleanedDEC + 1);  // +1 to jump sign
            } else {
                OBJECT_DEC_D = atof(cleanedDEC);
            }
        } else {
            // No '°' symbol, try to parse directly
            if (sign == '-') {
                OBJECT_DEC_D = -atof(cleanedDEC + 1);
            } else {
                OBJECT_DEC_D = atof(cleanedDEC);
            }
        }
        OBJECT_DEC_M = 0;
        
// Debug: afficher les valeurs parsées
//        Serial.print("Parsed RA: ");
//        Serial.print(OBJECT_RA_H);
//        Serial.print("h ");
//        Serial.print(OBJECT_RA_M);
//        Serial.print("m,//  DEC: ");
//        Serial.print(OBJECT_DEC_D);
//        Serial.println("°");
    }

} else if (objects == 3) { 
    TRACKING_MOON = false;
    char* starData = Iter_Stars[index_];
    
    // Find the separators
    char* i1 = strchr(starData, ';');
    char* i2 = strchr(i1 + 1, ';');
    
    if (i1 && i2) {
        // Extract names and catalogs
        char namePart[50], catalogPart[50];
        strncpy(namePart, i1 + 1, i2 - i1 - 1);
        namePart[i2 - i1 - 1] = '\0';
        strncpy(catalogPart, starData, i1 - starData);
        catalogPart[i1 - starData] = '\0';
        
        // Build OBJECT_NAME
        snprintf(OBJECT_NAME, sizeof(OBJECT_NAME), "%s from %s", namePart, catalogPart);
        
        // Extract RA - format is "13h24m"
        char OBJ_RA[20];
        strncpy(OBJ_RA, i2 + 1, strlen(i2 + 1));
        OBJ_RA[strlen(i2 + 1)] = '\0';
        
        // Find RA end
        char* i3 = strchr(i2 + 1, ';');
        if (i3) {
            OBJ_RA[i3 - (i2 + 1)] = '\0';
        }
        
        // Parse RA
        char* hPos = strchr(OBJ_RA, 'h');
        if (hPos) {
            *hPos = '\0';
            OBJECT_RA_H = atof(OBJ_RA);
            char* mPos = strchr(hPos + 1, 'm');
            if (mPos) {
                *mPos = '\0';
                OBJECT_RA_M = atof(hPos + 1);
            }
        }
        
        // Extract DEC
        char OBJ_DEC[20];
        if (i3) {
            // Format with separator
            strcpy(OBJ_DEC, i3 + 1);
        } else {
            // Format without separator
            char* decStart = strchr(i2 + 1, ';');
            if (!decStart) {
                char* mPos = strchr(i2 + 1, 'm');
                if (mPos) {
                    decStart = mPos + 1;
                } else {
                    decStart = i2 + strlen(OBJ_RA) + 1;
                }
            }
            strcpy(OBJ_DEC, decStart);
        }
        
        // Clean and parse DEC
        char cleanedDEC[20];
        int j = 0;
        for (int i = 0; i < strlen(OBJ_DEC) && j < 19; i++) {
            if (OBJ_DEC[i] != ' ' && OBJ_DEC[i] != '\t') {
                cleanedDEC[j++] = OBJ_DEC[i];
            }
        }
        cleanedDEC[j] = '\0';
        
        char sign = '+';
        if (cleanedDEC[0] == '-') {
            sign = '-';
        }
        
        char* degPos = strchr(cleanedDEC, '°');
        if (degPos) {
            *degPos = '\0';
            if (sign == '-') {
                OBJECT_DEC_D = -atof(cleanedDEC + 1);
            } else {
                OBJECT_DEC_D = atof(cleanedDEC);
            }
        } else {
            if (sign == '-') {
                OBJECT_DEC_D = -atof(cleanedDEC + 1);
            } else {
                OBJECT_DEC_D = atof(cleanedDEC);
            }
        }
        OBJECT_DEC_M = 0;
        
//        // Debug
//        Serial.print("Iterative - Parsed RA: ");
//        Serial.print(OBJECT_RA_H);
//        Serial.print("h ");
//        Serial.print(OBJECT_RA_M);
//        Serial.print("m, DEC: ");
//        Serial.print(OBJECT_DEC_D);
//        Serial.println("°");
    }
}
}
///////////////////////////////////////////////////// Calculate Sidereal Rate Function /////////////////////////////////////////////////////
void Sidereal_rate() {
  // when a manual movement of the drive happens. - This will avoid moving the stepepers with a wrong Step Mode.
  if ((IS_MANUAL_MOVE == false) && (IS_TRACKING) && (IS_STEPPERS_ON)) {
    if (RA_mode_steps != MICROSteps) {
      setmStepsMode("R", MICROSteps);
    }
    digitalWrite(RA_DIR, STP_BACK);
    PIOC->PIO_SODR = (1u << 26);
    delayMicroseconds(2);
    PIOC->PIO_CODR = (1u << 26);
    RA_microSteps += 1;
  }
}
//////////////////////////////////////////////////////////// Slew-To Function ///////////////////////////////////////////////////////////

void cosiderSlewTo_NonBlocking() {
  
  // === DEBUG ===
  #ifdef serial_debug
  static unsigned long lastCallTime = 0;
  if (millis() - lastCallTime > 1000) {
    Serial.print("cosiderSlewTo called - slewInit="); Serial.print(slewInitialized);
    Serial.print(" IS_OBJ_FOUND="); Serial.print(IS_OBJ_FOUND);
    Serial.print(" RA_found="); Serial.print(IS_OBJECT_RA_FOUND);
    Serial.print(" DEC_found="); Serial.print(IS_OBJECT_DEC_FOUND);
    Serial.print(" RA_active="); Serial.print(slewRA_active);
    Serial.print(" DEC_active="); Serial.println(slewDEC_active);
    lastCallTime = millis();
  }
  #endif
  
  // === PROTECTION : incoherent state ===
  if (!slewInitialized && (slewRA_active || slewDEC_active)) {
    #ifdef serial_debug
    Serial.println("ERROR: Inconsistent state, forcing cleanup");
    #endif
    stopSlewRA();
    stopSlewDEC();
    slewRA_active = false;
    slewDEC_active = false;
    IS_OBJECT_RA_FOUND = false;
    IS_OBJECT_DEC_FOUND = false;
    IS_OBJ_FOUND = true;
    RA_move_ending = 0;
    RA_waitingToStart = false;
    return;
  }
  
  if (!slewInitialized) {
    slewInitialized = true;



    DEC_accelerationDone = false;
    RA_accelerationDone = false;


    
    RA_move_ending = 0;
    RA_waitingToStart = false;
    
    // initial computing
    float HAH = (HAHour >= 12) ? (HAHour - 12) : HAHour;
    float HAM = HAMin;
    IS_MERIDIAN_PASSED = (HAHour < 12);
    
    double HA_decimal = ((HAH + (HAM / 60)) * 15) + delta_a_RA;
    double DEC_decimal = OBJECT_DEC_D + (OBJECT_DEC_M / 60) + delta_a_DEC;
    
    SLEW_RA_microsteps = HA_decimal * HA_H_CONST;
    SLEW_DEC_microsteps = DEC_90 - (DEC_decimal * DEC_D_CONST);
    
    if (IS_MERIDIAN_PASSED) {
      SLEW_DEC_microsteps *= -1;
    }

    // Position HOME
    int home_pos = 0;
    if ((OBJECT_RA_H == 12) && (OBJECT_RA_M == 0) && 
        (OBJECT_DEC_D == 90) && (OBJECT_DEC_M == 0)) {
      SLEW_RA_microsteps = RA_90;
      SLEW_DEC_microsteps = 0;
      home_pos = 1;
    }
    
    // initial configuration
    setmStepsMode("R", 16);
    setmStepsMode("D", 16);
    delay(10);
    
    long delta_RA_steps_signed = SLEW_RA_microsteps - RA_microSteps;
    long delta_DEC_steps_signed = SLEW_DEC_microsteps - DEC_microSteps;
    
    long delta_RA_steps = abs(delta_RA_steps_signed);
    long delta_DEC_steps = abs(delta_DEC_steps_signed);

    #ifdef serial_debug
    Serial.println("=== SLEW INIT ===");
    Serial.print("Current RA: "); Serial.println(RA_microSteps);
    Serial.print("Current DEC: "); Serial.println(DEC_microSteps);
    Serial.print("Target RA: "); Serial.println(SLEW_RA_microsteps);
    Serial.print("Target DEC: "); Serial.println(SLEW_DEC_microsteps);
    Serial.print("Delta RA: "); Serial.println(delta_RA_steps_signed);
    Serial.print("Delta DEC: "); Serial.println(delta_DEC_steps_signed);
    #endif

    // === PROTECTION : too small distance ===
    const long MIN_SLEW_DISTANCE = 50;
    
    if (delta_RA_steps < MIN_SLEW_DISTANCE && delta_DEC_steps < MIN_SLEW_DISTANCE) {
      #ifdef serial_debug
      Serial.println("*** TARGET TOO CLOSE - Already on target ***");
      #endif
      
      // End sequence completed
      IS_OBJECT_RA_FOUND = true;
      IS_OBJECT_DEC_FOUND = true;
      IS_OBJ_FOUND = true;
      RA_move_ending = 0;
      slewInitialized = false;
      
      stopSlewRA();
      stopSlewDEC();
      
      Slew_RA_timer = 0;
      RA_finish_last = 0;
      
      #ifdef serial_debug
      Serial.println("=== SLEW COMPLETE (no movement needed) ===");
      #endif
      
      Serial.println("SLEW:COMPLETE");
      
      // Gestion tracking
      if (isParkOperation) {
        isAtPark = true;
        isParkOperation = false;
        IS_TRACKING = false;
        trackingPending = false;
        stopTracking();
        safeStringCopy(OBJECT_DESCR, "Telescope parked", sizeof(OBJECT_DESCR));
        safeStringCopy(OBJECT_DETAILS, "Unpark telescope to unlock", sizeof(OBJECT_DETAILS));
      } else if (isHomeOperation) {
        isAtHome = true;
        isHomeOperation = false;
        IS_TRACKING = false;
        trackingPending = false;
        stopTracking();
        safeStringCopy(OBJECT_DESCR, "Telescope at Home", sizeof(OBJECT_DESCR));
        safeStringCopy(OBJECT_DETAILS, "Ready for use", sizeof(OBJECT_DETAILS));
      } else {
        if (ALT > 0) {
          if (trackingPending) {
            IS_TRACKING = true;
            startTracking();
            trackingPending = false;
          } else {
            if (!IS_TRACKING) {
              IS_TRACKING = true;
              startTracking();
            }
          }
        }
      }
      
      if (IS_IN_OPERATION) {
        drawMainScreen();
      } else {
        drawConstelationScreen(SELECTED_STAR);
      }
      
      return;
    }

    // Find who finish first
    if (delta_RA_steps >= delta_DEC_steps) {
      RA_finish_last = 1;
      Slew_RA_timer = millis();
    } else {
      RA_finish_last = 0;
    }
    
    // Start DEC
    if (delta_DEC_steps >= MIN_SLEW_DISTANCE) {
      int dirDEC = (delta_DEC_steps_signed > 0) ? STP_BACK : STP_FWD;
      long margin = max((long)100, delta_DEC_steps / 10);
      long safeDEC_steps = delta_DEC_steps + margin;
      long periodDEC = 800;
      startSlewDEC(dirDEC, safeDEC_steps, periodDEC);
      
      #ifdef serial_debug
      Serial.print("DEC START: dir="); Serial.print(dirDEC == STP_BACK ? "BACK" : "FWD");
      Serial.print(" steps="); Serial.println(safeDEC_steps);
      #endif
      
      // RA start after delay planification
      if (delta_RA_steps >= MIN_SLEW_DISTANCE) {
        RA_startRequestTime = millis() + RA_START_DELAY_MS;
        RA_waitingToStart = true;
        
        #ifdef serial_debug
        Serial.print("RA will start in "); Serial.print(RA_START_DELAY_MS); Serial.println(" ms");
        #endif
      }
    } else {
      #ifdef serial_debug
      Serial.println("DEC distance too small, skipping");
      #endif
      IS_OBJECT_DEC_FOUND = true;
    }
    
    // RA start only if RA_finish_last == 1
    if (delta_RA_steps >= MIN_SLEW_DISTANCE && RA_finish_last == 1 && delta_DEC_steps < MIN_SLEW_DISTANCE) {
      // Ces where only RA have to move
      int dirRA = (delta_RA_steps_signed > 0) ? STP_BACK : STP_FWD;
      long margin = max((long)100, delta_RA_steps / 10);
      long safeRA_steps = delta_RA_steps + margin;
      long periodRA = 800;
      startSlewRA(dirRA, safeRA_steps, periodRA);
      
      #ifdef serial_debug
      Serial.println("RA START (no DEC movement)");
      #endif
    } else if (delta_RA_steps < MIN_SLEW_DISTANCE) {
      #ifdef serial_debug
      Serial.println("RA distance too small, skipping");
      #endif
      IS_OBJECT_RA_FOUND = true;
    }
  }
  
  // === CODE EXECUTED AFTER INITIALISATION (at each call) ===
  
  // Start RA after delay
  if (RA_waitingToStart && millis() >= RA_startRequestTime) {
    long delta_RA_steps_signed = SLEW_RA_microsteps - RA_microSteps;
    long delta_RA_steps = abs(delta_RA_steps_signed);
    
    const long MIN_SLEW_DISTANCE = 50;
    
    if (delta_RA_steps >= MIN_SLEW_DISTANCE) {
      int dirRA = (delta_RA_steps_signed > 0) ? STP_BACK : STP_FWD;
      long margin = max((long)100, delta_RA_steps / 10);
      long safeRA_steps = delta_RA_steps + margin;
      long periodRA = 800;
      
      Slew_RA_timer = millis();
      setmStepsMode("R", 16);
      delay(10);
      
      startSlewRA(dirRA, safeRA_steps, periodRA);
      
      #ifdef serial_debug
      Serial.println("RA START (delayed after DEC start)");
      #endif
    }
    
    RA_waitingToStart = false;
  }
  
  // === PROGRESSIVE DEC ACCELERATION ===
  //static bool DEC_accelerationDone = false;
  //static int DEC_accel_step = 0;
  
  if (!DEC_accelerationDone && slewDEC_active) {
    unsigned long delta_DEC_time = millis() - Slew_timer;

      if (delta_DEC_time >= 4200) {
        if (DEC_mode_steps != 1) setmStepsMode("D", 1);
        DEC_accelerationDone = true;
      } else if (delta_DEC_time >= 3400) {
        if (DEC_mode_steps != 2) setmStepsMode("D", 2);
      } else if (delta_DEC_time >= 2600) {
        if (DEC_mode_steps != 4) setmStepsMode("D", 4);
      } else if (delta_DEC_time >= 1800) {
        if (DEC_mode_steps != 8) setmStepsMode("D", 8);
      }
  }
  
  // === PROGRESSIVE RA ACCELERATION ===
  //static bool RA_accelerationDone = false;
  //static int RA_accel_step = 0;
  
  if (!RA_accelerationDone && slewRA_active) {
    unsigned long delta_RA_time = millis() - Slew_RA_timer;
   
      if (delta_RA_time >= 4200) {
        if (RA_mode_steps != 1) setmStepsMode("R", 1);
        RA_accelerationDone = true;
      } else if (delta_RA_time >= 3400) {
        if (RA_mode_steps != 2) setmStepsMode("R", 2);
      } else if (delta_RA_time >= 2600) {
        if (RA_mode_steps != 4) setmStepsMode("R", 4);
      } else if (delta_RA_time >= 1800) {
        if (RA_mode_steps != 8) setmStepsMode("R", 8);
      }
  }
  
  //  DECEL
  static unsigned long lastDecelCheck = 0;
  if (millis() - lastDecelCheck > 50) {
    
    // DEC - PRGRESSIVE DECEL
    if (slewDEC_active) {
      long remaining_to_target = abs(SLEW_DEC_microsteps - DEC_microSteps);
      
      #ifdef serial_debug
      Serial.print("REAL REMAIN DEC: "); Serial.println(remaining_to_target);
      #endif
      
      if ((remaining_to_target >= DECEL_THRESHOLD_MODE_4 && remaining_to_target <= DECEL_THRESHOLD_MODE_8) && (DEC_mode_steps == 16 || DEC_mode_steps == 8)) {
        setmStepsMode("D", 2);
        #ifdef serial_debug
        Serial.println("DEC -> mode 2");
        #endif
      } else if ((remaining_to_target >= DECEL_THRESHOLD_MODE_2 && remaining_to_target < DECEL_THRESHOLD_MODE_4) && (DEC_mode_steps == 8 || DEC_mode_steps == 4)) {
        setmStepsMode("D", 4);
        #ifdef serial_debug
        Serial.println("DEC -> mode 4");
        #endif
      } else if ((remaining_to_target >= DECEL_THRESHOLD_MODE_1 && remaining_to_target < DECEL_THRESHOLD_MODE_2) && (DEC_mode_steps == 4 || DEC_mode_steps == 2)) {
        setmStepsMode("D", 8);
        #ifdef serial_debug
        Serial.println("DEC -> mode 8");
        #endif
      } else if (remaining_to_target < DECEL_THRESHOLD_MODE_1 && (DEC_mode_steps == 2 || DEC_mode_steps == 1)) {
        setmStepsMode("D", 16);
        #ifdef serial_debug
        Serial.println("DEC -> mode 16");
        #endif
      }
    }
    
    // RA - DECEL
    if (slewRA_active) {
      long remaining_to_target = abs(SLEW_RA_microsteps - RA_microSteps);
      
      #ifdef serial_debug
      Serial.print("REAL REMAIN RA: "); Serial.println(remaining_to_target);
      #endif

      if ((remaining_to_target >= DECEL_THRESHOLD_MODE_4 && remaining_to_target <= DECEL_THRESHOLD_MODE_8) && (RA_mode_steps == 16 || RA_mode_steps == 8)) {
      setmStepsMode("R", 2);
      #ifdef serial_debug
      Serial.println("RA -> mode 2");
      #endif
      } else if ((remaining_to_target >= DECEL_THRESHOLD_MODE_2 && remaining_to_target < DECEL_THRESHOLD_MODE_4) && (RA_mode_steps == 8 || RA_mode_steps == 4)) {
      setmStepsMode("R", 4);
      #ifdef serial_debug
      Serial.println("RA -> mode 4");
      #endif
      } else if ((remaining_to_target >= DECEL_THRESHOLD_MODE_1 && remaining_to_target < DECEL_THRESHOLD_MODE_2) && (RA_mode_steps == 4 || RA_mode_steps == 2)) {
      setmStepsMode("R", 8);
      #ifdef serial_debug
      Serial.println("RA -> mode 8");
      #endif
      } else if (remaining_to_target < DECEL_THRESHOLD_MODE_1 && (RA_mode_steps == 2 || RA_mode_steps == 1)) {
      setmStepsMode("R", 16);
      #ifdef serial_debug
      Serial.println("RA -> mode 16");
      #endif
      }
    }
    
    lastDecelCheck = millis();
  }
  
  // === RA Start after DEC ===
  if (RA_finish_last == 0 && IS_OBJECT_DEC_FOUND && !IS_OBJECT_RA_FOUND && !RA_waitingToStart && !slewRA_active) {
    long delta_RA_steps_signed = SLEW_RA_microsteps - RA_microSteps;
    long delta_RA_steps = abs(delta_RA_steps_signed);
    
    const long MIN_SLEW_DISTANCE = 50;
    
    if (delta_RA_steps >= MIN_SLEW_DISTANCE) {
      int dirRA = (delta_RA_steps_signed > 0) ? STP_BACK : STP_FWD;
      long margin = max((long)100, delta_RA_steps / 10);
      long safeRA_steps = delta_RA_steps + margin;
      long periodRA = 800;
      
      RA_accelerationDone = false;
      RA_finish_last = 1;
      Slew_RA_timer = millis();
      
      setmStepsMode("R", 16);
      delay(10);
      
      startSlewRA(dirRA, safeRA_steps, periodRA);
      
      #ifdef serial_debug
      Serial.println("RA START (fallback after DEC)");
      #endif
    } else {
      #ifdef serial_debug
      Serial.println("RA distance too small after DEC");
      #endif
      IS_OBJECT_RA_FOUND = true;
    }
  }
  
  // === DEBUG ===
  #ifdef serial_debug
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 500) {
    Serial.print("RA_left="); Serial.print(slewRA_targetSteps);
    Serial.print(" DEC_left="); Serial.print(slewDEC_targetSteps);
    Serial.print(" RA_mode="); Serial.print(RA_mode_steps);
    Serial.print(" DEC_mode="); Serial.print(DEC_mode_steps);
    Serial.print(" RA_pos="); Serial.print(RA_microSteps);
    Serial.print(" DEC_pos="); Serial.print(DEC_microSteps);
    Serial.print(" RA_found="); Serial.print(IS_OBJECT_RA_FOUND);
    Serial.print(" DEC_found="); Serial.println(IS_OBJECT_DEC_FOUND);
    lastDebug = millis();
  }
  #endif
  
  // === SLEW END ===
//  if (IS_OBJECT_RA_FOUND && IS_OBJECT_DEC_FOUND) {
//    IS_OBJ_FOUND = true;
//    RA_move_ending = 0;
//    slewInitialized = false;
//    DEC_accelerationDone = false;
//    RA_accelerationDone = false;
//    DEC_accel_step = 0;
//    RA_accel_step = 0;
//    RA_waitingToStart = false;

if (IS_OBJECT_RA_FOUND && IS_OBJECT_DEC_FOUND) {
    IS_OBJ_FOUND = true;
    RA_move_ending = 0;
    slewInitialized = false;
    RA_waitingToStart = false;




    
    stopSlewRA();
    stopSlewDEC();
    
    #ifdef serial_debug
    Serial.println("=== SLEW COMPLETE ===");
    Serial.print("Final RA: "); Serial.println(RA_microSteps);
    Serial.print("Final DEC: "); Serial.println(DEC_microSteps);
    #endif
    
    Serial.println("SLEW:COMPLETE");
    
    // tracking ASCOM management
    if (isParkOperation) {
      isAtPark = true;
      isParkOperation = false;
      IS_TRACKING = false;
      trackingPending = false;
      stopTracking();
      safeStringCopy(OBJECT_DESCR, "Telescope parked", sizeof(OBJECT_DESCR));
      safeStringCopy(OBJECT_DETAILS, "Unpark telescope to unlock", sizeof(OBJECT_DETAILS));
    } else if (isHomeOperation) {
      isAtHome = true;
      isHomeOperation = false;
      IS_TRACKING = false;
      trackingPending = false;
      stopTracking();
      safeStringCopy(OBJECT_DESCR, "Telescope at Home", sizeof(OBJECT_DESCR));
      safeStringCopy(OBJECT_DETAILS, "Ready for use", sizeof(OBJECT_DETAILS));
    } else {
      if (ALT > 0) {
        if (trackingPending) {
          IS_TRACKING = true;
          startTracking();
          trackingPending = false;
        } else {
          startTracking();
        }
      }
    }
    
    if (IS_SOUND_ON) {
      SoundOn(note_C, 64);
    }
    
    Slew_RA_timer = 0;
    RA_finish_last = 0;
    
    if (IS_BT_MODE_ON) {
      Serial3.println("Slew done! Object in scope!");
    }
    
    if (IS_IN_OPERATION) {
      drawMainScreen();
    } else {
      drawConstelationScreen(SELECTED_STAR);
    }
  }
}

/////////////////////////////////////////////////////////// Consider Manual Move Function ///////////////////////////////////////////////////////////

// Global variables
unsigned long manual_move_start_time_ra = 0;
unsigned long manual_move_start_time_dec = 0;
unsigned long last_manual_move_time = 0;
unsigned long last_ra_activity = 0;
unsigned long last_dec_activity = 0;
int manual_move_speed_ra = 16;
int manual_move_speed_dec = 16;
bool is_manual_moving_ra = false;
bool is_manual_moving_dec = false;
int last_xp = 512, last_yp = 512;
int last_ra_direction = 0;  // -1=left, 0=center, 1=right
int last_dec_direction = 0; // -1=bottom, 0=center, 1=top

// independant acceleration by axis
void consider_Manual_Move(int xP, int yP) {
  unsigned long current_time = millis();
  
  // verify axis
  bool ra_centered = (xP > 470 && xP < 630);
  bool dec_centered = (yP > 440 && yP < 600);
  
  // actual RA axis direction
  int current_ra_direction = 0;
  if (xP <= 470) current_ra_direction = -1;  // Left
  else if (xP >= 630) current_ra_direction = 1;   // Right
  
  // Independant RA management
  if (ra_centered) {
    // Mark the stop moment but do not reset immediately
    last_ra_activity = current_time;
    
    // Reset after 500ms pause
    if (is_manual_moving_ra && (current_time - last_ra_activity > 500)) {
      is_manual_moving_ra = false;
      manual_move_start_time_ra = 0;
      manual_move_speed_ra = 16;
      last_ra_direction = 0;
    }
  } else {
    last_ra_activity = current_time;
    
    // verify direction change
    bool direction_changed = (last_ra_direction != 0) && (current_ra_direction != last_ra_direction);
    
    if (!is_manual_moving_ra) {
      // First movement or after a pause
      is_manual_moving_ra = true;
      manual_move_start_time_ra = current_time;
      manual_move_speed_ra = 16;
    } else if (direction_changed) {
      // Direction change: restart acceleration from the beginning
      manual_move_start_time_ra = current_time;
      manual_move_speed_ra = 16;
    }
    
    last_ra_direction = current_ra_direction;
    
    // acceleration computing for RA
    unsigned long move_duration_ra = current_time - manual_move_start_time_ra;
    
    if (move_duration_ra >= 0 && move_duration_ra < 500) {
      manual_move_speed_ra = 16;
    } else if (move_duration_ra >= 500 && move_duration_ra < 1000) {
      manual_move_speed_ra = 8;
    } else if (move_duration_ra >= 1000 && move_duration_ra < 1500) {
      manual_move_speed_ra = 4;
    } else if (move_duration_ra >= 1500) {
      manual_move_speed_ra = 2;
    }
  }
  
  // determine DEC direction
  int current_dec_direction = 0;
  if (yP <= 440) current_dec_direction = -1;  // Down
  else if (yP >= 600) current_dec_direction = 1;   // Up
  
  // Independant DEC direction management
  if (dec_centered) {
    // Mark stop moment, but not reset immediately
    last_dec_activity = current_time;
    
    // Reset after 500ms pause
    if (is_manual_moving_dec && (current_time - last_dec_activity > 500)) {
      is_manual_moving_dec = false;
      manual_move_start_time_dec = 0;
      manual_move_speed_dec = 16;
      last_dec_direction = 0;
    }
  } else {
    last_dec_activity = current_time;
    
    // Verify direction change
    bool direction_changed = (last_dec_direction != 0) && (current_dec_direction != last_dec_direction);
    
    if (!is_manual_moving_dec) {
    // First movement or after a pause
      is_manual_moving_dec = true;
      manual_move_start_time_dec = current_time;
      manual_move_speed_dec = 16;
    } else if (direction_changed) {
     // Change of direction: restart acceleration from the beginning
      manual_move_start_time_dec = current_time;
      manual_move_speed_dec = 16;
    }
    
    last_dec_direction = current_dec_direction;
    
    // acceleration computing for DEC
    unsigned long move_duration_dec = current_time - manual_move_start_time_dec;
    
    if (move_duration_dec >= 0 && move_duration_dec < 500) {
      manual_move_speed_dec = 16;
    } else if (move_duration_dec >= 500 && move_duration_dec < 1000) {
      manual_move_speed_dec = 8;
    } else if (move_duration_dec >= 1000 && move_duration_dec < 1500) {
      manual_move_speed_dec = 4;
    } else if (move_duration_dec >= 1500) {
      manual_move_speed_dec = 2;
    }
  }
  
  // step freq control
  unsigned long step_interval_micros = 500;
  
  // Adjsut max speed for both axis
  int fastest_speed = min(manual_move_speed_ra, manual_move_speed_dec);
  if (fastest_speed == 2) step_interval_micros = 200;
  else if (fastest_speed == 4) step_interval_micros = 300;
  else if (fastest_speed == 8) step_interval_micros = 500;
  else step_interval_micros = 800;
  
  if (micros() - last_manual_move_time < step_interval_micros) {
    return;
  }
  last_manual_move_time = micros();
  
  // RA movment
  if (!ra_centered) {
    if ((xP >= 0) && (xP <= 210)) {
      int ra_speed = max(manual_move_speed_ra, 2);
      setmStepsMode("R", ra_speed);
      digitalWrite(RA_DIR, STP_FWD);
      PIOC->PIO_SODR = (1u << 26);
      delayMicroseconds(5);
      PIOC->PIO_CODR = (1u << 26);
      RA_microSteps -= RA_mode_steps;
    } else if ((xP > 210) && (xP <= 340)) {
      int ra_speed = (manual_move_speed_ra > 4) ? manual_move_speed_ra : 4;
      setmStepsMode("R", ra_speed);
      digitalWrite(RA_DIR, STP_FWD);
      PIOC->PIO_SODR = (1u << 26);
      delayMicroseconds(5);
      PIOC->PIO_CODR = (1u << 26);
      RA_microSteps -= RA_mode_steps;
    } else if ((xP > 340) && (xP <= 470)) {
      int ra_speed = (manual_move_speed_ra > 8) ? manual_move_speed_ra : 8;
      setmStepsMode("R", ra_speed);
      digitalWrite(RA_DIR, STP_FWD);
      PIOC->PIO_SODR = (1u << 26);
      delayMicroseconds(5);
      PIOC->PIO_CODR = (1u << 26);
      RA_microSteps -= RA_mode_steps;
    } else if ((xP > 630) && (xP <= 760)) {
      int ra_speed = (manual_move_speed_ra > 8) ? manual_move_speed_ra : 8;
      setmStepsMode("R", ra_speed);
      digitalWrite(RA_DIR, STP_BACK);
      PIOC->PIO_SODR = (1u << 26);
      delayMicroseconds(5);
      PIOC->PIO_CODR = (1u << 26);
      RA_microSteps += RA_mode_steps;
    } else if ((xP > 760) && (xP <= 890)) {
      int ra_speed = (manual_move_speed_ra > 4) ? manual_move_speed_ra : 4;
      setmStepsMode("R", ra_speed);
      digitalWrite(RA_DIR, STP_BACK);
      PIOC->PIO_SODR = (1u << 26);
      delayMicroseconds(5);
      PIOC->PIO_CODR = (1u << 26);
      RA_microSteps += RA_mode_steps;
    } else if ((xP > 890) && (xP <= 1023)) {
      int ra_speed = max(manual_move_speed_ra, 2);
      setmStepsMode("R", ra_speed);
      digitalWrite(RA_DIR, STP_BACK);
      PIOC->PIO_SODR = (1u << 26);
      delayMicroseconds(5);
      PIOC->PIO_CODR = (1u << 26);
      RA_microSteps += RA_mode_steps;
    }
  }

  // DEC movment
  if (!dec_centered) {
    if ((yP >= 0) && (yP <= 180)) {
      int dec_speed = max(manual_move_speed_dec, 2);
      setmStepsMode("D", dec_speed);
      digitalWrite(DEC_DIR, STP_BACK);
      PIOC->PIO_SODR = (1u << 24);
      delayMicroseconds(5);
      PIOC->PIO_CODR = (1u << 24);
      DEC_microSteps += DEC_mode_steps;
    } else if ((yP > 180) && (yP <= 310)) {
      int dec_speed = (manual_move_speed_dec > 4) ? manual_move_speed_dec : 4;
      setmStepsMode("D", dec_speed);
      digitalWrite(DEC_DIR, STP_BACK);
      PIOC->PIO_SODR = (1u << 24);
      delayMicroseconds(5);
      PIOC->PIO_CODR = (1u << 24);
      DEC_microSteps += DEC_mode_steps;
    } else if ((yP > 310) && (yP <= 440)) {
      int dec_speed = (manual_move_speed_dec > 8) ? manual_move_speed_dec : 8;
      setmStepsMode("D", dec_speed);
      digitalWrite(DEC_DIR, STP_BACK);
      PIOC->PIO_SODR = (1u << 24);
      delayMicroseconds(5);
      PIOC->PIO_CODR = (1u << 24);
      DEC_microSteps += DEC_mode_steps;
    } else if ((yP > 600) && (yP <= 730)) {
      int dec_speed = (manual_move_speed_dec > 8) ? manual_move_speed_dec : 8;
      setmStepsMode("D", dec_speed);
      digitalWrite(DEC_DIR, STP_FWD);
      PIOC->PIO_SODR = (1u << 24);
      delayMicroseconds(5);
      PIOC->PIO_CODR = (1u << 24);
      DEC_microSteps -= DEC_mode_steps;
    } else if ((yP > 730) && (yP <= 860)) {
      int dec_speed = (manual_move_speed_dec > 4) ? manual_move_speed_dec : 4;
      setmStepsMode("D", dec_speed);
      digitalWrite(DEC_DIR, STP_FWD);
      PIOC->PIO_SODR = (1u << 24);
      delayMicroseconds(5);
      PIOC->PIO_CODR = (1u << 24);
      DEC_microSteps -= DEC_mode_steps;
    } else if ((yP > 860) && (yP <= 1023)) {
      int dec_speed = max(manual_move_speed_dec, 2);
      setmStepsMode("D", dec_speed);
      digitalWrite(DEC_DIR, STP_FWD);
      PIOC->PIO_SODR = (1u << 24);
      delayMicroseconds(5);
      PIOC->PIO_CODR = (1u << 24);
      DEC_microSteps -= DEC_mode_steps;
    }
  }
}

///////////////////////////////////////////////////////////////// GPS Power Feed Function ///////////////////////////////////////////////////////////

// Keep the GPS sensor "fed" until we find the data.
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  if (IS_SOUND_ON) {
    SoundOn(note_c, 8);
  }
  do
  {
    while (Serial2.available())
      gps.encode(Serial2.read());
  } while (millis() - start < ms);
}

///////////////////////////////////////////////////////////////// Set Steps Mode Function ///////////////////////////////////////////////////////////

void setmStepsMode(const char* P, int mod) {
  // P means the axis: RA or DEC; mod means MicroSteppping mode: x32, x16, x8....
  // setmStepsMode(R,2) - means RA with 1/2 steps; setmStepsMode(R,4) - means RA with 1/4 steps


  // PINS Mapping for fast switching
  // DEC_M2 - Pin 8 UP - PC22 - PIOC->PIO_SODR=(1u<<22);
  // DEC_M1 - Pin 9 UP - PC21 -  PIOC->PIO_SODR=(1u<<21);
  // DEC_M0 - Pin 10 UP - PC29 -  PIOC->PIO_SODR=(1u<<29);
  // RA_M0 - Pin 11 UP - PD7 -  PIOD->PIO_SODR=(1u<<7);
  // RA_M1 - Pin 12 UP - PD8 -  PIOD->PIO_SODR=(1u<<8);
  // RA_M2 - Pin 13 UP - PB27 -  PIOB->PIO_SODR=(1u<<27);
  // DEC_M2 - Pin 8 DOWN - PC22 - PIOC->PIO_CODR=(1u<<22);
  // DEC_M1 - Pin 9 DOWN - PC21 -  PIOC->PIO_CODR=(1u<<21);
  // DEC_M0 - Pin 10 DOWN - PC29 -  PIOC->PIO_CODR=(1u<<29);
  // RA_M0 - Pin 11 DOWN - PD7 -  PIOD->PIO_CODR=(1u<<7);
  // RA_M1 - Pin 12 DOWN - PD8 -  PIOD->PIO_CODR=(1u<<8);
  // RA_M2 - Pin 13 DOWN - PB27 -  PIOB->PIO_CODR=(1u<<27);
  //
  // PIOC->PIO_SODR=(1u<<25); // Set Pin High
  // PIOC->PIO_CODR=(1u<<25); // Set Pin Low

  if (P == "R") { // Set RA modes
    if (mod == 1) {                     // Full Step
      PIOD->PIO_CODR = (1u << 7);
      PIOD->PIO_CODR = (1u << 8);
      PIOB->PIO_CODR = (1u << 27);
    }
    if (mod == 2) {                     // 1/2 Step
      PIOD->PIO_SODR = (1u << 7);
      PIOD->PIO_CODR = (1u << 8);
      PIOB->PIO_CODR = (1u << 27);
    }
    if (mod == 4) {                     // 1/4 Step
      PIOD->PIO_CODR = (1u << 7);
      PIOD->PIO_SODR = (1u << 8);
      PIOB->PIO_CODR = (1u << 27);
    }
    if (mod == 8) {                     // 1/8 Step
      PIOD->PIO_SODR = (1u << 7);
      PIOD->PIO_SODR = (1u << 8);
      PIOB->PIO_CODR = (1u << 27);
    }
    if (mod == 16) {                     // 1/16 Step
      PIOD->PIO_CODR = (1u << 7);
      PIOD->PIO_CODR = (1u << 8);
      PIOB->PIO_SODR = (1u << 27);
    }
    if (mod == 32) {                     // 1/32 Step
      PIOD->PIO_SODR = (1u << 7);
      PIOD->PIO_CODR = (1u << 8);
      PIOB->PIO_SODR = (1u << 27);
    }
    RA_mode_steps = MICROSteps / mod;
  }
  if (P == "D") { // Set DEC modes
    if (mod == 1) {                     // Full Step
      PIOC->PIO_CODR = (1u << 29);
      PIOC->PIO_CODR = (1u << 21);
      PIOC->PIO_CODR = (1u << 22);
    }
    if (mod == 2) {                     // 1/2 Step
      PIOC->PIO_SODR = (1u << 29);
      PIOC->PIO_CODR = (1u << 21);
      PIOC->PIO_CODR = (1u << 22);
    }
    if (mod == 4) {                     // 1/4 Step
      PIOC->PIO_CODR = (1u << 29);
      PIOC->PIO_SODR = (1u << 21);
      PIOC->PIO_CODR = (1u << 22);
    }
    if (mod == 8) {                     // 1/8 Step
      PIOC->PIO_SODR = (1u << 29);
      PIOC->PIO_SODR = (1u << 21);
      PIOC->PIO_CODR = (1u << 22);
    }
    if (mod == 16) {                     // 1/16 Step
      PIOC->PIO_CODR = (1u << 29);
      PIOC->PIO_CODR = (1u << 21);
      PIOC->PIO_SODR = (1u << 22);
    }
    if (mod == 32) {                     // 1/32 Step
      PIOC->PIO_SODR = (1u << 29);
      PIOC->PIO_CODR = (1u << 21);
      PIOC->PIO_SODR = (1u << 22);
    }
    DEC_mode_steps = MICROSteps / mod;
  }
  delayMicroseconds(5);   // Makes sure the DRV8825 can follow
}
///////////////////////////////////////////////////////////////// Sound-ON Function ///////////////////////////////////////////////////////////

void SoundOn(int note, int duration) {
  duration *= 10000;
  long elapsed_time = 0;
  while (elapsed_time < duration) {
    digitalWrite(speakerOut, HIGH);
    delayMicroseconds(note / 2);
    // DOWN
    digitalWrite(speakerOut, LOW);
    delayMicroseconds(note / 2);
    // Keep track of how long we pulsed
    elapsed_time += (note);
  }
}
///////////////////////////////////////////////////////////////// Update Observed Objects Function ///////////////////////////////////////////////////////////

void UpdateObservedObjects() {
    char currentTime[10];
    safeStringCopy(currentTime, rtc.getTimeStr(), sizeof(currentTime));
    
    char prevTime[10];
    safeStringCopy(prevTime, Prev_Obj_Start, sizeof(prevTime));
    
    int currentMin = (atoi(strtok(currentTime, ":")) * 60) + atoi(strtok(NULL, ":"));
    int prevMin = (atoi(strtok(prevTime, ":")) * 60) + atoi(strtok(NULL, ":"));
    
    int Delta_Time = currentMin - prevMin;
    if (Delta_Time < 0) Delta_Time += 1440;
    
    char deltaStr[10];
    snprintf(deltaStr, sizeof(deltaStr), ";%d", Delta_Time);
    strcat(ObservedObjects[Observed_Obj_Count - 1], deltaStr);
    
    char newEntry[300];
    snprintf(newEntry, sizeof(newEntry), "%s;%s;%.5s;%d;%d;%dh %dm;%d",
             OBJECT_NAME, OBJECT_DETAILS, rtc.getTimeStr(), 
             (int)_temp, (int)_humid, (int)HAHour, (int)HAMin, (int)ALT);
    
    safeStringCopy(ObservedObjects[Observed_Obj_Count], newEntry, sizeof(ObservedObjects[0]));
    Observed_Obj_Count += 1;
    
    char timePart[6];
    strncpy(timePart, rtc.getTimeStr(), 5);
    timePart[5] = '\0';
    safeStringCopy(Prev_Obj_Start, timePart, sizeof(Prev_Obj_Start));
}

/////////////////////////////////////////////////////////////////// Current RA/DEC Function ///////////////////////////////////////////////////////////

void Current_RA_DEC() {
  //curr_RA_H, curr_RA_M, curr_RA_S, curr_DEC_D, curr_DEC_M, curr_DEC_S;
  // curr_RA_lz, curr_DEC_lz, curr_HA_lz;

float tmp_dec = (float(DEC_90) - float(abs(DEC_microSteps))) / float(DEC_D_CONST);
tmp_dec -= delta_a_DEC;

int sDEC_tel = (tmp_dec < 0) ? 45 : 43;

// DEC conversion
curr_DEC_D = (int)tmp_dec;  // Integer part = degrees

//double fractional_degrees = fabs(tmp_dec - curr_DEC_D);  // fract part (always positive)

double fractional_degrees;
if (tmp_dec < 0 && curr_DEC_D < 0) {
    fractional_degrees = fabs(tmp_dec) - fabs(curr_DEC_D);
} else {
    fractional_degrees = fabs(tmp_dec - curr_DEC_D);
}

double total_arcmin = fractional_degrees * 60.0;
curr_DEC_M = (int)total_arcmin;  // Entire part = arcminutes
double fractional_arcmin = total_arcmin - curr_DEC_M;

curr_DEC_S = fractional_arcmin * 60.0;  // Arcsecondes

// DEC overcome management
if (curr_DEC_S >= 60.0) {
  curr_DEC_M += (int)(curr_DEC_S / 60.0);
  curr_DEC_S = fmod(curr_DEC_S, 60.0);
}

if (curr_DEC_M >= 60) {
  curr_DEC_D += (curr_DEC_D >= 0) ? (curr_DEC_M / 60) : -(curr_DEC_M / 60);
  curr_DEC_M = fmod(curr_DEC_M, 60);
}

sprintf(curr_DEC_lz, "%c%02d%c%02d:%02d", sDEC_tel, int(abs(curr_DEC_D)), 223, int(abs(curr_DEC_M)), int(curr_DEC_S));
sprintf(curr_DEC_lz_disp, "%c%02d:%c%02d:%02d", sDEC_tel, int(abs(curr_DEC_D)), 223, int(abs(curr_DEC_M)), int(curr_DEC_S));

  // HOUR ANGLE
  // To correct for the Star Alignment
  double tmp_ha = double(RA_microSteps) / double(HA_H_CONST);
  tmp_ha -= delta_a_RA;
  if (DEC_microSteps > 0) {
    tmp_ha += 180;
  }
  tmp_ha /= 15;

  float tmp_ha_h = 0;
  float tmp_ha_m = 0;
  float tmp_ha_s = 0;
  tmp_ha_h = floor(tmp_ha);
  tmp_ha_m = (tmp_ha - floor(tmp_ha)) * 60;
  tmp_ha_s = (tmp_ha_m - floor(tmp_ha_m)) * 60;
  sprintf(curr_HA_lz, "%02d:%02d:%02d", int(tmp_ha_h), int(tmp_ha_m), int(tmp_ha_s));

// RIGHT ASC.
double tmp_ra = LST - tmp_ha;
if (LST < tmp_ha) {
  tmp_ra += 24;
}
curr_RA_H = (int)tmp_ra;  // Entire Part = hour
double fractional_hours = tmp_ra - curr_RA_H;  // Frac part

double total_minutes = fractional_hours * 60.0;
curr_RA_M = (int)total_minutes;  // Entire part = minutes
double fractional_minutes = total_minutes - curr_RA_M;

curr_RA_S = fractional_minutes * 60.0;  // Secondes

// Overflow management
if (curr_RA_S >= 60.0) {
  curr_RA_M += (int)(curr_RA_S / 60.0);
  curr_RA_S = fmod(curr_RA_S, 60.0);
}

if (curr_RA_M >= 60) {
  curr_RA_H += curr_RA_M / 60;
  curr_RA_M = fmod(curr_RA_M, 60);
}

if (curr_RA_H >= 24) {
  curr_RA_H = fmod(curr_RA_H, 24.0);
}

sprintf(curr_RA_lz, "%02d:%02d:%02d", int(curr_RA_H), int(curr_RA_M), int(curr_RA_S));

}
///////////////////////////////////////////////////////////////// Draw Button Function ///////////////////////////////////////////////////////////

void DrawButton(int X, int Y, int Width, int Height, const char* Caption, int16_t BodyColor, int16_t BorderColor, int16_t TextColor, int tSize, boolean Bold)
{
  //  TYPE: 0:Solid color, no Frame; 1: Frame Only button; 2: Solid color and Frame button 3: Only Frame;

  if ((BodyColor != 0) && (BorderColor == 0))
  {
    // Button Type = 0 ... Solid color, no Frame
    tft.fillRect(X, Y, Width, Height, BodyColor);
  }
  else if ((BodyColor == 0) && (BorderColor != 0))
  {
    // Button Type = 1 ... Frame Only button
    tft.drawRect(X, Y, Width, Height, BorderColor);
    tft.fillRect(X + 1, Y + 1, Width - 2, Height - 2, BLACK);
  }
  else if ((BodyColor != 0) && (BorderColor != 0))
  {
    // Button Type = 2 ... Solid color & Frame button
    tft.drawRect(X, Y, Width, Height, BorderColor);
    tft.fillRect(X + 1, Y + 1, Width - 2, Height - 2, BodyColor);
  }
  else
  {
    // Will not Draw Button and will return to code!
    return;
  }
  
  float TX = 0;
  float TY = 0;
  int captionLength = strlen(Caption);
  
  if (tSize == 2) {      // 10 x 14 px. (W x H)
    TX = (X + 1 + Width / 2) - (captionLength * 6);
    TY = Y + Height / 2 - 5;
  } else if (tSize == 1) { // 5 x 7 px. (W x H)
    TX = (X + 1 + Width / 2) - (captionLength * 3);
    TY = Y + Height / 2 - 3;
  } else if (tSize == 3) { // 15 x 21 px. (W x H)
    TX = (X + 1 + Width / 2) - (captionLength * 8);
    TY = Y + Height / 2 - 10;
  }
  
  tft.cursorToXY((int)TX, (int)TY);
  tft.setTextScale(tSize);
  tft.setTextColor(TextColor);
  
  if (strcmp(Caption, "+") == 0) {
    TX -= 5;
    tft.drawLine((int)TX + 10, (int)TY - 5, (int)TX + 10, (int)TY - 5, TextColor);
    tft.drawLine((int)TX + 8, (int)TY - 4, (int)TX + 12, (int)TY - 4, TextColor);
    tft.drawLine((int)TX + 6, (int)TY - 3, (int)TX + 14, (int)TY - 3, TextColor);
    tft.drawLine((int)TX + 4, (int)TY - 2, (int)TX + 16, (int)TY - 2, TextColor);
    tft.drawLine((int)TX + 2, (int)TY - 1, (int)TX + 18, (int)TY - 1, TextColor);
    tft.drawLine((int)TX, (int)TY, (int)TX + 20, (int)TY, TextColor);
    tft.drawLine((int)TX - 2, (int)TY + 1, (int)TX + 22, (int)TY + 1, TextColor);
    tft.drawLine((int)TX - 4, (int)TY + 2, (int)TX + 24, (int)TY + 2, TextColor);

    tft.drawLine((int)TX - 4, (int)TY + 8, (int)TX + 24, (int)TY + 8, TextColor);
    tft.drawLine((int)TX - 2, (int)TY + 9, (int)TX + 22, (int)TY + 9, TextColor);
    tft.drawLine((int)TX, (int)TY + 10, (int)TX + 20, (int)TY + 10, TextColor);
    tft.drawLine((int)TX + 2, (int)TY + 11, (int)TX + 18, (int)TY + 11, TextColor);
    tft.drawLine((int)TX + 4, (int)TY + 12, (int)TX + 16, (int)TY + 12, TextColor);
    tft.drawLine((int)TX + 6, (int)TY + 13, (int)TX + 14, (int)TY + 13, TextColor);
    tft.drawLine((int)TX + 8, (int)TY + 14, (int)TX + 12, (int)TY + 14, TextColor);
    tft.drawLine((int)TX + 10, (int)TY + 15, (int)TX + 10, (int)TY + 15, TextColor);
  } else {
    tft.print(Caption);
  }
}
///////////////////////////////////////////////////////////// Calculate Summer Time Function ///////////////////////////////////////////////////////////

/*
   isSummerTime() : uses Time.h
   ---------

   This function versifies if the date time needs to be updated by adding 1h due to summer-time

   Return: true if in summer-time, false otherwise
*/

bool isSummerTime() //in Italy: Summer time ends the last sunday of october and begins the last of march
{
  bool summer_time = false;

#ifdef serial_debug
  Serial.print(day());
  Serial.print("/");
  Serial.print(month());
  Serial.print("/");
  Serial.print(year());
  Serial.print("  weekday: ");
  Serial.println(weekday());
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
#endif

  // If I'm in October
  if (month() == 10)
  {
    // If it's Sunday
    if (weekday() == 1)
    {
      if (day() + 7 > 31 && hour() >= 3) summer_time = false;
      else summer_time = true;
    }
    else
    {
      // If last sunday has passed
      if ((day() + 7 - (weekday() - 1)) > 31) summer_time = false;
      else  summer_time = true;
    }
  }
  // If I'm in March
  else if (month() == 3)
  {
    // If It's Sunday
    if (weekday() == 1)
    {
      // If It is the Last Sunday
      if ((day() + 7) > 31 && hour() >= 2) summer_time = true;
      else summer_time = false;
    }
    else
    {
      // If it's not Sunday,but the last has already passed
      if ((day() + 7 - (weekday() - 1)) > 31) summer_time = true;
      else  summer_time = false;
    }
  }
  // If the Day Light Saving Time
  else if (month() >= 4 && month() <= 9) summer_time = true;
  // If we are in the Winter Time Period
  else if ((month() >= 1 && month() <= 2) || (month() >= 11 && month() <= 12)) summer_time = false;

  return summer_time;
}
//////////////////////////////////////////////////////////////// Calibrate Joystick Function ///////////////////////////////////////////////////////////

// Performs ~310.000 readings to find the mean value of the joypad (error: 3*10^-4 % )
void calibrateJoypad(int *x_cal, int *y_cal)
{
  tft.println("\n");
  tft.setTextColor(l_text);
  tft.println("Please avoid touching the Joypad for the next 3 \nseconds to let the software calibrate.\n");
  tft.setTextColor(l_text);

  int now_t = millis();
  int prev_t = millis();
  int c = 3;

  tft.print("3.. ");
  while (millis() < now_t + 3000)
  {
    if (millis() > prev_t + 1000)
    {
      c--;
      tft.print(c);
      tft.print(".. ");
      prev_t = millis();
    }
    *x_cal = (*x_cal + analogRead(A1)) / 2;
    *y_cal = (*y_cal + analogRead(A0)) / 2;
  }

  tft.setTextColor(GREEN);
  tft.print("done!");
  tft.setTextColor(l_text);
#ifdef serial_debug
  Serial.print("x_cal: ");
  Serial.println(*x_cal);
  Serial.print("y_cal: ");
  Serial.println(*y_cal);
#endif
}
//////////////////////////////////////////////////////////////// Store Options to SD Function ///////////////////////////////////////////////////////////

void storeOptions_SD()
{
  if (!SD.begin(SD_CS, SD_SPI_SPEED)) {
    tft.setTextColor(WHITE);
    tft.setTextScale(2);
    tft.cursorToXY(10, 100);
    tft.println("SD Init Failed!");
    delay(1000);
    return;
  }

  // buffer
  char optionsBuffer[200] = {0};
  
  snprintf(optionsBuffer, sizeof(optionsBuffer), 
           "(%d;%s;%d;%d;%s;%s;%d;%d;%.2f;%.2f;%.2f;%.2f)",
           TFT_Brightness,
           TFT_Time,
           Tracking_type,
           IS_MERIDIAN_FLIP_AUTOMATIC,
           Fan1_State,
           GTMEM_State,
           IS_SOUND_ON,
           IS_STEPPERS_ON,
           slopex,
           slopey,
           interx,
           intery);

  // direct writing w/o temp file
  File dataFile = SD.open("options.txt", O_WRITE | O_CREAT | O_TRUNC);
  if (!dataFile) {
    tft.setTextColor(WHITE);
    tft.setTextScale(2);
    tft.cursorToXY(10, 100);
    tft.println("File Open Error!");
    delay(1000);
    return;
  }

  dataFile.print(optionsBuffer);
  dataFile.close();

  delay(500);
}

///////////////////////////////////////////////////////////////// Load Options from SD  Function ///////////////////////////////////////////////////////////

void loadOptions_SD()
{
  File dataFile = SD.open("options.txt");

  if (dataFile)
  {
    char optionsBuffer[100] = {0};
    dataFile.read(optionsBuffer, sizeof(optionsBuffer) - 1);
    optionsBuffer[sizeof(optionsBuffer) - 1] = '\0';

#ifdef serial_debug
    Serial.print("options:");
    Serial.println(optionsBuffer);
#endif

    // Find parenthesis
    char* iniPac = strchr(optionsBuffer, '(');
    char* endPac = strchr(optionsBuffer, ')');

    if (iniPac != NULL && endPac != NULL && endPac > iniPac + 1)
    {
      // Extract between parenthesis
      char packetIn[100] = {0};
      strncpy(packetIn, iniPac + 1, endPac - iniPac - 1);
      packetIn[endPac - iniPac - 1] = '\0';

#ifdef serial_debug
      Serial.print("packetIn:");
      Serial.println(packetIn);
#endif

      // Parse value separator ;
      char valoriIn[12][50] = {0}; // 12 valeurs de 50 caractères max
      int valueCount = 0;
      char* token = strtok(packetIn, ";");
      
      while (token != NULL && valueCount < 12)
      {
        safeStringCopy(valoriIn[valueCount], token, sizeof(valoriIn[0]));
#ifdef serial_debug
        Serial.print("valoriIn[");
        Serial.print(valueCount);
        Serial.print("] :");
        Serial.println(valoriIn[valueCount]);
#endif
        token = strtok(NULL, ";");
        valueCount++;
      }

      // Update variables
      TFT_Brightness = atoi(valoriIn[0]);
      safeStringCopy(TFT_Time, valoriIn[1], sizeof(TFT_Time));
      Tracking_type = atoi(valoriIn[2]);
      IS_MERIDIAN_FLIP_AUTOMATIC = atoi(valoriIn[3]);
      safeStringCopy(Fan1_State, valoriIn[4], sizeof(Fan1_State));
      IS_SOUND_ON = atoi(valoriIn[6]);
      IS_STEPPERS_ON = atoi(valoriIn[7]);

      // add JG
      slopex = atof(valoriIn[8]);
      slopey = atof(valoriIn[9]);
      interx = atof(valoriIn[10]);
      intery = atof(valoriIn[11]);
      // end add JG

      setDisplayBrightness(TFT_Brightness / 255.0);

      if (Tracking_type == 0) {
        safeStringCopy(Tracking_Mode, "Lunar", sizeof(Tracking_Mode));
      } else if (Tracking_type == 2) {
        safeStringCopy(Tracking_Mode, "Solar", sizeof(Tracking_Mode));
      } else {
        Tracking_type = 1;
        safeStringCopy(Tracking_Mode, "Celest", sizeof(Tracking_Mode));
      }

      // TFT_timeout management
      if (strcmp(TFT_Time, "30 s") == 0) {
        TFT_timeout = 30000;
      } else if (strcmp(TFT_Time, "60 s") == 0) {
        TFT_timeout = 60000;
      } else if (strcmp(TFT_Time, "2 min") == 0) {
        TFT_timeout = 120000;
      } else if (strcmp(TFT_Time, "5 min") == 0) {
        TFT_timeout = 300000;
      } else if (strcmp(TFT_Time, "10 min") == 0) {
        TFT_timeout = 600000;
      } else {
        safeStringCopy(TFT_Time, "AL-ON", sizeof(TFT_Time));
        TFT_timeout = 0;
      }

#ifdef serial_debug
      Serial.print("TFT_timeout = ");
      Serial.println(TFT_timeout);
#endif

      if (IS_MERIDIAN_FLIP_AUTOMATIC) {
        safeStringCopy(Mer_Flip_State, "AUTO", sizeof(Mer_Flip_State));
      } else {
        IS_MERIDIAN_FLIP_AUTOMATIC = false;
        safeStringCopy(Mer_Flip_State, "OFF", sizeof(Mer_Flip_State));
      }

      if (strcmp(Fan1_State, "OFF") == 0) {
        IS_FAN1_ON = false;
        digitalWrite(FAN1, LOW);
      } else {
        safeStringCopy(Fan1_State, "ON", sizeof(Fan1_State));
        IS_FAN1_ON = true;
        digitalWrite(FAN1, HIGH);
      }

      if (IS_SOUND_ON) {
        safeStringCopy(Sound_State, "ON", sizeof(Sound_State));
      } else {
        IS_SOUND_ON = false;
        safeStringCopy(Sound_State, "OFF", sizeof(Sound_State));
      }

      if (IS_STEPPERS_ON) {
        safeStringCopy(Stepper_State, "ON", sizeof(Stepper_State));
      } else {
        IS_STEPPERS_ON = false;
        safeStringCopy(Stepper_State, "OFF", sizeof(Stepper_State));
      }
    }
    dataFile.close();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

double J2000(double yy, double m, double dd, double hh, double mm)
{
  double DD = 0; // use 0 for J2000
  double d = (double)367 * yy - (double)7 * ((double)yy + ((double)m + (double)9) / (double)12.0) / (double)4.0 + (double)275 * m / (double)9.0 + DD + (double)dd - (double)730531; // days since J2000 // JG info : 730530 in formula from astrophysics websites. Probably cascading error copies from programming sources. But need to keep 730531 for moon cal, no impact on planet cal position.
  d = floor(d) + hh / 24.0 + mm / (24.0 * 60.0);
  return d;
}

double ipart(double xx)
{
  double sgn;

  if (xx < 0)
  {
    sgn = -1.0;
  }

  else if (xx == 0)
  {
    sgn = 0.0;
  }

  else if (xx > 0)
  {
    sgn = 1.0;
  }
  double ret = sgn * ((int)fabs(xx));

  return ret;
}

double FNdegmin(double xx)
{
  double a = ipart(xx) ;
  double b = xx - a ;
  double e = ipart(60 * b) ;
  //   deal with carry on minutes
  if ( e >= 60 )
  {
    e = 0 ;
    a = a + 1 ;
  }
  return (a + (e / 100) );
}

double dayno(int dx, int mx, int yx, double fx)
{
  dno = (367 * yx) -  (int)(7 * (yx + (int)((mx + 9) / 12)) / 4) + (int)(275 * mx / 9) + dx - 730530 + fx; //JG modif, changed 730531.5 by 730530 from astrophysic sources, better RA precision on planets.
  //dno -= 4975.5; //JG modif take now J2000 informations, not an information from 2013 that was computed using J2000 informations.
  return dno;
}

double frange(double x)
{
  x = x / (2 * M_PI);
  x = (2 * M_PI) * (x - ipart(x));
  if (x < 0) x = x + (2 * M_PI);
  return x;
}

double fkep( double m, double ecc)
{
  double e = ecc;
  double v = m + (2 * e - 0.25 * pow(e, 3) + 5 / 96 * pow(e, 5)) * sin(m) + (1.25 * pow(e, 2) - 11 / 24 * pow(e, 4)) * sin(2 * m) + (13 / 12 * pow(e, 3) - 43 / 64 * pow(e, 5)) * sin(3 * m) + 103 / 96 * pow(e, 4) * sin(4 * m) + 1097 / 960 * pow(e, 5) * sin(5 * m);

  if (v < 0)  v = v + (2 * PI);
  return v;
}

double fnatan(double x, double y)
{
  double a = atan(y / x);
  if (x < 0)
    a = a + PI;
  if ((y < 0) && (x > 0))
    a = a + (2 * PI);
  return a;
}

double JulianDay (int j_date, int j_month, int j_year, double UT)
{
  if (j_month <= 2) {
    j_month = j_month + 12;
    j_year = j_year - 1;
  }
  return (int)(365.25 * j_year) + (int)(30.6001 * (j_month + 1)) - 15 + 1720996.5 + j_date + UT / 24.0;
}

///////////////////////////////////////////////////// Draw Battery Level Function ///////////////////////////////////////////////////////////////////
void drawBatteryLevel(int x, int y, int level)
{
  tft.drawRect(x,    y,   43, 22, title_bg); // Battery Indicator Rectangle >> orange
  tft.drawRect(x + 43, y + 6,  4,  10, title_bg); // Battery Tip

  tft.fillRect(x + 1, y + 1, 40, 20, BLACK); //background for empty battery >> Black

  if (level > 20)  {
    tft.fillRect(x + 2, y + 2, round((double)level / 2.5) - 1, 18, title_bg); // orange >>> was BLACK
    tft.cursorToXY(x + 6, y + 8);
    tft.setTextScale(1);
    tft.setTextColor(btn_d_text);
    tft.print(level);
    tft.print(" %");
  }
  else if (IS_NIGHTMODE) {
    tft.fillRect(x + 2, y + 2, round((double)level / 2.5) - 1, 18, btn_d_text); // Dark Green
  } else {
    tft.fillRect(x + 2, y + 2, round((double)level / 2.5), 18, RED);
  }
}
/////////////////////////////////////////////////////////////// Calculate Battery Level Function ///////////////////////////////////////////////////////////

int calculateBatteryLevel()
{
  float RatioFactor = 10.08; //Resistors Ratio Factor 10.08
  int value = LOW;
  float Tvoltage = 0.0;
  float Vvalue = 0.0, Rvalue = 0.0;

  Vvalue = Vvalue + analogRead(BAT_PIN);     //Read analog Voltage
  delay(5);                              //ADC stable

  Vvalue = (float)Vvalue / 10.0;        //Find average of 10 values
  Rvalue = (float)(Vvalue / 1024.0) * 12; //Convert Voltage in 12v factor
  Tvoltage = (Rvalue * RatioFactor);      //Find original voltage by multiplying with factor
  tft.setTextScale(1);
  tft.setFontMode(gTextFontModeSolid);// Set font mode to transparent (No Highlight)
  tft.setTextColor(title_bg, BLACK);
  tft.cursorToXY(260, 360);
  tft.print(Tvoltage);
  tft.print(" V");
  tft.setFontMode(gTextFontModeTransparent);// Set font mode to transparent (No Highlight)
  int batt_percentage = (Tvoltage * 100) / 12;
  return batt_percentage;
}

/////////////////////////////////////////////////////////// Pulse Guiding Setup ///////////////////////////////////////////////////////////////

void considerPulseGuiding()
{
  if (IS_PulseGuiding == true)
  {
    if (digitalRead (RA_PlusPin) == HIGH)
    {
      setmStepsMode("R", 16);
      digitalWrite(RA_DIR, STP_BACK); // JG modif for new wiring (PCB)
      digitalWrite(RA_STP, HIGH);
      digitalWrite(RA_STP, LOW);
      RA_microSteps += RA_mode_steps;
      ch0_data = (digitalRead (RA_PlusPin) * LEVEL_HIGH);
      DrawButton(10, 80, 300, 40, "RA+ Pulse", 0, Button_State_ON, Button_State_ON, 3, false);
    } else {
      ch0_data = LEVEL_LOW;
    }
    if (digitalRead (RA_MinusPin) == HIGH)
    {
      setmStepsMode("R", 16);
      digitalWrite(RA_DIR, STP_FWD); // JG modif for new wiring (PCB)
      digitalWrite(RA_STP, HIGH);
      digitalWrite(RA_STP, LOW);
      RA_microSteps -= RA_mode_steps;
      ch1_data = (digitalRead (RA_MinusPin) * LEVEL_HIGH);
      DrawButton(10, 135, 300, 40, "RA- Pulse", 0, Button_State_ON, Button_State_ON, 3, false);
    } else {
      ch1_data = LEVEL_LOW;
    }
    if (digitalRead (DEC_MinusPin) == HIGH)
    {
      setmStepsMode("D", 16);
      digitalWrite(DEC_DIR, STP_BACK); // JG modif for new wiring (PCB)
      digitalWrite(DEC_STP, HIGH);
      digitalWrite(DEC_STP, LOW);
      DEC_microSteps += DEC_mode_steps;
      ch2_data = (digitalRead (DEC_PlusPin) * LEVEL_HIGH);
      DrawButton(10, 190, 300, 40, "DEC+ Pulse", 0, Button_State_ON, Button_State_ON, 3, false);
    } else
    {
      ch2_data = LEVEL_LOW;
    }
 if (digitalRead (DEC_PlusPin) == HIGH)
    {
       setmStepsMode("D", 16);
       digitalWrite(DEC_DIR, STP_FWD); // JG modif for new wiring (PCB)
       digitalWrite(DEC_STP, HIGH);
       digitalWrite(DEC_STP, LOW);
       DEC_microSteps -= DEC_mode_steps;
        ch3_data = (digitalRead (DEC_MinusPin) * LEVEL_HIGH);
        DrawButton(10, 245, 300, 40, "DEC- Pulse", 0, Button_State_ON, Button_State_ON, 3, false);
      } else
      {
        ch3_data = LEVEL_LOW;
      }
  }
  DrawButton(10, 80, 300, 40, "RA+ Pulse", 0, btn_d_border, btn_d_border, 3, false);
  DrawButton(10, 135, 300, 40, "RA- Pulse", 0, btn_d_border, btn_d_border, 3, false);
  DrawButton(10, 190, 300, 40, "DEC+ Pulse", 0, btn_d_border, btn_d_border, 3, false);
  DrawButton(10, 245, 300, 40, "DEC- Pulse", 0, btn_d_border, btn_d_border, 3, false);
  delayMicroseconds(1500); // was 1500
}

///////////////////////////////////////////////////// Draw Binary Images Function ///////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           drawBin
** Descriptions:            draw a 565 format 16 bit raw image file
***************************************************************************************/

// This function opens a ".raw" image file and displays it at the given coordinates.
// It is faster than plotting BMP images as the file is already in the correct
// format to pipe directly to the display.
// The width and height of the image in pixels must be passed to the function
// as these parameters are not in the file

// Set buffer size, 2 x this size of RAM bytes will be used in the array.
// Reduce if RAM is short with some performance penalty
// Powers of 2 work well

#define BinBUFF_SIZE 760 //760 

void drawBin(char *filename, int16_t x, int16_t y, int16_t rawWidth, int16_t rawHeight) {
  File     rawFile;
  uint8_t  sdbuffer[2 * BinBUFF_SIZE];   // SD read pixel buffer (16 bits per pixel)
  Serial.println(filename);

  // Check file exists and open it
  if ((rawFile = SD.open(filename)) == NULL) {
    Serial.println(F(" File not found"));
    return;
  }

  // Prepare the TFT screen area to receive the data
  startTime = millis();
  tft.setAddrWindow(x, y, x + rawWidth - 1, y + rawHeight - 1);
  // Work out how many whole buffers to send
  uint16_t nr = ((long)rawHeight * rawWidth) / BinBUFF_SIZE;
  while (nr--) {
    rawFile.read(sdbuffer, sizeof(sdbuffer));
    int i = 0;
    //while (i < sizeof(sdbuffer)) tft.pushColor((sdbuffer[i++] << 8) + sdbuffer[i++]); //big-endian
    while (i < sizeof(sdbuffer)) tft.pushColor(sdbuffer[i++] + (sdbuffer[i++] << 8)); // little-endian
  }

  // Send any partial buffer
  nr = ((long)rawHeight * rawWidth) % BinBUFF_SIZE;
  if (nr) {
    rawFile.read(sdbuffer, nr << 1); // We load  2 x nr bytes
    int i = 0;
    //while (i < nr) tft.pushColor((sdbuffer[i++] << 8) + sdbuffer[i++]); // big-endian
    while (i < nr) tft.pushColor((sdbuffer[i++] ) + (sdbuffer[i++] << 8)); // little-endian
  }

  // Close the file
  rawFile.close();
  duration = millis() - startTime;
  Serial.print(duration);
  Serial.println(" mSec");
}

//////////////////////////////////////////////////////// DISPLAY TRACKING STATUS //////////////////////////////////////////

void displayTrackingStatus() {
    tft.setTextScale(3);
    tft.fillRect(0, 116, 320, 27, BLACK);
    
    if (IS_TRACKING == true) {
        tft.cursorToXY(1, 119);
        tft.setTextColor(btn_l_border);
        //if (OBJECT_NAME.length() > 7) {
          if (strlen(OBJECT_NAME) > 7) {
            tft.setTextScale(2);
            tft.cursorToXY(1, 129);
            tft.print("TRK:");
        } else {
            tft.setTextScale(3);
            tft.print("TRACKING:");
        }
    } else {
        tft.cursorToXY(1, 119);
        tft.setTextColor(l_text);
        //if (OBJECT_NAME.length() > 7) {
          if (strlen(OBJECT_NAME) > 7) {
            tft.setTextScale(2);
            tft.cursorToXY(1, 129);
            tft.print("OBS:");
        } else {
            tft.setTextScale(3);
            tft.print("OBSERVING:");
        }
    }
    tft.setTextColor(title_bg);
    tft.print(OBJECT_NAME);
}

/////////////////////////////////////////////////////////// ASCOM CONTROL /////////////////////////////////////////////////////////// 

///////////////////////////////////////////////// ASCOM input GOTO ///////////////////////////////////////////////

// Add JG ASCOM

void ASCOMinput() {
  tft.fillRect(0, 116, 320, 27, BLACK);
  if (!isParkOperation && !isHomeOperation) {
    safeStringCopy(OBJECT_NAME, "ASCOM INPUT", sizeof(OBJECT_NAME));
    safeStringCopy(OBJECT_DESCR, "", sizeof(OBJECT_DESCR));
    safeStringCopy(OBJECT_DETAILS, "Received a goto command from the ASCOM driver", sizeof(OBJECT_DETAILS));
  }
  
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
    
    // Stop slew timers if active
    stopSlewRA();
    stopSlewDEC();
    
    // Stop tracking
    Timer3.stop();
    IS_TRACKING = false;
    IS_OBJ_FOUND = false;
    IS_OBJECT_RA_FOUND = false;
    IS_OBJECT_DEC_FOUND = false;
    
    Tracking_type = 1;
    safeStringCopy(Tracking_Mode, "Celest", sizeof(Tracking_Mode));
    
    Slew_timer = millis();
    Slew_RA_timer = Slew_timer + 20000;
  }
  
  sun_confirm = false;
  Current_RA_DEC();
  drawMainScreen();
  drawStatusBar();
  sendTrackingStatus();
}

/////////////////////////////////////////////////////////// SERIAL READING /////////////////////////////////////////////////////////// 

void serialPrintNB(const char* data) {
    Serial.print(data);
}

/////////////////////////////////////////////////////////// COMMAND PROCESSING //////////////////////////////////////////////////////// 

void processSerialCommand(const char* command) {

    if (strcmp(command, "REMOTE") == 0) {
        ascomConnected = true;
        locationSent = false;
        delay(100); // small pause
        sendTrackingStatus(); // First send immediately
        delay(100);
        sendTrackingStatus(); // second send to be sure
        delay(100);
        sendTrackingStatus(); // third send
        sendLocationAndCurrentPosition();
    
    drawMainScreen();

    }
    else if (strcmp(command, "DISCONNECT") == 0) {
        ascomConnected = false;
        locationSent = false;
        safeStringCopy(OBJECT_NAME, "", sizeof(OBJECT_NAME));
        safeStringCopy(OBJECT_DESCR, "", sizeof(OBJECT_DESCR));
        safeStringCopy(OBJECT_DETAILS, "", sizeof(OBJECT_DETAILS));
        drawMainScreen();
    }
    else if (strncmp(command, "SLEW:", 5) == 0) {
        slewCommandReceived = true;
        slewascomsent = true;
        strncpy(pendingSlewCommand, command, sizeof(pendingSlewCommand)-1);
    }
    else if (strncmp(command, "MOVE", 4) == 0) {
        moveCommandReceived = true;
        strncpy(pendingMoveCommand, command, sizeof(pendingMoveCommand)-1);
    }
    else if (strncmp(command, "PARK:", 5) == 0) {
        parkCommandReceived = true;
        strncpy(pendingParkCommand, command, sizeof(pendingParkCommand)-1);
    }
    else if (strcmp(command, "UNPARK") == 0) {
        handleUnpark();
    }
    else if (strcmp(command, "HOME") == 0) {
        homeCommandReceived = true;
    }

else if (strcmp(command, "GET_TRACKING") == 0) {
    // Send the command at first to synchronise the driver
    char modeCommand[30] = "TRACK:";
    
    if (Tracking_type == 1) {
        strcat(modeCommand, "SIDEREAL");
    } else if (Tracking_type == 2) {
        strcat(modeCommand, "SOLAR");  
    } else if (Tracking_type == 0) {
        strcat(modeCommand, "LUNAR");
    }
    
    // Send the mode command To the DRIVER
    Serial.println(modeCommand);
    delay(50);
    sendTrackingStatus();
}

    // === DIRECT USE OF TRACK COMMANDS ===
    else if (strncmp(command, "TRACK:", 6) == 0) {

        if (strstr(command, "SIDEREAL")) {
            Tracking_type = 1;
            IS_TRACKING = true;
            TRACKING_MOON = false;
            //Tracking_Mode = "Celest";
            safeStringCopy(Tracking_Mode, "Celest", sizeof(Tracking_Mode));
            drawStatusBar();
            
        } else if (strstr(command, "SOLAR")) {
            Tracking_type = 2;
            IS_TRACKING = true;
            TRACKING_MOON = false;
            //Tracking_Mode = "Solar";
            safeStringCopy(Tracking_Mode, "Solar", sizeof(Tracking_Mode));
            drawStatusBar();

        } else if (strstr(command, "LUNAR")) {
            Tracking_type = 0;
            IS_TRACKING = true;
            TRACKING_MOON = true;
            //Tracking_Mode = "Lunar";
            safeStringCopy(Tracking_Mode, "Lunar", sizeof(Tracking_Mode));
            drawStatusBar();
        }
        
        // Start tracking immediately
        if (IS_TRACKING && IS_STEPPERS_ON && IS_OBJ_FOUND) {
            startTracking();
        } else if (IS_TRACKING) {
            trackingPending = true;
        }
        
        sendTrackingStatus();
    }
    // === Direct go for tracking ===
    else if (strncmp(command, "TRACKING:", 9) == 0) {

        // TRAITEMENT IMMÉDIAT
        if (strcmp(command, "TRACKING:ON") == 0) {
            IS_TRACKING = true;
            if (IS_STEPPERS_ON && IS_OBJ_FOUND) {
                startTracking();
            } else {
                trackingPending = true;
            }
        } else if (strcmp(command, "TRACKING:OFF") == 0) {
            IS_TRACKING = false;
            trackingPending = false;
            stopTracking();
        }
        
        sendTrackingStatus();
    }
    else if (strcmp(command, "STOP") == 0) {
        stopCommandReceived = true;
    }
    else if (strncmp(command, "SYNC:", 5) == 0) {
    syncCommandReceived = true;
    strncpy(pendingSyncCommand, command, sizeof(pendingSyncCommand) - 1);
    }
    else if (strncmp(command, "GUIDE ", 6) == 0) {
        guideCommandReceived = true;
        strncpy(pendingGuideCommand, command, sizeof(pendingGuideCommand)-1);
    }
}

///////////////////////////////////////////////////////////  COMMAND HANDLERS /////////////////////////////////////////////////////////// 

//// SLEW

void handleSlewCommand(const char* command) {
    int ra_h = 0;
    float ra_m = 0;
    int dec_d = 0;
    float dec_m = 0;
    
    if (sscanf(command, "SLEW:RA=%d;M=%f;DEC=%d;M=%f", &ra_h, &ra_m, &dec_d, &dec_m) == 4) {
        isParkOperation = false;
        isHomeOperation = false;
        IS_CRITICAL_MOVEMENT = true;
        
        OBJECT_RA_H = ra_h;
        OBJECT_RA_M = ra_m;
        OBJECT_DEC_D = dec_d;
        OBJECT_DEC_M = dec_m;
        
        ASCOMinput();
    }
}

//// MOVE

void handleMoveCommand(const char* command) {
    char axisType[24] = {0};
    float rate = 0.0f;

    IS_CRITICAL_MOVEMENT = true;

    // parse format "MOVE:AXIS=RA;RATE=+1.00"
    if (strncmp(command, "MOVE:", 5) == 0) {
        // recherche AXIS=
        const char* pAxis = strstr(command, "AXIS=");
        if (pAxis) {
            pAxis += 5;
            if (strncmp(pAxis, "RA", 2) == 0) strncpy(axisType, "axisPrimary", sizeof(axisType)-1);
            else if (strncmp(pAxis, "DEC", 3) == 0) strncpy(axisType, "axisSecondary", sizeof(axisType)-1);
        }
        const char* pRate = strstr(command, "RATE=");
        if (pRate) {
            pRate += 5;
            rate = atof(pRate);
        }
    }
    // OtherW, parse "MOVE %s %f"
    else if (sscanf(command, "MOVE %23s %f", axisType, &rate) != 2) {
        // if fail, Try strncmp on "MOVE"
        if (strncmp(command, "MOVE", 4) == 0) {
        // fallback: if we find "RA" or "DEC" somewhere, or nothing at all => stop
            if (strstr(command, "RA")) strncpy(axisType, "axisPrimary", sizeof(axisType)-1);
            else if (strstr(command, "DEC")) strncpy(axisType, "axisSecondary", sizeof(axisType)-1);
            else {
                IS_CRITICAL_MOVEMENT = false;
                return;
            }
            // Attempt to read a number
            const char* p = command;
            while (*p && !((*p >= '0' && *p <= '9') || *p == '+' || *p == '-' || *p == '.')) p++;
            if (*p) rate = atof(p);
        } else {
            IS_CRITICAL_MOVEMENT = false;
            return;
        }
    }

    bool isPositive = (rate > 0.0f);
    float absRate = fabsf(rate);

    if (absRate > 0.0001f) {
        lastASCOMMove = millis();
    }
    if (absRate < 0.0001f) {
        stopManualMove();
        return;
    }

    // --- tracking management ONLY for RA ---
    bool isRA = (strcmp(axisType, "axisPrimary") == 0 || strcmp(axisType, "RA") == 0);

    if (isRA) {
        if (IS_TRACKING) {
            trackingWasActiveBeforeMove = true;
            IS_TRACKING = false;
            //trackingPending = false;
            stopTracking();
        } else {
            trackingWasActiveBeforeMove = false;
        }
    }

    // Microstep selection adapted per axis - NOW SAME LOGIC FOR BOTH RA AND DEC
    int microStepMode;
    
    // Unified logic based on rate for both axes
    if (absRate <= 0.2f) microStepMode = 16;
    else if (absRate <= 0.4f) microStepMode = 8;
    else if (absRate <= 0.6f) microStepMode = 4;
    else microStepMode = 2;

    // Initializes the global (non-blocking) movement state
    noInterrupts(); // Minimal protection if ISR also writes these flags
    moveActive = true;
    moveRate = absRate;
    moveMicroStepMode = microStepMode;
    moveDirPositive = isPositive;
    move_pulseState = 0;
    move_lastMicros = micros();
    
    if (strcmp(axisType, "axisPrimary") == 0 || strcmp(axisType, "RA") == 0) {
        moveAxis = MOVE_AXIS_RA;
        setmStepsMode("R", microStepMode);
        digitalWrite(RA_DIR, moveDirPositive ? STP_FWD : STP_BACK);
    } else if (strcmp(axisType, "axisSecondary") == 0 || strcmp(axisType, "DEC") == 0) {
        moveAxis = MOVE_AXIS_DEC;
        setmStepsMode("D", microStepMode);
        digitalWrite(DEC_DIR, moveDirPositive ? STP_FWD : STP_BACK);
    } else {
        // axis unknown -> stop
        moveActive = false;
    }
    interrupts();
}

void stopManualMove() {
    moveActive = false;

    // remettre STEP à LOW par sécurité
    digitalWrite(RA_STP, LOW);
    digitalWrite(DEC_STP, LOW);

    if (trackingWasActiveBeforeMove) {

        IS_TRACKING = true;

        if (IS_STEPPERS_ON) {

            // RESET tracking
            stopTracking();
            delay(5);  // très important (laisser Timer3 se couper)

            // set microstep before tracking
            setmStepsMode("R", MICROSteps);

            if (IS_OBJ_FOUND) {
                startTracking();
            } else {
                trackingPending = true;
            }

        } else {
            trackingPending = true;
        }

        trackingWasActiveBeforeMove = false;
    }
}

void runMoveTask() {
    const unsigned long MOVE_TIMEOUT = 300;
    if (moveActive && (millis() - lastASCOMMove > MOVE_TIMEOUT)) {
        stopManualMove();
        return;
    }
    if (!moveActive) return;
    
    unsigned long now = micros();
    
    // NO MORE SPECIAL FACTOR FOR RA - same timing for both axes
    unsigned long pulseHighMicros = move_pulseHighMicros;
    unsigned long pulseLowMicros = move_pulseLowMicros;

    // Non-blocking sequence to generate the high/low edge
    if (move_pulseState == 0) {
        // We set the pin HIGH (start of pulse)
        if (moveAxis == MOVE_AXIS_RA) {
            digitalWrite(RA_STP, HIGH);
        } else if (moveAxis == MOVE_AXIS_DEC) {
            digitalWrite(DEC_STP, HIGH);
        }
        move_lastMicros = now;
        move_pulseState = 1;
        return;
    }
    else if (move_pulseState == 1) {
        // We're waiting for the duration of the HIGH (with appropriate timing)
        if ((now - move_lastMicros) >= pulseHighMicros) {
            // put LOW
            if (moveAxis == MOVE_AXIS_RA) {
                digitalWrite(RA_STP, LOW);
            } else if (moveAxis == MOVE_AXIS_DEC) {
                digitalWrite(DEC_STP, LOW);
            }
            move_lastMicros = now;
            move_pulseState = 2;

            // microSteps update (on the falling edge)
            if (moveAxis == MOVE_AXIS_RA) {
                RA_microSteps += moveDirPositive ? -RA_mode_steps : RA_mode_steps;
            } else if (moveAxis == MOVE_AXIS_DEC) {
                DEC_microSteps += moveDirPositive ? -DEC_mode_steps : DEC_mode_steps;
            }
            return;
        }
        return;
    }
    else if (move_pulseState == 2) {
        // We wait for the duration of the LOW before resuming (with appropriate timing)
        if ((now - move_lastMicros) >= pulseLowMicros) {
            // Ready for new cycle
            move_pulseState = 0;
            move_lastMicros = now;
        }
        return;
    }
}

//// PARK

void handleParkCommand(const char* command) {
    int ra_h = 0;
    float ra_m = 0;
    int dec_d = 0;
    float dec_m = 0;
    
    if (sscanf(command, "PARK:RA=%d;M=%f;DEC=%d;M=%f", &ra_h, &ra_m, &dec_d, &dec_m) == 4) {
        isParkOperation = true;
        isHomeOperation = false;
        isAtHome = false;
        IS_CRITICAL_MOVEMENT = true;
        
        OBJECT_RA_H = ra_h;
        OBJECT_RA_M = ra_m;
        OBJECT_DEC_D = dec_d;
        OBJECT_DEC_M = dec_m;
        
        safeStringCopy(OBJECT_NAME, "PARK", sizeof(OBJECT_NAME));
        safeStringCopy(OBJECT_DESCR, "Parking Telescope", sizeof(OBJECT_DESCR));
        safeStringCopy(OBJECT_DETAILS, "Moving telescope to park position", sizeof(OBJECT_DETAILS));

        
        ASCOMinput();
    }
}

void handleUnpark() {
    isAtPark = false;
    isParkOperation = false;
    isAtHome = false;

     safeStringCopy(OBJECT_DESCR, "Telescope unparked", sizeof(OBJECT_DESCR));
     safeStringCopy(OBJECT_DETAILS, "Ready for usen", sizeof(OBJECT_DETAILS));

    drawMainScreen();
    Serial.println("UNPARK:COMPLETE");
}

//// HOME

void handleHomeCommand() {
    OBJECT_RA_H = 12;
    OBJECT_RA_M = 0;
    OBJECT_DEC_D = 90;
    OBJECT_DEC_M = 0;
    isHomeOperation = true;
    isParkOperation = false;
    isAtPark = false;
    IS_CRITICAL_MOVEMENT = true;
    
    safeStringCopy(OBJECT_NAME, "HOME", sizeof(OBJECT_NAME));
    safeStringCopy(OBJECT_DESCR, "Going to Home Positio", sizeof(OBJECT_DESCR));
    safeStringCopy(OBJECT_DETAILS, "Moving telescope to home position", sizeof(OBJECT_DETAILS));
    
    ASCOMinput();
    serialPrintNB("SLEW:COMPLETE\n");
}

//// TRACKING

void handleTrackingCommand(const char* command) {
    bool wasTrackingBefore = IS_TRACKING; // Save previous state
    
    // Gestion des commandes GET_TRACKING
    if (strcmp(command, "GET_TRACKING") == 0) {
        sendTrackingStatus();
        return;
    }
    
    if (strstr(command, "SIDEREAL")) {
        Tracking_type = 1;
        IS_TRACKING = true;
        TRACKING_MOON = false;
        //Tracking_Mode = "Celest";
        safeStringCopy(Tracking_Mode, "Celest", sizeof(Tracking_Mode));

    } else if (strstr(command, "SOLAR")) {
        Tracking_type = 2;
        IS_TRACKING = true;
        TRACKING_MOON = false;
        //Tracking_Mode = "Solar";
        safeStringCopy(Tracking_Mode, "Solar", sizeof(Tracking_Mode));
    
    } else if (strstr(command, "LUNAR")) {
        Tracking_type = 0;
        IS_TRACKING = true;
        TRACKING_MOON = true;
        //Tracking_Mode = "Lunar";
        safeStringCopy(Tracking_Mode, "Lunar", sizeof(Tracking_Mode));
        
    } else if (strcmp(command, "TRACKING:ON") == 0) {
        // explicit command to activate tracking w/o mode change
        if (IS_STEPPERS_ON) {
            IS_TRACKING = true;
            if (IS_OBJ_FOUND) {
                startTracking();
            } else {
                trackingPending = true;  // Tracking wait slew ending
            }
        }
    } else if (strcmp(command, "TRACKING:OFF") == 0) {
        // explicit command to deactivate tracking
        IS_TRACKING = false;
        trackingPending = false;  // Cancel waitng
        stopTracking();
        sendTrackingStatus();
        return; // Go out here to avoind tracking to be launch two times
    } else {
        // Send status and out
        sendTrackingStatus();
        return;
    }
    
    // If the tracking is set, track activation
    if (IS_TRACKING && IS_STEPPERS_ON) {
        if (IS_OBJ_FOUND) {
            // Item found, start immediately
            startTracking();
        } else {
            // Waiting for slew to finish
            trackingPending = true;
        }
    } else if (IS_TRACKING && !IS_STEPPERS_ON) {
        // Motors down, wait
        trackingPending = true;
    }
    // Send status on end
    sendTrackingStatus();
}

//// STOP / ABORT / EMERGENCY STOP

void handleStopCommand() {

// Immediate stop of asynchronous movement
    moveActive = false;
    moveAxis = MOVE_AXIS_NONE;

    stopSlewRA();
    stopSlewDEC();

    if (isParkOperation && !isAtPark) {
        isParkOperation = false;
        serialPrintNB("Park Operation Cancelled\n");
    } else if (isHomeOperation && !isAtHome) {
        isHomeOperation = false;
        serialPrintNB("Home Operation Cancelled\n");
    }
    // Also cancel any pending tracking
    trackingPending = false;

    slewInitialized = false;
    RA_move_ending = 0;
    RA_waitingToStart = false;
    slewRA_active = false;
    slewDEC_active = false;
    DEC_accelerationDone = false;
    RA_accelerationDone = false;
    DEC_accel_step = 0;
    RA_accel_step = 0;

    
    
    // Gradual stop only for movements in progress
    if (!IS_OBJ_FOUND) { // If a slew is in progress
        gradualEmergencyStop();
    }
    
    IS_OBJ_FOUND = true;
    IS_OBJECT_RA_FOUND = true;
    IS_OBJECT_DEC_FOUND = true;
    IS_CRITICAL_MOVEMENT = false;

    if ((millis() - lastASCOMMove) > 500) { // don't refresh screen after manual move
        drawMainScreen();
    }
    Serial.println("STOP:COMPLETE");
}

void gradualEmergencyStop() {

    stopSlewRA();
    stopSlewDEC();

    // Save the current target position (for microstep counting)
    int targetRA = SLEW_RA_microsteps;
    int targetDEC = SLEW_DEC_microsteps;
    
    // Decel param
    const int totalDecelerationTime = 800; // total ms to stop
    const int rampSteps = 20;              // Number of steps in the ramp
    const int stepDelay = totalDecelerationTime / rampSteps;
    
    // Speed ramp parameters
    int currentDelayRA = 100; // µs between steps (initial velocity)
    int currentDelayDEC = 100;
    int finalDelay = 2000;    // µs between steps (very slow final speed)

    setmStepsMode("R", 16);
    setmStepsMode("D", 16);
    delay(10);

    // Decel ramp
    for (int i = 0; i < rampSteps; i++) {
        // Calculate the progressive delay (linear interpolation)
        int newDelayRA = map(i, 0, rampSteps, 100, finalDelay);
        int newDelayDEC = map(i, 0, rampSteps, 100, finalDelay);
        
        // continue the movment during decel
        if (targetRA > RA_microSteps) {
            digitalWrite(RA_DIR, STP_BACK);
            PIOC->PIO_SODR = (1u << 26);
            delayMicroseconds(newDelayRA);
            PIOC->PIO_CODR = (1u << 26);
            RA_microSteps += RA_mode_steps;
        } else if (targetRA < RA_microSteps) {
            digitalWrite(RA_DIR, STP_FWD);
            PIOC->PIO_SODR = (1u << 26);
            delayMicroseconds(newDelayRA);
            PIOC->PIO_CODR = (1u << 26);
            RA_microSteps -= RA_mode_steps;
        }
        
        if (targetDEC > DEC_microSteps) {
            digitalWrite(DEC_DIR, STP_BACK);
            PIOC->PIO_SODR = (1u << 24);
            delayMicroseconds(newDelayDEC);
            PIOC->PIO_CODR = (1u << 24);
            DEC_microSteps += DEC_mode_steps;
        } else if (targetDEC < DEC_microSteps) {
            digitalWrite(DEC_DIR, STP_FWD);
            PIOC->PIO_SODR = (1u << 24);
            delayMicroseconds(newDelayDEC);
            PIOC->PIO_CODR = (1u << 24);
            DEC_microSteps -= DEC_mode_steps;
        }
        
        // Small delay to let decel
        delay(stepDelay);
        
        // stay responsive
        if (i % 3 == 0) processSerialBuffer();
    }
    
    // Complete progressive stop
    for (int i = 0; i < 5; i++) {
        if (targetRA > RA_microSteps) {
            digitalWrite(RA_DIR, STP_BACK);
            PIOC->PIO_SODR = (1u << 26);
            delayMicroseconds(finalDelay * 2); // slower
            PIOC->PIO_CODR = (1u << 26);
            RA_microSteps += RA_mode_steps;
        }
        
        if (targetDEC > DEC_microSteps) {
            digitalWrite(DEC_DIR, STP_BACK);
            PIOC->PIO_SODR = (1u << 24);
            delayMicroseconds(finalDelay * 2);
            PIOC->PIO_CODR = (1u << 24);
            DEC_microSteps += DEC_mode_steps;
        }
        
        delay(50);
    }
    
    // Set position
    stopSlewRA();
    stopSlewDEC();
    slewInitialized = false;
    RA_move_ending = 0;
    RA_waitingToStart = false;
    Current_RA_DEC();
    sendTelescopeCoordinates();
}

/// SYNC

void handleSyncCommand(const char* command) {
    int ra_h = 0;
    float ra_m = 0.0f;
    int dec_d = 0;
    float dec_m = 0.0f;
    
    if (sscanf(command, "SYNC:RA=%d;M=%f;DEC=%d;M=%f", &ra_h, &ra_m, &dec_d, &dec_m) == 4) {
        
        // Convert target coordinates in decimal
        float target_ra_decimal = ra_h + ra_m / 60.0f;
        
        // For DEC, manage sign
        float target_dec_decimal;
        if (dec_d < 0) {
            target_dec_decimal = dec_d + dec_m / 60.0f;
        } else {
            target_dec_decimal = dec_d + dec_m / 60.0f;
        }
        
        // Calculate coordinates
        Current_RA_DEC();
        
        float current_ra_decimal = curr_RA_H + curr_RA_M / 60.0f + curr_RA_S / 3600.0f;
        float current_dec_decimal = curr_DEC_D + (curr_DEC_D >= 0 ? 1 : -1) * (curr_DEC_M / 60.0f + curr_DEC_S / 3600.0f);
        
        // Calculate the differences (in hours for RA, in degrees for DEC)
        float diff_ra = target_ra_decimal - current_ra_decimal;
        float diff_dec = target_dec_decimal - current_dec_decimal;
        
        delta_a_RA += diff_ra * 15.0;  // Convert hours in degrees
        delta_a_DEC -= diff_dec;       // DEC stay in degrees
        // Recalculate with new offsets
        Current_RA_DEC();
        
        IS_OBJ_FOUND = true;
        drawStatusBar();
        Serial.println("SYNC:COMPLETE");
    } else {
        Serial.println("SYNC:ERROR");
    }
}
/////////////////////////////////////////////////////////// SEND COORDINATES /////////////////////////////////////////////////////////// 

void sendLocationAndCurrentPosition() {
    if (ascomConnected) {
        // Observation location coordinates
        char buffer[100];
        snprintf(buffer, sizeof(buffer), 
            "LOC:lat=%.6f;lon=%.6f;alt=%.2f\n",
            OBSERVATION_LATTITUDE, OBSERVATION_LONGITUDE, OBSERVATION_ALTITUDE
        );
        serialPrintNB(buffer);
        locationSent = true;
        
        Current_RA_DEC();
        
        // Send actual coordinates RADEC
        snprintf(buffer, sizeof(buffer),
            "RADEC:ra=%d:%d:%d;dec=%d:%d:%d\n",
            (int)curr_RA_H, (int)curr_RA_M, (int)curr_RA_S,
            (int)curr_DEC_D, (int)curr_DEC_M, (int)curr_DEC_S
        );
        serialPrintNB(buffer);
        
        // Send tracking status
        sendTrackingStatus();
    }
}

void sendTelescopeCoordinates() {
// No need to check locationSent here anymore as it's already been sent to the connection
    char buffer[100];
    snprintf(buffer, sizeof(buffer),
        "RADEC:ra=%d:%d:%d;dec=%d:%d:%d\n",
        (int)curr_RA_H, (int)curr_RA_M, (int)curr_RA_S,
        (int)curr_DEC_D, (int)curr_DEC_M, (int)curr_DEC_S
    );
    serialPrintNB(buffer);
}

/////////////////////////////////////////////////////////// TRACKING CONTROL /////////////////////////////////////////////////////////// 

//// SEND TRACKING STATUS

void sendTrackingStatus() {
    char response[50];
    snprintf(response, sizeof(response), "TRACKING:IS=%d;TYPE=%d;MOON=%d",
             IS_TRACKING ? 1 : 0,
             Tracking_type,
             TRACKING_MOON ? 1 : 0);
    Serial.println(response);
}

//// START TRACKING

void startTracking() {
    if (!IS_STEPPERS_ON) return;
    
    IS_TRACKING = true;
    
    // Update interface
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
    if (Tracking_type == 1) {
        Timer3.start(Clock_Sidereal);
    } else if (Tracking_type == 2) {
        Timer3.start(Clock_Solar);
    } else if (Tracking_type == 0) {
        Timer3.start(Clock_Lunar);
    }
}

//// STOP TRACKING

void stopTracking() {
    IS_TRACKING = false;
    
    // Update interface
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
    
    // Stop the timer and switch back to full steps mode
    setmStepsMode("R", 1);
    Timer3.stop();
}

/////////////////////////////////////////////////////////// PROCESS SERIAL BUFFER /////////////////////////////////////////////////////////// 

void processSerialBuffer() {
    static int charCount = 0; // count characters received
    
    while (Serial.available()) {
        char c = Serial.read();
        charCount++;
        
        if (c == '\n' || inputIndex >= SERIAL_INPUT_BUFFER_SIZE - 1) {
            serialInputBuffer[inputIndex] = '\0';
            
            processSerialCommand(serialInputBuffer);
            inputIndex = 0;
        } else if (c >= 32 && c <= 126) {
            serialInputBuffer[inputIndex++] = c;
        }
    }
}

/////////////////////////////////////////////////////////// ASCOM PULSE AUTOGUIDING ///////////////////////////////////////////////////////////

void handleGuideCommand(const char* command) {
    // Parse "GUIDE guideDirection duration" command
    char direction[20];
    int duration = 0;
    
    if (sscanf(command, "GUIDE %19s %d", direction, &duration) == 2) {

        char processedDirection[20] = "";
        
        if (strcmp(direction, "guideNorth") == 0) {
        safeStringCopy(processedDirection, "NORTH", sizeof(processedDirection));
        } else if (strcmp(direction, "guideSouth") == 0) {
        safeStringCopy(processedDirection, "SOUTH", sizeof(processedDirection));
        } else if (strcmp(direction, "guideEast") == 0) {
        safeStringCopy(processedDirection, "EAST", sizeof(processedDirection));
        } else if (strcmp(direction, "guideWest") == 0) {
        safeStringCopy(processedDirection, "WEST", sizeof(processedDirection));
        } else {
            Serial.print("ERROR: Direction inconnue: ");
            Serial.println(direction);
            return;
        }
        
        // Stop guiding
        stopCurrentGuiding();
        
        // Start guiding only if the new guiding duration is > 0
        if (duration > 0) {
            isGuiding = true;
            guideStartTime = millis();
            guideDuration = duration;
            safeStringCopy(currentGuideDirection, processedDirection, sizeof(currentGuideDirection));
            lastGuidePulse = 0; // Reset to start immediately
            
            // High precision motors config
            setmStepsMode("R", 16);
            setmStepsMode("D", 16);
            
            Serial.print("GUIDE_START:");
            Serial.print(processedDirection);
            Serial.print(":");
            Serial.println(duration);
        }
    } else {
        Serial.println("ERROR: Format de commande GUIDE invalide");
        Serial.print("Reçu: ");
        Serial.println(command);
    }
}

//// STOP CURRENT GUIDING

void stopCurrentGuiding() {
    if (isGuiding) {
        isGuiding = false;
        strcpy(currentGuideDirection, "");
        guideDuration = 0;
        Serial.println("GUIDE_STOP");
    }
}

//// PROCESS GUIDING

void processGuiding() {
    if (!isGuiding) return;
    
    // chef if guide time is over
    unsigned long currentTime = millis();
    if (currentTime - guideStartTime >= (unsigned long)guideDuration) {
        stopCurrentGuiding();
        return;
    }
    
    // Control pulse frequency
    if (currentTime - lastGuidePulse < GUIDE_PULSE_INTERVAL) {
        return;
    }
    
    lastGuidePulse = currentTime;
    
      if (strcmp(currentGuideDirection, "NORTH") == 0) {
        digitalWrite(DEC_DIR, STP_BACK);
        digitalWrite(DEC_STP, HIGH);
        digitalWrite(DEC_STP, LOW);
        DEC_microSteps += DEC_mode_steps;
    }
      else if (strcmp(currentGuideDirection, "SOUTH") == 0) {
        digitalWrite(DEC_DIR, STP_FWD);
        digitalWrite(DEC_STP, HIGH);
        digitalWrite(DEC_STP, LOW);
        DEC_microSteps -= DEC_mode_steps;
    }
      else if (strcmp(currentGuideDirection, "EAST") == 0) {
        digitalWrite(RA_DIR, STP_BACK);
        digitalWrite(RA_STP, HIGH);
        digitalWrite(RA_STP, LOW);
        RA_microSteps += RA_mode_steps;
    }
      else if (strcmp(currentGuideDirection, "WEST") == 0) {
        digitalWrite(RA_DIR, STP_FWD);
        digitalWrite(RA_STP, HIGH);
        digitalWrite(RA_STP, LOW);
        RA_microSteps -= RA_mode_steps;
    }
}

////////////////////////////////////  NEW BRIGHTNESS CONTROL /////////////////////////

// Timer configuration for optimal frequency PWM
void setupPWMBrightness() {
    pinMode(TFTBright, OUTPUT);
    digitalWrite(TFTBright, LOW);
    Timer1.attachInterrupt(pwmISR);
    Timer1.start(63);
    
    #ifdef serial_debug
    Serial.println("PWM brightness initialized at 800Hz");
    #endif
}

void pwmISR() {
    pwm_counter++;
    digitalWrite(TFTBright, (pwm_counter < pwm_duty) ? HIGH : LOW);
    if (pwm_counter >= 255) {
        pwm_counter = 0;
    }
}

float gammaCorrection(float value) {
    // gamma curve ~2.2 for human perception
    if (value <= 0.0) return 0.0;
    if (value >= 1.0) return 1.0;
    return pow(value, 2.2);
}

void setDisplayBrightness(float value) {
    if (value < 0) value = 0;
    if (value > 1) value = 1;
    
    // Apply gamma correction
    float corrected_value = gammaCorrection(value);
    
    pwm_duty = (int)round(255 * corrected_value);
    TFT_Brightness = (int)round(255 * value); // Keeps the original value for the interface
    
    #ifdef serial_debug
    Serial.print("Brightness: ");
    Serial.print(value * 100);
    Serial.print("% -> ");
    Serial.print(corrected_value * 100);
    Serial.print("% (PWM: ");
    Serial.print(pwm_duty);
    Serial.println(")");
    #endif
}

// gamma curve adapted to LCD screen
float adaptiveGammaCorrection(float value) {
    if (value <= 0.0) return 0.0;
    if (value >= 1.0) return 1.0;
    
    // custom curve for low light environment
    if (value < 0.1) {
        return value * 0.3; // more sensitive in low light env
    } else if (value < 0.5) {
        return 0.03 + (value - 0.1) * 0.4; // linear in medium light env
    } else {
        return 0.19 + pow(value - 0.5, 1.5) * 1.62; // curve in high light env
    }
}

///////////////////////////////////// STRING COPY FUNCTION /////////////////////////////

void safeStringCopy(char* dest, const char* src, size_t destSize) {
    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
}

///////////////////////////////////// CATALOGS MENU /////////////////////////////

// Helper fonction for pagination
void handlePager(int& pager, int maxPager, const char* catName, int& objPager) {
    safeStringCopy(CAT_NAME, catName, sizeof(CAT_NAME));
    if (pager >= 0 && pager <= maxPager) {
        objPager = pager;
        drawLoadObjects();
    } else {
        pager = (pager < 0) ? 0 : maxPager;
        objPager = pager;
    }
}

void handlePagerWithTwoFiles(int& pager, int fileThreshold, const char* file1, const char* file2, int maxPager, int& objPager) {
    if (pager <= fileThreshold) {
        safeStringCopy(CAT_NAME, file1, sizeof(CAT_NAME));
    } else {
        safeStringCopy(CAT_NAME, file2, sizeof(CAT_NAME));
    }
    if (pager >= 0 && pager <= maxPager) {
        objPager = pager;
        drawLoadObjects();
    } else {
        pager = (pager < 0) ? 0 : maxPager;
        objPager = pager;
    }
}

/////////////////////////////// NEW MENU WITH SEARCH ENGINE //////////////////

void drawLoadMenu() {
  CURRENT_SCREEN = SCREEN_LOAD_MENU;
  tft.fillScreen(BLACK);
  
  // Background
  char bgPath[50];
  if (IS_NIGHTMODE) {
    safeStringCopy(bgPath, "UI/night/load_menu.bin", sizeof(bgPath));
  } else {
    safeStringCopy(bgPath, "UI/day/load_menu.bin", sizeof(bgPath));
  }
  
  if (SD.open(bgPath)) {
    drawBin(bgPath, 0, 0, 320, 480);
  } else {
    tft.fillScreen(IS_NIGHTMODE ? BLACK : WHITE);
  }
  
  if (IS_NIGHTMODE) {
  // Night mode: red buttons with black outline, black text
    DrawButton(60, 150, 200, 60, "Load Object", 
               RED,    // red background
               BLACK,  // black outline
               BLACK,  // Text
               2, false);
    
    DrawButton(60, 230, 200, 60, "Search Object", 
               RED,    // red background  
               BLACK,  // black outline
               BLACK,  // Text
               2, false);
    
    DrawButton(60, 310, 200, 60, "Back", 
               RED,    // red background
               BLACK,  // black outline
               BLACK,  // Text
               2, false);
  } else {
    // Day mode
    DrawButton(60, 150, 200, 60, "Load Object", 
               LIGHTBLUE, 
               WHITE, 
               BLACK, 2, false);
    
    DrawButton(60, 230, 200, 60, "Search Object", 
               LIGHTGRAY, 
               BLACK, 
               BLACK, 2, false);
    
    DrawButton(60, 310, 200, 60, "Back", 
               RED, 
               WHITE, 
               BLACK, 2, false);
  }
}

void drawSearchScreen() {
    CURRENT_SCREEN = SCREEN_SEARCH_OBJECT;
    tft.fillScreen(BLACK);
    
    // Background
    char bgPath[50];
    if (IS_NIGHTMODE) {
        safeStringCopy(bgPath, "UI/night/search_screen.bin", sizeof(bgPath));
    } else {
        safeStringCopy(bgPath, "UI/day/search_screen.bin", sizeof(bgPath));
    }
    
    if (SD.open(bgPath)) {
        drawBin(bgPath, 0, 0, 320, 480);
    } else {
        tft.fillScreen(IS_NIGHTMODE ? BLACK : WHITE);
    }
    
    if (IS_NIGHTMODE) {
    // Night mode: red button with black outline, black text
        DrawButton(265, 0, 55, 30, "Back", 
                   RED,    // red background
                   BLACK,  // black outline
                   BLACK,  // Text
                   2, false);
    } else {
    // Daytime mode: original behavior
        DrawButton(265, 0, 55, 30, "Back", 
                   RED, 
                   RED, 
                   BLACK, 2, false);
    }
    
    // Search field
    if (IS_NIGHTMODE) {
        tft.fillRect(10, 50, 250, 35, RED); // Red background
        tft.drawRect(10, 50, 250, 35, BLACK); // black outline
    } else {
        tft.fillRect(10, 50, 250, 35, LIGHTGRAY);
        tft.drawRect(10, 50, 250, 35, BLACK);
    }
    
    tft.setTextColor(IS_NIGHTMODE ? BLACK : BLACK);
    tft.setTextScale(2);
    tft.cursorToXY(15, 55);
    
    // Truncate if too long
    char displayQuery[20];
    safeStringCopy(displayQuery, searchQuery, sizeof(displayQuery));
    if (strlen(displayQuery) > 18) {
        displayQuery[15] = '.';
        displayQuery[16] = '.';
        displayQuery[17] = '.';
        displayQuery[18] = '\0';
    }
    tft.print(displayQuery);
    
    if (IS_NIGHTMODE) {
        // Night mode: red buttons with black outline, black text
        DrawButton(10, 90, 120, 35, "Search", 
                   RED,    // Red background
                   BLACK,  // black outline
                   BLACK,  // Black text
                   2, false);
        
        DrawButton(140, 90, 120, 35, "Clear", 
                   RED,    // Red background
                   BLACK,  // black outline
                   BLACK,  // Text
                   2, false);
    } else {
        // Daytime mode: original behavior
        DrawButton(10, 90, 120, 35, "Search", 
                   LIGHTBLUE, 
                   WHITE, 
                   BLACK, 2, false);
        
        DrawButton(140, 90, 120, 35, "Clear", 
                   LIGHTGRAY, 
                   BLACK, 
                   BLACK, 2, false);
    }
    
    // result zone with scrollbar
    displaySearchResults();
    
    // Virtual keyboard
    drawKeyboard();
    
    if (selectedResult >= 0) {
        if (IS_NIGHTMODE) {
            DrawButton(200, 430, 110, 40, "GOTO", 
                       BLACK,    // Red background
                       RED,  // black outline
                       RED,  // Text
                       2, false);
        } else {
            DrawButton(200, 430, 110, 40, "GOTO", 
                       GREEN, 
                       WHITE, 
                       BLACK, 2, false);
        }
    }
}

void drawKeyboard() {
    int keyWidth = 32;
    int keyHeight = 40;
    int startX = 0;
    int startY = 320;
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 10; col++) {
            int x = startX + col * keyWidth;
            int y = startY + row * keyHeight;
            
            char keyChar = keyboardLayout[row][col];
            char keyStr[3] = {keyChar, '\0'};
            
            if (keyChar == '_') {
                safeStringCopy(keyStr, "___", sizeof(keyStr));
            } else if (keyChar == '<') {
                safeStringCopy(keyStr, "Bk", sizeof(keyStr));
            } else if (keyChar == ' ') {
                safeStringCopy(keyStr, "Sp", sizeof(keyStr));
            }
            
            if (IS_NIGHTMODE) {
                // Night mode : red keys with black outline and text
                DrawButton(x, y, keyWidth, keyHeight - 2, 
                           keyStr, 
                           RED,    // Red background
                           BLACK,  // black outline
                           BLACK,  // Text
                           2, false);
            } else {
                // Day mode
                DrawButton(x, y, keyWidth, keyHeight - 2, 
                           keyStr, 
                           LIGHTGRAY, 
                           BLACK, 
                           BLACK, 2, false);
            }
        }
    }
}

void displaySearchResults() {
    int startY = 130;
    int maxVisibleResults = 5;
    int resultHeight = 30;
    
    if (strlen(searchQuery) < 2) {
        tft.setTextColor(IS_NIGHTMODE ? BLACK : LIGHTGRAY); // Black text in night mode
        tft.setTextScale(2);
        tft.cursorToXY(15, startY);
        tft.print("Enter 2+ characters");
        return;
    }
    
    if (searchResultCount == 0) {
        tft.setTextColor(IS_NIGHTMODE ? BLACK : LIGHTGRAY); // Black text in night mode
        tft.setTextScale(2);
        tft.cursorToXY(15, startY);
        tft.print("No results found");
        return;
    }
    
    // Results up to 250
    int resultsWidth = 250;
    
    for (int i = 0; i < maxVisibleResults; i++) {
        int resultIndex = scrollOffset + i;
        if (resultIndex >= searchResultCount) break;
        
        int yPos = startY + i * resultHeight;
        
        // Night mode color
        uint16_t bgColor, textColor, borderColor;
        
        if (IS_NIGHTMODE) {
            if (resultIndex == selectedResult) {
                bgColor = BLACK; 
                textColor = RED; 
                borderColor = RED; 
            } else {
                bgColor = RED;
                textColor = BLACK;
                borderColor = RED;
            }
        } else {
            // Day mode
            if (resultIndex == selectedResult) {
                bgColor = YELLOW;
                textColor = BLACK;
                borderColor = BLACK;
            } else {
                bgColor = LIGHTGRAY;
                textColor = BLACK;
                borderColor = BLACK;
            }
        }
        
        tft.fillRect(10, yPos, resultsWidth, resultHeight - 2, bgColor);
        
        // Outline only in day mode
        if (!IS_NIGHTMODE || resultIndex == selectedResult) {
            tft.drawRect(10, yPos, resultsWidth, resultHeight - 2, borderColor);
        }
        
        tft.setTextColor(textColor);
        tft.setTextScale(2);
        tft.cursorToXY(15, yPos + 8);
        
        // display name and catalog
        char displayText[35];
        snprintf(displayText, sizeof(displayText), "%s (%s)", 
                 searchResults[resultIndex], searchCatalog[resultIndex]);
        
        if (strlen(displayText) > 28) {
            displayText[25] = '.';
            displayText[26] = '.';
            displayText[27] = '.';
            displayText[28] = '\0';
        }
        tft.print(displayText);
    }
    
    // Scrollbar
    if (searchResultCount > maxVisibleResults) {
        int scrollbarWidth = 20;
        int scrollbarX = 290 - (scrollbarWidth / 2);
        int scrollbarY = startY;
        int scrollbarHeight = maxVisibleResults * resultHeight - 2;
        
        // Scrollbar background
        uint16_t scrollbarBg = IS_NIGHTMODE ? RED : LIGHTGRAY;
        uint16_t scrollbarBorder = IS_NIGHTMODE ? RED : BLACK;
        
        tft.fillRect(scrollbarX, scrollbarY, scrollbarWidth, scrollbarHeight, scrollbarBg);
        
        // Outline only in day mode
        if (!IS_NIGHTMODE) {
            tft.drawRect(scrollbarX, scrollbarY, scrollbarWidth, scrollbarHeight, scrollbarBorder);
        }
        
        // scrollbar cursor
        int cursorHeight = (maxVisibleResults * scrollbarHeight) / searchResultCount;
        cursorHeight = max(cursorHeight, 15);
        
        int cursorY = scrollbarY + (scrollOffset * (scrollbarHeight - cursorHeight)) / max(1, searchResultCount - maxVisibleResults);
        
        uint16_t cursorColor = IS_NIGHTMODE ? BLACK : LIGHTBLUE; // Black cursor in night mode
        tft.fillRect(scrollbarX, cursorY, scrollbarWidth, cursorHeight, cursorColor);
        
        // buttons up down
        int arrowSize = 12;
        int arrowWidth = 20;
        
        // up arrow ↑
        int upArrowX = scrollbarX;
        int upArrowY = scrollbarY - arrowSize - 5;
        
        uint16_t arrowBg = IS_NIGHTMODE ? RED : LIGHTGRAY;
        uint16_t arrowBorder = IS_NIGHTMODE ? RED : BLACK;
        
        tft.fillRect(upArrowX, upArrowY, arrowWidth, arrowSize, arrowBg);
        
        // Outline only in day mode
        if (!IS_NIGHTMODE) {
            tft.drawRect(upArrowX, upArrowY, arrowWidth, arrowSize, arrowBorder);
        }
        
        // up arrow ↑
        tft.setTextColor(IS_NIGHTMODE ? BLACK : BLACK);
        tft.setTextScale(1);
        tft.cursorToXY(upArrowX + 7, upArrowY + 2);
        tft.print("U");
        
        // down arrow ↓
        int downArrowX = scrollbarX;
        int downArrowY = scrollbarY + scrollbarHeight + 5;
        
        tft.fillRect(downArrowX, downArrowY, arrowWidth, arrowSize, arrowBg);
        
        // Outline only in day mode
        if (!IS_NIGHTMODE) {
            tft.drawRect(downArrowX, downArrowY, arrowWidth, arrowSize, arrowBorder);
        }
        
        // down arrow ↓
        tft.cursorToXY(downArrowX + 7, downArrowY + 2);
        tft.print("D");
        
        // position indicator
        char positionText[10];
        snprintf(positionText, sizeof(positionText), "%d/%d", 
                 min(scrollOffset + maxVisibleResults, searchResultCount), 
                 searchResultCount);
        tft.setTextColor(IS_NIGHTMODE ? BLACK : BLACK);
        tft.cursorToXY(scrollbarX, downArrowY + arrowSize + 8);
        tft.print(positionText);
    }
}

void searchObject(const char* query) {

    searchResultCount = 0;
    selectedResult = -1;
    currentSearchPage = 0;
    scrollOffset = 0;
    
    if (strlen(query) < 2) {
        return;
    }
    
    char lowerQuery[50];
    safeStringCopy(lowerQuery, query, sizeof(lowerQuery));
    for (int i = 0; lowerQuery[i]; i++) {
        lowerQuery[i] = tolower(lowerQuery[i]);
    }
    
    // 50 results max by catalog
    searchInCatalog("Bright_CAT.csv", lowerQuery);
    if (searchResultCount < 50) searchInCatalog("Double_CAT.csv", lowerQuery);
    if (searchResultCount < 50) searchInCatalog("Variable_CAT.csv", lowerQuery);
    if (searchResultCount < 50) searchInCatalog("messier.csv", lowerQuery);
    if (searchResultCount < 50) searchInCatalog("IC_CAT.csv", lowerQuery);
    if (searchResultCount < 50) searchInCatalog("custom_CAT.csv", lowerQuery);
    
    // look in NGC
    if (searchResultCount < 50) {
        const char* ngcFiles[] = {
            "NGC_CAT1_10.csv", "NGC_CAT2_10.csv", "NGC_CAT3_10.csv", "NGC_CAT4_10.csv",
            "NGC_CAT5_10.csv", "NGC_CAT6_10.csv", "NGC_CAT7_10.csv", "NGC_CAT8_10.csv",
            "NGC_CAT9_10.csv", "NGC_CAT10_10.csv"
        };
        
        for (int i = 0; i < 10 && searchResultCount < 50; i++) {
            searchInCatalog(ngcFiles[i], lowerQuery);
            if (searchResultCount >= 50) break;
        }
    }
}

void searchInCatalog(const char* filename, const char* query) {
    File file = SD.open(filename);
    if (!file) {
        Serial.print("Error opening: ");
        Serial.println(filename);
        return;
    }
    
    char line[200];
    int lineCount = 0;
    
    // DELETE line limit read all the file
    while (file.available() && searchResultCount < 50) {
        safeStringCopy(line, "", sizeof(line));
        int bytesRead = file.readBytesUntil('\n', line, sizeof(line) - 1);
        
        if (bytesRead <= 0) continue;
        
        line[bytesRead] = '\0';
        lineCount++;
        
        if (strlen(line) < 5) continue;
        
        // Extract name
        char* i1 = strchr(line, ';');
        if (!i1) continue;
        
        char objectName[50];
        strncpy(objectName, line, i1 - line);
        objectName[i1 - line] = '\0';
        
        // convert in lowercase
        char lowerName[50];
        safeStringCopy(lowerName, objectName, sizeof(lowerName));
        for (int i = 0; lowerName[i]; i++) {
            lowerName[i] = tolower(lowerName[i]);
        }
        
        // don't bother with capitalization
        if (strstr(lowerName, query) != NULL) {
            char* i2 = strchr(i1 + 1, ';');
            char* i3 = strchr(i2 + 1, ';');
            
            if (i2 && i3) {
                // Extract RA
                char OBJ_RA[20];
                strncpy(OBJ_RA, i1 + 1, i2 - i1 - 1);
                OBJ_RA[i2 - i1 - 1] = '\0';
                
                float raHours = 0, raMinutes = 0;
                char* hPos = strchr(OBJ_RA, 'h');
                if (hPos) {
                    *hPos = '\0';
                    raHours = atof(OBJ_RA);
                    raMinutes = atof(hPos + 1);
                }
                float raDecimal = raHours + raMinutes / 60.0;
                
                // Extract DEC
                char OBJ_DEC[20];
                strncpy(OBJ_DEC, i2 + 1, i3 - i2 - 1);
                OBJ_DEC[i3 - i2 - 1] = '\0';
                
                float decDegrees = 0, decMinutes = 0;
                int sign = 1;
                
                if (OBJ_DEC[0] == '-') sign = -1;
                
                char* degPos = strchr(OBJ_DEC, '°');
                if (degPos) {
                    *degPos = '\0';
                    decDegrees = atof(sign == -1 ? OBJ_DEC + 1 : OBJ_DEC);
                    decMinutes = atof(degPos + 1);
                }
                float decDecimal = sign * (decDegrees + decMinutes / 60.0);
                
                // Store result
                if (searchResultCount < 50) {
                    safeStringCopy(searchResults[searchResultCount], objectName, sizeof(searchResults[0]));
                    
                    char catalogShort[20];
                    if (strstr(filename, "Bright")) safeStringCopy(catalogShort, "Bright", sizeof(catalogShort));
                    else if (strstr(filename, "Double")) safeStringCopy(catalogShort, "Double", sizeof(catalogShort));
                    else if (strstr(filename, "Variable")) safeStringCopy(catalogShort, "Variable", sizeof(catalogShort));
                    else if (strstr(filename, "messier")) safeStringCopy(catalogShort, "Messier", sizeof(catalogShort));
                    else if (strstr(filename, "NGC")) safeStringCopy(catalogShort, "NGC", sizeof(catalogShort));
                    else if (strstr(filename, "IC")) safeStringCopy(catalogShort, "IC", sizeof(catalogShort));
                    else if (strstr(filename, "custom")) safeStringCopy(catalogShort, "Custom", sizeof(catalogShort));
                    else safeStringCopy(catalogShort, filename, sizeof(catalogShort));
                    
                    safeStringCopy(searchCatalog[searchResultCount], catalogShort, sizeof(searchCatalog[0]));
                    searchRA[searchResultCount] = raDecimal;
                    searchDEC[searchResultCount] = decDecimal;
                    searchResultCount++;
                }
            }
        }
        
        if (searchResultCount >= 50) break;
    }
    file.close();
    
    Serial.print(filename);
    Serial.print(": ");
    Serial.print(lineCount);
    Serial.print(" lines, ");
    Serial.print(searchResultCount);
    Serial.println(" results");
}

void searchCatalogFile(const char* filename, const char* query) {
    File file = SD.open(filename);
    if (!file) return;
    
    char line[200];
    
    while (file.available() && searchResultCount < 10) {
        safeStringCopy(line, "", sizeof(line));
        file.readBytesUntil('\n', line, sizeof(line) - 1);
        
        // ignore empty lines or too short
        if (strlen(line) < 5) continue;
        
        // extract object name
        int firstSpace = strcspn(line, " \t");
        char objectName[50];
        strncpy(objectName, line, firstSpace);
        objectName[firstSpace] = '\0';
        
        // convert in lowercase for comparison
        char lowerName[50];
        safeStringCopy(lowerName, objectName, sizeof(lowerName));
        for (int i = 0; lowerName[i]; i++) {
            lowerName[i] = tolower(lowerName[i]);
        }
        
        // don't bother with capitalization
        if (strstr(lowerName, query) != NULL) {
            float ra, dec;
            if (extractCoordinates(line, &ra, &dec)) {
                safeStringCopy(searchResults[searchResultCount], objectName, sizeof(searchResults[0]));
                
                // simplify catalog name for display
                char catalogShort[20];
                if (strstr(filename, "Bright")) safeStringCopy(catalogShort, "Bright Star", sizeof(catalogShort));
                else if (strstr(filename, "Double")) safeStringCopy(catalogShort, "Double Star", sizeof(catalogShort));
                else if (strstr(filename, "Variable")) safeStringCopy(catalogShort, "Variable Star", sizeof(catalogShort));
                else if (strstr(filename, "messier")) safeStringCopy(catalogShort, "Messier", sizeof(catalogShort));
                else if (strstr(filename, "NGC")) safeStringCopy(catalogShort, "NGC", sizeof(catalogShort));
                else if (strstr(filename, "IC")) safeStringCopy(catalogShort, "IC", sizeof(catalogShort));
                else if (strstr(filename, "custom")) safeStringCopy(catalogShort, "Custom", sizeof(catalogShort));
                else safeStringCopy(catalogShort, filename, sizeof(catalogShort));
                
                safeStringCopy(searchCatalog[searchResultCount], catalogShort, sizeof(searchCatalog[0]));
                searchRA[searchResultCount] = ra;
                searchDEC[searchResultCount] = dec;
                searchResultCount++;
            }
        }
    }
    file.close();
}

void updateSearchResultsOnly() {
    int startY = 130;
    int maxVisibleResults = 5;
    int resultHeight = 30;
    int resultsWidth = 250;
    
// redraw
    
    if (strlen(searchQuery) < 2) {
        // Effacer uniquement la zone du premier résultat pour le message
        if (IS_NIGHTMODE) {
            tft.fillRect(10, startY, resultsWidth, resultHeight - 2, BLACK);
        } else {
            tft.fillRect(10, startY, resultsWidth, resultHeight - 2, WHITE);
        }
        tft.setTextColor(IS_NIGHTMODE ? BLACK : LIGHTGRAY);
        tft.setTextScale(2);
        tft.cursorToXY(15, startY);
        tft.print("Enter 2+ characters");
        return;
    }
    
    if (searchResultCount == 0) {
        // Effacer uniquement la zone du premier résultat pour le message
        if (IS_NIGHTMODE) {
            tft.fillRect(10, startY, resultsWidth, resultHeight - 2, BLACK);
        } else {
            tft.fillRect(10, startY, resultsWidth, resultHeight - 2, WHITE);
        }
        tft.setTextColor(IS_NIGHTMODE ? BLACK : LIGHTGRAY);
        tft.setTextScale(2);
        tft.cursorToXY(15, startY);
        tft.print("No results found");
        return;
    }

// Display the results (they erase the background by drawing their own background)
    for (int i = 0; i < maxVisibleResults; i++) {
        int resultIndex = scrollOffset + i;
        int yPos = startY + i * resultHeight;
        
        if (resultIndex >= searchResultCount) {
            // Effacer les lignes vides restantes
            if (IS_NIGHTMODE) {
                tft.fillRect(10, yPos, resultsWidth, resultHeight - 2, BLACK);
            } else {
                tft.fillRect(10, yPos, resultsWidth, resultHeight - 2, WHITE);
            }
            continue;
        }
        
        uint16_t bgColor, textColor, borderColor;
        
        if (IS_NIGHTMODE) {
            if (resultIndex == selectedResult) {
                bgColor = BLACK; 
                textColor = RED; 
                borderColor = RED; 
            } else {
                bgColor = RED;
                textColor = BLACK;
                borderColor = RED;
            }
        } else {
            if (resultIndex == selectedResult) {
                bgColor = YELLOW;
                textColor = BLACK;
                borderColor = BLACK;
            } else {
                bgColor = LIGHTGRAY;
                textColor = BLACK;
                borderColor = BLACK;
            }
        }
        
        tft.fillRect(10, yPos, resultsWidth, resultHeight - 2, bgColor);
        
        if (!IS_NIGHTMODE || resultIndex == selectedResult) {
            tft.drawRect(10, yPos, resultsWidth, resultHeight - 2, borderColor);
        }
        
        tft.setTextColor(textColor);
        tft.setTextScale(2);
        tft.cursorToXY(15, yPos + 8);
        
        char displayText[35];
        snprintf(displayText, sizeof(displayText), "%s (%s)", 
                 searchResults[resultIndex], searchCatalog[resultIndex]);
        
        if (strlen(displayText) > 28) {
            displayText[25] = '.';
            displayText[26] = '.';
            displayText[27] = '.';
            displayText[28] = '\0';
        }
        tft.print(displayText);
    }
    
// Scrollbar (it redraws itself over the background)
    if (searchResultCount > maxVisibleResults) {
        int scrollbarWidth = 20;
        int scrollbarX = 290 - (scrollbarWidth / 2);
        int scrollbarY = startY;
        int scrollbarHeight = maxVisibleResults * resultHeight - 2;
        
        uint16_t scrollbarBg = IS_NIGHTMODE ? RED : LIGHTGRAY;
        uint16_t scrollbarBorder = IS_NIGHTMODE ? RED : BLACK;
        
        tft.fillRect(scrollbarX, scrollbarY, scrollbarWidth, scrollbarHeight, scrollbarBg);
        
        if (!IS_NIGHTMODE) {
            tft.drawRect(scrollbarX, scrollbarY, scrollbarWidth, scrollbarHeight, scrollbarBorder);
        }
        
        int cursorHeight = (maxVisibleResults * scrollbarHeight) / searchResultCount;
        cursorHeight = max(cursorHeight, 15);
        
        int cursorY = scrollbarY + (scrollOffset * (scrollbarHeight - cursorHeight)) / max(1, searchResultCount - maxVisibleResults);
        
        uint16_t cursorColor = IS_NIGHTMODE ? BLACK : LIGHTBLUE;
        tft.fillRect(scrollbarX, cursorY, scrollbarWidth, cursorHeight, cursorColor);
        
        int arrowSize = 12;
        int arrowWidth = 20;
        
        int upArrowX = scrollbarX;
        int upArrowY = scrollbarY - arrowSize - 5;
        
        uint16_t arrowBg = IS_NIGHTMODE ? RED : LIGHTGRAY;
        uint16_t arrowBorder = IS_NIGHTMODE ? RED : BLACK;
        
        tft.fillRect(upArrowX, upArrowY, arrowWidth, arrowSize, arrowBg);
        
        if (!IS_NIGHTMODE) {
            tft.drawRect(upArrowX, upArrowY, arrowWidth, arrowSize, arrowBorder);
        }
        
        tft.setTextColor(IS_NIGHTMODE ? BLACK : BLACK);
        tft.setTextScale(1);
        tft.cursorToXY(upArrowX + 7, upArrowY + 2);
        tft.print("U");
        
        int downArrowX = scrollbarX;
        int downArrowY = scrollbarY + scrollbarHeight + 5;
        
        tft.fillRect(downArrowX, downArrowY, arrowWidth, arrowSize, arrowBg);
        
        if (!IS_NIGHTMODE) {
            tft.drawRect(downArrowX, downArrowY, arrowWidth, arrowSize, arrowBorder);
        }
        
        tft.cursorToXY(downArrowX + 7, downArrowY + 2);
        tft.print("D");
        
        char positionText[10];
        snprintf(positionText, sizeof(positionText), "%d/%d", 
                 min(scrollOffset + maxVisibleResults, searchResultCount), 
                 searchResultCount);
        tft.setTextColor(IS_NIGHTMODE ? BLACK : BLACK);
        tft.cursorToXY(scrollbarX, downArrowY + arrowSize + 8);
        tft.print(positionText);
    } else {
        // Pas de scrollbar : effacer la zone où elle pourrait être
        int scrollbarX = 280;
        int scrollbarY = startY - 20;
        if (IS_NIGHTMODE) {
            tft.fillRect(scrollbarX, scrollbarY, 40, 180, BLACK);
        } else {
            tft.fillRect(scrollbarX, scrollbarY, 40, 180, WHITE);
        }
    }
    
  // Redraw the GOTO button if necessary
    if (selectedResult >= 0) {
        if (IS_NIGHTMODE) {
            DrawButton(200, 430, 110, 40, "GOTO", 
                       BLACK, RED, RED, 2, false);
        } else {
            DrawButton(200, 430, 110, 40, "GOTO", 
                       GREEN, WHITE, BLACK, 2, false);
        }
    } 
}

bool extractCoordinates(const char* line, float* ra, float* dec) {
  // pattern : "Name     RA     DEC     ..."
  char tempLine[200];
  safeStringCopy(tempLine, line, sizeof(tempLine));
  
  // jump name
  char* token = strtok(tempLine, " ");
  if (!token) return false;
  
  // RA (2nd token)
  token = strtok(NULL, " ");
  if (!token) return false;
  *ra = parseRA(token);
  
  // DEC (3rd token)
  token = strtok(NULL, " ");
  if (!token) return false;
  *dec = parseDEC(token);
  
  return true;
}

float parseRA(const char* raStr) {
// Supported formats: "0h8m", "23h30.1m", "12h34m56s"
  float hours = 0, minutes = 0, seconds = 0;
  char temp[20];
  safeStringCopy(temp, raStr, sizeof(temp));
  
  char* h = strtok(temp, "h");
  if (h) hours = atof(h);
  
  char* m = strtok(NULL, "m");
  if (m) minutes = atof(m);
  
  char* s = strtok(NULL, "s");
  if (s) seconds = atof(s);
  
  return hours + minutes/60.0 + seconds/3600.0;
}

float parseDEC(const char* decStr) {
// Supported formats: "+29°6'", "-45°30.5'"
  float degrees = 0, minutes = 0, seconds = 0;
  int sign = 1;
  char temp[20];
  safeStringCopy(temp, decStr, sizeof(temp));
  
  if (temp[0] == '-') {
    sign = -1;
    memmove(temp, temp + 1, strlen(temp));
  } else if (temp[0] == '+') {
    memmove(temp, temp + 1, strlen(temp));
  }
  
  char* d = strtok(temp, "°");
  if (d) degrees = atof(d);
  
  char* m = strtok(NULL, "'");
  if (m) minutes = atof(m);
  
  char* s = strtok(NULL, "\"");
  if (s) seconds = atof(s);
  
  return sign * (degrees + minutes/60.0 + seconds/3600.0);
}

void toLowerCase(char* str) {
  for (int i = 0; str[i]; i++) {
    str[i] = tolower(str[i]);
  }
}

void gotoSelectedObject() {
    if (selectedResult >= 0 && selectedResult < searchResultCount) {
        // Convert RA decimal in hour/minute
        float raDecimal = searchRA[selectedResult];
        OBJECT_RA_H = (int)raDecimal;
        OBJECT_RA_M = (raDecimal - OBJECT_RA_H) * 60.0;
        
        // Convert DEC decimal in degrees/minutes  
        float decDecimal = searchDEC[selectedResult];
        OBJECT_DEC_D = (int)decDecimal;
        OBJECT_DEC_M = (decDecimal - OBJECT_DEC_D) * 60.0;
        
        // Update name and details
        safeStringCopy(OBJECT_NAME, searchResults[selectedResult], sizeof(OBJECT_NAME));
        
        char details[100];
        snprintf(details, sizeof(details), "From %s catalog - RA: %.1fh DEC: %.1f°", 
                 searchCatalog[selectedResult], raDecimal, decDecimal);
        safeStringCopy(OBJECT_DESCR, details, sizeof(OBJECT_DESCR));
        
        safeStringCopy(OBJECT_DETAILS, "Object selected via search function", sizeof(OBJECT_DETAILS));
        
        // launch GOTO
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
            
            Timer3.stop();
            IS_TRACKING = false;
            IS_OBJ_FOUND = false;
            IS_OBJECT_RA_FOUND = false;
            IS_OBJECT_DEC_FOUND = false;
            Tracking_type = 1;
            safeStringCopy(Tracking_Mode, "Celest", sizeof(Tracking_Mode));
            sendTrackingStatus();
            Slew_timer = millis();
            Slew_RA_timer = Slew_timer + 20000;
        }
        
        UpdateObservedObjects();
        drawMainScreen();
    }
}

void updateSearchField() {
    if (IS_NIGHTMODE) {
        tft.fillRect(10, 50, 250, 35, RED);
        tft.drawRect(10, 50, 250, 35, BLACK);
    } else {
        tft.fillRect(10, 50, 250, 35, LIGHTGRAY);
        tft.drawRect(10, 50, 250, 35, BLACK);
    }
    
    tft.setTextColor(IS_NIGHTMODE ? BLACK : BLACK);
    tft.setTextScale(2);
    tft.cursorToXY(15, 55);
    // Truncate if too long
    char displayQuery[20];
    safeStringCopy(displayQuery, searchQuery, sizeof(displayQuery));
    if (strlen(displayQuery) > 18) {
        displayQuery[15] = '.';
        displayQuery[16] = '.';
        displayQuery[17] = '.';
        displayQuery[18] = '\0';
    }
    tft.print(displayQuery);
}


//////////////////////////////////////////////// NEW CODE FOR HARDWARE SLEW CONTROL //////////////////////////

// === ISR ===
void slewRA_ISR() {
  if (slewRA_active && slewRA_targetSteps > 0) {
    PIOC->PIO_SODR = (1u << 26);
    delayMicroseconds(10);
    PIOC->PIO_CODR = (1u << 26);
    
    if (slewRA_currentDir == STP_BACK) {
      RA_microSteps += RA_mode_steps;
    } else {
      RA_microSteps -= RA_mode_steps;
    }
    
    slewRA_targetSteps--;
    
    // take a look if the target is reach
    if (SLEW_RA_microsteps >= (RA_microSteps - RA_mode_steps) && 
        SLEW_RA_microsteps <= (RA_microSteps + RA_mode_steps)) {
      IS_OBJECT_RA_FOUND = true;
      slewRA_active = false;
      Timer4.stop();
      #ifdef serial_debug
      Serial.println("RA TARGET REACHED");
      #endif
    } else if (slewRA_targetSteps <= 0) {
      // Security : stop when step are finished
      IS_OBJECT_RA_FOUND = true;
      slewRA_active = false;
      Timer4.stop();
      #ifdef serial_debug
      Serial.println("RA STEPS EXHAUSTED");
      #endif
    }
  }
}

void slewDEC_ISR() {
  if (slewDEC_active && slewDEC_targetSteps > 0) {
    PIOC->PIO_SODR = (1u << 24);
    delayMicroseconds(10);
    PIOC->PIO_CODR = (1u << 24);
    
    if (slewDEC_currentDir == STP_BACK) {
      DEC_microSteps += DEC_mode_steps;
    } else {
      DEC_microSteps -= DEC_mode_steps;
    }
    
    slewDEC_targetSteps--;
    
    // take a look if the target is reach
    if (SLEW_DEC_microsteps >= (DEC_microSteps - DEC_mode_steps) && 
        SLEW_DEC_microsteps <= (DEC_microSteps + DEC_mode_steps)) {
      IS_OBJECT_DEC_FOUND = true;
      slewDEC_active = false;
      Timer5.stop();
      #ifdef serial_debug
      Serial.println("DEC TARGET REACHED");
      #endif
    } else if (slewDEC_targetSteps <= 0) {
      // Security : stop when step are finished
      IS_OBJECT_DEC_FOUND = true;
      slewDEC_active = false;
      Timer5.stop();
      #ifdef serial_debug
      Serial.println("DEC STEPS EXHAUSTED");
      #endif
    }
  }
}

// === CONTROL FUNCTIONS ===

void startSlewRA(int direction, long steps, long periodMicros) {
  if (steps <= 0) return;
  
  Timer4.stop();  // Stop before reconfigure
  
  slewRA_currentDir = direction;
  slewRA_targetSteps = steps;
  slewRA_active = true;
  
  digitalWrite(RA_EN, LOW);
  digitalWrite(RA_DIR, direction);
  delayMicroseconds(50);
  
  Timer4.attachInterrupt(slewRA_ISR);
  Timer4.start(periodMicros);
  
  #ifdef serial_debug
  Serial.print("RA START: steps=");
  Serial.print(steps);
  Serial.print(" period=");
  Serial.println(periodMicros);
  #endif
}

void startSlewDEC(int direction, long steps, long periodMicros) {
  if (steps <= 0) return;
  
  Timer5.stop();
  
  slewDEC_currentDir = direction;
  slewDEC_targetSteps = steps;
  slewDEC_active = true;
  
  digitalWrite(DEC_EN, LOW);
  digitalWrite(DEC_DIR, direction);
  delayMicroseconds(50);
  
  Timer5.attachInterrupt(slewDEC_ISR);
  Timer5.start(periodMicros);
  
  #ifdef serial_debug
  Serial.print("DEC START: steps=");
  Serial.print(steps);
  Serial.print(" period=");
  Serial.println(periodMicros);
  #endif
}

void stopSlewRA() {
  slewRA_active = false;
  Timer4.stop();
  slewRA_targetSteps = 0;
}

void stopSlewDEC() {
  slewDEC_active = false;
  Timer5.stop();
  slewDEC_targetSteps = 0;
}

void updateSlewSpeed(char axis, long periodMicros) {
  if (axis == 'R' || axis == 'r') {
    if (slewRA_active) {
      Timer4.setPeriod(periodMicros);
    }
  } else if (axis == 'D' || axis == 'd') {
    if (slewDEC_active) {
      Timer5.setPeriod(periodMicros);
    }
  }
}
