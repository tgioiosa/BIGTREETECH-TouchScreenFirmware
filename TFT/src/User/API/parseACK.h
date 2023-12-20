#ifndef _PARSE_ACK_H_
#define _PARSE_ACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "SerialConnection.h"

void setHostDialog(bool isHostDialog);
bool getHostDialog(void);
void setCurrentAckSrc(SERIAL_PORT_INDEX portIndex);
void parseACK(void);
void parseRcvGcode(void);

extern const char magic_error[];    //TG 12/19/23 make available to other modules
extern const char magic_warning[];  //TG 12/19/23 make available to other modules

#ifdef __cplusplus
}
#endif

#endif
