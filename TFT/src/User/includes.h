//TG MODIFIED*****
#ifndef _INCLUDES_H_
#define _INCLUDES_H_

// global includes (always first)
#include "variants.h"
#include "main.h"

// standard libs
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// User
#include "delay.h"
#include "my_misc.h"
#include "os_timer.h"
#include "SanityCheck.h"

// User/HAL/stm32f10x or // HAL/stm32f2_f4xx
#include "lcd_dma.h"
#include "lcd.h"
#include "Serial.h"
#include "spi_slave.h"  // it uses infoSettings. HAL should be independent by that!
#include "spi.h"
#include "timer_pwm.h"
#include "uart.h"

// User/HAL/USB
#include "usbh_msc_core.h"  // HAL/STM32_USB_HOST_Library/Class/MSC/inc
#include "usbh_core.h"      // HAL/STM32_USB_HOST_Library/Core/inc
#include "usbh_usr.h"       // HAL/STM32_USB_HOST_Library/Usr/inc

// User/HAL
#include "buzzer.h"
#include "Knob_LED.h"
#include "LCD_Encoder.h"
#include "LCD_Init.h"
#include "sd.h"
#include "sw_spi.h"
#include "w25qxx.h"
#include "xpt2046.h"

// User/Fatfs
#include "ff.h"
#include "myfatfs.h"

// User/API
#include "Gcode/gcode.h"
#include "Gcode/mygcodefs.h"
#include "printf/printf.h"
#include "Vfs/vfs.h"

// User/API/Language
#include "Language.h"
#include "utf8_decode.h"

// User/API/UI
#include "CharIcon.h"
#include "GUI.h"
#include "HD44780_Emulator.h"  // it uses infoSettings
#include "ListItem.h"          // it uses infoSettings
#include "ListManager.h"
#include "Numpad.h"            // it uses infoSettings
#include "ST7920_Emulator.h"   // it uses infoSettings
#include "TouchProcess.h"
#include "ui_draw.h"

// User/API
#include "AddonHardware.h"
#include "BabystepControl.h"
#include "boot.h"
#include "BuzzerControl.h"
#include "CaseLightControl.h"
#include "comment.h"
#include "config.h"
#include "coordinate.h"
#include "debug.h"
#include "FanControl.h"
#include "FlashStore.h"
#include "HomeOffsetControl.h"
#include "HW_Init.h"
#include "interfaceCmd.h"
#include "LCD_Colors.h"
#include "LCD_Dimming.h"
#include "LED_Colors.h"
#include "LED_Event.h"
#include "LevelingControl.h"
#include "MachineParameters.h"
#include "menu.h"
#include "ModeSwitching.h"
#include "Notification.h"
#include "parseACK.h"
#include "PowerFailed.h"
#include "Printing.h"
#include "ProbeHeightControl.h"
#include "ProbeOffsetControl.h"
#ifndef NO_RRFSupport   //TG 3/2/23 added global switch for RepRap support
  #include "RRFStatusControl.h"
#endif
#include "ScreenShot.h"
#include "SerialConnection.h"
#include "Settings.h"
#include "SpeedControl.h"
#include "Temperature.h"
#include "Touch_Encoder.h"

// User/Menu
#include "ABL.h"
#include "Babystep.h"
//#include "BedLeveling.h"        //TG 7/17/22  removing BedLeveling module
//#include "BedLevelingLayer2.h"  //TG 7/17/22  removing BedLeveling module
#include "BLTouch.h"
#include "CaseLight.h"
#include "common.h"
#include "ConnectionSettings.h"
//#include "Extrude.h"            //TG 2/8/21  removing Extrude module
#include "Fan.h"
#include "FeatureSettings.h"
//#include "Heat.h"					   //TG 1/16/20  removed heat module
#include "Home.h"
#include "LEDColor.h"
//#include "LevelCorner.h"        //TG 7/17/22  removing LevelCorner module
//#include "Leveling.h"           //TG 7/17/22  removing Leveling module    
#include "LoadUnload.h"
#include "MachineSettings.h"
#include "MainPage.h"
#include "MarlinMode.h"
#include "MBL.h"
//#include "MeshEditor.h"       //TG 7/17/22  removing MeshEditor module
//#include "MeshTuner.h"        //TG 7/17/22  removing MeshTuner module
#include "MeshValid.h"
#include "More.h"
#include "Move.h"
#include "NotificationMenu.h"
#include "ParameterSettings.h"
#include "PersistentInfo.h"
//#include "MPC.h"              //TG 2/14/21 removed for CNC
//#include "Pid.h"              //TG 2/14/21 removed for CNC
#include "Popup.h"
#include "PreheatMenu.h"
#include "Print.h"
#include "PrintingMenu.h"
#include "PrintRestore.h"
#ifndef NO_RRFSupport   //TG 3/2/23 added global switch for RepRap support
  #include "RRFMacros.h"
