//TG MODIFIED*****
#include "includes.h"
#include "More.h"
#include "Configuration.h"

MENUITEMS more2Items = {  //TG removed const so this menu can be dynamically changed
  // title
  LABEL_MORE2,
  // icon                          label
  {
	{ICON_GCODE,                    LABEL_TERMINAL},
	{ICON_PROBE_STOCK,              LABEL_PROBE_STOCK},
	#ifdef USING_AVR_TRIAC_CONTROLLER
  {ICON_AVR_CTL,                  LABEL_AVR_CTL},
  #else
  {ICON_BACKGROUND,               LABEL_BACKGROUND},
  #endif
	{ICON_BACKGROUND,               LABEL_BACKGROUND},
	{ICON_BACKGROUND,               LABEL_BACKGROUND},
  {ICON_BACKGROUND,               LABEL_BACKGROUND},
  {ICON_BACKGROUND,               LABEL_BACKGROUND},
  {ICON_BACK,                     LABEL_BACK},
  }
};

void menu2More(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&more2Items);

  while (infoMenu.menu[infoMenu.cur] == menu2More)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:	//TG modified this
        infoMenu.menu[++infoMenu.cur] = menuTerminal;
        break;

      case KEY_ICON_1:  // Probe Stock code goes here
        if (isPrinting() && !isPaused())  // need paused before spindle
        {        
          setDialogText(LABEL_WARNING, LABEL_IS_PAUSE, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_ALERT, isPauseExtrude, NULL, NULL);
        }
        else
        {
          // Place Probe Stock code here
        }        
        break;

      case KEY_ICON_2:
        #ifdef USING_AVR_TRIAC_CONTROLLER
		      infoMenu.menu[++infoMenu.cur] = menuTriac;
        #endif
        break;

      case KEY_ICON_3:
        break;

      case KEY_ICON_4:
        break;

      case KEY_ICON_5:
        break;

      case KEY_ICON_6:
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
