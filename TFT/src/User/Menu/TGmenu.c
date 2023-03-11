
#include "TGmenu.h"
#include "SendGcode.h"
#include "includes.h"
  
//extern char gcodeBuf[CMD_MAX_CHAR];
//char* gcodeBufPtr = &gcodeBuf[0];

void menuTGmenu(void)
{
  //TG examples of variable usage
/*
static uint8_t ublSlot;
static bool ublIsSaving = true;
static bool ublSlotSaved = false;
*/

  MENUITEMS TGmenuItems = {
    // title
    LABEL_TGMENU,
    // icon                         label
    {{ICON_Z_0,                     LABEL_Z_0},
     {ICON_Z_300,                   LABEL_Z_300},
     {ICON_M503,                    LABEL_M503},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK}}
  };

  KEY_VALUES key_num = KEY_IDLE;

 
 //TG this looks like an example of how to modify an existing menu on the fly based on
 //   information or other variables elsewhere in code
 /* switch (infoMachineSettings.leveling)
  {
    case BL_BBL:
      autoLevelingItems.title.index = LABEL_ABL_SETTINGS_BBL;
      break;

    case BL_UBL:
      autoLevelingItems.title.index = LABEL_ABL_SETTINGS_UBL;
      autoLevelingItems.items[1].icon = ICON_EEPROM_SAVE;
      autoLevelingItems.items[1].label.index = LABEL_SAVE;
      autoLevelingItems.items[2].icon = ICON_EEPROM_RESTORE;
      autoLevelingItems.items[2].label.index = LABEL_LOAD;
      break;

    default:
      break;
  }
*/


  menuDrawPage(&TGmenuItems);

  while (infoMenu.menu[infoMenu.cur] == menuTGmenu)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        storeCmd("G1 Z0\n");
        break;

      case KEY_ICON_1:
        storeCmd("G1 Z300\n");
        break;

      case KEY_ICON_2:
      /*
      gcodeBuf[0]=(char)'M';
      gcodeBuf[1]=(char)'5';
      gcodeBuf[2]=(char)'0';
      gcodeBuf[3]=(char)'3';
      gcodeBuf[4]=(char)'/n';
      gcodeBuf[5]=0;
      storeCmd(gcodeBuf);
      */


        storeCmd("M503\n");
        //gcodeBuf[0] = 0;
        infoMenu.menu[++infoMenu.cur] = menuTerminal;
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}
