
#include "TGmenu.h"
#include "includes.h"
  
//extern char gcodeBuf[CMD_MAX_CHAR];
//char* gcodeBufPtr = &gcodeBuf[0];

COORDINATE oldpos;  //TG 5/16/23 for remembering xyz position before/after nozzle clean
volatile int16_t N_FW = 0;    //TG 8/27/23 N factor(adc offset to apply) for Marlin filament width calculation
char tempstr[40];             //TG 8/27/23 sprintf buffer
uint8_t CancelFlag=0;         // for general pop up msg box responses

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
     {ICON_PARAMETER,               LABEL_FIL_CAL},     //TG 8/27/23 - added filament width sensor calibration
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

      case KEY_ICON_4:  //TG 8/27/23 - added filament width sensor calibration
        if (infoSettings.fil_width) //TG 1/4/24 made this conditional on the state of infoSettings.fil_width
        {
          sprintf(tempstr, "Input the actual measured\nfilament diameter\nPress OK to iterate ...");
          popupInfoOKOnly((uint8_t*)"Fil Width Sensor", (uint8_t*)tempstr);
          volatile float val = numPadFloat((uint8_t*)"Real Filament Width",fil_width_meas,1.75,true);
          uint32_t start = OS_GetTimeMs() + 1000; 
          while(1)
          {  
            if(OS_GetTimeMs()==start)
            {
              //compare actual to fil_width_meas(from Marlin) and calculate N(adc offset) needed to make them match
              volatile int16_t new_NFW = (val - fil_width_meas) * ((float)4095 / (float)3.3) - 1.0126;

              if (new_NFW < -1 || new_NFW > 0)  //only change the offset if it is not between zero(and -1) from equation above)
              { 
                N_FW += new_NFW;                        //adjust to new required offset
                infoSettings.adc_offset_N = N_FW;       //copy to settings structure
                send_adc_offset_to_Marlin();            //send updated N_FW to Marlin and wait up to 1 second for ok response
                saveSettings();                         //and to EEPROM
                sprintf(tempstr, "Iterating.....\nactual  : %4.3fmm\nmeasured: %4.3fmm\nadc offset: %d lsb stored\nContinue? ...", val, fil_width_meas,N_FW);
                Buzzer_play(SOUND_NOTIFY);
                popupQuestionOK((uint8_t*)"Fil Width Sensor", (uint8_t*)tempstr);
                if(CancelFlag==1) break;                //if "No" was selected, exit out
                start = OS_GetTimeMs() + 2000; 
              }
              else
              {
                Buzzer_play(SOUND_SUCCESS);
                popupInfoOKOnly((uint8_t*)"Fil Width Sensor", (uint8_t*)"Iteration completed\nFilament Sensor Calibrated!");
                break;
              }
              
            }
            loopProcess();
          }
        }
        else    //TG 1/4/24 can't run calibration if Fil Width isn't ENABLED
        {
          popupInfoOKOnly((uint8_t*)"Fil Width Sensor", (uint8_t*)"Fil Width must be ENABLED in Feature Settings!\n");  //TG 1/4/24 added
        }
        
        menuDrawPage(&TGmenuItems);
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

//TG function to display a popup with Confirm/Cancel keys over existing menu and wait for response
//returns to original menu once a key was pressed with the global CancelFlag = 0(confirm) or 1(cancel)
void clrCancel(){ CancelFlag = 0; }
void setCancel(){ CancelFlag = 1; }

void popupInfoOKOnly(uint8_t* title, uint8_t* msg)
{
  CancelFlag = 0;
  setDialogText(title, msg, LABEL_CONFIRM, LABEL_NULL);     // sets the strings
  showDialog(DIALOG_TYPE_INFO, setCancel, NULL, NULL);  // draws the dialog box
  loopProcess();                      // allows loop popup() to be called and set menu ptr ahead
  (*infoMenu.menu[infoMenu.cur])();   // switch the menu to the showDialog menu
}
void popupQuestionOK(uint8_t* title, uint8_t* msg)
{
  CancelFlag = 0;
  setDialogText(title, msg, (uint8_t*)"Yes", (uint8_t*)"No");     // sets the strings
  showDialog(DIALOG_TYPE_QUESTION, clrCancel, setCancel, NULL);  // draws the dialog box
  loopProcess();                      // allows loop popup() to be called and set menu ptr ahead
  (*infoMenu.menu[infoMenu.cur])();   // switch the menu to the showDialog menu
}

void send_adc_offset_to_Marlin()
{            
  sprintf(tempstr, "M7800 S%i\n", infoSettings.adc_offset_N);
  gcodeSendAndWaitForOK(tempstr,1500);    //send updated N_FW to Marlin and wait up to 1 second for ok response
}

