#ifndef _PREHEAT_MENU_H_
#define _PREHEAT_MENU_H_
//TG modified

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "menu.h"
#include "Settings.h"
#include "Configuration.h"

typedef enum
{
  BOTH = 0,
  BED_PREHEAT = 1,
  TOOL0_PREHEAT = 2,
}TOOLPREHEAT;

void refreshPreheatIcon(PREHEAT_STORE * preheatStore, uint8_t index, bool redrawIcon);
void menuPreheat(void);

#ifdef __cplusplus
}
#endif

#endif
