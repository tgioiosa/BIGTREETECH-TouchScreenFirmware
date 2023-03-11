#include "StatusScreen.h"
#include "includes.h"
#include "common.h"   //TG 10/4/22 - added for workspace display support

#ifdef TFT70_V3_0
  #define KEY_SPEEDMENU         KEY_ICON_3
  #define KEY_FLOWMENU          (KEY_SPEEDMENU + 1)
  #define KEY_MAINMENU          (KEY_FLOWMENU + 1)
  #define SET_SPEEDMENUINDEX(x) setSpeedItemIndex(x)
#else
  #define KEY_SPEEDMENU         KEY_ICON_3
  #define KEY_MAINMENU          (KEY_SPEEDMENU + 1)
  #define SET_SPEEDMENUINDEX(x)
#endif

#define UPDATE_TOOL_TIME 2000  // 1 seconds is 1000

#ifdef PORTRAIT_MODE
  #define XYZ_STATUS "X:%.2f Y:%.2f Z:%.2f"
#else
  #define XYZ_STATUS "   X: %.2f   Y: %.2f   Z: %.2f   "
#endif

MENUITEMS StatusItems = {         //TG 1/12/20 removed const so this menu can be dynamically modified
  // title
  LABEL_READY,
  // icon                          label
  {
	  {ICON_STATUS_SPINDLE,          LABEL_NULL},    //TG 1/14/20 changed
    {ICON_STATUS_VACUUM,           LABEL_NULL},    //TG 1/14/20 changed
    {ICON_STATUS_FAN,              LABEL_NULL},
    {ICON_STATUS_SPEED,            LABEL_NULL},
    #ifdef TFT70_V3_0
      {ICON_STATUS_FLOW,             LABEL_NULL},
      {ICON_MAINMENU,                LABEL_MAINMENU},
    #else
      {ICON_MAINMENU,                LABEL_MAINMENU},
      {ICON_NULL,                    LABEL_NULL},
    #endif
    {ICON_NULL,                    LABEL_NULL},
    {ICON_PRINT,                   LABEL_PRINT},
  }
};

const uint8_t bedIcons[2] = {ICON_STATUS_VACUUM, ICON_STATUS_VACUUM};   //TG 2/24/23 modified
const uint8_t speedIcons[2] = {ICON_STATUS_SPEED, ICON_STATUS_FLOW};
const ITEM SpeedItems[2] = {
  // icon                        label
  {ICON_STATUS_SPEED,            LABEL_NULL},
  {ICON_STATUS_FLOW,             LABEL_NULL},
};

#define UPDATE_TOOL_TIME  2000 // 1 seconds is 1000  //TG 2/21/21 changed, was 2000
//static u32 nextToolTime = 0;
SCROLL     msgScroll;static int8_t lastConnectionStatus = -1;
static bool msgNeedRefresh = false;

static char msgTitle[20];
static char msgBody[MAX_MSG_LENGTH];
SCROLL msgScroll;

const char *const speedID[2] = SPEED_ID;

// text position rectangles for Live icons
const GUI_POINT ss_title_point = {SS_ICON_WIDTH - BYTE_WIDTH/2, SS_ICON_NAME_Y0}; // 95-24/2, 7 = 89,7
const GUI_POINT ss_val_point   = {SS_ICON_WIDTH/2, SS_ICON_VAL_Y0};  // icon x center, y 75 = 47.5,75
const GUI_POINT ss_val2_point = {SS_ICON_WIDTH - 3*BYTE_WIDTH-BYTE_WIDTH/4, SS_ICON_NAME_Y0 + BYTE_HEIGHT};

#ifdef TFT70_V3_0
  const GUI_POINT ss_val_point_2 = {SS_ICON_WIDTH / 2, SS_ICON_VAL_Y0_2};
#endif

