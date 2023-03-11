#include "main.h"
#include "includes.h"

MENU infoMenu;     // Menu structure
HOST infoHost;     // Information interaction with Marlin
CLOCKS mcuClocks;  // System clocks: SYSCLK, AHB, APB1, APB2, APB1_Timer, APB2_Timer2

int main(void)
{ //TG 12/23/22 tried this to stop GDB 'jtag status contains invalid mode value - communication failure' errors
  //DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY, ENABLE); 
  SystemClockInit();

  SCB->VTOR = VECT_TAB_FLASH;

  HW_Init();
  
  #ifdef USING_AVR_TRIAC_CONTROLLER
    //TG 8/5/22 try to reset AVR controller to sync to current stored presets
    mustStoreCmd("%s \n", "M7980");         //TG send cmd to Marlin to request AVR reset
    TFTtoMARLIN_wait(comp_7980);            //wait for command acknoledgement
  #endif

  #if defined(SERIAL_DEBUG_PORT) && defined(SERIAL_DEBUG_ENABLED)
    dbg_print("Main Startup: Generic debug output is enabled.\n");
  #endif

  for(; ;)                                 // infinite loop on whatever menu is currently at the top
  {                                       // of infoMenu.menu stack, after restart this will be menuStatus
    (*infoMenu.menu[infoMenu.cur])();     
  }                                     
}