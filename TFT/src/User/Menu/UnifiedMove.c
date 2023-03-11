//TG MODIFIED*****
#include "UnifiedMove.h"
#include "includes.h"

#if DELTA_PROBE_TYPE != 0  // if Delta printer
  void deltaCalibration(void)
  {
    mustStoreCmd("G33\n");
  }
#endif
//TG - this only gets called from menuMain, which would only be active in 
//Classic Mode(see selectMode.c)  THIS WILL NEVER BE USED IN CNC VERSION!
void menuUnifiedMove(void)
{
  MENUITEMS UnifiedMoveItems = {
    // title
    LABEL_UNIFIEDMOVE,
    // icon                          label
    {
      {ICON_HOME,                    LABEL_HOME},
      {ICON_MOVE,                    LABEL_MOVE},
      {ICON_REMOVED,				         LABEL_REMOVED},            //TG 2/10/21 was EXTRUDE, removed for CNC
	    {ICON_DISABLE_STEPPERS,        LABEL_DISABLE_STEPPERS},
      {ICON_BABYSTEP,                LABEL_BABYSTEP},
      {ICON_REMOVED,				         LABEL_REMOVED},           //TG 7/17/22 removed, was MANUAL_LEVEL-LEVELING
      {ICON_NULL,                    LABEL_NULL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  //if (infoMachineSettings.leveling != BL_DISABLED)      //TG 7/17/22 removed next 4 lines since Leveling.c removed
  //{
  //  UnifiedMoveItems.items[6].icon = ICON_LEVELING;
  //  UnifiedMoveItems.items[6].label.index = LABEL_BED_LEVELING;
  //}

  menuDrawPage(&UnifiedMoveItems);

  while (MENU_IS(menuUnifiedMove))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        OPEN_MENU(menuHome);
        break;

      case KEY_ICON_1:
        OPEN_MENU(menuMove);
        break;

      case KEY_ICON_2:
        //OPEN_MENU(menuExtrude); //TG removed for CNC
        break;

      case KEY_ICON_3:
        storeCmd("M84\n");
        break;

      case KEY_ICON_4:
        OPEN_MENU(menuBabystep);
        break;

      case KEY_ICON_5:
      //  OPEN_MENU(menuManualLeveling);     //TG 7/17/22 menuManualLeveling.c was removed
        break;

      case KEY_ICON_6:
      //  if (infoMachineSettings.leveling != BL_DISABLED)        //TG 7/17/22 menuBedLeveling.c was removed
      //    OPEN_MENU(menuBedLeveling);
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