// info box msg area
#ifdef PORTRAIT_MODE
  const  GUI_RECT msgRect = {START_X + 0.5 * ICON_WIDTH + 0 * SPACE_X + 2, ICON_START_Y + 0 * ICON_HEIGHT + 0 * SPACE_Y + STATUS_MSG_BODY_YOFFSET,
                             START_X + 2.5 * ICON_WIDTH + 1 * SPACE_X - 2, ICON_START_Y + 1 * ICON_HEIGHT + 0 * SPACE_Y - STATUS_MSG_BODY_BOTTOM};

  const GUI_RECT recGantry = {START_X - 3,                                SS_ICON_HEIGHT + ICON_START_Y + STATUS_GANTRY_YOFFSET,
                              START_X + 3 + 3 * ICON_WIDTH + 2 * SPACE_X, ICON_HEIGHT + SPACE_Y + ICON_START_Y - STATUS_GANTRY_YOFFSET};
#else
  const  GUI_RECT msgRect = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + 2, ICON_START_Y + 1 * ICON_HEIGHT + 1 * SPACE_Y + STATUS_MSG_BODY_YOFFSET,
                             START_X + 3 * ICON_WIDTH + 2 * SPACE_X - 2, ICON_START_Y + 2 * ICON_HEIGHT + 1 * SPACE_Y - STATUS_MSG_BODY_BOTTOM};

  const GUI_RECT recGantry = {START_X,                                SS_ICON_HEIGHT + ICON_START_Y + STATUS_GANTRY_YOFFSET,
                              START_X + 4 * ICON_WIDTH + 3 * SPACE_X, ICON_HEIGHT + SPACE_Y + ICON_START_Y - STATUS_GANTRY_YOFFSET};
#endif

//TG code to update a single lvIcon text line on an icon. Up to three lines can be drawn on any icon, indexes 0,1,2
void drawSingleLiveIconLine(uint8_t icon, uint8_t currentToggleID) {
  //icons and their values are updated one by one to reduce flicker/clipping
  if(infoMenu.menu[infoMenu.cur] != menuStatus)
    return;
  
  char tempstr[45];
  LIVE_INFO lvIcon;
  lvIcon.enabled[1] = true;                         // single item at 2nd line position
  lvIcon.lines[1].h_align = CENTER;                 // center justified
  lvIcon.lines[1].v_align = CENTER;                 // center aligned
  lvIcon.lines[1].fn_color = SS_VAL_COLOR;          // black
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;   // no background
  lvIcon.lines[1].pos = ss_val_point;               // value location
  lvIcon.lines[1].font = FONT_SIZE_NORMAL;          // Normal font size
  #ifndef TFT70_V3_0
    lvIcon.enabled[2] = false;                      // no 3rd item
  #else
    lvIcon.enabled[2] = true;
    lvIcon.lines[2].h_align = CENTER;
    lvIcon.lines[2].v_align = CENTER;
    lvIcon.lines[2].fn_color = SSICON_VAL2_COLOR;
    lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;
    lvIcon.lines[2].pos = ss_val2_point;
    lvIcon.lines[2].large_font = VAL2_LARGE_FONT;  
  #endif
  #ifdef TFT70_V3_0
    char tempstr2[45];
    TOOL / EXT
    lvIcon.lines[0].text = (u8 *)heatDisplayID[currentTool];
    sprintf(tempstr, "%d℃", heatGetCurrentTemp(currentTool));
    sprintf(tempstr2, "%d℃", heatGetTargetTemp(currentTool));
    lvIcon.lines[1].text = (u8 *)tempstr;
    lvIcon.lines[2].text = (u8 *)tempstr2;
    showLiveInfo(0, &lvIcon, &StatusItems.items[0]);
    //BED
    lvIcon.lines[0].text = (u8 *)heatDisplayID[BED];
    sprintf(tempstr, "%d℃", heatGetCurrentTemp(BED));
    sprintf(tempstr2, "%d℃", heatGetTargetTemp(BED));
    lvIcon.lines[1].text = (u8 *)tempstr;
    lvIcon.lines[2].text = (u8 *)tempstr2;
    showLiveInfo(1, &lvIcon, &StatusItems.items[1]);
    lvIcon.enabled[2] = false;
  #else
    //TOOL / SPINDLE    //TG 2/17/21 updated for spindle, was for EXT, now alternates between Set/Cur speed every UPDATE_TOOL_TIME
    lvIcon.iconIndex = icon;
    lvIcon.lines[0].text = (u8 *)spindleDisplayID[currentTool];
    sprintf(tempstr, infoSettings.cutter_disp_unit == MPCT ? "%d%%" : "%d", currentToggleID ? 
            convertSpeed_Marlin_2_LCD(currentTool,spindleGetSetSpeed(currentTool)) : 
            convertSpeed_Marlin_2_LCD(currentTool,spindleGetCurSpeed(currentTool))
           );
    lvIcon.lines[1].text = (u8 *)tempstr;
    // icon index(0-7), address of live icon data, address of current icon image, live icon line(0-2)
    showSingleLiveIconLine(0, &lvIcon, &StatusItems.items[0],1);
 #endif
}


