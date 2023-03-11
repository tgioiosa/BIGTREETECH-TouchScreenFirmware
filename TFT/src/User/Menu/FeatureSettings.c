#include "FeatureSettings.h"
#include "includes.h"
#include "ListItem.h"   //TG 3/2/23 added

static uint16_t fe_cur_page = 0;

// parameter values

#define ONOFF_NUM   2      //TG 2/5/21 new for ON/OFF texts
const LABEL OnOffitem[ONOFF_NUM] = {LABEL_OFF, LABEL_ON};

#define PCT_RPM_NUM   2      //TG 2/5/21 new for RPM/PCT texts
const LABEL PctRpmitem[PCT_RPM_NUM] = {LABEL_RPM , LABEL_PCT};

#define CUTTER_PWR_UNIT_NUM  CUTTER_PWR_SIZE      //TG 2/5/21 new for MRPM, MPWM, MPCT texts
const LABEL cutterPwrUnititem[CUTTER_PWR_UNIT_NUM] = {LABEL_RPM, LABEL_PCT, LABEL_PWM};

#define SPINDLE_SPIN_NUM 2  //TG CW/CCW for spindle rotation
const LABEL itemSpindleSpin[SPINDLE_SPIN_NUM] = {
                                              //item value text(only for custom value)
                                              LABEL_CCW,
                                              LABEL_CW
                                            };

#define ITEM_TOGGLE_AUTO_NUM 3
const LABEL itemToggleAuto[ITEM_TOGGLE_AUTO_NUM] =
{
  LABEL_OFF,
  LABEL_ON,
  LABEL_AUTO
};

#define ITEM_TOGGLE_SMART_NUM 2
const LABEL itemToggleSmart[ITEM_TOGGLE_SMART_NUM] =
{
  LABEL_ON,
  LABEL_SMART
};

// add key number index of the items
typedef enum
{
  SKEY_TERMINAL_ACK = 0,
  SKEY_SHOULD_M0_PAUSE,   //TG 10/3/22 new
  SKEY_PERSISTENT_INFO,
  SKEY_FILE_LIST_MODE,
  SKEY_SPIN,              //TG 1/12/20 new
  SKEY_SPINDLE_RMAX,      //TG 2/5/21 new
  SKEY_SPINDLE_PMAX,      //TG 2/5/21 new
  SKEY_LCD_DISP_UNIT,     //TG 2/5/21 new
  SKEY_CUTTER_POWER_UNIT, //TG 2/14/21 new
  SKEY_SPINDLE_USE_PID,   //TG 9/27/21 new
  SKEY_LASER,             //TG 1/12/20 new
  SKEY_SERIAL_ALWAYS_ON,
  SKEY_SPEED,
  SKEY_AUTO_LOAD_LEVELING,
  SKEY_FAN_SPEED_PERCENT,
  SKEY_PROBING_Z_OFFSET,
  SKEY_Z_STEPPERS_ALIGNMENT,

  SKEY_EMULATED_M600,
  SKEY_EMULATED_M109_M190,
  SKEY_EVENT_LED,
  SKEY_FILE_COMMENT_PARSING,
  
  #ifdef PS_ON_PIN
    SKEY_PS_AUTO_SHUTDOWN,
  #endif

  #ifdef FIL_RUNOUT_PIN
    SKEY_FIL_RUNOUT,
  #endif

  SKEY_PL_RECOVERY,
  SKEY_PL_RECOVERY_HOME,
  SKEY_BTT_MINI_UPS,
  SKEY_START_GCODE_ENABLED,
  SKEY_END_GCODE_ENABLED,
  SKEY_CANCEL_GCODE_ENABLED,
  #ifdef LCD_LED_PWM_CHANNEL
    SKEY_LCD_BRIGHTNESS,
    SKEY_LCD_BRIGTHNESS_DIM,
    SKEY_LCD_DIM_IDLE_TIMER,
  #endif
  #ifdef ST7920_SPI
    SKEY_ST7920_FULLSCREEN,
  #endif
  SKEY_RESET_SETTINGS,        // Keep reset always at the bottom of the settings menu list.
  SKEY_COUNT                  // keep this always at the end
} SKEY_LIST;

