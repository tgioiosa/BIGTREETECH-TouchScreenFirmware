//TG MODIFIED*****
/*
  This module provides the AVR Triac control menu for setting the PID on/off, selecting from 3 PID loop speeds and one custom loop speed
  where KP, KI, KD, PID rate and Marlin-AVR data rates can be adjusted. The 3 preset sPID speeds are not adjustable from the screen. Also
  provides switching display screens on the AVR boards LCD (2004a), updating the settings in AVR EEPROM, and resetting the AVR mcu.

  'M' g-codes are used to send/receive to Marlin, and Marlin exchanges data with the AVR mcu over I2C.
  The g-code handling is done in parseAck.c

*/

#include "Configuration.h"
#ifdef USING_AVR_TRIAC_CONTROLLER

#include "gcode.h"
#include "includes.h"
#include "Numpad.h"
#include "Settings.h"

uint8_t UpdateFlag=0;         // for AVR EEPROM Update pop up box responses
uint8_t popupResp=0;         // for general pop up msg box responses

MENUITEMS triacItems = {
// title
LABEL_AVR_CTL,
// icon                       label
 {{ICON_KP,                   LABEL_NULL},
  {ICON_NULL,           LABEL_NULL},
  {ICON_NULL,           LABEL_NULL},
  {ICON_FAST_SPEED,           LABEL_FAST},
  {ICON_PID_OFF,              LABEL_NULL},
  {ICON_AVR_DSP0,             LABEL_NULL},
  {ICON_AVR_RST,              LABEL_NULL},
  {ICON_BACK,                 LABEL_BACK},}
};

#define pidVarsize 5        // Icons list for KP,KI,KD parameters
const ITEM pidVar[pidVarsize] =
{
// icon                          label
  {ICON_KP,              LABEL_NULL},
  {ICON_KI,              LABEL_NULL},
  {ICON_KD,              LABEL_NULL},
  {ICON_MARLIN_INT,      LABEL_NULL},
  {ICON_PIDRUN_INT,      LABEL_NULL},
};
const uint16_t pidVarStrings[pidVarsize] =
{
    LABEL_KP,
    LABEL_KI,
    LABEL_KD,
    LABEL_MARLIN_INT,
    LABEL_PIDRUN_INT,
};

#define pidSpeedssize 4     // Icons list for PID speeds
const ITEM pidSpeeds[pidSpeedssize] =
{
// icon                          label
  {ICON_SLOW_SPEED,      LABEL_SLOW},
  {ICON_NORMAL_SPEED,    LABEL_NORMAL},
  {ICON_FAST_SPEED,      LABEL_FAST},
  {ICON_CUST_SPEED,      LABEL_CUST_SPEED},
};

const ITEM itemPIDONOFF[2] = {    // Icons list PID On/Off steps
// icon                       label
  {ICON_PID_OFF,             LABEL_PID_OFF},
  {ICON_PID_ON,              LABEL_PID_ON},
};

const ITEM AVRdisp[3] = {         // Icons list for AVR Display page steps
  {ICON_AVR_DSP0,            LABEL_NULL},
  {ICON_AVR_DSP1,            LABEL_NULL},
  {ICON_AVR_DSP2,            LABEL_NULL},
};