void drawStatus(void)   //TG was drawAllLiveIconData() in prior versions
{
  // icons and their values are updated one by one to reduce flicker/clipping
  char tempstr[45];

  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;                         // 1st item   (S0, V0, F0, Sp. white text in upper right corner of icon)
  lvIcon.lines[0].h_align = RIGHT;                  // right-justified
  lvIcon.lines[0].v_align = TOP;                    // top aligned
  lvIcon.lines[0].pos = ss_title_point;             // name location
  lvIcon.lines[0].font = SS_ICON_TITLE_FONT_SIZE;   // Normal font size
  lvIcon.lines[0].fn_color = SS_NAME_COLOR;         // white
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;   // transparent, no background

  lvIcon.enabled[1] = true;                         // 2nd item   (Black text in white bar on bottom of icon)
  lvIcon.lines[1].h_align = CENTER;                 // center justified
  lvIcon.lines[1].v_align = CENTER;                 // center aligned
  lvIcon.lines[1].pos = ss_val_point;               // value location
  lvIcon.lines[1].font = SS_ICON_VAL_FONT_SIZE;     // Normal font size
  lvIcon.lines[1].fn_color = SS_VAL_COLOR;          // black
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;   // transparent, no background

  lvIcon.lines[2].h_align = LEFT;                   //TG 2/22/21 for white "Set/Act" text on spindle speed
  lvIcon.lines[2].v_align = TOP;                    // top aligned
  lvIcon.lines[2].fn_color = SS_NAME_COLOR;         // white
  lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;   // transparent, no background
  lvIcon.lines[2].pos = ss_val2_point;              // text location
  lvIcon.lines[2].font = SS_ICON_VAL_FONT_SIZE;     // Normal font size     

  #ifndef TFT70_V3_0
    lvIcon.enabled[2] = false;
  #else
    lvIcon.enabled[2] = true;
    lvIcon.lines[2].h_align = CENTER;
    lvIcon.lines[2].v_align = CENTER;
    lvIcon.lines[2].pos = ss_val_point_2;
    lvIcon.lines[2].font = SS_ICON_VAL_FONT_SIZE_2;
    lvIcon.lines[2].fn_color = SS_VAL_COLOR_2;
    lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;  // default value
  #endif

  #ifdef TFT70_V3_0
    char tempstr2[45];

    // TOOL / EXT
    lvIcon.iconIndex = ICON_STATUS_NOZZLE;
    lvIcon.lines[0].text = (uint8_t *)heatShortID[currentTool];
    sprintf(tempstr, "%3d℃", heatGetCurrentTemp(currentTool));
    sprintf(tempstr2, "%3d℃", heatGetTargetTemp(currentTool));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    lvIcon.lines[2].text = (uint8_t *)tempstr2;
    showLiveInfo(0, &lvIcon, false);

    // BED / CHAMBER
    lvIcon.iconIndex = bedIcons[currentBCIndex];
    lvIcon.lines[0].text = (uint8_t *)heatShortID[BED + currentBCIndex];
    sprintf(tempstr, "%3d℃", heatGetCurrentTemp(BED + currentBCIndex));
    sprintf(tempstr2, "%3d℃", heatGetTargetTemp(BED + currentBCIndex));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    lvIcon.lines[2].text = (uint8_t *)tempstr2;
    showLiveInfo(1, &lvIcon, infoSettings.chamber_en == 1);

    lvIcon.enabled[2] = false;
  #else
    //TOOL / SPINDLE    //TG 2/17/21 updated for spindle, was for EXT
    //the speed line[1] is also shown every 1000ms from Marlin RPM gate interval which calls drawSingleLiveIconLine()
    //the order of showing Set/Act is reversed with ref to currentSpeedFlowID so that Speed/Flow icon will display actual
    //spindle Act rotation speed while the spindle icon is showing Set spindle speed.            
    lvIcon.iconIndex = ICON_STATUS_SPINDLE;
    lvIcon.lines[0].text = (u8 *)spindleDisplayID[currentTool];
    //sprintf(tempstr, "%3d/%-3d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));
    sprintf(tempstr, infoSettings.cutter_disp_unit == MPCT ? "%d%%" : "%d", currentSpindleSpeedID ? 
            convertSpeed_Marlin_2_LCD(currentTool,spindleGetSetSpeed(currentTool)) : 
            convertSpeed_Marlin_2_LCD(currentTool,spindleGetCurSpeed(currentTool)) 
           );
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    lvIcon.lines[2].text = currentSpindleSpeedID ? (u8*)"Set" : (u8*)"Act";
    lvIcon.enabled[2] = true;         // only show line[2] for spindle/laser icon (0)
    showLiveInfo(0, &lvIcon, false);  // the icon info is now in the lvIcon.iconIndex
    lvIcon.enabled[2] = false;        // turn off for other icons

    // BED
    lvIcon.iconIndex = ICON_STATUS_VACUUM;
    lvIcon.lines[0].text = (u8 *)vacuumDisplayID[0];
    sprintf(tempstr, "%s %s", (vacuumState & 2) == 2 ? (char*)"Auto" : (char*)"", (vacuumState & 1) == 1 ? (char*)"On" : (char*)"Off");
    lvIcon.lines[1].text = (u8 *)tempstr;
    showLiveInfo(1, &lvIcon, false);
  #endif

  // FAN
  lvIcon.iconIndex = ICON_STATUS_FAN;
  lvIcon.lines[0].text = (uint8_t *)fanID[currentFan];
  if (infoSettings.fan_percentage == 1)
    sprintf(tempstr, "%3d%%", fanGetCurPercent(currentFan));
  else
    sprintf(tempstr, "%3d", fanGetCurSpeed(currentFan));

  lvIcon.lines[1].text = (uint8_t *)tempstr;
  showLiveInfo(2, &lvIcon, false);

  #ifdef TFT70_V3_0
    // SPEED
    lvIcon.iconIndex = ICON_STATUS_SPEED;
    lvIcon.lines[0].text = (uint8_t *)speedID[0];
    sprintf(tempstr, "%3d%%", speedGetCurPercent(0));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(3, &lvIcon, false);

    // FLOW
    lvIcon.iconIndex = ICON_STATUS_FLOW;
    lvIcon.lines[0].text = (uint8_t *)speedID[1];
    sprintf(tempstr, "%3d%%", speedGetCurPercent(1));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(4, &lvIcon, false);
  #else
    //SPEED / flow  //TG 2/24/21 (flow no longer applies for CNC, so we put spindle speed/laser power in it's place)
    //TG TODO need to update this for Laser Power
    lvIcon.iconIndex = speedIcons[currentSpeedFlowID];  //TG set this because redraw=true below
    lvIcon.lines[0].text = (uint8_t *)speedID[currentSpeedFlowID];
    if(currentSpeedFlowID == 0) 
    { // show plotting speed
      sprintf(tempstr, "%d%%", speedGetCurPercent(currentSpeedFlowID));  
    }
    //TG 2/24/21 don't alternate plot/spindle speed by keeping currentSpeedFlowID=0 in toggleTool(), makes
    //the display confusing, so for CNC version we always get here with currentSpeedFlowID == 0
    else
    { // show spindle speed
      sprintf(tempstr, infoSettings.cutter_disp_unit == MPCT ? "%d%%" : "%d", 
              convertSpeed_Marlin_2_LCD(currentTool,spindleGetCurSpeed(currentTool))
             );  
    }

    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(3, &lvIcon, true); // map onto ICON_STATUS_SPEED
  #endif

  // Gantry X,Y,Z position above status area
  sprintf(tempstr, XYZ_STATUS, coordinateGetAxisActual(X_AXIS), coordinateGetAxisActual(Y_AXIS), coordinateGetAxisActual(Z_AXIS));
  #ifdef PORTRAIT_MODE
    int paddingWidth = ((recGantry.x1 - recGantry.x0) - (strlen(tempstr) * BYTE_WIDTH)) / 2;

    GUI_SetColor(GANTRY_XYZ_BG_COLOR);
    GUI_FillRect(recGantry.x0, recGantry.y0, recGantry.x0 + paddingWidth, recGantry.y1);  // left padding
    GUI_FillRect(recGantry.x1 - paddingWidth, recGantry.y0, recGantry.x1, recGantry.y1);  // right padding
  #endif
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetColor(GANTRY_XYZ_FONT_COLOR);
  GUI_SetBkColor(GANTRY_XYZ_BG_COLOR);
  GUI_DispStringInPrect(&recGantry, (uint8_t *)tempstr);
  GUI_RestoreColorDefault();
}

