//TG MODIFIED*****
#include "common.h"
#include "includes.h"

// indexes for status icon toggles
uint8_t currentTool = TOOL0;        //TG for CNC will always be just the one tool
uint8_t currentFan = 0;             //TG for multiple fan indices
uint8_t currentSpeedID = 0;         //TG for speed or flow, 2 indices
uint8_t currentSpindleSpeedID = 0;  //TG 2/24/21 new for CNC will always be just the one tool
static uint32_t lastTime = 0;
bool nextWCSupdate = false;         //TG 10/4/22 - added flag, it gets set whenever parseAck.c gets a workspace change msg from Marlin

//Icons list for tool change  //TG 1/16/20 changed to just 4 tools //TODO can be removed after removing PreHeat.c, Pid.c, Heat.c
const ITEM itemTool[MAX_TOOL_COUNT] =
{
// icon                       label
  {ICON_SPINDLE,               LABEL_SPINDLE},
  {ICON_LASER,                 LABEL_LASER},
  {ICON_VACUUM,                LABEL_VACUUM},
  {ICON_REMOVED,				       LABEL_REMOVED},  //TG 7/17/22 removed, was CHAMBER for use in Heat.c and pid.c
};

//Icons list for spindle change  //TG 1/16/20 changed to just 4 tools
const ITEM itemSpindle[MAX_TOOL_COUNT] =
{
// icon                       label
  {ICON_SPINDLE,               LABEL_SPINDLE},
  {ICON_SPINDLE,               LABEL_SPINDLE},
  {ICON_SPINDLE,               LABEL_SPINDLE},
  {ICON_SPINDLE,               LABEL_SPINDLE},
};

//Icons list for lasers change  //TG 1/16/20 changed to just 4 tools
const ITEM itemLaser[MAX_TOOL_COUNT] =
{
// icon                       label
  {ICON_LASER,               LABEL_LASER},
  {ICON_LASER,               LABEL_LASER},
  {ICON_LASER,               LABEL_LASER},
  {ICON_LASER,               LABEL_LASER},
};

//Icons list for dustcontrol change  //TG 1/16/20 changed to just 4 tools
const ITEM itemVacuum[MAX_TOOL_COUNT] =
{
// icon                       label
  {ICON_VACUUM,               LABEL_VACUUM},
  {ICON_VACUUM,               LABEL_VACUUM},
  {ICON_VACUUM,               LABEL_VACUUM},
  {ICON_VACUUM,               LABEL_VACUUM},
};

// Icons list for Temperature step change
const ITEM itemDegreeSteps[ITEM_DEGREE_NUM] =
{// icon                       label
  {ICON_REMOVED,             LABEL_REMOVED},    //TG 2/18/21 removed was 1_DEGREE
  {ICON_REMOVED,             LABEL_REMOVED},    //TG 2/18/21 removed was 5_DEGREE
  {ICON_REMOVED,             LABEL_REMOVED},    //TG 2/18/21 removed was 10_DEGREE

};

// List for temperature step change
const uint8_t degreeSteps[ITEM_DEGREE_NUM] = {1, 5, 10};
// Icons list for RPM step change  //TG 1/16/20 new for RPM steps
const ITEM itemRPMSteps[ITEM_RPM_NUM] =
{
// icon                       label
  {ICON_500_RPM,             LABEL_500_RPM,},
  {ICON_1000_RPM,            LABEL_1000_RPM},
  {ICON_5000_RPM,            LABEL_5000_RPM},
  {ICON_10000_RPM,           LABEL_10000_RPM,},
};
// List for RPM step change   //TG 1/16/20 new for RPM steps
const u16 RPMSteps[ITEM_RPM_NUM] = {500, 1000, 5000, 10000};

// Icons list for PWM unit change steps  //TG 1/16/20 new for PWM steps
const ITEM itemPWMSteps[ITEM_PWM_NUM] =
{
// icon                       label
  {ICON_1_MM,                 LABEL_1_MM},
  {ICON_10_MM,                LABEL_10_MM},
  {ICON_100_MM,               LABEL_100_MM},
};
// List for length/distance change steps
const uint8_t PWMSteps[ITEM_PWM_NUM] = {1, 10, 100};

// Icons list for axis movement speed change steps
const ITEM itemSpeed[ITEM_SPEED_NUM] =
{
// icon                          label
  {ICON_SLOW_SPEED,              LABEL_SLOW},
  {ICON_NORMAL_SPEED,            LABEL_NORMAL},
  {ICON_FAST_SPEED,              LABEL_FAST},
};


