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

typedef enum retcodes{
  base        = 0x00,
  comp_7988   = 0x01,
  comp_7989   = 0x02,
 } msgcodes;
 
void menuVFD(void);
char* toStr(float value, uint8_t dec_places);
uint8_t TFTtoMARLIN_wait(msgcodes retmsg);
void popupErrorOK(uint8_t* title, uint8_t* msg);

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
}iregBits;
extern iregBits inputReg;

extern bool VFDpresent;          // true if VFD connected
extern uint8_t vfdStatus;
extern float vfdP;
extern uint8_t msg_complete;
extern uint8_t CancelFlag;     // for general pop up msg box responses
extern float sw_ver;
extern float cpu_ver;
extern uint16_t f164;
extern uint16_t f165;

#endif // #ifdef USING_VFD_CONTROLLER

#ifdef __cplusplus
}
#endif

#endif // #ifndef _VFD_H_
