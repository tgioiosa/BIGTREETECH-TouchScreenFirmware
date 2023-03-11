#include "includes.h"

HOST   infoHost;   // Information interaction with Marlin
MENU   infoMenu;   // Menu structure
CLOCKS mcuClocks;  // system clocks: SYSCLK, AHB, APB1, APB2, APB1_Timer, APB2_Timer2

void mcu_GetClocksFreq(CLOCKS *clk)
{
  RCC_GetClocksFreq(&clk->rccClocks);
  if (clk->rccClocks.PCLK1_Frequency < clk->rccClocks.HCLK_Frequency)  // if (APBx presc = 1) x1 else x2
    clk->PCLK1_Timer_Frequency = clk->rccClocks.PCLK1_Frequency * 2;
  else
    clk->PCLK1_Timer_Frequency = clk->rccClocks.PCLK1_Frequency;

  if (clk->rccClocks.PCLK2_Frequency < clk->rccClocks.HCLK_Frequency)
    clk->PCLK2_Timer_Frequency = clk->rccClocks.PCLK2_Frequency * 2;
  else
    clk->PCLK2_Timer_Frequency = clk->rccClocks.PCLK2_Frequency;
}

void Hardware_GenericInit(void)
{
  mcu_GetClocksFreq(&mcuClocks);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  Delay_init();

  #ifdef DISABLE_JTAG
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  // disable JTAG, enable SWD
  #endif

  #ifdef DISABLE_DEBUG
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);  //disable JTAG & SWD
  #endif

  #if defined(MKS_32_V1_4) || defined (MKS_28_V1_0)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
  #endif

  XPT2046_Init();           // init Touchscreen controller
  OS_TimerInitMs();         // System clock timer, cycle 1ms, called after XPT2046_Init()
  W25Qxx_Init();            // init W25Q64 - 64Mbit flash chip
  LCD_Init();               // init LCD to rgb, set screen black, and enable backlight
  readStoredPara();         // Read settings parameters from MCU flash area 0x08004000 - 0x08004300  (768 bytes)
  LCD_RefreshDirection();   // refresh display direction after reading settings
  scanUpdates();            // scan icon, fonts and config files for updates (boot.c)
  checkflashSign();         // check font/icon/config signature in SPI flash for update if needed
  initMachineSetting();     // load default machine settings, sets infoMachineSettings.isMarlinFirmware = -1
  //readMarlinAVRInfoBlock(400); // initial read of settings from AVR Triac controller

  #ifdef LED_COLOR_PIN
    knob_LED_Init();
  #endif

  #ifdef BUZZER_PIN
    Buzzer_Config();
  #endif

  #if !defined(MKS_32_V1_4) && !defined (MKS_28_V1_0)
    //causes hang if we deinit spi1
    SD_DeInit();
  #endif

  #if LCD_ENCODER_SUPPORT
    HW_EncoderInit();
  #endif
  #if ENC_ACTIVE_SIGNAL
    HW_EncActiveSignalInit();
  #endif

  #ifdef PS_ON_PIN
    PS_ON_Init();
  #endif

  #ifdef FIL_RUNOUT_PIN
    FIL_Runout_Init();
  #endif

  #ifdef U_DISK_SUPPORT
    USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_cb);
  #endif

  if (readIsTSCExist() == false)  // Read settings parameter
  {
    TSC_Calibration();
    storePara();
  }
  else if (readIsRestored())
  {
    storePara();
  }

  #ifdef LCD_LED_PWM_CHANNEL
    Set_LCD_Brightness(LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
  #endif
  
  switchMode();
}

int main(void)
{ //TG 12/23/22 tried this to stop GDB 'jtag status contains invalid mode value - communication failure' errors
  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY, ENABLE); 
  SystemClockInit();

  SCB->VTOR = VECT_TAB_FLASH;

  Hardware_GenericInit();

  #ifdef USING_AVR_TRIAC_CONTROLLER
    //TG 8/5/22 try to reset AVR controller to sync to current stored presets
    mustStoreCmd("%s \n", "M7980");         //TG send cmd to Marlin to request AVR reset
    TFTtoMARLIN_wait(comp_7980);            //wait for command acknoledgement
  #endif

  for(;;)                                 // infinite loop on whatever menu is currently at the top
  {                                       // of infoMenu.menu stack, after restart this will be menuStatus
    (*infoMenu.menu[infoMenu.cur])();     
  }                                     
}