//TG MODIFIED*****
#ifndef _AVRTRIAC_H_
#define _AVRTRIAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Configuration.h"
#ifdef USING_AVR_TRIAC_CONTROLLER

#include <stdint.h>
#include <stdbool.h>
#include "Settings.h"
#include "menu.h"
MENUITEMS triacItems;

uint8_t  pidSpeed;            //TG 7/17/22 new for PID preselects Fast,Normal,Slow


uint8_t pidVarIndex;    // ptr to KP, KI, or KD buttons

typedef enum retcodes{
  base        = 0x00,
  comp_7900   = 0x01,
  comp_7979   = 0x02,
  comp_7980   = 0x04,
  comp_7981   = 0x08,
  comp_7982   = 0x10,
  comp_7983   = 0x20,
  comp_7984   = 0x40,
 } msgcodes;

//  int16_t actTarget;
//  int16_t actCurrent;

//  char* spindleID[MAX_SPINDLE_COUNT];      
//  char* spindleDisplayID[MAX_SPINDLE_COUNT]; 
//  char* spindleCmd[MAX_SPINDLE_COUNT]; 
//  MENUITEMS spindleItems; 

//  //Icons list for on/off change  //TG 1/16/20 new for spindle  //TODO CHANGE ICONS
//  const ITEM itemSpindleONOFF[2];

//  void showSpeed(u8 index);
void menuTriac(void);       //TG 7/17/22 new
void updateTriacStatusDisplay(u8 index);
uint16_t get_chksum();
uint8_t TFTtoMARLIN_wait(enum retcodes retmsg);
void clrCancel();
void setCancel();
void popupConfirmCancel(uint8_t* title, uint8_t* msg);
void popupErrorOK(uint8_t* title, uint8_t* msg);
void readMarlinAVRInfoBlock();
void ResetAVRPopup();

extern uint8_t msg_complete;
extern uint8_t CancelFlag;

//  void toolSetCurrentIndex(uint8_t index);
//  void drawSpindleStatusInIcon(void);
// void updateSpeedStatusDisplay(u8 index, bool speed_only);
#endif // #ifdef USING_AVR_TRIAC_CONTROLLER

#ifdef __cplusplus
}
#endif

#endif // #ifndef _AVRTRIAC_H_