// perform action on button press - get here when a listview item is pressed and take action to modify values as needed
void updateFeatureSettings(uint8_t item_index)
{
  uint8_t itemPos = item_index % LISTITEM_PER_PAGE;   // position used by setDynamic functions can only range 0-5
  switch (item_index)
  { // the menuFeatureSettings will update the icons by calling listViewRefreshItem() after calling here
    case SKEY_TERMINAL_ACK:
      TOGGLE_BIT(infoSettings.terminal_ack,0);break;
    case SKEY_SHOULD_M0_PAUSE:
      TOGGLE_BIT(infoSettings.should_M0_pause,0); break;
    case SKEY_PERSISTENT_INFO:
      TOGGLE_BIT(infoSettings.persistent_info,0); break;
    case SKEY_FILE_LIST_MODE:
      TOGGLE_BIT(infoSettings.files_list_mode,0); break;
    case SKEY_SPIN:
      infoSettings.spin_dir = (infoSettings.spin_dir +1) % SPINDLE_SPIN_NUM; break;
    case SKEY_SPINDLE_RMAX:      //TG 2/5/21 new
    {
      uint32_t val = infoSettings.spindle_rpm_max[0];    // get current parameter value
      val = numPadInt(NULL, val, val, false);            // show numpad menu to get new val
      val = NOBEYOND(0,val,default_max_spindleRPM[0]);   // apply limits
      //sprintf(tempstr,(char *)textSelect(LABEL_SPINDLE_RMAX ),val);   //TG 2/13/21 needed for Marlin mode?
      infoSettings.spindle_rpm_max[0] = val;             // store new value
      setDynamicIntValue(itemPos, val);                  
      listViewRefreshMenu();  // restore prev menu -  was menuDrawListPage(&featureSettingsItems) in prior version
      break;
    }
    case SKEY_SPINDLE_PMAX:      //TG 2/5/21 new
    {
      //char tempstr[8];
      uint16_t val = infoSettings.spindle_pwm_max[0];    // get current parameter value
      val = numPadInt(NULL, val, val, false);
      val = NOBEYOND(0,val,default_max_spindlePWM[0]);
      //sprintf(tempstr,(char *)textSelect(LABEL_SPINDLE_PMAX ),val);   //TG 2/13/21 needed for Marlin mode?
      infoSettings.spindle_pwm_max[0] = val;
      setDynamicIntValue(itemPos, val);
      listViewRefreshMenu();  //menuDrawListPage(&featureSettingsItems) in prior version
      break;
    }
    case SKEY_LCD_DISP_UNIT:     //TG 2/5/21 new
      infoSettings.cutter_disp_unit = (infoSettings.cutter_disp_unit + 1) % CUTTER_PWR_UNIT_NUM; break;
    case SKEY_CUTTER_POWER_UNIT: //TG 2/14/21 new
      infoSettings.cutter_power_unit = (infoSettings.cutter_power_unit + 1) % CUTTER_PWR_UNIT_NUM; break;
    case SKEY_SPINDLE_USE_PID:   //TG 9/27/21 new
      //TG 7/22/22 don't modify spindle_use_pid, Marlin has precedence, we're basically disabling this Feature Setting
      //AVRInfoBlock.spindle_use_pid = (AVRInfoBlock.spindle_use_pid + 1) % TOGGLE_NUM;
      TOGGLE_BIT(AVRInfoBlock.PIDFLAG, 0);
      //TG 7/22/22 don't send this to Marlin, Marlin has precedence, we're basically disabling this Feature Setting
      //storeCmd("%s S%d \n", "M7979", AVRInfoBlock.spindle_use_pid);
    case SKEY_LASER:             //TG 1/12/20 new
      TOGGLE_BIT(infoSettings.laser_mode,0); break;
    case SKEY_SERIAL_ALWAYS_ON:
      TOGGLE_BIT(infoSettings.serial_always_on, 0); break;
    case SKEY_SPEED:
      infoSettings.move_speed = (infoSettings.move_speed + 1) % ITEM_SPEED_NUM; break;
    case SKEY_AUTO_LOAD_LEVELING:
      TOGGLE_BIT(infoSettings.auto_load_leveling, 0); break;
    case SKEY_FAN_SPEED_PERCENT:
      TOGGLE_BIT(infoSettings.fan_percentage, 0); break;
    case SKEY_PROBING_Z_OFFSET:
      TOGGLE_BIT(infoSettings.probing_z_offset, 0); break;
    case SKEY_Z_STEPPERS_ALIGNMENT:
      TOGGLE_BIT(infoSettings.z_steppers_alignment, 0); break;
    case SKEY_EMULATED_M600:
    case SKEY_EMULATED_M109_M190:
    case SKEY_EVENT_LED:
    case SKEY_FILE_COMMENT_PARSING:
      TOGGLE_BIT(infoSettings.general_settings, ((item_index - SKEY_EMULATED_M600) + INDEX_EMULATED_M600));
      break;
#ifdef PS_ON_PIN
    case SKEY_PS_AUTO_SHUTDOWN:
      infoSettings.auto_shutdown = (infoSettings.auto_shutdown + 1) % ITEM_TOGGLE_AUTO_NUM;
      break;
#endif
#ifdef FIL_RUNOUT_PIN
    case SKEY_FIL_RUNOUT:
      infoSettings.runout ^= (1U << 0);
      break;
#endif
    case SKEY_PL_RECOVERY:
      TOGGLE_BIT(infoSettings.plr, 0);
      break;
    case SKEY_PL_RECOVERY_HOME:
      TOGGLE_BIT(infoSettings.plr_home, 0);
      break;
    case SKEY_BTT_MINI_UPS:
      TOGGLE_BIT(infoSettings.btt_ups, 0);
      break;

    case SKEY_START_GCODE_ENABLED:
    case SKEY_END_GCODE_ENABLED:
    case SKEY_CANCEL_GCODE_ENABLED:
      TOGGLE_BIT(infoSettings.send_gcodes, (item_index - SKEY_START_GCODE_ENABLED));
      break;
#ifdef LCD_LED_PWM_CHANNEL
    case SKEY_LCD_BRIGHTNESS:
      {
        infoSettings.lcd_brightness = (infoSettings.lcd_brightness + 1) % LCD_BRIGHTNESS_COUNT;
        if(infoSettings.lcd_brightness == 0)
          infoSettings.lcd_brightness = 1;      // In Normal it should not be off. Set back to 5%

        char tempstr[8];
        sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), lcd_brightness[infoSettings.lcd_brightness]);
        setDynamicTextValue(itemPos, tempstr);  // update here to display as LABEL_DYNAMIC option in list view
        LCD_SET_BRIGHTNESS(lcd_brightness[infoSettings.lcd_brightness]);
        break;
      }
    case SKEY_LCD_BRIGTHNESS_DIM:
      {
        infoSettings.lcd_idle_brightness = (infoSettings.lcd_idle_brightness + 1) % LCD_BRIGHTNESS_COUNT;
        char tempstr[8];
        sprintf(tempstr,(char *)textSelect(LABEL_PERCENT_VALUE),lcd_brightness[infoSettings.lcd_idle_brightness]);
        setDynamicTextValue(itemPos,tempstr);   // update here to display as LABEL_DYNAMIC option in list view
        LCD_SET_BRIGHTNESS(lcd_brightness[infoSettings.lcd_brightness]);
        break;
      }
    case SKEY_LCD_DIM_IDLE_TIMER:
      infoSettings.lcd_idle_time = (infoSettings.lcd_idle_time + 1) % LCD_IDLE_TIME_COUNT;
      break;
