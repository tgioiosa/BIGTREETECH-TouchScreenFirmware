//TG MODIFIED*****
#ifndef _PROBESTOCK_H_
#define _PROBESTOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "coordinate.h"
#include "Configuration.h"

float getProbeThickness();
float getMIN_Z_Position();
float probeDowntoTarget(float target, char* tname, char* title, char* msg);
float doProbeSpoilBoard(bool skipProbeThk, bool skipMinZ);
void doProbeToolLength(bool skipProbeThk);
float doProbeStock(bool skipProbeThk, bool skipMinZ, bool skipTool);
void doHomeAll();
void menuProbeStock(void);
void updateProbeStockDisplay(void);
void marlinError(void);
void displayProgress(char* msg);
void displayMessage(char* msg, bool clear);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _PROBESTOCK_H_