void statusScreen_setMsg(const uint8_t *title, const uint8_t *msg)
{
  strncpy(msgTitle, (char *)title, sizeof(msgTitle));
  strncpy(msgBody, (char *)msg, sizeof(msgBody));
  msgNeedRefresh = true;
}

void statusScreen_setReady(void)
{
  strncpy(msgTitle, (char *)textSelect(LABEL_STATUS), sizeof(msgTitle));

  if (infoHost.connected == false)
  {
    strncpy(msgBody, (char *)textSelect(LABEL_UNCONNECTED), sizeof(msgBody));
  }
  else
  {
    strncpy(msgBody, (char *)machine_type, sizeof(msgBody));
    strcat(msgBody, " ");
    strcat(msgBody, (char *)textSelect(LABEL_READY));
  }

  msgNeedRefresh = true;
}


//TG draws the statusText rectangle and puts in the scrolling message
void drawStatusScreenMsg(void)
{
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  IMAGE_ReadDisplay(rect_of_keySS[KEY_INFOBOX].x0, rect_of_keySS[KEY_INFOBOX].y0, INFOBOX_ADDR);
  GUI_SetColor(INFOMSG_BG_COLOR);
  GUI_DispString(rect_of_keySS[KEY_INFOBOX].x0 + STATUS_MSG_ICON_XOFFSET,
                 rect_of_keySS[KEY_INFOBOX].y0 + STATUS_MSG_ICON_YOFFSET,
                 IconCharSelect(CHARICON_INFO));

  GUI_DispString(rect_of_keySS[KEY_INFOBOX].x0 + BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET,
                 rect_of_keySS[KEY_INFOBOX].y0 + STATUS_MSG_ICON_YOFFSET,
                 (uint8_t *)msgTitle);

  GUI_SetBkColor(INFOMSG_BG_COLOR);
  GUI_FillPrect(&msgRect);
  Scroll_CreatePara(&scrollLine, (uint8_t *)msgBody, &msgRect);
  GUI_RestoreColorDefault();

  msgNeedRefresh = false;
}

