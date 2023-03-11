//TG MODIFIED*****
#ifndef _VFD_H_
#define _VFD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Configuration.h"
#ifdef USING_VFD_CONTROLLER

#include <stdint.h>
#include <stdbool.h>
#include "Settings.h"
#include "menu.h"

extern uint8_t msg_complete;
// enum codes for gcodeSendAndWaitForAnswer
typedef enum retcodes{
  base        = 0x0,
  comp_7900,
  comp_7979,
  comp_7980,
  comp_7981,
  comp_7982,
  comp_7983,
  comp_7984,
  comp_7986,
  comp_7988,
  comp_7989,
  } msgcodes;

void menuVFD(void);
char* toStr(float value, uint8_t dec_places);
uint8_t TFTtoMARLIN_wait(msgcodes retmsg);
// these two also are defined in AVRTriac.c but only one of vfd.c or AVRTriac.c is active at any time
void popupErrorOK(uint8_t* title, uint8_t* msg);
void popupConfirmCancel(uint8_t* title, uint8_t* msg);
void popupInfoOKOnly(uint8_t* title, uint8_t* msg);
void popupQuestionOK(uint8_t* title, uint8_t* msg);
void popupSuccessOKOnly(uint8_t* title, uint8_t* msg);

typedef struct 
{
  uint16_t  freq_out;
  uint16_t  freq_set;
  uint16_t  current_out;
  uint16_t  speed_out;
  uint16_t  dc_voltage;
  uint16_t  ac_voltage;
  uint16_t  temperature;
  uint16_t  counter;
  uint16_t  PID_target;
  uint16_t  PID_feedback;
  uint16_t  fault_code;
  uint16_t  total_hours;
  void (*clear)();            // function ptr can be assigned here
}iregBits;
extern iregBits inputReg;

extern bool VFDpresent;       // true if VFD connected
extern uint8_t vfdStatus;
extern float vfdP;
extern uint8_t CancelFlag;    // for general pop up msg box responses
extern float sw_ver;
extern float cpu_ver;
extern uint16_t f164;
extern uint16_t f165;

#endif // #ifdef USING_VFD_CONTROLLER

#ifdef __cplusplus
}
#endif

#endif // #ifndef _VFD_H_