void menuTriac(void)
{
  uint16_t chksumStart;     // for remembering the AVRInfoBlock checksum state on entry
  uint8_t tempval;


  // New Marlin Commands TFT <> Marlin exchange
  // M7979  - spindle_use_pid flag, pid on/off
  // M7980  - send reset AVR cmd to Marlin
  // M7981  - P,I,D constants Kp, Ki, Kd
  // M7982  - send AVR LCD display page # to Marlin
  // M7984  - change the selected AVR PID speed
  // M7900  - receive/send AVRInfoBlock.PIDFLAG, AVRInfoBlock.Reset_Flag, AVRInfoBlock.Display_Page, AVRInfoBlock.PID_Speed,
  //                       AVRInfoBlock.Update_EEPROM, AVRInfoBlock.EE_chksum, AVRInfoBlock.dummy_pad_byte
    
/*
   Request AVRInfoBlock from CURRENT MARLIN, parseAck.c will respond to the message and fill in the data, MARLIN and AVR
   exchange the complete block over I2C every AVRInfoBlock.Data_Interval*16.7ms. 
   The TFT can send individual G-Codes to MARLIN for these operations to get quick response to actions, and then only
   when user is finished and exits with KEY_ICON_7, will an Update_EEPROM msg be sent to MARLIN/AVR.
*/
  readMarlinAVRInfoBlock();                       // get complete info
  chksumStart = get_chksum();                     // update current checksum of block data on entry here
  AVRInfoBlockOrg = AVRInfoBlock;                 // make a copy of original settings on entry

  // Set initial screen icons to match the variables states as loaded above
  triacItems.items[3] = pidSpeeds[AVRInfoBlock.PID_Speed];
  triacItems.items[4] = itemPIDONOFF[AVRInfoBlock.PIDFLAG];  //TG set icon per state of pid
  triacItems.items[5] = AVRdisp[AVRInfoBlock.Display_Page];  //TG set icon per state of pid
  menuDrawPage(&triacItems);
  updateTriacStatusDisplay(0);
  
  while(infoMenu.menu[infoMenu.cur] == menuTriac)
  {
    KEY_VALUES key_num = menuKeyGetValue();
    
    switch(key_num)
    {
      case KEY_ICON_0:  // cycle thru icons for pid Terms for KI, KP, or KD    
        pidVarIndex = (pidVarIndex + 1) % pidVarsize;         
        triacItems.items[key_num] = pidVar[pidVarIndex];      
        menuDrawItem(&triacItems.items[key_num], key_num);    // redraw the screen with icon change
        updateTriacStatusDisplay(0);                          // display the current term/values in INFOBOX area
        break;
      
      case KEY_INFOBOX: //TG cool, didn't know this existed, touch the infobox area and you can type direct values!
      { 
        float tempf[] = {0.0,0.0,0.0};
        uint8_t tempi[2] = {0,0};

        if (pidSpeeds[AVRInfoBlock.PID_Speed].label.index == LABEL_CUST_SPEED) // can only modify parameters in Custom PID Speed
        {  
          char titlestr[30];
          // display the keyboard and update the AVRInfoBlock.K[] value if user made changes ONLY if user presses KEY_ICON_7
          // to exit this menu, we'll compare checksums and offer to Update the AVR EEPROM or not.
          sprintf(titlestr, "%s  Min:0 | Max:10", textSelect(pidVarStrings[pidVarIndex]));    // show variable name
          if(pidVarIndex < 3)                                                                 // KP,KI,KD are floats
          {
            tempf[0] = AVRInfoBlock.K[0];tempf[1] = AVRInfoBlock.K[1];tempf[2] = AVRInfoBlock.K[2]; // save all first
            float newval = numPadFloat((u8 *) titlestr, AVRInfoBlock.K[pidVarIndex], 0, false);
            AVRInfoBlock.K[pidVarIndex] = newval;

            mustStoreCmd("%s P%3.4f I%3.4f D%3.4f\n", " M7981",AVRInfoBlock.K[0],AVRInfoBlock.K[1],AVRInfoBlock.K[2]);
            if(TFTtoMARLIN_wait(comp_7981))            // allow time for TFT->MARLIN(UART) and MARLIN->AVR(I2C)
              {AVRInfoBlock.K[0] = tempf[0] ; AVRInfoBlock.K[1]=tempf[1]; AVRInfoBlock.K[2] = tempf[2];}  // failed, revert
          }
          else                                                                                // others are integers
          {
            tempi[0] = AVRInfoBlock.Data_Interval; tempi[1] = AVRInfoBlock.PID_Interval;      // save all first
            uint8_t* parm = (pidVarIndex == 3 ? &AVRInfoBlock.Data_Interval : &AVRInfoBlock.PID_Interval);
            uint8_t newval = numPadInt((u8 *) titlestr, *parm, 0, false);
            *parm = newval;

            infoHost.wait = false;
            mustStoreCmd("%s F%d R%d N%d S%d U%d C%u B%d D%d P%d\n", " M7900", AVRInfoBlock.PIDFLAG, AVRInfoBlock.Reset_Flag, 
                          AVRInfoBlock.Display_Page, AVRInfoBlock.PID_Speed, AVRInfoBlock.Update_EEPROM, AVRInfoBlock.EE_chksum,
                          AVRInfoBlock.Reload_Preset, AVRInfoBlock.Data_Interval, AVRInfoBlock.PID_Interval);
            if(TFTtoMARLIN_wait(comp_7900))            // allow time for TFT->MARLIN(UART) and MARLIN->AVR(I2C)
              {AVRInfoBlock.Data_Interval = tempi[0]; AVRInfoBlock.PID_Interval = tempi[1];}  // failed, revert
          }
          
//          // the following data is sent in two steps to avoid one big block of comm data and minimize choking the UARTS
//          // send AVR PID constants immediately with any changes
//          mustStoreCmd("%s P%3.4f I%3.4f D%3.4f\n", " M7981",AVRInfoBlock.K[0],AVRInfoBlock.K[1],AVRInfoBlock.K[2]);
//          if(TFTtoMARLIN_wait(comp_7981))            // allow time for TFT->MARLIN(UART) and MARLIN->AVR(I2C)
//            {AVRInfoBlock.K[0] = tempf[0] ; AVRInfoBlock.K[1]=tempf[1]; AVRInfoBlock.K[2] = tempf[2];}  // failed, revert
//          else
//          {
//          // then send flags, Update EEPROM signal, and check sum with new values
//          infoHost.wait = false;
//          mustStoreCmd("%s F%d R%d N%d S%d U%d C%u B%d D%d P%d\n", " M7900", AVRInfoBlock.PIDFLAG, AVRInfoBlock.Reset_Flag, 
//                        AVRInfoBlock.Display_Page, AVRInfoBlock.PID_Speed, AVRInfoBlock.Update_EEPROM, AVRInfoBlock.EE_chksum,
//                        AVRInfoBlock.Reload_Preset, AVRInfoBlock.Data_Interval, AVRInfoBlock.PID_Interval);
//          if(TFTtoMARLIN_wait(comp_7900))            // allow time for TFT->MARLIN(UART) and MARLIN->AVR(I2C)
//            {AVRInfoBlock.Data_Interval = tempi[0]; AVRInfoBlock.PID_Interval = tempi[1];}  // failed, revert
//          
//          }
          menuDrawPage(&triacItems);
          updateTriacStatusDisplay(0);
        }
        else
          popupReminder(DIALOG_TYPE_ERROR,(uint8_t *)"Parameter Warning", 
                  (uint8_t *)"This value is from a Preset\nthat can only be changed in\nthe AVR firmware!"); // msg with OK only

        break;
      }
      
      case KEY_ICON_3:  // change the PID speed preset                             
        tempval = AVRInfoBlock.PID_Speed;
        AVRInfoBlock.PID_Speed = (AVRInfoBlock.PID_Speed + 1) % pidSpeedssize;      // cycle thru the pid speeds
        //triacItems.items[key_num] = pidSpeeds[AVRInfoBlock.PID_Speed];              // update icon/label in menu
        
        // SEND COMMAND IMMEDIATELY TO MARLIN/AVR HERE, NO EEPROM UPDATE HAPPENS TILL EXIT
        mustStoreCmd("%s S%d \n", "M7983", AVRInfoBlock.PID_Speed);     // TG send cmd to Marlin to request change AVR pid speed
        if(TFTtoMARLIN_wait(comp_7983))                                 // allow time for TFT->MARLIN(UART) and MARLIN->AVR(I2C)
          AVRInfoBlock.PID_Speed = tempval;                             // if not zero send to Marlin failed
        else
          readMarlinAVRInfoBlock();                                     // send succeded, get updated PID_Speed info
        
        triacItems.items[key_num] = pidSpeeds[AVRInfoBlock.PID_Speed];              // update icon/label in menu
        //menuDrawItem(&triacItems.items[key_num], key_num);              // redraw the screen with change
        menuDrawPage(&triacItems);      // redraw to get rid of any sticky popup boxes
        updateTriacStatusDisplay(0);
        break;
      
      case KEY_ICON_4:  // turn PID on-off 
        tempval = AVRInfoBlock.PIDFLAG;
        AVRInfoBlock.PIDFLAG ^= 1;    // toggle value when pressed
        //triacItems.items[key_num] = itemPIDONOFF[AVRInfoBlock.PIDFLAG]; // update icon/label in menu
        // SEND COMMAND IMMEDIATELY TO MARLIN/AVR HERE, NO EEPROM UPDATE HAPPENS TILL EXIT
        mustStoreCmd("%s S%d \n", "M7979", AVRInfoBlock.PIDFLAG);           //TG send cmd to Marlin to update there
        if(TFTtoMARLIN_wait(comp_7979))
          AVRInfoBlock.PIDFLAG = tempval;
        
        triacItems.items[key_num] = itemPIDONOFF[AVRInfoBlock.PIDFLAG]; // update icon/label in menu
        //menuDrawItem(&triacItems.items[key_num], key_num);            // redraw the menu with updates
        menuDrawPage(&triacItems);      // redraw updates and get rid of any sticky popup boxes
        updateTriacStatusDisplay(0);                                    // update status in INFOBOX area too
        break;
      
      case KEY_ICON_5:  // change the AVR LCD display page 
        tempval = AVRInfoBlock.Display_Page;                      // save org value if send to Marlin fails
        AVRInfoBlock.Display_Page = (AVRInfoBlock.Display_Page + 1) % 3;  // cycle thru the AVR LCD display pages
        // SEND COMMAND IMMEDIATELY TO MARLIN/AVR HERE, NO EEPROM UPDATE HAPPENS TILL EXIT
        mustStoreCmd("%s P%d \n", "M7982", AVRInfoBlock.Display_Page);    //TG send cmd to Marlin to change AVR display pag
        if(TFTtoMARLIN_wait(comp_7982))                                   // if not zero send to Marlin failed, revert value
          AVRInfoBlock.Display_Page = tempval;

        triacItems.items[key_num] = AVRdisp[AVRInfoBlock.Display_Page]; // update icon/label in menu
        //menuDrawItem(&triacItems.items[key_num], key_num);              // redraw the screen with change
        
        menuDrawPage(&triacItems);      // redraw updates and get rid of any sticky popup boxes
        updateTriacStatusDisplay(0);
        break;

      case KEY_ICON_6:  // RESET the AVR mcu   
        triacItems.items[key_num].icon = ICON_RESET_VALUE;              // change icon to resetting.....
        menuDrawItem(&triacItems.items[key_num], key_num);              // redraw the screen with change
        // SEND COMMAND IMMEDIATELY TO MARLIN/AVR HERE, NO EEPROM UPDATE HAPPENS TILL EXIT
        mustStoreCmd("%s \n", "M7980");                                 //TG send cmd to Marlin to request AVR reset
        
        TFTtoMARLIN_wait(comp_7980);

        triacItems.items[key_num].icon = ICON_AVR_RST;                  // restore org icon
        menuDrawItem(&triacItems.items[key_num], key_num);              // redraw the screen with restored icon
        readMarlinAVRInfoBlock();                                       // get updated info
        triacItems.items[3] = pidSpeeds[AVRInfoBlock.PID_Speed];
        triacItems.items[4] = itemPIDONOFF[AVRInfoBlock.PIDFLAG];  //TG set icon per state of pid
        triacItems.items[5] = AVRdisp[AVRInfoBlock.Display_Page];  //TG set icon per state of pid
        menuDrawPage(&triacItems);
        updateTriacStatusDisplay(0);                             // update status in INFOBOX area too
        //menuDrawItem(&triacItems.items[key_num], key_num);              // redraw the screen with change
        break;
      
      case KEY_ICON_7:  // Exit back to prev menu
        // On exit we'll determine whether we need to SEND the TFT's copy of AVRInfoBlock to update Marlin/AVR.
        // If checksum has changed, we post a msg asking to confirm or cancel the changes for update. 
        // If CANCEL, request Marlin/AVR to reload(M7984) the current pidSpeed presets to restore AVRInfoBlock
        // and exit menuTriac. Then return to current menu and display unchanged values.
        // If CONFIRM, then keep AVRInfoBlock changes, Set the Update flag, and send to Marlin(M7981,M7900).
        // Then post a message confirming AVR has been updated and return to previous menu.
        
        AVRInfoBlock.EE_chksum = get_chksum();      // get latest check sum, includes any/all InfoBlock changes
        
        if (AVRInfoBlock.EE_chksum != chksumStart)  // check sum has changed, post a popup msg to user
        { //***** CHANGES WERE DETECTED ********************************************************************************
          popupResp = 0;   // clear flag first
          // ask user to CONFIRM or CANCEL updating the AVR EEPROM with changes, making them permanent
          popupConfirmCancel((uint8_t *)"Settings Changed", (uint8_t *)"Update AVR EEPROM?"); // get result in popupResp
          UpdateFlag = popupResp;
          
          //***** IF CHANGES ACCEPTED - SAVE TO AVR EEPROM ***********************************************************
          if (UpdateFlag == 0)                      // user CONFIRMS update 
          { 
            AVRInfoBlock.Update_EEPROM = 1;         // user CONFIRMS update - set the Update Flag
            // the following data is sent in two steps to avoid one big block of comm data and minimize choking the UARTS
            // send AVR PID constants first with any changes
            mustStoreCmd("%s P%3.4f I%3.4f D%3.4f\n", " M7981",AVRInfoBlock.K[0],AVRInfoBlock.K[1],AVRInfoBlock.K[2]);
            TFTtoMARLIN_wait(comp_7981);            // allow time for TFT->MARLIN(UART) and MARLIN->AVR(I2C)
            // then send flags, Update EEPROM signal, and check sum with new values
            infoHost.wait = false;
            mustStoreCmd("%s F%d R%d N%d S%d U%d C%u B%d D%d P%d\n", " M7900", AVRInfoBlock.PIDFLAG, AVRInfoBlock.Reset_Flag, 
                          AVRInfoBlock.Display_Page, AVRInfoBlock.PID_Speed, AVRInfoBlock.Update_EEPROM, AVRInfoBlock.EE_chksum,
                          AVRInfoBlock.Reload_Preset, AVRInfoBlock.Data_Interval, AVRInfoBlock.PID_Interval);
            TFTtoMARLIN_wait(comp_7900);            // allow time for TFT->MARLIN(UART) and MARLIN->AVR(I2C)
            AVRInfoBlock.Update_EEPROM = 0;         // clear the update command after sending it once
            AVRInfoBlock.Reset_Flag = 0;            // clear the reset flag after sending it once
            //***** IF UPDATE WAS SUCCESSFUL ***************************************************************************
            if (popupResp==0)     // popup msg that update is done, then return to previous menu
              popupReminder(DIALOG_TYPE_SUCCESS, (uint8_t *)"Settings Changed", (uint8_t *)"AVR Updated")
          }
          
          //***** CHANGES WERE CANCELLED - RELOAD CURRENT AVR PID VALUES - EXIT MENU BUT KEEP INFOMENU PTR TO HERE *****
          //***** WE DON'T ACTUALLY EXIT TO THE PREVIOUS MENU - WE JUST RELOAD VALUES FROM AVR AND COME BACK QUICK *****
          if (UpdateFlag == 1)  // just prepare to exit menu without changing infoMenu.cur ptr so that we immediately
          {                     // resume the current menu(being edited) again but with original un-edited values restored
            AVRInfoBlock.Reload_Preset = 1;          // reload PID speed command
            mustStoreCmd("%s S%d \n", "M7984", 1);   // request(M7984 S1) to Marlin to reload current AVR pid speed values
            TFTtoMARLIN_wait(comp_7984);             // allow time for TFT->MARLIN(UART) and MARLIN->AVR(I2C)
            return;                                  // return to the current menuTriac as edited
          }
        }
        
        //***** WE ARE EXITING THIS MENU FOR GOOD, AND WILL RETURN TO THE PREVIOUS MENU BEFORE IT **********************
        //
        // If user cancelled an AVR update, some settings may still have changed prior to cancel, so let's revert to the
        // AVR EEPROM default pidSpeed setting first(M7984 S2). This gives a clean menu for default pidSpeed when the user
        // freshly enters menuTriac the NEXT time. If no changes made, or AVR EEPROM was updated, just skip and exit normally.
        if (UpdateFlag == 1)     
        { 
          UpdateFlag = 0;                            // clear so we only do this once
          AVRInfoBlock.Reload_Preset = 2;            // on exiting menu back, select AVR EEPROM default pidSpeed
          mustStoreCmd("%s S%d \n", "M7984", 2);     // TG send cmd to Marlin to request change AVR pid speed
          TFTtoMARLIN_wait(comp_7984);               // allow time for TFT->MARLIN(UART) and MARLIN->AVR(I2C)
          loopProcess();
        }
        
        //***** TOTAL EXIT - INFOMENU PTR IS SET BACK TO PREVIOUS MENU, WHICH WILL SHOW BY FOR-NEXT LOOP IN MAIN.C *****
        // this resets to KP icon/value for next-time entry to menuTriac in case it was modified before exit.
        triacItems.items[0] = pidVar[0];
        pidVarIndex = 0;                           
        infoMenu.cur--;
        break;
      
      default:        
        break;
    }
    loopProcess();  // run this while staying inside menuTriac while loop
  } // end menuTriac while loop
} // return back to caller

