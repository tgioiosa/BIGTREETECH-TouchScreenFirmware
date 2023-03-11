#include "Home.h"
#include "includes.h"

const MENUITEMS homeItems = {
  // title
  LABEL_HOME,
  // icon                         label
  {
    {ICON_HOME,                    LABEL_HOME},
    {ICON_X_HOME,                  LABEL_X},
    {ICON_Y_HOME,                  LABEL_Y},
    {ICON_Z_HOME,                  LABEL_Z},
    {ICON_NULL,                    LABEL_NULL},
    {ICON_NULL,                    LABEL_NULL},
    {ICON_NULL,                    LABEL_NULL},
    {ICON_BACK,                    LABEL_BACK},
   }
};

const MENUITEMS cncHomeItems = {        //tg 1/12/20 added the new MENUITEMS cncHomeItems
//   title
LABEL_HOME,
// icon                       label
 {{ICON_HOME,                 LABEL_XY},
  {ICON_Z_HOME,               LABEL_Z},
  #if M_ANY(USING_VFD_CONTROLLER, USING_AVRTRIAC_CONTROLLER)
    {ICON_PROBE_STOCK,          LABEL_PROBE_STOCK},
  #else
    {ICON_NULL,           LABEL_NULL},
  #endif
  {ICON_NULL,           LABEL_NULL},
  {ICON_ZERO_X,               LABEL_ZERO_X},
  {ICON_ZERO_Y,               LABEL_ZERO_Y},
  {ICON_ZERO_Z,               LABEL_ZERO_Z},
  {ICON_BACK,                 LABEL_BACK},}
};


void menuHome(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&cncHomeItems);

  while (MENU_IS(menuHome))
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
        case KEY_ICON_0: storeCmd("G28 XY\n");   break;
        case KEY_ICON_1:
          storeCmd("G28 Z\n");
          if(infoSettings.touchplate_on == 1)
          {
            storeCmd("G92 Z%.3f\n", infoSettings.touchplate_height);
          }
          break;
        case KEY_ICON_2:
          #if M_ANY(USING_VFD_CONTROLLER, USING_AVRTRIAC_CONTROLLER)
            OPEN_MENU(menuProbeStock);
          #endif
        case KEY_ICON_4: storeCmd("G92 X0\n"); break;
        case KEY_ICON_5: storeCmd("G92 Y0\n"); break;
        case KEY_ICON_6: storeCmd("G92 Z0\n"); break;
        case KEY_ICON_7: CLOSE_MENU();      break;
        default:break;
    }

    loopProcess();
  }
}
