//TG MODIFIED*****
#include "Tuning.h"
#include "includes.h"

void itemDisabledPopup(uint16_t L1)   //TG 2/28/23 - new function added to concatenate two LABEL items and issue warning popup
{
  static uint8_t tempstr1[MAX_LANG_LABEL_LENGTH] = {0};
  static uint8_t tempstr2[MAX_LANG_LABEL_LENGTH] = {0};

  loadLabelText((uint8_t*)&tempstr1, L1);                               // get text string 1
  loadLabelText((uint8_t*)&tempstr2, LABEL_DISABLED_FOR_CNC);           // get text string 2
  strcat((char * __restrict__)tempstr1,(char * __restrict__)tempstr2);  // concatenate for msg
  {popupDialog(DIALOG_TYPE_ALERT, LABEL_WARNING, tempstr1, LABEL_CONFIRM, LABEL_NULL, NULL, NULL, NULL);}  // show OK button only
}

void menuTuning(void)
{
  MENUITEMS TuningItems = {
    // title
    LABEL_TUNING,
    // icon                          label
    {
      {ICON_MPC_PID,                 LABEL_MPC},              //TG 8/22/21 removed for CNC, was LABEL_MPC
      {ICON_MPC_PID,                 LABEL_PID},              //TG 8/22/21 removed for CNC, was LABEL_PID
      {ICON_TUNE_EXTRUDER,           LABEL_TUNE_EXTRUDER},    //TG 8/22/21 removed for CNC, was LABEL_PID
      #if DELTA_PROBE_TYPE == 0  // if not Delta printer
        {ICON_PROBE_OFFSET,            LABEL_H_OFFSET},
      #else
        {ICON_NULL,                    LABEL_NULL},
      #endif
      {ICON_PROBE_OFFSET,            LABEL_PROBING_Z_OFFSET}, //TG 2/28/23 added this in, but not used for CNC
      {ICON_PROBE_STOCK,             LABEL_PROBE_STOCK},      //TG 2/10/21 added for CNC
      {ICON_NULL,                    LABEL_NULL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  /* //TG Removed Pid.c for CNC
  if (!hasMPC())
  {
    for (uint8_t i = 0; i < 4; i++)
    {
      TuningItems.items[i] = TuningItems.items[i + 1];
    }
  }
  else if (!infoSettings.bed_en)
  {
    for (uint8_t i = 1; i < 4; i++)
    {
      TuningItems.items[i] = TuningItems.items[i + 1];
    }
  }
  */

  menuDrawPage(&TuningItems);

  while (MENU_IS(menuTuning))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    { //TG - 2/28/23 modified some key actions below since MPC.c and Pid.c are removed for CNC
      case KEY_ICON_0:
        itemDisabledPopup(LABEL_MPC);
        /* //TG - 2/28/23 comment out for CNC
        if (hasMPC())
        {   
          OPEN_MENU(menuMPC);
        }
        else
          OPEN_MENU(menuPid);
        */
        break;

      case KEY_ICON_1:
        itemDisabledPopup(LABEL_PID);
        /* //TG - 2/28/23 comment out for CNC
        if (hasMPC() && infoSettings.bed_en)
          OPEN_MENU(menuPid);
        else
          OPEN_MENU(menuTuneExtruder);
        */
        break;
        
      case KEY_ICON_2:
        itemDisabledPopup(LABEL_TUNE_EXTRUDER);
        /* //TG - 2/28/23 comment out for CNC
        if (hasMPC() && infoSettings.bed_en)
          OPEN_MENU(menuTuneExtruder);
        #if DELTA_PROBE_TYPE == 0  // if not Delta printer
          else
          {
            storeCmd("M206\n");
            zOffsetSetMenu(false);  // use Home Offset menu
            OPEN_MENU(menuZOffset);
          }
        #endif
        */
        break;
        
      case KEY_ICON_3:
        //itemDisabledPopup(LABEL_HOME_OFFSET);
        #if DELTA_PROBE_TYPE == 0  // if not Delta printer
          //TG Removed Pid.c and MPC.c for CNC
          //if (hasMPC() && infoSettings.bed_en)
          {
            storeCmd("M206\n");
            zOffsetSetMenu(false);  // use Home Offset menu
            OPEN_MENU(menuZOffset);
          }
        #endif
        break;

      case KEY_ICON_4:
        itemDisabledPopup(LABEL_PROBING_Z_OFFSET);
        /* //TG - 2/28/23 comment out for CNC
        if (infoMachineSettings.zProbe == ENABLED)
        {
          #if DELTA_PROBE_TYPE != 2
            storeCmd("M851\n");
            zOffsetSetMenu(true);  // use Probe Offset menu
            OPEN_MENU(menuZOffset);
          #else
            popupDialog(DIALOG_TYPE_ALERT, LABEL_WARNING, LABEL_DISCONNECT_PROBE, LABEL_CONTINUE, LABEL_CANCEL, deltaZOffset, NULL, NULL);
          #endif
        }
        */
        break;

      case KEY_ICON_5:
        OPEN_MENU(menuProbeStock);
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
