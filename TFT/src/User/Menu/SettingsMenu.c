//TG MODIFIED*****
#include "SettingsMenu.h"
#include "includes.h"

const MENUITEMS settingsItems = {
  // title
  LABEL_SETTINGS,
  // icon                          label
  {
    {ICON_SCREEN_SETTINGS,         LABEL_SCREEN_SETTINGS},
    {ICON_MACHINE_SETTINGS,        LABEL_MACHINE_SETTINGS},
    {ICON_FEATURE_SETTINGS,        LABEL_FEATURE_SETTINGS},
    {ICON_SCREEN_INFO,             LABEL_SCREEN_INFO},
    {ICON_CONNECTION_SETTINGS,     LABEL_CONNECTION_SETTINGS},
    #ifdef USING_AVR_TRIAC_CONTROLLER
    {ICON_AVR_CTL,                 LABEL_AVR_CTL},
    #endif
    #ifdef USING_VFD_CONTROLLER
    {ICON_VFD_CONTROL,             LABEL_VFD_CONTROL},
    #else
    {ICON_NULL,              LABEL_NULL},
    #endif
    {ICON_NULL,                    LABEL_NULL},
    {ICON_BACK,                    LABEL_BACK},
  }
};

// const GUI_POINT clocks[] = {
//   {0 * LCD_WIDTH / 3, 0 * BYTE_HEIGHT},
//   {1 * LCD_WIDTH / 3, 0 * BYTE_HEIGHT},
//   {2 * LCD_WIDTH / 3, 0 * BYTE_HEIGHT},
//   {0 * LCD_WIDTH / 3, 1 * BYTE_HEIGHT},
//   {1 * LCD_WIDTH / 3, 1 * BYTE_HEIGHT},
//   {2 * LCD_WIDTH / 3, 1 * BYTE_HEIGHT},};

static uint8_t firmare_name[64] = "Unknown system";  // Marlin firmware version
uint8_t machine_type[64] = "3D Printer";  // Marlin machine type
uint8_t access_point[64] = "Connecting...";  // Access point for RepRapFirmware
uint8_t ip_address[20] = "0.0.0.0";  // IP address for RepRapFirmware
uint8_t Marlin_folder_name[64] = {};

void infoSetFirmwareName(uint8_t *name, uint8_t name_len)
{
  if (name_len > sizeof(firmare_name) - 1)
    name_len = sizeof(firmare_name) - 1;
  uint8_t i;
  for (i = 0; i < name_len; i++)
  {
    firmare_name[i] = name[i];
  }
  firmare_name[i] = 0;
}

//TG 2/13/23 added this to capture Marlin Source Code URL for info screen display
void infoSetSourceCodeURL(uint8_t *name, uint8_t name_len)
{
  if (name_len > sizeof(Marlin_folder_name) - 1)
    name_len = sizeof(Marlin_folder_name) - 1;
  uint8_t i;
  for (i = 0; i < name_len; i++)
  {
    Marlin_folder_name[i] = name[i];
  }
  Marlin_folder_name[i] = 0;
}

void infoSetMachineType(uint8_t *machine, uint8_t type_len)
{
  if (type_len > sizeof(machine_type) - 1)
    type_len = sizeof(machine_type) - 1;
  uint8_t i;
  for (i = 0; i < type_len; i++)
  {
    machine_type[i] = machine[i];
  }
  machine_type[i] = 0;
  statusScreen_setReady();
}

void infoSetAccessPoint(uint8_t *ssid, uint8_t ssid_len)
{
  if (ssid_len > sizeof(access_point) - 1)
    ssid_len = sizeof(access_point) - 1;
  uint8_t i;
  for (i = 0; i < ssid_len; i++)
  {
    access_point[i] = ssid[i];
  }
  access_point[i] = 0;
}

