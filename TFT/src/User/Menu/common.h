//TG Modified
#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "Settings.h"
#include "menu.h"
//#include "Heat.h"     //TG 8/22/21 removed for CNC
#define ITEM_RPM_NUM            4   //TG 2/5/21
#define ITEM_PWM_NUM            3   //TG 2/14/21
#define ITEM_DEGREE_NUM         3
#define ITEM_SPEED_NUM          3
#define ITEM_PERCENT_STEPS_NUM  3
#define ITEM_MOVE_LEN_NUM       5
#define ITEM_FINE_MOVE_LEN_NUM  3
#define ITEM_EXT_LEN_NUM        5
#define ITEM_TOGGLE_NUM         2

typedef enum
{
  VALUE_NONE = 0,
  VALUE_BYTE,
  VALUE_INT,
  VALUE_FLOAT,
  VALUE_STRING,
} VALUE_TYPE;

typedef enum
{
  COLD = 0,
  SETTLING,
  HEATED,
} NOZZLE_STATUS;

extern SCROLL scrollLine;

extern uint8_t currentTool;         // current hotend index
extern uint8_t currentBCIndex;      // current bed/chamber index
extern uint8_t currentFan;          // current fan index
extern uint8_t currentSpeedFlowID;  //TG 3/2/23 renamed current speed/flow index
extern uint8_t currentSpindleSpeedID;  //TG 2/24/21 new
extern bool nextWCSupdate;      //TG 10/4/22 - added flag, it gets set whenever parseAck.c gets a workspace change msg from Marlin

extern const ITEM itemTool[MAX_TOOL_COUNT];
extern const ITEM itemSpindle[MAX_TOOL_COUNT];
extern const ITEM itemLaser[MAX_TOOL_COUNT];
extern const ITEM itemVacuum[MAX_TOOL_COUNT];
extern const ITEM itemDegreeSteps[ITEM_DEGREE_NUM]; //TG 1/16/20 remove after completed dev, replaced by RPM 
extern const u8 degreeSteps[ITEM_DEGREE_NUM];

extern const ITEM itemRPMSteps[ITEM_RPM_NUM];       //TG 1/16/20 new for RPM
extern const u16 RPMSteps[ITEM_RPM_NUM];

extern const ITEM itemPWMSteps[ITEM_PWM_NUM]; 
extern const uint8_t PWMSteps[ITEM_PWM_NUM];

extern const ITEM itemSpeed[ITEM_SPEED_NUM];

extern const ITEM itemPercent[ITEM_PERCENT_STEPS_NUM];
extern const uint8_t percentSteps[ITEM_PERCENT_STEPS_NUM];

extern const ITEM itemMoveLen[ITEM_MOVE_LEN_NUM];
extern const float moveLenSteps[ITEM_MOVE_LEN_NUM];

extern const ITEM itemExtLenSteps[ITEM_EXT_LEN_NUM];
extern const float extlenSteps[ITEM_EXT_LEN_NUM];

extern const LABEL itemToggle[ITEM_TOGGLE_NUM];
extern const uint16_t iconToggle[ITEM_TOGGLE_NUM];

// Check if next screen update is due
bool nextScreenUpdate(uint32_t duration);

extern const bool warmupTemperature(uint8_t toolIndex, void (* callback)(void));

#ifdef FRIENDLY_Z_OFFSET_LANGUAGE
  void invertZAxisIcons(MENUITEMS * menuItems);

  #define  INVERT_Z_AXIS_ICONS(menuItemsPtr) invertZAxisIcons(menuItemsPtr)
#else
  #define  INVERT_Z_AXIS_ICONS(menuItemsPtr)
#endif

void drawBorder(const GUI_RECT *rect, uint16_t color, uint16_t edgeDistance);

void drawBackground(const GUI_RECT *rect, uint16_t bgColor, uint16_t edgeDistance);

void drawStandardValue(const GUI_RECT *rect, VALUE_TYPE valType, const void *val, uint16_t font,
                       uint16_t color, uint16_t bgColor, uint16_t edgeDistance, bool clearBgColor);

// Show/draw temperature in a standard menu
void temperatureReDraw(uint8_t toolIndex, int16_t * temp, bool drawHeader);

// Show/draw fan in a standard menu
void fanReDraw(uint8_t fanIndex, bool drawHeader);

// Show/draw extruder in a standard menu
void extruderReDraw(uint8_t extruderIndex, float extrusion, bool drawHeader);

// Show/draw percentage in a standard menu
void percentageReDraw(uint8_t itemIndex, bool drawHeader);

// Edit temperature in a standard menu
int32_t editIntValue(int32_t minValue, int32_t maxValue, int32_t resetValue, int32_t value);

// Edit a float value in a standard menu
float editFloatValue(float minValue, float maxValue, float resetValue, float value);

NOZZLE_STATUS warmupNozzle(void);

#ifdef SAFETY_ALERT
  const void cooldownTemperature(void);

  #define COOLDOWN_TEMPERATURE() cooldownTemperature()
#else
  #define COOLDOWN_TEMPERATURE()
#endif

// Show/draw Active Workspace in title bar RH side
void drawWCSinfo();    //TG 10/4/22 - added new to support displaying active WCS in upper right of titlebar

#ifdef __cplusplus
}
#endif

#endif
