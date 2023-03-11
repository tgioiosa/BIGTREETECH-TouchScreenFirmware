//TG MODIFIED*****
#include "UnifiedHeat.h"
#include "includes.h"
#ifndef NO_UNIFIED_HEAT_MENU  //TG 3/2/23 added global flag to exclude this code

const MENUITEMS UnifiedHeatItems = {
  // title
  LABEL_UNIFIEDHEAT,
  // icon                          label
  {
    {ICON_REMOVED,                 LABEL_REMOVED},      //TG 7/17/22 was HEAT-PREHEAT    
    {ICON_REMOVED,                 LABEL_REMOVED},      //TG 7/17/22 was HEAT-HEAT
    {ICON_FAN,                     LABEL_FAN},
    {ICON_NULL,              LABEL_NULL},
    {ICON_NULL,              LABEL_NULL},
    {ICON_REMOVED,                 LABEL_REMOVED},       //TG 2/18/21 was COOLDOWN
    {ICON_NULL,              LABEL_NULL},
    {ICON_BACK,                    LABEL_BACK},
  }
};

void menuUnifiedHeat(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&UnifiedHeatItems);

  while (MENU_IS(menuUnifiedHeat))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        //OPEN_MENU(menuPreheat);  //TG 7/17/22 removed
        break;

      case KEY_ICON_1:
        OPEN_MENU(menuSpindle);
        break;

      case KEY_ICON_2:
        OPEN_MENU(menuFan);
        break;

      case KEY_ICON_5:
        //heatCoolDown();                               //TG 7/17/22 removed
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

#endif