// Show/draw PID values in upper center status area
void updateTriacStatusDisplay(u8 index)
{
  char tempstr[18];
  GUI_RECT widerRect;

  // clean up previous messages since some strings are short and some long, they clobber each other
  widerRect = (GUI_RECT){exhibitRect.x0,exhibitRect.y0,exhibitRect.x1, exhibitRect.y1};
  GUI_ClearPrect(&widerRect);             // clear any previous info rectangle with a wider rectangle
  sprintf(tempstr, "%s", "PID Speed");    // display static text above pidSpeed icon
  GUI_DispString(LCD_WIDTH-SPACE_X_PER_ICON, BYTE_HEIGHT, (u8 *)tempstr); // above the speeds
  setFontSize(FONT_SIZE_LARGE);;
  
  // show the name of the variable in large text
  GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1)>>1, exhibitRect.y0, textSelect(pidVarStrings[pidVarIndex]));

  // show the value of the variable in large text
  setFontSize(FONT_SIZE_LARGE);;
  if(pidVarIndex > 2)   // if custom_speed variables
  {
    if(pidVarIndex == 3) sprintf(tempstr, "%d", AVRInfoBlock.Data_Interval);
    if(pidVarIndex == 4) sprintf(tempstr, "%d", AVRInfoBlock.PID_Interval); 
  }
  else                  // std preset variables
    sprintf(tempstr, "%4.4f", AVRInfoBlock.K[pidVarIndex]);
  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);   // draw it on LCD
  
  //GUI_SetColor(infoSettings.font_color);  //TG 8/31/21 restore normal font color for code that follows                                       
  setFontSize(FONT_SIZE_NORMAL);;  // back to normal font

  // show an explanation of the variable below the value
  // need a wider rectangle than standard to clean up previous messages since one starts further left and on ends further right
  widerRect = (GUI_RECT){exhibitRectLower.x0 - BYTE_WIDTH,exhibitRectLower.y0,exhibitRectLower.x1 + BYTE_WIDTH, exhibitRectLower.y1};
  GUI_ClearPrect(&widerRect);   // clear any previous info rectangle with a wider rectangle
  GUI_SetColor(AVRInfoBlock.PIDFLAG==0 ? RED : DARKGREEN);  // color per PID on/off state
  GUI_SetBkColor(infoSettings.title_bg_color);
  sprintf(tempstr, "%s", pidVarIndex==0? "Proportional" :
                         pidVarIndex==1? "Integral" :
                         pidVarIndex==2? "Derivative":
                         pidVarIndex==3? "Marlin-AVR rate":
                         "PID Run rate");    // display the current variable
  GUI_DispStringCenter(exhibitRectLower.x0 + ICON_WIDTH + BYTE_WIDTH, exhibitRectLower.y0, (u8 *)tempstr);         // below the Temp
   GUI_RestoreColorDefault();
}

