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
  #endif
  #ifdef USING_VFD_CONTROLLER
  {ICON_VFD_CONTROL,              LABEL_VFD_CONTROL},
  #endif
	{ICON_NULL,               LABEL_NULL},
	{ICON_NULL,               LABEL_NULL},
  {ICON_NULL,               LABEL_NULL},
  {ICON_NULL,               LABEL_NULL},
  {ICON_BACK,                     LABEL_BACK},
  }
};

//TG 2/28/23 - these functions added to support popupDialog actions in the menu below
void isPauseTerminal(void)
{
  if (pausePrint(true, PAUSE_NORMAL))
    REPLACE_MENU(menuTerminal);
}
void isPauseProbeStock(void)
{
  if (pausePrint(true, PAUSE_NORMAL))
    REPLACE_MENU(menuProbeStock);
}
void isPauseVFD(void)
{
  if (pausePrint(true, PAUSE_NORMAL))
    REPLACE_MENU(menuVFD);
}


void menu2More(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&more2Items);
  
  static uint8_t tempstr1[MAX_LANG_LABEL_LENGTH] = {0};
  static uint8_t tempstr2[MAX_LANG_LABEL_LENGTH] = {0};
  while (infoMenu.menu[infoMenu.cur] == menu2More)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:	//TG 2/28/23 modified this
        if (isPrinting() && !isPaused())  // currently printing and not paused?
            {popupDialog(DIALOG_TYPE_ALERT, LABEL_WARNING, LABEL_IS_PAUSE, LABEL_CONFIRM, LABEL_CANCEL, LABEL_NULL, isPauseTerminal, NULL, NULL, NULL);} //TG 3/29/23 added NULL's for 3-button popup
        else
            {OPEN_MENU(menuTerminal);}
        break;

      case KEY_ICON_1:  //TG 2/28/23      // Probe Stock code goes here
        if (isPrinting() && !isPaused())  // currently printing and not paused?
        {    
            loadLabelText((uint8_t*)&tempstr1, LABEL_PROBE_STOCK);
            loadLabelText((uint8_t*)&tempstr2, LABEL_DISABLED_FOR_CNC);
            strcat((char * __restrict__)tempstr1,(char * __restrict__)tempstr2);
            {popupDialog(DIALOG_TYPE_ALERT, LABEL_WARNING, tempstr1, LABEL_CONFIRM, LABEL_CANCEL, LABEL_NULL, NULL, NULL, NULL, NULL);} //TG 3/29/23 added NULL's for 3-button popup
        }
        else
            {OPEN_MENU(menuProbeStock);}
        break;

      case KEY_ICON_2:  //TG 2/28/23
        if (isPrinting() && !isPaused())  // currently printing and not paused?
            {popupDialog(DIALOG_TYPE_ALERT, LABEL_WARNING, LABEL_IS_PAUSE, LABEL_CONFIRM, LABEL_CANCEL, LABEL_NULL, NULL, NULL, NULL, NULL);} //TG 3/29/23 added NULL's for 3-button popup
        else
        {
          #ifdef USING_AVR_TRIAC_CONTROLLER
            OPEN_MENU(menuTriac);
          #endif
          #ifdef USING_VFD_CONTROLLER
            OPEN_MENU(menuVFD);
          #endif
        }
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