#endif //LCD_LED_PWM_CHANNEL
#ifdef ST7920_SPI
    case SKEY_ST7920_FULLSCREEN:
      TOGGLE_BIT(infoSettings.marlin_fullscreen,0);
      break;
#endif

    case SKEY_RESET_SETTINGS:
      popupDialog(DIALOG_TYPE_ALERT, LABEL_SETTINGS_RESET, LABEL_SETTINGS_RESET_INFO, LABEL_CONFIRM, LABEL_CANCEL, resetSettings, NULL, NULL);
      break;

    default:
      return;
  }
}  // updateFeatureSettings

// load values on page change and reload - this is usually called via a function pointer(*action_prepareItem) 
// from listViewCreate(), listViewSetCurPage, or listViewRefreshItem
void loadFeatureSettings(LISTITEM * item, uint16_t item_index, uint8_t itemPos)
{
  if (item_index < SKEY_COUNT)
  {
    switch (item_index)
    {
    case SKEY_TERMINAL_ACK:
      item->icon = iconToggle[infoSettings.terminal_ack];             // just ON/OFF icon, no value display
      break;
    case SKEY_SHOULD_M0_PAUSE:
      item->icon = iconToggle[infoSettings.should_M0_pause];          // just ON/OFF icon, no value display
      break;
    case SKEY_PERSISTENT_INFO:
      item->icon = iconToggle[infoSettings.persistent_info];          // just ON/OFF icon, no value display
      break;
    case SKEY_FILE_LIST_MODE:
      item->icon = iconToggle[infoSettings.files_list_mode];          // just ON/OFF icon, no value display
      break;
    case SKEY_SPIN:
      item->valueLabel = itemSpindleSpin[infoSettings.spin_dir];      // sets valueLabel to LABEL_CW or LABEL_CCW
      break;
    case SKEY_SPINDLE_RMAX:      //TG 2/5/21 new
      setDynamicIntValue(itemPos,infoSettings.spindle_rpm_max[0]);    // must be set here to display text value via LABEL_DYNAMIC option in list view
      break;
    case SKEY_SPINDLE_PMAX:      //TG 2/5/21 new
      setDynamicIntValue(itemPos,infoSettings.spindle_pwm_max[0]);    // must be set here to display text value via LABEL_DYNAMIC option in list view
      break;
    case SKEY_LCD_DISP_UNIT:     //TG 2/5/21 new
      item->valueLabel = cutterPwrUnititem[infoSettings.cutter_disp_unit];  // set valueLabel to LABEL_RPM, LABEL_PCT, or LABEL_PWM
      break;
    case SKEY_CUTTER_POWER_UNIT: //TG 2/14/21 new
      item->valueLabel = cutterPwrUnititem[infoSettings.cutter_power_unit]; // set valueLabel to LABEL_RPM, LABEL_PCT, or LABEL_PWM
      break;
    case SKEY_SPINDLE_USE_PID:   //TG 9/27/21 new
      item->icon = iconToggle[AVRInfoBlock.PIDFLAG];                  // just ON/OFF icon, no value display
      break;
    case SKEY_LASER:             //TG 1/12/20 new
      item->icon = iconToggle[infoSettings.laser_mode];               // just ON/OFF icon, no value display
      break;
    /*  
    case SKEY_INVERT_X:
      settingPage[item_index].icon = toggleitem[infoSettings.invert_axis[X_AXIS]];
      break;
    case SKEY_INVERT_Y:
      settingPage[item_index].icon = toggleitem[infoSettings.invert_axis[Y_AXIS]];
      break;
    case SKEY_INVERT_Z:
      settingPage[item_index].icon = toggleitem[infoSettings.invert_axis[Z_AXIS]];
      break;
    case SKEY_ACK_NOTIFICATION:
      setDynamicTextValue(i, (char *)itemNotificationType[infoSettings.ack_notification]);
      break;
    */
    case SKEY_SERIAL_ALWAYS_ON:
      item->icon = iconToggle[infoSettings.serial_always_on];         // just ON/OFF icon, no value display
      break;
    case SKEY_SPEED:
      item->valueLabel = itemSpeed[infoSettings.move_speed].label;    // set valueLabel to LABEL_SLOW, LABEL_NORMAL or LABEL_FAST
      break;
    case SKEY_AUTO_LOAD_LEVELING:
      item->icon = iconToggle[infoSettings.auto_load_leveling];       // just ON/OFF icon, no value display
      break;
    case SKEY_FAN_SPEED_PERCENT:
      item->icon = iconToggle[infoSettings.fan_percentage];           // just ON/OFF icon, no value display
      break;
  // case SKEY_XY_OFFSET_PROBING:
  //   settingPage[item_index].icon = iconToggle[infoSettings.xy_offset_probing];
  //   break;
    case SKEY_PROBING_Z_OFFSET:
      item->icon = iconToggle[infoSettings.probing_z_offset];         // just ON/OFF icon, no value display
      break;
    case SKEY_Z_STEPPERS_ALIGNMENT:
      item->icon = iconToggle[infoSettings.z_steppers_alignment];     // just ON/OFF icon, no value display
      break;
    case SKEY_EMULATED_M600:
    case SKEY_EMULATED_M109_M190:
    case SKEY_EVENT_LED:
    case SKEY_FILE_COMMENT_PARSING:
      item->icon = iconToggle[GET_BIT(infoSettings.general_settings, ((item_index - SKEY_EMULATED_M600) + INDEX_EMULATED_M600))];  // just ON/OFF icon, no value display
      break;
      
  #ifdef PS_ON_PIN
    case SKEY_PS_AUTO_SHUTDOWN:
      item->valueLabel = itemToggleAuto[infoSettings.auto_shutdown];  // set valueLabel to LABEL_OFF, LABEL_ON, or LABEL_AUTO
      break;
  #endif

  #ifdef FIL_RUNOUT_PIN
    case SKEY_FIL_RUNOUT:
    {
      LABEL sensorLabel = itemToggleSmart[GET_BIT(infoSettings.runout, 1)];
      item->valueLabel.index = (GET_BIT(infoSettings.runout, 0)) ? sensorLabel.index : LABEL_OFF ; // set valueLabel to LABEL_ON, LABEL_SMART or LABEL_OFF
      break;
    }
  #endif

    case SKEY_PL_RECOVERY:
      item->icon = iconToggle[infoSettings.plr];                      // just ON/OFF icon, no value display
      break;
    case SKEY_PL_RECOVERY_HOME:
      item->icon = iconToggle[infoSettings.plr_home];                 // just ON/OFF icon, no value display
      break;
    case SKEY_BTT_MINI_UPS:
      item->icon = iconToggle[infoSettings.btt_ups];                  // just ON/OFF icon, no value display
      break;
    case SKEY_START_GCODE_ENABLED:
    case SKEY_END_GCODE_ENABLED:
    case SKEY_CANCEL_GCODE_ENABLED:
      item->icon = iconToggle[GET_BIT(infoSettings.send_gcodes, (item_index - SKEY_START_GCODE_ENABLED))];    // just ON/OFF icon, no value display
      break;
  /*
  #ifdef LED_COLOR_PIN
    case SKEY_KNOB_LED_COLOR:
      settingPage[item_index].valueLabel = itemLedcolor[infoSettings.knob_led_color];
      featureSettingsItems.items[i] = settingPage[item_index];
      break;

    #ifdef LCD_LED_PWM_CHANNEL
      case SKEY_KNOB_LED_IDLE:
        settingPage[item_index].icon = iconToggle[infoSettings.knob_led_idle];
        break;
    #endif
  #endif
  */   
#ifdef LCD_LED_PWM_CHANNEL
    case SKEY_LCD_BRIGHTNESS:
      {
        char tempstr[8];
        sprintf(tempstr, (char *)textSelect(LABEL_PERCENT_VALUE), lcd_brightness[infoSettings.lcd_brightness]);  // set here to display text value via LABEL_DYNAMIC option in list view 
        setDynamicTextValue(itemPos, tempstr);  // set here to display text value from lcd_brightness array via LABEL_DYNAMIC option in list view 
        break;
      }
    case SKEY_LCD_BRIGTHNESS_DIM:
      {
        char tempstr[8];
        sprintf(tempstr,(char *)textSelect(LABEL_PERCENT_VALUE),lcd_brightness[infoSettings.lcd_idle_brightness]);
        setDynamicTextValue(itemPos,tempstr);   // set here to display text value lcd_brightness array via LABEL_DYNAMIC option in list view 
        break;
      }
    case SKEY_LCD_DIM_IDLE_TIMER:
      item->valueLabel = lcd_idle_time_names[infoSettings.lcd_idle_time];   // set valueLabel from lcd_idle_time_names array of LABEL
      break;
#endif //LCD_LED_PWM_CHANNEL
#ifdef ST7920_SPI
    case SKEY_ST7920_FULLSCREEN:
      item->icon = iconToggle[infoSettings.marlin_fullscreen];          // just ON/OFF icon, no value display
      break;
#endif

    case SKEY_RESET_SETTINGS:
      break;

    default:
      break;
    }
  }
}  // loadFeatureSettings