uint16_t get_chksum()
{	
	// create a unique checksum, some byte values can be ambiguous (i.e. suppose PIDFLAG=1 and
	// Reset_Flag=0, if they each toggled state there's no change to a simple summing checksum)
	// therefore we multiply each item by different amounts to make them unique.
	uint8_t i;
	uint16_t cs = 0;
	uint8_t * ifb = &AVRInfoBlock.PIDFLAG;

	uint8_t max = sizeof(AVRInfoBlock) - 2;		// don't include cs word, last 2 bytes
	for (i=0; i<max; i++)
	{
		cs += (*ifb++) * (max-i);				// give each item a different "weight"
	}
	return cs;
}


//TG function to display a popup with Confirm/Cancel keys over existing menu and wait for response
//returns to original menu once a key was pressed with the global popupResp = 0(confirm) or 1(cancel)
void clrCancel(){ popupResp = 0; }
void setCancel(){ popupResp = 1; }

void popupConfirmCancel(uint8_t* title, uint8_t* msg)
{
  popupResp = 0;
  setDialogText(title, msg, LABEL_CONFIRM, LABEL_CANCEL);     // sets the strings
  showDialog(DIALOG_TYPE_ALERT, clrCancel, setCancel, NULL);  // draws the dialog box
  loopProcess();                      // allows loop popup() to be called and set menu ptr ahead
  (*infoMenu.menu[infoMenu.cur])();   // switch the menu to the showDialog menu
}

