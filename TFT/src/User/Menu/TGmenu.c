
#include "TGmenu.h"
#include "includes.h"
  
//extern char gcodeBuf[CMD_MAX_CHAR];
//char* gcodeBufPtr = &gcodeBuf[0];

COORDINATE oldpos;  //TG 5/16/23 for remembering xyz position before/after nozzle clean

void menuTGmenu(void)
{
  //TG examples of variable usage
/*
static uint8_t ublSlot;
static bool ublIsSaving = true;
static bool ublSlotSaved = false;
*/

  MENUITEMS TGmenuItems = {
    // title
    LABEL_TGMENU,
    // icon                         label
    {{ICON_Z_0,                     LABEL_Z_0},
     {ICON_Z_300,                   LABEL_Z_300},
     {ICON_M503,                    LABEL_M503},
     {ICON_NOZZLE_CLEAN,            LABEL_NOZZLE_CLEAN},
     {ICON_NULL,                    LABEL_NULL},
     {ICON_NULL,                    LABEL_NULL},
     {ICON_NULL,                    LABEL_NULL},
     {ICON_BACK,                    LABEL_BACK}}
  };

  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&TGmenuItems);

  while (infoMenu.menu[infoMenu.cur] == menuTGmenu)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        storeCmd("G1 Z0\n");
        break;

      case KEY_ICON_1:
        storeCmd("G1 Z300\n");
        break;

      case KEY_ICON_2:
        storeCmd("M503\n");
        infoMenu.menu[++infoMenu.cur] = menuTerminal;
        break;
      
      case KEY_ICON_3:  //TG 5/14/23 added Nozzle Clean
      //TG the cleaning boundary is preset in Marlin Configuration.h, currently (34,309) - (70,315) 
      //TG - the Z position defaults to 4mm above bed surface. You can specify the 'F' parameter 
      //when calling G12 to force a fixed Z-level of your choice, it will override the default!
        coordinateGetAllActual(&oldpos);  // save current position
        storeCmd("G1 Z20 F%d\n",infoSettings.z_speed[infoSettings.move_speed]);  // get Z up high to avoid any obstacles                
        storeCmd("M211 S0\n");            // disable soft endstops
        storeCmd("G12 P1 S2 T5 F1.2\n");  // 5 triangles repeat 2 times 'F' here specifies Z-height of 1.2
        storeCmd("G12 P0 S3 F1.2\n");     // Horiz line repeat 3 times  'F' here specifies Z-height of 1.2
        storeCmd("G12 P0 S3 F1.0\n");     // Horiz line repeat 3 times  'F' here specifies Z-height of 1.0
        storeCmd("M211 S1\n");            // enable soft endstops
        storeCmd("G1 Z20 F%d\n",infoSettings.z_speed[infoSettings.move_speed]);  // get Z up high to avoid any obstacles
        
        // restore previous position even though Marlin may do it also if NOZZLE_CLEAN_GOBACK is enabled
        storeCmd("G1 X%3.4f Y%3.4f F%d\n",oldpos.axis[X_AXIS],oldpos.axis[Y_AXIS], infoSettings.xy_speed[infoSettings.move_speed]);
        storeCmd("G1 Z%3.4f F%d\n",oldpos.axis[Z_AXIS],infoSettings.z_speed[infoSettings.move_speed]);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}