//**********************************************************************************************************
// GOING TO NEED 6 NEW ICONS FOR PERCENT STEPS AND FOR RPM STEPS
//TG Do we really need these if no extruders?
// Icons list for percent change steps
const ITEM itemPercent[ITEM_PERCENT_STEPS_NUM] =
{
// icon                          label
  {ICON_E_1_PERCENT,             LABEL_1_PERCENT},
  {ICON_E_5_PERCENT,             LABEL_5_PERCENT},
  {ICON_E_10_PERCENT,            LABEL_10_PERCENT},
};

// List for percent change steps
const uint8_t percentSteps[ITEM_PERCENT_STEPS_NUM] = {1, 5, 10};

// Icons list for axis length/distance change steps
const ITEM itemMoveLen[ITEM_MOVE_LEN_NUM] =
{
// icon                          label
  {ICON_001_MM,                  LABEL_001_MM},
  {ICON_01_MM,                   LABEL_01_MM},
  {ICON_1_MM,                    LABEL_1_MM},
  {ICON_10_MM,                   LABEL_10_MM},
  {ICON_100_MM,                  LABEL_100_MM},
};

// List for length/distance change steps
const float moveLenSteps[ITEM_MOVE_LEN_NUM] = {0.01f, 0.1f, 1, 10, 100};

// Icons list for Extruder length/distance change steps    //TG removed for CNC version 2/10/21
/*
const ITEM itemExtLenSteps[ITEM_EXT_LEN_NUM] =
{
// icon                          label
  {ICON_E_1_MM,                  LABEL_1_MM},
  {ICON_E_5_MM,                  LABEL_5_MM},
  {ICON_E_10_MM,                 LABEL_10_MM},
  {ICON_E_100_MM,                LABEL_100_MM},
  {ICON_E_200_MM,                LABEL_200_MM},
};

// List for extruder length/distance change steps
const float extlenSteps[ITEM_EXT_LEN_NUM] = {1.0f, 5.0f, 10.0f, 100.0f, 200.0f};
*/


// Labels list for ON/OFF settings
const LABEL itemToggle[ITEM_TOGGLE_NUM] =
{
  LABEL_OFF,
  LABEL_ON
};

const uint16_t iconToggle[ITEM_TOGGLE_NUM] =
{
  ICONCHAR_TOGGLE_OFF,
  ICONCHAR_TOGGLE_ON
};

// Check time elapsed against the time specified in milliseconds for displaying/updating info on screen
// Use this for timed screen updates in menu loops only
bool nextScreenUpdate(uint32_t duration)
{
  uint32_t curTime = OS_GetTimeMs();
  if (curTime > (lastTime + duration))
  {
    lastTime = curTime;
    return true;
  }
  else
  {
    return false;
  }
}

const bool warmupTemperature(uint8_t toolIndex, void (* callback)(void))
{//TG 8/22/21 entire sub commented out for CNC, since some LABELS were removed from Language.inc for room
/*
  if (heatGetCurrentTemp(toolIndex) < infoSettings.min_ext_temp)
  { // low temperature warning
    char tempMsg[120];
    LABELCHAR(tempStr, LABEL_EXT_TEMPLOW);

    sprintf(tempMsg, tempStr, infoSettings.min_ext_temp);
    strcat(tempMsg, "\n");
    sprintf(tempStr, (char *) textSelect(LABEL_HEAT_HOTEND), infoSettings.min_ext_temp);
    strcat(tempMsg, tempStr);

    setDialogText(LABEL_WARNING, (uint8_t *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL);
    showDialog(DIALOG_TYPE_ERROR, callback, NULL, NULL);

    return false;
  }
*/
  return true;
}

const void cooldownTemperature(void)
{//TG 8/22/21 entire sub commented out for CNC, since some LABELS were removed from Language.inc for room
/*
  if (!isPrinting())
  {
    for (uint8_t i = 0; i < MAX_TOOL_COUNT; i++)
    {
      if (heatGetTargetTemp(i) > 0)
      {
        setDialogText(LABEL_WARNING, LABEL_HEATERS_ON, LABEL_CONFIRM, LABEL_CANCEL);
        showDialog(DIALOG_TYPE_QUESTION, heatCoolDown, NULL, NULL);
        break;
      }
    }
  }
*/
}