void popupErrorOK(uint8_t* title, uint8_t* msg)
{
  popupResp = 0;
  setDialogText(title, msg, LABEL_CONFIRM, LABEL_CANCEL);     // sets the strings
  showDialog(DIALOG_TYPE_ERROR, setCancel, NULL, NULL);  // draws the dialog box
  loopProcess();                      // allows loop popup() to be called and set menu ptr ahead
  (*infoMenu.menu[infoMenu.cur])();   // switch the menu to the showDialog menu
}

// Request FULL AVRInfoBlock from CURRENT MARLIN, parseAck.c will respond to the message and fill in the data
// MARLIN and AVR exchange the complete block over I2C every 300ms, but the exchange between TFT and MARLIN
// for individual parameters doesn't justify sending the entire block over UART (>20 bytes). TFT can send
// individual G-Codes to MARLIN for these operations to get quick response to actions, and then only when user
// closes this menu and exits with KEY_ICON_7, an Update_EEPROM msg can be sent to MARLIN/AVR.
void readMarlinAVRInfoBlock()
{
  mustStoreCmd("M7900 R\n");          //TG send cmd to Marlin to read AVR InfoBlock
  TFTtoMARLIN_wait(comp_7900);        // wait for UART transmission to complete
  mustStoreCmd("M7981 R\n");          // Request PID values from CURRENT MARLIN as a seperate action
  TFTtoMARLIN_wait(comp_7981);        // wait for UART transmission to complete
}