#endif
#include "ScreenSettings.h"
#include "SelectMode.h"
#include "SettingsMenu.h"
#include "Speed.h"
#include "StatusScreen.h"
#include "Terminal.h"
#include "Touchmi.h"
//#include "TuneExtruder.h"     //TG 2/10/21 removed for CNC
#include "Tuning.h"
#include "UnifiedHeat.h"
#include "UnifiedMove.h"
#include "ZOffset.h"

//==========================================================================
// Custom TG added items
//==========================================================================
#include "Spindle.h"                //TG 1/16/20 replaces Heat.h
#include "Laser.h"                  //TG 1/12/20 new
#include "Spindle.h"                //TG 2/4/21 new
//#include "SpindleSpeed.h"         //TG 8/31/21 not needed unless trying to use spindle on fan PWM
#ifdef USING_AVR_TRIAC_CONTROLLER
  #include "avrTriac.h"             //TG 7/17/22 new
  #include "ProbeStock.h"           //TG 12/26/22 new
#endif
#ifdef USING_VFD_CONTROLLER
  #include "vfd.h"                  //TG 12/23/22 new
  #include "ProbeStock.h"           //TG 12/26/22 new
#endif
#include "Vacuum.h"                 //TG 1/12/20 new
#include "delay.h"

// other possibly needed includes not in original file
#include "CircularQueue.h"
#include "coordinate.h"
#include "Printing.h"
#include "ListItem.h"
#include "PowerFailed.h"

//TG 12/26/22 borrowed these from Marlin
// Use NUM_ARGS(__VA_ARGS__) to get the number of variadic arguments

//
// Primitives supporting precompiler REPEAT
//
#define FIRST(a,...)     a
#define SECOND(a,b,...)  b
#define THIRD(a,b,c,...) c
// Macros to support option testing
#define _CAT(a,V...) a##V
#define CAT(a,V...) _CAT(a,V)
#define IS_PROBE(V...) SECOND(V, 0)     // Get the second item passed, or 0
#define _ISENA_     ~,1
#define _ISENA_1    ~,1
#define _ISENA_0x1  ~,1
#define _ISENA_true ~,1
#define _ISENA(V...) IS_PROBE(V)
#define IS_PROBE(V...) SECOND(V, 0)     // Get the second item passed, or 0
#define PROBE() ~, 1                    // Second item will be 1 if this is passed
#define _NOT_0 PROBE()
#define NOT(x) IS_PROBE(_CAT(_NOT_, x)) // NOT('0') gets '1'. Anything else gets '0'.
#define _BOOL(x) NOT(NOT(x))            // NOT('0') gets '0'. Anything else gets '1'.
#define _ENA_1(O)           _ISENA(CAT(_IS,CAT(ENA_, O)))
#define _DIS_1(O)           NOT(_ENA_1(O))
#define COUNT_ENABLED(V...) DO(ENA,+,V)
#define _NUM_ARGS(_,n,m,l,k,j,i,h,g,f,e,d,c,b,a,Z,Y,X,W,V,U,T,S,R,Q,P,O,N,M,L,K,J,I,H,G,F,E,D,C,B,A,OUT,...) OUT
#define NUM_ARGS(V...) _NUM_ARGS(0,V,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
// Macros to chain up to 5 conditions
#define _DO_1(W,C,A)       (_##W##_1(A))
#define _DO_2(W,C,A,B)     (_##W##_1(A) C _##W##_1(B))
#define _DO_3(W,C,A,V...)  (_##W##_1(A) C _DO_2(W,C,V))
#define _DO_4(W,C,A,V...)  (_##W##_1(A) C _DO_3(W,C,V))
#define _DO_5(W,C,A,V...)  (_##W##_1(A) C _DO_4(W,C,V))
#define __DO_N(W,C,N,V...) _DO_##N(W,C,V)
#define _DO_N(W,C,N,V...)  __DO_N(W,C,N,V)
#define DO(W,C,V...)       (_DO_N(W,C,NUM_ARGS(V),V))
#define M_ENABLED(V...)       DO(ENA,&&,V)
#define M_DISABLED(V...)      DO(DIS,&&,V)
#define M_ANY(V...)          !M_DISABLED(V)

#endif