void resetSettings(void)
{
  initSettings();
  storePara();
  popupReminder(DIALOG_TYPE_SUCCESS, LABEL_INFO, LABEL_SETTINGS_RESET_DONE);
}

void menuFeatureSettings(void)
{
  LABEL title = {LABEL_FEATURE_SETTINGS};

  // set item types
  LISTITEM settingPage[SKEY_COUNT] = {
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_TERMINAL_ACK,             LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_SHOULD_M0_PAUSE,          LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PERSISTENT_INFO,          LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_FILES_LIST_MODE,          LABEL_NULL},
  {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_SPINDLE_ROTATION,         LABEL_CW},          //TG 1/12/20 new
  {CHARICON_SETTING1,    LIST_CUSTOMVALUE,   LABEL_SPINDLE_RMAX ,            LABEL_DYNAMIC},     //TG 1/12/20 new
  {CHARICON_SETTING1,    LIST_CUSTOMVALUE,   LABEL_SPINDLE_PMAX ,            LABEL_DYNAMIC},     //TG 1/12/20 new
  {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_LCD_POWER_UNIT,           LABEL_RPM},         //TG 1/12/20 new
  {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_CUTTER_POWER_UNIT,        LABEL_RPM},         //TG 1/14/20 new
  {CHARICON_BLANK,       LIST_TOGGLE,        LABEL_SPINDLE_USE_PID,          LABEL_OFF},         //TG 9/27/21 new
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_LASER_MODE,               LABEL_NULL},        //TG 1/12/20 new
  //{ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_INVERT_XAXIS,           LABEL_BACKGROUND},
  //{ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_INVERT_YAXIS,           LABEL_BACKGROUND},
  //{ICONCHAR_TOGGLE_ON,   LIST_TOGGLE,        LABEL_INVERT_ZAXIS,           LABEL_BACKGROUND},
  //{ICONCHAR_BLANK,       LIST_CUSTOMVALUE,   LABEL_ACK_NOTIFICATION,       LABEL_DYNAMIC},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_SERIAL_ALWAYS_ON,         LABEL_NULL},
  {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_MOVE_SPEED,               LABEL_NORMAL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_AUTO_LOAD_LEVELING,       LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_FAN_SPEED_PERCENTAGE,     LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PROBING_Z_OFFSET,         LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_Z_STEPPERS_ALIGNMENT,     LABEL_NULL},
  
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_EMULATED_M600,            LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_EMULATED_M109_M190,       LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_EVENT_LED,                LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_FILE_COMMENT_PARSING,     LABEL_NULL},
  