// waits for the passed retmsg to be seen by ParseAck(). Returns 1=success or 0=timeout, not received
uint8_t TFTtoMARLIN_wait(enum retcodes retmsg)
{
  uint16_t waitPeriod;
  waitPeriod = isPrinting() ? 3000 : 2000;      // allow extra time for responses if printing
  uint32_t start = OS_GetTimeMs() + waitPeriod; // timeout 2sec for response (3sec if printing);
  msg_complete = 0;                             // clear the response flag (sets in parseAck.c)
  
  while ((msg_complete & retmsg) == 0)          // until we get the retmsg we expect
  {
    //sendQueueCmd();                           // do USART related background processes
    //parseACK();
    //parseRcvGcode();
    loopProcess();
    if(OS_GetTimeMs() >= start){                // exceeded timeout with no response? 
      if(retmsg != comp_7980) ResetAVRPopup();  // 7980 is already a reset command, prevent recusrion
      return 1;                                 // timed out
    }
  }
  return 0;                                     // response completed, no errors
}

void ResetAVRPopup(){
  uint8_t oldCancelFlag = popupResp;       // copy current popupResp since popupConfirmCancel() will overwite it
  popupResp = 0;
  popupConfirmCancel((uint8_t *)"No response from AVR", (uint8_t *)"System Restart?"); // get result in popupResp
  if(!popupResp)
    NVIC_SystemReset();                     // if we reset, we don't care about restoring popupResp
  popupResp = oldCancelFlag;               // otherwise restore original value of popupResp
}


#endif // #ifdef USING_AVR_TRIAC_CONTROLLER