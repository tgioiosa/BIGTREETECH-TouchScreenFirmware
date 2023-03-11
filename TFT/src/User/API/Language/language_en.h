#ifndef _LANGUAGE_EN_H_
#define _LANGUAGE_EN_H_

    // config.ini Parameter Settings - Screen Settings and Feature Settings
    #define STRING_LANGUAGE               "English"
    #define STRING_EMULATED_M600          "Emulated M600"
    #define STRING_EMULATED_M109_M190     "Emulated M109 / M190"
    #define STRING_EVENT_LED              "Event LED"
    #define STRING_FILE_COMMENT_PARSING   "File comment parsing"
    #define STRING_ROTATED_UI             "Rotate UI"
    #define STRING_ACK_NOTIFICATION       "ACK notification style"
    #define STRING_FILES_SORT_BY          "Sort files by"
    #define STRING_FILES_LIST_MODE        "Files viewer List Mode"
    #define STRING_FILENAME_EXTENSION     "Show filename extension"
    #define STRING_FAN_SPEED_PERCENTAGE   "Fan Speed in Percentage"
    #define STRING_PERSISTENT_INFO        "Persistent Status Info"
    #define STRING_TERMINAL_ACK           "Show ACK in Terminal"
    #define STRING_SERIAL_ALWAYS_ON       "Serial Always ON"
    #define STRING_MARLIN_FULLSCREEN      "Fullscreen Mode"
    #define STRING_MARLIN_SHOW_TITLE      "Show Title"
    #define STRING_MARLIN_TYPE            "Marlin Mode Type"
    #define STRING_MOVE_SPEED             "Move speed(X Y Z)"
    #define STRING_AUTO_LOAD_LEVELING     "Auto Load Leveling"
    #define STRING_PROBING_Z_OFFSET       "Z offset"                        //TG 2/28/23 removed "Probing for"
    #define STRING_Z_STEPPERS_ALIGNMENT   "Z Steppers Auto-Alignment"
    #define STRING_PS_AUTO_SHUTDOWN       "Auto shutdown"
    #define STRING_FIL_RUNOUT             "Filament sensor"
    #define STRING_PL_RECOVERY            "Power loss recovery"
    #define STRING_PL_RECOVERY_HOME       "Home after power loss"
    #define STRING_BTT_MINI_UPS           "BTT UPS Support"
    #define STRING_TOUCH_SOUND            "Touch sounds"
    #define STRING_TOAST_SOUND            "Toast Notifications"
    #define STRING_ALERT_SOUND            "Popups and Alerts"
    #define STRING_HEATER_SOUND           "Heater notification"
    #define STRING_LCD_BRIGHTNESS         "Brightness"
    #define STRING_LCD_IDLE_BRIGHTNESS    "Idle brightness"
    #define STRING_LCD_IDLE_TIME          "Idle timeout"
    #define STRING_LCD_LOCK_ON_IDLE       "LCD lock on idle"
    #define STRING_LED_ALWAYS_ON          "LED always ON"
    #define STRING_KNOB_LED_COLOR         "Rotary Knob LED"
    #define STRING_KNOB_LED_IDLE          "Rotary Knob LED idle"
    #define STRING_START_GCODE_ENABLED    "Start Gcode before print"
    #define STRING_END_GCODE_ENABLED      "End Gcode after print"
    #define STRING_CANCEL_GCODE_ENABLED   "Cancel Gcode"

    // Machine Parameter Settings - Param Title (ordered by gcode)
    #define STRING_STEPS_SETTING          "Steps per mm"
    #define STRING_FILAMENT_SETTING       "Filament Diameter"
    #define STRING_MAXACCELERATION        "Max Acceleration"
    #define STRING_MAXFEEDRATE            "Max Feed Rate"
    #define STRING_ACCELERATION           "Acceleration"
    #define STRING_JERK                   "Jerk"
    #define STRING_JUNCTION_DEVIATION     "Junction Deviation"
    #define STRING_HOME_OFFSET            "Home Offset"
    #define STRING_FWRETRACT              "FW Retraction"
    #define STRING_FWRECOVER              "FW Retraction Recover"
    #define STRING_RETRACT_AUTO           "Auto Firmware Retract"
    #define STRING_HOTEND_OFFSET          "Offset 2nd Nozzle"
    #define STRING_HOTEND_PID             "Hotend PID"
    #define STRING_BED_PID                "Bed PID"
    #define STRING_STEALTH_CHOP           "TMC StealthChop"
    #define STRING_INPUT_SHAPING          "Input Shaping"
    #define STRING_DELTA_CONFIGURATION    "Delta Configuration"
    #define STRING_DELTA_TOWER_ANGLE      "Tower Angle Trim"
    #define STRING_DELTA_DIAGONAL_ROD     "Diagonal Rod Trim"
    #define STRING_DELTA_ENDSTOP          "Endstop Adjustments"
    #define STRING_PROBE_OFFSET           "Probe Offset"
    #define STRING_LIN_ADVANCE            "Linear Advance"
    #define STRING_CURRENT_SETTING        "Driver Current (mA)"
    #define STRING_HYBRID_THRESHOLD       "TMC HybridThreshold"
    #define STRING_BUMP_SENSITIVITY       "TMC bump sensitivity"
    #define STRING_MBL_OFFSET             "MBL Offset"

    // Machine Parameter Settings - Param Attributes (ordered by gcode)
    #define STRING_PRINT_ACCELERATION     "Print Acceleration"
    #define STRING_RETRACT_ACCELERATION   "Retract Acceleration"
    #define STRING_TRAVEL_ACCELERATION    "Travel Acceleration"
    #define STRING_RETRACT_LENGTH         "Retract length"
    #define STRING_RETRACT_SWAP_LENGTH    "Retract swap length"
    #define STRING_RETRACT_FEEDRATE       "Retract feedrate"
    #define STRING_RETRACT_Z_LIFT         "Z lift on retraction"
    #define STRING_RECOVER_LENGTH         "Extra recover length"
    #define STRING_SWAP_RECOVER_LENGTH    "Extra recover swap length"
    #define STRING_RECOVER_FEEDRATE       "Recover feedrate"
    #define STRING_SWAP_RECOVER_FEEDRATE  "Swap recover feedrate"

    // Save / Load
    #define STRING_SAVE                   "Save"
    #define STRING_RESTORE                "Restore"
    #define STRING_RESET                  "Reset"
    #define STRING_EEPROM_SAVE_INFO       "Save printer settings to EEPROM?"
    #define STRING_EEPROM_RESTORE_INFO    "Reload settings from EEPROM?"
    #define STRING_EEPROM_RESET_INFO      "Reset EEPROM to stock printer settings?"
    #define STRING_SETTINGS_SAVE          "Save settings"
    #define STRING_SETTINGS_RESTORE       "Restore settings"
    #define STRING_SETTINGS_RESET         "Reset default settings"
    #define STRING_SETTINGS_RESET_INFO    "Reset all settings to default values?"
    #define STRING_SETTINGS_RESET_DONE    "Reset all settings successful.\nPlease restart the device."

    // Navigation Buttons
    #define STRING_PAGE_UP                "Page up"
    #define STRING_PAGE_DOWN              "Page down"
    #define STRING_UP                     "Up"
    #define STRING_DOWN                   "Down"
    #define STRING_NEXT                   "Next"
    #define STRING_BACK                   "Back"

    // Value Buttons
    #define STRING_INC                    "Increase"
    #define STRING_DEC                    "Decrease"
    #define STRING_LOAD                   "Load"
    #define STRING_UNLOAD                 "Unload"
    #define STRING_ON                     "ON"
    #define STRING_OFF                    "OFF"
    #define STRING_AUTO                   "AUTO"
    #define STRING_SMART                  "SMART"
    #define STRING_SLOW                   "Slow"
    #define STRING_NORMAL                 "Normal"
    #define STRING_FAST                   "Fast"
    #define STRING_ZERO                   "Zero"
    #define STRING_HALF                   "Half"
    #define STRING_FULL                   "Full"
    #define STRING_CUSTOM                 "Custom"
    #define STRING_CLEAR                  "Clear"
    #define STRING_DEFAULT                "Default"

    // Action Buttons
    #define STRING_START                  "Start"
    #define STRING_STOP                   "Stop"
    #define STRING_PAUSE                  "Pause"
    #define STRING_RESUME                 "Resume"
    #define STRING_INIT                   "Init"
    #define STRING_DISCONNECT             "Disconnect"
    #define STRING_SHUT_DOWN              "Shut down"
    #define STRING_FORCE_SHUT_DOWN        "Force"
    #define STRING_EMERGENCYSTOP          "EM. STOP"
    #define STRING_PREHEAT                "Preheat"
    #define STRING_PREHEAT_BOTH           "Both"
    #define STRING_COOLDOWN               "Cool down"

    // Dialog Buttons
    #define STRING_CONFIRM                "\u088E OK"  // Confirm Sign
    #define STRING_CANCEL                 "\u088F Cancel"  // Cancel Sign
    #define STRING_WARNING                "Warning"  // Exclamation Sign
    #define STRING_CONTINUE               "Continue"
    #define STRING_CONFIRMATION           "Are you sure?"

    // Process Status
    #define STRING_STATUS                 "Status"
    #define STRING_READY                  "Ready"
    #define STRING_BUSY                   "Busy processing, please wait..."
    #define STRING_LOADING                "Loading..."
    #define STRING_UNCONNECTED            " Waiting for Controller!"
    #define STRING_LISTENING              "TFT in Listening Mode!"
             
    // Process Info
    #define STRING_INFO                   "Info"  // Info Sign
    #define STRING_INVALID_VALUE          "Invalid value(s)"
    #define STRING_TIMEOUT_REACHED        "Timeout reached!"
    #define STRING_DISCONNECT_INFO        "Now you can control the printer by your computer!"
    #define STRING_SHUTTING_DOWN          "Shutting down..."
    #define STRING_WAIT_TEMP_SHUT_DOWN    "Wait for the hotend temperature to be lower than %d℃"
    #define STRING_POWER_FAILED           "Continue printing?"  // Question Sign
    #define STRING_PROCESS_RUNNING        "Process already running!"
    #define STRING_PROCESS_COMPLETED      "Process completed!"
    #define STRING_PROCESS_ABORTED        "Process aborted!"

    // TFT Media, Onboard Media, Filament Runout Process Commands / Status / Info
    #define STRING_TFT_SD                 "TFT SD"
    #define STRING_TFT_SD_INSERTED        "SD card inserted"
    #define STRING_TFT_SD_REMOVED         "SD card removed"
    #define STRING_TFT_SD_NOT_DETECTED    "No SD card detected!"
    #define STRING_TFT_SD_READ_ERROR      "SD card reading error!"
    #define STRING_TFT_USB                "TFT USB"
    #define STRING_TFT_USB_INSERTED       "USB disk inserted"
    #define STRING_TFT_USB_REMOVED        "USB disk removed"
    #define STRING_TFT_USB_NOT_DETECTED   "No USB disk detected!"
    #define STRING_TFT_USB_READ_ERROR     "USB disk reading error!"
    #define STRING_ONBOARD_SD             "Onboard SD"
    #define STRING_ONBOARD_USB            "OnboardUSB"
    #define STRING_ONBOARD_SD_READ_ERROR  "Onboard media reading error!"
    #define STRING_FILAMENT_RUNOUT        "Filament runout!"

    // Steppers, Print, Probe Process Commands / Status / Info
    #define STRING_DISABLE_STEPPERS       "Disarm All"
    #define STRING_XY_UNLOCK              "Disarm XY"

    #define STRING_START_PRINT            "Start printing:\n%s?"
    #define STRING_STOP_PRINT             "Stop printing?"
    #define STRING_IS_PAUSE               "Action not allowed while printing.\n\nPause printing?"
    #define STRING_M0_PAUSE               "Paused by M0 command"

    #define STRING_TEST                   "Test"
    #define STRING_DEPLOY                 "Deploy"
    #define STRING_STOW                   "Stow"
    #define STRING_REPEAT                 "Repeat"
    #define STRING_HS_ON                  "HS: On"
    #define STRING_HS_OFF                 "HS: Off"

    // Printer Tools
    #define STRING_NOZZLE                 "Nozzle"
    #define STRING_BED                    "Hotbed"
    //#define STRING_CHAMBER                "Chamber"   //TG 7/22/22 removed
    #define STRING_FAN                    "Fan"

    //#define STRING_BLTOUCH                "BLTouch"   //TG 7/22/22 removed
    //#define STRING_TOUCHMI                "TouchMi"   //TG 7/22/22 removed

    // Values
    //#define STRING_1_DEGREE               "1℃"        //TG 2/18/21 removed
    //#define STRING_5_DEGREE               "5℃"        //TG 2/18/21 removed
    //#define STRING_10_DEGREE              "10℃        //TG 2/18/21 removed

    #define STRING_001_MM                 "0.01mm"
    #define STRING_01_MM                  "0.1mm"
    #define STRING_1_MM                   "1mm"
    #define STRING_5_MM                   "5mm"
    #define STRING_10_MM                  "10mm"
    #define STRING_100_MM                 "100mm"
    #define STRING_200_MM                 "200mm"

    #define STRING_1_PERCENT              "1%"
    #define STRING_5_PERCENT              "5%"
    #define STRING_10_PERCENT             "10%"
    #define STRING_PERCENT_VALUE          "%d%%"

    #define STRING_5_SECONDS              "5 Sec."
    #define STRING_10_SECONDS             "10 Sec."
    #define STRING_30_SECONDS             "30 Sec."
    #define STRING_60_SECONDS             "1 Min."
    #define STRING_120_SECONDS            "2 Min."
    #define STRING_300_SECONDS            "5 Min."

    // Colors
    #define STRING_WHITE                  "White"
    #define STRING_BLACK                  "Black"
    #define STRING_BLUE                   "Blue"
    #define STRING_RED                    "Red"
    #define STRING_GREEN                  "Green"
    #define STRING_CYAN                   "Cyan"
    #define STRING_YELLOW                 "Yellow"
    #define STRING_BROWN                  "Brown"
    #define STRING_GRAY                   "Gray"
    #define STRING_ORANGE                 "Orange"
    #define STRING_INDIGO                 "Indigo"
    #define STRING_VIOLET                 "Violet"
    #define STRING_MAGENTA                "Magenta"
    #define STRING_PURPLE                 "Purple"
    #define STRING_LIME                   "Lime"
    #define STRING_DARKBLUE               "DarkBlue"
    #define STRING_DARKGREEN              "DarkGreen"
    #define STRING_DARKGRAY               "DarkGray"

    // Menus
    #define STRING_HEAT                   "Heat"
    #define STRING_MOVE                   "Move"
    #define STRING_HOME                   "Home"
    #define STRING_PRINT                  "Print"
    //#define STRING_EXTRUDE                "Extrude"       //TG 2/10/21 removed for CNC

    #define STRING_SETTINGS               "Settings"
    #define STRING_SCREEN_SETTINGS        "Screen"
    #define STRING_UI_SETTINGS            "UI"
    #define STRING_SOUND                  "Sound"
    #define STRING_MARLIN_MODE_SETTINGS   "MarlinMode"
    #define STRING_MACHINE_SETTINGS       "Machine"
    #define STRING_PARAMETER_SETTINGS     "Settings"
    #define STRING_FEATURE_SETTINGS       "Feature"
    #define STRING_CONNECTION_SETTINGS    "Connection"
    #define STRING_SERIAL_PORTS           "S. Ports"
    #define STRING_BAUDRATE               "BaudRate"
    #define STRING_EEPROM_SETTINGS        "EEPROM"
    #define STRING_RGB_SETTINGS           "LED Color"
    #define STRING_RGB_OFF                "LED Off"
    #define STRING_TERMINAL               "Terminal"
    //#define STRING_LEVELING               "Leveling"              //TG 7/17/21 removed for CNC
    //#define STRING_POINT_1                "Point 1"               //TG 7/17/21 removed for CNC
    //#define STRING_POINT_2                "Point 2"               //TG 7/17/21 removed for CNC
    //#define STRING_POINT_3                "Point 3"               //TG 7/17/21 removed for CNC
    //#define STRING_POINT_4                "Point 4"               //TG 7/17/21 removed for CNC
    //#define STRING_POINT_5                "Point 5"               //TG 7/17/21 removed for CNC
    //#define STRING_BED_LEVELING           "Bed Level"             //TG 7/17/21 removed for CNC
    #define STRING_BL_COMPLETE            "Bed Leveling Complete" //TG 7/17/21 removed for CNC
    #define STRING_BL_SMART_FILL          "Missing probe points\nhave been smart filled.\n \nNote: remember to save!"
    #define STRING_BL_ENABLE              "BL: on"
    #define STRING_BL_DISABLE             "BL: off"
    #define STRING_ABL                    "ABL"
    #define STRING_BBL                    "BBL"
    #define STRING_UBL                    "UBL"
    #define STRING_MBL                    "MBL"
    #define STRING_MBL_SETTINGS           "Mesh Bed Leveling"
    #define STRING_ABL_SETTINGS           "Auto Bed Leveling"
    #define STRING_ABL_SETTINGS_BBL       "Bilinear Bed Leveling"
    #define STRING_ABL_SETTINGS_UBL       "Unified Bed Leveling"
    #define STRING_ABL_SETTINGS_UBL_SAVE  "Save to slot"
    #define STRING_ABL_SETTINGS_UBL_LOAD  "Load from slot"
    #define STRING_ABL_SLOT0              "Slot 0"
    #define STRING_ABL_SLOT1              "Slot 1"
    #define STRING_ABL_SLOT2              "Slot 2"
    #define STRING_ABL_SLOT3              "Slot 3"
    #define STRING_ABL_SLOT_EEPROM        "Save the mesh to EEPROM to load after reboot?"
    #define STRING_ABL_Z                  "Z Fade"
    #define STRING_LEVEL_CORNER           "L corner"
    #define STRING_LEVEL_CORNER_INFO      "Edge distance is below probe offset X/Y and some target points could be not reachable.\nDo you want to use a safe edge distance?"
    #define STRING_P_OFFSET               "P Offset"
    #define STRING_H_OFFSET               "H Offset"
    #define STRING_DISTANCE               "Distance"
    #define STRING_SHIM                   "Shim"
    #define STRING_LOAD_UNLOAD            "Load/Unload"
    #define STRING_LOAD_UNLOAD_SHORT      "(Un)Load"
    #define STRING_TOUCHSCREEN_ADJUST     "TSC Adjust"
    #define STRING_MORE                   "More"
    #define STRING_MORE2                  "More2"                       //TG 10/12/22 added
    #define STRING_SCREEN_INFO            "Info"
    #define STRING_BG_COLOR               "Background color"
    #define STRING_FONT_COLOR             "Font color"
    #define STRING_PERCENTAGE             "Percentage"
    #define STRING_PERCENTAGE_SPEED       "Speed"
    //#define STRING_PERCENTAGE_FLOW        "Flow"                      //TG 10/12/22 removed LABEL_PERCENTAGE_FLOW
    #define STRING_BABYSTEP               "BabyStep"
    #define STRING_X_INC                  "X+"
    #define STRING_Y_INC                  "Y+"
    #define STRING_Z_INC                  "Z+"
    #define STRING_X_DEC                  "X-"
    #define STRING_Y_DEC                  "Y-"
    #define STRING_Z_DEC                  "Z-"
    #define STRING_X                      "X"
    #define STRING_Y                      "Y"
    #define STRING_Z                      "Z"
    #define STRING_ADJUST_TITLE           "Touch Screen Calibration"
    #define STRING_ADJUST_INFO            "Please click on the red dot"
    #define STRING_ADJUST_OK              "Adjustment success"
    #define STRING_ADJUST_FAILED          "Adjustment failed, please try again."
    #define STRING_UNIFIEDMOVE            "Movement"
    #define STRING_UNIFIEDHEAT            "Heat/Fan"
    #define STRING_TOUCH_TO_EXIT          "Touch anywhere to exit"
    #define STRING_MAINMENU               "Menu"
    #define STRING_TUNING                 "Tuning"
    #define STRING_PID                    "PID"                       //TG 7/17/21 removed for CNC, //TG 2/28/23 added back
    //#define STRING_PID_TITLE              "PID autotune"              //TG 7/17/21 removed for CNC
    //#define STRING_PID_START_INFO         "PID autotune may take\nsome time to complete.\nContinue?"  //TG 7/17/21 removed for CNC
    //#define STRING_PID_START_INFO_2       "PID autotune in progress!" //TG 7/17/21 removed for CNC
    //#define STRING_PID_START_INFO_3       "Do not touch the screen until completed (green LED ON)!"   //TG 7/17/21 removed for CNC
    #define STRING_TUNE_EXTRUDER          "Tune steps"                //TG 2/10/21 removed for CNC, //TG 2/28/23 added back
    //#define STRING_TUNE_EXT_EXTRUDE_100   "Ext. 100mm"                //TG 2/10/21 removed for CNC
    //#define STRING_TUNE_EXT_TEMP          "Extruder tuning | Heat"    //TG 2/10/21 removed for CNC
    //#define STRING_TUNE_EXT_TEMPLOW       "Desired temperature too low!\nMinimum temperature: %d℃"    //TG 2/18/21 removed
    //#define STRING_TUNE_EXT_DESIREDVAL    "Temperature is not yet at the desired value"
    //#define STRING_TUNE_EXT_MARK120MM     "Mark 120 mm on your fillament\nPress '%s' when ready\nMeasure remaining length\nafter extruding"
    //#define STRING_TUNE_EXT_HEATOFF       "Turn the heater off?"
    //#define STRING_TUNE_EXT_ADJ_ESTEPS    "Adjust E-Steps"
    //#define STRING_TUNE_EXT_ESTEPS_SAVED  "New E-steps saved!\nDon't forget to\nsave it in EEPROM\nNew value: %0.2f"
    //#define STRING_TUNE_EXT_MEASURED      "Length remaining:"
    //#define STRING_TUNE_EXT_OLD_ESTEP     "Old e-steps: %0.2f"
    //#define STRING_TUNE_EXT_NEW_ESTEP     "New e-steps: %0.2f"
    #define STRING_MPC                    "MPC"
    #define STRING_MPC_TITLE              "MPC autotune"
    #define STRING_TUNE_START_INFO        "Autotune may take some time to complete.\nContinue?"
    #define STRING_NOTIFICATIONS          "Notifications"
    //#define STRING_MESH_EDITOR            "Mesh edit"               //TG 7/17/21 removed for CNC
    //#define STRING_MESH_TUNER             "Mesh tuner"              //TG 7/17/21 removed for CNC
    #define STRING_CASE_LIGHT             "Case light"
    #define STRING_LOAD_STARTED           "Loading filament.\nPlease wait for the process\nto complete."
    #define STRING_UNLOAD_STARTED         "Unloading filament.\nPlease wait for the process\nto complete."
    //#define STRING_HEATERS_ON             "Some heaters are still ON.\nDo you want to stop them?"  //TG removed 8/22/21
    #define STRING_PRINT_FINISHED         "   Print\n   finished"
    #define STRING_MAIN_SCREEN            "Main"
    #define STRING_PREVIOUS_PRINT_DATA    "Brief"
    #define STRING_FILE_NAME              "File: "
    #define STRING_PRINT_TIME             "Print Time: "
    #define STRING_FILAMENT_LENGTH        "Filament length: %1.2fm"   //TG 2/18/21 removed
    #define STRING_FILAMENT_WEIGHT        "Filament weight: %1.2fg"   //TG 2/18/21 removed
    #define STRING_FILAMENT_COST          "Filament cost: %1.2f"      //TG 2/18/21 removed
    #define STRING_NO_FILAMENT_STATS      "No filament statistics."
    #define STRING_CLICK_FOR_MORE         "Click for more."
    //#define STRING_EXT_TEMPLOW            "Hotend temperature is below\nminimum temperature (%d℃)."  //TG 2/18/21 removed
    #define STRING_HEAT_HOTEND            "Heat hotend to %d℃?"
    #define STRING_DESIRED_TEMPLOW        "Hotend temperature is below desired temperature (%d℃)."
    #define STRING_WAIT_HEAT_UP           "Wait for it to heat up."
    #define STRING_Z_ALIGN                "Z Align"
    #define STRING_MACROS                 "Macros"
    #define STRING_MESH_VALID             "Mesh Validation"
    #define STRING_CONNECT_PROBE          "Connect probe before starting the process. Make sure to disconnect it afterwards."
    #define STRING_DISCONNECT_PROBE       "Make sure you have disconnected probe before using this feature."
    #define STRING_CALIBRATION            "Calibrate"
    #define STRING_TGMENU                 "TG Menu"
    #define STRING_M503                   "Read EEPROM"
    #define STRING_VACUUM                 "Vacuum"                  //TG 1/12/20 new
    #define STRING_SPINDLE                "Spindle"                 //TG 1/12/20 new
    #define STRING_LASER                  "Laser"                   //TG 1/12/20 new
    #define STRING_ZERO_X                 "Zero X"                  //TG 1/12/20 new
    #define STRING_ZERO_Y                 "Zero Y"                  //TG 1/12/20 new
    #define STRING_ZERO_Z                 "Zero Z"                  //TG 1/12/20 new
    #define STRING_XY                     "XY"                      //TG 1/12/20 new
    #define STRING_CUT                    "Cut"                     //TG 1/12/20 new
    #define STRING_LASER_2                "Laser 2%"                //TG 1/12/20 new
    #define STRING_LASER_100              "Laser 100%"              //TG 1/12/20 new
    #define STRING_LASER_OFF              "Laser Off"               //TG 1/12/20 new
    #define STRING_SPINDLE_ROTATION       "Spindle Rotation"        //TG 1/12/20 new
    #define STRING_LASER_MODE             "Laser Mode"              //TG 1/12/20 new
    #define STRING_CW                     "CW"                      //TG 1/12/20 new
    #define STRING_CCW                    "CCW"                     //TG 1/12/20 new
    #define STRING_SPINDLE_RMAX           "Spindle RPM Maximum"     //TG 2/5/21 new
    #define STRING_SPINDLE_PMAX           "Spindle PWM Max"         //TG 2/5/21 new
    #define STRING_LCD_POWER_UNIT         "LCD Spindle units"       //TG 2/4/21 new
    #define STRING_PCT                    "%"                       //TG 2/5/21 new
    #define STRING_RPM                    "RPM"                     //TG 2/5/21 new
    #define STRING_500_RPM                "500_RPM"                 //TG 2/5/21 new
    #define STRING_1000_RPM               "1000_RPM"                //TG 2/5/21 new
    #define STRING_5000_RPM               "5000_RPM"                //TG 2/5/21 new
    #define STRING_10000_RPM              "10000_RPM"               //TG 2/5/21 new
    #define STRING_SSTART                 "Start"                   //TG 2/5/21 new
    #define STRING_SSTOP                  "Stop"                    //TG 2/5/21 new
    #define STRING_CUTTER_POWER_UNIT      "Marlin Cutter Units"     //TG 2/14/21 new
    #define STRING_PWM                    "PWM"                     //TG 2/14/21 new
    #define STRING_AUTO_MODE              "Auto Mode"               //TG 2/18/21 new
    #define STRING_AUTO_ON                "Auto ON"                 //TG 2/18/21 new
    #define STRING_REMOVED                "Removed"                 //TG 2/18/21 new
    #define STRING_INVERT_XAXIS           "Invert X Axis"
    #define STRING_INVERT_YAXIS           "Invert Y Axis"
    #define STRING_INVERT_ZAXIS           "Invert Z Axis"
    #define STRING_SPINDLE_USE_PID        "Spindle use PID"         //TG 9/27/21 new
    //#define STRING_Z_0                    "Z to 0mm"
    //#define STRING_Z_300                  "Z to 300mm"
    #define STRING_KP                     "Kp"                      //TG 7/17/22 new
    #define STRING_KI                     "Ki"                      //TG 7/17/22 new
    #define STRING_KD                     "Kd"                      //TG 7/17/22 new
    #define STRING_PID_ON                 "PID On"                  //TG 7/17/22 new
    #define STRING_PID_OFF                "PID Off"                 //TG 7/17/22 new
    #define STRING_AVR_RST                "Restart AVR"             //TG 7/17/22 new
    #define STRING_AVR_DSP0               "AVR LCD pg 0"            //TG 7/17/22 new
    #define STRING_AVR_DSP1               "AVR LCD pg 1"            //TG 7/17/22 new
    #define STRING_AVR_DSP2               "AVR LCD pg 2"            //TG 7/17/22 new
    #define STRING_AVR_CTL                "AVR Control"             //TG 7/17/22 new
    #define STRING_MARLIN_INT             "Marlin_Int"              //TG 7/17/22 new
    #define STRING_PIDRUN_INT             "PIDRUN_Int"              //TG 7/17/22 new
    #define STRING_CUST_SPEED             "Custom"                  //TG 7/17/22 new
    #define STRING_SHOULD_M0_PAUSE        "Should M0 Pause"         //TG 7/17/22 new
    #define STRING_PROBE_STOCK            "Probe Stk"               //TG 10/12/22 new
    #define STRING_VFD_CONTROL            "VFD Control"             //TG 12/26/22 new
    #define STRING_DISABLED_FOR_CNC       " disabled for CNC!"      //TG  2/28/23 new

#endif