#ifdef PS_ON_PIN
  {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_PS_AUTO_SHUTDOWN,         LABEL_OFF},
#endif

#ifdef FIL_RUNOUT_PIN
  {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_FIL_RUNOUT,               LABEL_OFF},
#endif

  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PL_RECOVERY,              LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PL_RECOVERY_HOME,         LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_BTT_MINI_UPS,             LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_START_GCODE_ENABLED,      LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_END_GCODE_ENABLED,        LABEL_NULL},
  {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_CANCEL_GCODE_ENABLED,     LABEL_NULL},

  #ifdef LCD_LED_PWM_CHANNEL
    {CHARICON_BLANK,      LIST_CUSTOMVALUE,   LABEL_LCD_BRIGHTNESS,          LABEL_DYNAMIC},
    {CHARICON_BLANK,      LIST_CUSTOMVALUE,   LABEL_LCD_IDLE_BRIGHTNESS,     LABEL_DYNAMIC},
    {CHARICON_BLANK,      LIST_CUSTOMVALUE,   LABEL_LCD_IDLE_TIME,           LABEL_DYNAMIC},
  #endif
  #ifdef ST7920_SPI
    {CHARICON_BLANK,      LIST_TOGGLE,        LABEL_MARLIN_FULLSCREEN,       LABEL_OFF},  //TG fix for V27
  #endif
  // Keep reset settings always at the bottom of the settings menu list.
  {CHARICON_BLANK,       LIST_MOREBUTTON,    LABEL_SETTINGS_RESET,           LABEL_NULL}

  };

  uint16_t index = KEY_IDLE;

  listViewCreate(title, settingPage, SKEY_COUNT, &fe_cur_page, true, NULL, loadFeatureSettings);

  while (MENU_IS(menuFeatureSettings))
  {
    index = listViewGetSelectedIndex();

    if (index < SKEY_COUNT)
    {
      updateFeatureSettings(index);
      listViewRefreshItem(index);
    }

    loopProcess();
  }

  saveSettings();  // Save settings
}
