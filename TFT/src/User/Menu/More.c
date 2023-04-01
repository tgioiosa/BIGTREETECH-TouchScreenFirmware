//TG MODIFIED*****
#include "includes.h"
#include "More.h"

MENUITEMS moreItems = {  //TG removed const so this menu can be dynamically changed
  // title
  LABEL_MORE,
  // icon                          label
  {
	{ICON_SPINDLE,                 LABEL_SPINDLE},
	{ICON_VACUUM,                  LABEL_VACUUM},
	{ICON_FAN,                     LABEL_FAN},        //TG 2/20/21 was EXTRUDE, removed for CNC
	{ICON_PERCENTAGE,              LABEL_PERCENTAGE},
	{ICON_FEATURE_SETTINGS,        LABEL_FEATURE_SETTINGS},
	{ICON_MACHINE_SETTINGS,        LABEL_MACHINE_SETTINGS},
    #ifdef LOAD_UNLOAD_M701_M702
      {ICON_LOAD,                  LABEL_LOAD_UNLOAD_SHORT}, //TG 8/21/21 NEED TO CHECK THIS ICON!!
    #else
      {ICON_MORE,                  LABEL_MORE},
    #endif
    {ICON_BACK,                    LABEL_BACK},
  }
};

void isPauseSpindle(void)               //TG 3/2/23 renamed was isPauseExtrude
{
  if (pausePrint(true, PAUSE_NORMAL))   //TG 8/21/21 fixed for V27, and removed Extrude module
    REPLACE_MENU((infoSettings.laser_mode == 1) ? menuLaser : menuSpindle); //TG 2/8/21 was menuExtrude
}
 
void isPauseLoadUnload(void)
{
  if (pausePrint(true, PAUSE_NORMAL))
    REPLACE_MENU(menuLoadUnload);
}

void menuMore(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  moreItems.items[0].icon = (infoSettings.laser_mode == 1) ? ICON_LASER : ICON_SPINDLE;				//TG added
  moreItems.items[0].label.index = (infoSettings.laser_mode == 1) ? LABEL_LASER : LABEL_SPINDLE;	//TG added

  menuDrawPage(&moreItems);

  while (MENU_IS(menuMore))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
       case KEY_ICON_0:	//TG modified this
        if (isPrinting() && !isPaused())  // need paused before spindle     case KEY_ICON_0:
        {
          popupDialog(DIALOG_TYPE_ALERT, LABEL_WARNING, LABEL_IS_PAUSE, LABEL_CONFIRM, LABEL_CANCEL, LABEL_NULL, isPauseSpindle, NULL, NULL, NULL); //TG 3/29/23 added NULL's for 3-button popup
        }
        else
        {
          OPEN_MENU(menuSpindle);
        }
        break;
        
      case KEY_ICON_1:
        OPEN_MENU(menuVacuum);
        break;

      case KEY_ICON_2:
		OPEN_MENU(menuFan);
        break;

      case KEY_ICON_3:
        OPEN_MENU(menuSpeed);
        break;

      case KEY_ICON_4:
        OPEN_MENU(menuFeatureSettings);
        break;

      case KEY_ICON_5:
        OPEN_MENU(menuMachineSettings);
        break;

      case KEY_ICON_6:
        #ifdef LOAD_UNLOAD_M701_M702
          if (isPrinting() && !isPaused())  // need paused before extrude
          {
            popupDialog(DIALOG_TYPE_ALERT, LABEL_WARNING, LABEL_IS_PAUSE, LABEL_CONFIRM, LABEL_CANCEL, LABEL_NULL
                        isPauseLoadUnload, NULL, NULL, NULL); //TG 3/29/23 added NULL's for 3-button popup
          }
          else
          {
            OPEN_MENU(menuLoadUnload);
          }
        #else
          OPEN_MENU(menu2More);
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
