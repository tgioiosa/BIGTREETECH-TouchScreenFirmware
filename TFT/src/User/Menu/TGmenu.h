#ifndef _TGMENU_H_
#define _TGMENU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void menuTGmenu(void);
void send_adc_offset_to_Marlin();
void popupInfoOKOnly(uint8_t* title, uint8_t* msg);
void popupQuestionOK(uint8_t* title, uint8_t* msg);

#ifdef __cplusplus
}
#endif

#endif