static inline void scrollMsg(void)
{
  GUI_SetBkColor(INFOMSG_BG_COLOR);
  GUI_SetColor(INFOMSG_FONT_COLOR);
  Scroll_DispString(&scrollLine, CENTER);
  GUI_RestoreColorDefault();
}



static inline void toggleTool(void)
{
  if (nextScreenUpdate(UPDATE_TOOL_TIME))
  { 
    //TG commented this out not needed
    // if (infoSettings.hotend_count > 1)   // increment hotend index
    //   currentTool = (currentTool + 1) % infoSettings.hotend_count;

    //TG commented this out not needed
    //if (infoSettings.chamber_en == 1)     // switch bed/chamber index
    //  TOGGLE_BIT(currentBCIndex, 0);

    // increment fan index
    if ((infoSettings.fan_count + infoSettings.ctrl_fan_en) > 1)
    {
      do
      {
        currentFan = (currentFan + 1) % MAX_FAN_COUNT;
      } while (!fanIsValid(currentFan));
    }
    //TOGGLE_BIT(currentSpeedFlowID, 0);  //TG 2/24/21 don't toggle plot speed / spindle speed(old flow)
    TOGGLE_BIT(currentSpindleSpeedID,0);  //TG toggle spindle icon data

    drawStatus();

    // gcode queries must be call after drawStatus
    coordinateQuery(MS_TO_SEC(UPDATE_TOOL_TIME));
    speedQuery();         // send an M220 speed query
    ctrlFanQuery();      // send an M710 fan speed query
  }
}

