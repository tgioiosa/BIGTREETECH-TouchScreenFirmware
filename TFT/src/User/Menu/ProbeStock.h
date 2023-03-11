//TG MODIFIED*****
#ifndef _PROBESTOCK_H_
#define _PROBESTOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "coordinate.h"
#include "Configuration.h"

void menuProbeStock(void);
void doProbeStock(void);
void stopProbeStock(void);
void updateProbeStockDisplay(void);
void marlinError(void);
void displayProgress(char* msg);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _PROBESTOCK_H_