void infoSetIPAddress(uint8_t *ip, uint8_t ip_len)
{
  if (ip_len > sizeof(ip_address) - 1)
    ip_len = sizeof(ip_address) - 1;
  uint8_t i;
  for (i = 0; i < ip_len; i++)
  {
    ip_address[i] = ip[i];
  }
  ip_address[i] = 0;
}

// Version infomation
void menuInfo(void)
{
  char buf[128];

  const char *const msrcfile = (char *const)Marlin_folder_name;  //TG 2/12/23 added to display Marlin source folder name
  const char *const srcfile = SRC_FILE_NAME;  //TG 2/12/23 added to display source folder name
  const char *const hardware = HARDWARE_MANUFACTURER HARDWARE_VERSION;

  GUI_Clear(infoSettings.bg_color);
  GUI_SetColor(GRAY);

  // sprintf(buf, "SYS:%dMhz", mcuClocks.rccClocks.SYSCLK_Frequency / 1000000);
  // GUI_DispString(clocks[0].x, clocks[0].y, (uint8_t *)buf);

  // sprintf(buf, "APB1:%dMhz", mcuClocks.rccClocks.PCLK1_Frequency / 1000000);
  // GUI_DispString(clocks[1].x, clocks[1].y, (uint8_t *)buf);

  // sprintf(buf, "P1Tim:%dMhz", mcuClocks.PCLK1_Timer_Frequency / 1000000);
  // GUI_DispString(clocks[2].x, clocks[2].y, (uint8_t *)buf);

  // sprintf(buf, "AHB:%dMhz", mcuClocks.rccClocks.HCLK_Frequency / 1000000);
  // GUI_DispString(clocks[3].x, clocks[3].y, (uint8_t *)buf);

  // sprintf(buf, "APB2:%dMhz", mcuClocks.rccClocks.PCLK2_Frequency / 1000000);
  // GUI_DispString(clocks[4].x, clocks[4].y, (uint8_t *)buf);

  // sprintf(buf, "P2Tim:%dMhz", mcuClocks.PCLK2_Timer_Frequency / 1000000);
  // GUI_DispString(clocks[5].x, clocks[5].y, (uint8_t *)buf);

  // GUI_HLine(0, clocks[5].y + BYTE_HEIGHT, LCD_WIDTH);

  const uint16_t top_y = 0; //(LCD_HEIGHT - (7 * BYTE_HEIGHT)) / 2;  // 8 firmware info lines + 1 SPI flash info line
  const uint16_t start_x = strlen("SrcFolder:") * BYTE_WIDTH;
  const GUI_RECT version[9] = { //TG 2/12/23 was 7 added one more line
    {start_x, top_y + 0*BYTE_HEIGHT, LCD_WIDTH, top_y + 1*BYTE_HEIGHT},    // line 1
    {start_x, top_y + 1*BYTE_HEIGHT, LCD_WIDTH, top_y + 3*BYTE_HEIGHT},    // lines 2-3
    {start_x, top_y + 3*BYTE_HEIGHT, LCD_WIDTH, top_y + 5*BYTE_HEIGHT},    // lines 4-5 for Src Folder
    {start_x, top_y + 5*BYTE_HEIGHT, LCD_WIDTH, top_y + 6*BYTE_HEIGHT},    // line 6
    {start_x, top_y + 6*BYTE_HEIGHT, LCD_WIDTH, top_y + 7*BYTE_HEIGHT},    // line 7
    {start_x, top_y + 7*BYTE_HEIGHT, LCD_WIDTH, top_y + 8*BYTE_HEIGHT},    // line 8
    {start_x, top_y + 8*BYTE_HEIGHT, LCD_WIDTH, top_y + 10*BYTE_HEIGHT},   // line 9-10 for Src Folder
    {start_x, top_y + 10*BYTE_HEIGHT, LCD_WIDTH, top_y + 11*BYTE_HEIGHT},  //TG 2/12/23 line 11 added one line
    {start_x, top_y + 11*BYTE_HEIGHT, LCD_WIDTH, top_y + 12*BYTE_HEIGHT},  //TG 2/12/23 line 12 added one line
   };
  //draw titles
  GUI_DispString(0, version[0].y0, (uint8_t *)"Machine  :");    // lines 1 
  GUI_DispString(0, version[1].y0, (uint8_t *)"System   :");    // lines 2-3
  GUI_DispString(0, version[2].y0, (uint8_t *)"SrcFolder:");    // lines 4-5
      
  GUI_DispString(0, version[3].y0, (uint8_t *)"Display    :");  // line 6
  GUI_DispString(0, version[4].y0, (uint8_t *)"Firmware :");    // line 7
  GUI_DispString(0, version[5].y0, (uint8_t *)"SPIFlash :");    // line 8
  GUI_DispString(0, version[6].y0, (uint8_t *)"SrcFolder:");    // lines 9-10
  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
  {
    GUI_DispString(0, version[7].y0, (uint8_t *)"WIFI    :");   //TG was 5
    GUI_DispString(0, version[8].y0, (uint8_t *)"IP      :");   //TG was 6
  }
  

  // draw info
  GUI_SetColor(0xDB40);
  GUI_DispStringInPrectEOL(&version[0], machine_type);           //TG gets set by receipt of M115 in parseAck.c
  GUI_DispStringInPrect(&version[1], firmare_name);              //TG gets set by receipt of M115 in parseAck.c
  GUI_DispStringInPrect(&version[2], (uint8_t *)msrcfile);       //TG gets set by receipt of M115 in parseAck.c
  GUI_DispStringInPrectEOL(&version[3], (uint8_t *)hardware);
  // firmware info
  sprintf(buf, "V"STRINGIFY(SOFTWARE_VERSION) " " __DATE__ " in %dMhz", mcuClocks.rccClocks.SYSCLK_Frequency / 1000000);
  GUI_DispStringInPrectEOL(&version[4], (uint8_t *)buf);
  // spi flash info
  float usedMB = (float)FLASH_USED/1048576;
  sprintf(buf, "Used %.2f%% (%.2fMB/%uMB)", flashUsedPercentage(), usedMB, (W25Qxx_ReadCapacity() / 1048576));
  GUI_DispStringInPrectEOL(&version[5], (uint8_t *)buf);

  GUI_DispStringInPrect(&version[6], (uint8_t *)srcfile);
  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
  {
    GUI_DispStringInPrectEOL(&version[7], (uint8_t *)access_point); //TG was 5
    GUI_DispStringInPrectEOL(&version[8], (uint8_t *)ip_address);   //TG was 6
  }

  GUI_SetColor(GRAY);

  GUI_HLine(0, version[3].y0 -1, LCD_WIDTH);
  GUI_HLine(0, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH);

  GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));

  while (!isPress()) loopBackEnd();
  BUZZER_PLAY(SOUND_KEYPRESS);
  while (isPress()) loopBackEnd();

  GUI_RestoreColorDefault();
  CLOSE_MENU();
}

void menuSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&settingsItems);

  while (MENU_IS(menuSettings))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        OPEN_MENU(menuScreenSettings);
        break;

      case KEY_ICON_1:
        mustStoreCmd("M503 S0\n");
        OPEN_MENU(menuMachineSettings);
        break;

      case KEY_ICON_2:
        OPEN_MENU(menuFeatureSettings);
        break;

      case KEY_ICON_3:
        OPEN_MENU(menuInfo);
        break;

      case KEY_ICON_4:
        OPEN_MENU(menuConnectionSettings);
        break;

      case KEY_ICON_5:
        #ifdef USING_AVR_TRIAC_CONTROLLER
          OPEN_MENU(menuTriac);
        #endif
        #ifdef USING_VFD_CONTROLLER
          OPEN_MENU(menuVFD);    //TG 12/23/22 added
        #endif
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