//=========================================================================================
//TG ***** OPENING SCREEN after RESET, this version is customized for CNC application *****
//=========================================================================================
void menuStatus(void)
{
   //TG 1/12/20 added this code to modify menu for spindle or laser option
  StatusItems.items[0].icon = (infoSettings.laser_mode == 1) ? ICON_STATUS_LASER : ICON_STATUS_SPINDLE;
  StatusItems.items[0].label.index = (infoSettings.laser_mode == 1) ? LABEL_NULL : LABEL_NULL;
  
  // so we can track when spindle speed changes happen and display them quickly
  uint16_t lastCurrent = spindleGetCurSpeed(currentTool);
  uint16_t lastTarget = spindleGetSetSpeed(currentTool);
  KEY_VALUES key_num = KEY_IDLE;

  GUI_SetBkColor(infoSettings.bg_color);
  menuDrawPage(&StatusItems);
  GUI_SetColor(GANTRY_XYZ_BG_COLOR);
  GUI_FillPrect(&recGantry);
  drawStatus();                             // draws icons, gantry position, and fills in liveIcon data
  drawStatusScreenMsg();                    // draws the center status rectangle

  while (MENU_IS(menuStatus))
  {
    if (infoHost.connected != lastConnectionStatus)
    {
      statusScreen_setReady();
      lastConnectionStatus = infoHost.connected;
    }

    if (msgNeedRefresh)                     // time to update the status area?
      drawStatusScreenMsg();                // draws "Status" on message center

    if(nextWCSupdate){
      drawWCSinfo();                        //TG 10/4/22 put active workspace info in Upper Right title area
      nextWCSupdate = false;
      RAPID_PRINTING_COMM()                 // perform backend printing loop between drawing icons to avoid printer idling
    }

    scrollMsg();
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
        if(infoSettings.laser_mode == 0)    //TG 1/12/20 modified this code for spindle/laser
        {  
          spindleSetCurIndex(-1);           // set last used spindle index
          OPEN_MENU(menuSpindle);           //TG - stock SW calls menuHeat(-1) for NOZZLE here instead      
        }
        else
          OPEN_MENU(menuLaser);
        break;
      
      case KEY_ICON_1:
        //toolSetCurrentIndex(BED);         //TG only 1 vacuum allowed, so no need to set index
        OPEN_MENU(menuVacuum);              //TG - stock SW calls menuHeat(-2) for BED here instead         
        break;
      
      case KEY_ICON_2:
        OPEN_MENU(menuFan);
        break;
      
      case KEY_SPEEDMENU:
        SET_SPEEDMENUINDEX(0);
        OPEN_MENU(menuSpeed);
        break;

      #ifdef TFT70_V3_0
        case KEY_FLOWMENU:
          SET_SPEEDMENUINDEX(1);
          OPEN_MENU(menuSpeed);
          break;
      #endif

      case KEY_MAINMENU:
        OPEN_MENU(cncMenu);                 //TG - stock SW calls menuMain here instead
        break;

      case KEY_ICON_7:
        OPEN_MENU(menuPrint);
        break;

      case KEY_INFOBOX:
        OPEN_MENU(menuNotification);
      default:
        break;
    }

    // target speed changes will come from keypad or from Marlin "S0:" msg, which also updates CurSpeed
    // if there was a speed change(actual or target), capture it and update speeds in spindle icon
    if(lastCurrent != spindleGetCurSpeed(currentTool) || lastTarget != spindleGetSetSpeed(currentTool))
    {     
      lastCurrent = 0;
      drawSingleLiveIconLine(ICON_STATUS_SPINDLE, currentSpindleSpeedID);
      lastCurrent = spindleGetCurSpeed(currentTool);
      lastTarget = spindleGetSetSpeed(currentTool);
    }

    toggleTool();
    loopProcess();
  }

  coordinateQueryTurnOff();  // disable position auto report, if any
}