// Show/draw a temperature in a standard menu
const void temperatureReDraw(uint8_t toolIndex, int16_t * temp, bool skipHeader)
{
  char tempstr[20];

  setLargeFont(true);

  if (!skipHeader)
  {
    sprintf(tempstr, "%-15s", heatDisplayID[toolIndex]);
    setLargeFont(false);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
    setLargeFont(true);
    GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *) "�C");
  }

  if (temp != NULL)
    sprintf(tempstr, "  %d  ", *temp);
  else
    sprintf(tempstr, "%4d/%-4d", heatGetCurrentTemp(toolIndex), heatGetTargetTemp(toolIndex));

  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
}

// Show/draw fan in a standard menu
const void fanReDraw(uint8_t fanIndex, bool skipHeader)
{
  char tempstr[20];

  setLargeFont(true);

  if (!skipHeader)
  {
    sprintf(tempstr, "%-15s", fanID[fanIndex]);
    setLargeFont(false);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
    setLargeFont(true);

    if (infoSettings.fan_percentage == 1)
    {
      GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *) "%");
    }
    else
    {
      GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *) "PWM");
    }
  }

  if (infoSettings.fan_percentage == 1)
    sprintf(tempstr, "%4d/%-4d", fanGetCurPercent(fanIndex), fanGetSetPercent(fanIndex));
  else
    sprintf(tempstr, "%4d/%-4d", fanGetCurSpeed(fanIndex), fanGetSetSpeed(fanIndex));

  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
}

// Show/draw extruder in a standard menu
const void extruderReDraw(uint8_t extruderIndex, float extrusion, bool skipHeader)
{//TG 8/22/21 entire sub commented out for CNC, since some LABELS were removed from Language.inc for room
/*
  char tempstr[20];

  setLargeFont(true);

  if (!skipHeader)
  {
    sprintf(tempstr, "%-15s", extruderDisplayID[extruderIndex]);
    setLargeFont(false);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
    setLargeFont(true);
    GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *) "mm");
  }

  sprintf(tempstr, "  %.2f  ", extrusion);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
*/
}

// Show/draw percentage in a standard menu
const void percentageReDraw(uint8_t itemIndex, bool skipHeader)
{
  char tempstr[20];

  setLargeFont(true);

  if (!skipHeader)
  {
    setLargeFont(false);

    if (itemIndex == 0)
      sprintf(tempstr, "%-15s", textSelect(LABEL_PERCENTAGE_SPEED));
    else
      sprintf(tempstr, "%-15s", textSelect(LABEL_REMOVED)); //TG 10/12/22 removed LABEL_PERCENTAGE_FLOW

    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
    setLargeFont(true);
    GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *) "%");
  }

  sprintf(tempstr, "%4d/%-4d", speedGetCurPercent(itemIndex), speedGetSetPercent(itemIndex));
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
}

// Edit an integer value in a standard menu
const int16_t editIntValue(int16_t minValue, int16_t maxValue, int16_t resetValue, int16_t value)
{
  int16_t val;
  char tempstr[30];

  sprintf(tempstr, "Min:%i | Max:%i", minValue, maxValue);

  val = numPadInt((uint8_t *) tempstr, value, resetValue, false);
  val = NOBEYOND(minValue, val, maxValue);

  return val;
}

// Edit a float value in a standard menu
const float editFloatValue(float minValue, float maxValue, float resetValue, float value)
{
  float val;
  char tempstr[30];

  sprintf(tempstr, "Min:%.2f | Max:%.2f", minValue, maxValue);

  val = numPadFloat((uint8_t *) tempstr, value, resetValue, true);
  val = NOBEYOND(minValue, val, maxValue);

  return val;
}

//TG 10/4/22 - added new - put this in common.c so it can be used by several menus
// displays the active WCS in upper right of title bar on select menus
void drawWCSinfo()
{
  char mmsg[8];
  GUI_SetColor(WHITE);
  sprintf(mmsg, "WCS:%2d", infoMachineSettings.active_workspace);
  GUI_DispStringInPrect(&rect_of_titleBar_RHS, (u8 *)mmsg);
  //GUI_DispLenString(LCD_WIDTH-(7*BYTE_WIDTH), (TITLE_END_Y-BYTE_HEIGHT)/2, (u8*)mmsg, (8*BYTE_WIDTH), true);
  GUI_RestoreColorDefault();
}

