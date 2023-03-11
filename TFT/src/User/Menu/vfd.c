//TG MODIFIED*****
/*
  This module provides the VFD control menu 
  //TG 12/23/22 added
*/

#include "Configuration.h"

#ifdef USING_VFD_CONTROLLER
#include "includes.h"
#include "Numpad.h"
#include "Settings.h"

//#define CHAR_RADIO_CHECKED         "\u08A9"   //TG large radio buttons
//#define CHAR_RADIO_UNCHECKED       "\u08AA"

iregBits inputReg;        // VFD Input Register values from Marlin
bool VFDpresent;          // true if VFD connected
uint8_t vfdStatus;        // VFD status bits from Marlin
float vfdP;               // holds the VFD decimal point indicator, either 10.0 or 100.0, comes from Marlin
float sw_ver;             // holds software version from VFD, only received from Marlin at startup
float cpu_ver;            // holds motor CPU version from VFD, only received from Marlin at startup
uint16_t f164;            // holds baudrate setting from VFD, received on TFT request from Marlin
uint16_t f165;            // holds start,parity,data,stop setting from VFD, received on TFT request from Marlin
uint8_t msg_complete = 0; // indicates when an 'M' g-code request message has finished
uint8_t CancelFlag=0;     // for general pop up msg box responses

void menuVFD(void)
{
  char buf[128];          // buffer for sprintf
  
  GUI_Clear(infoSettings.bg_color);
  GUI_SetColor(GRAY);
  
  const uint16_t top_y = 0;                    // start at top of LCD
  const uint16_t start_xLH = 12 * BYTE_WIDTH;  // where the Left column values will start, first 10, then 31
  const uint16_t start_xRH = 34 * BYTE_WIDTH;  // where the Right column values will start, first 10, then 31

  const GUI_RECT version[11] = {  //an array of rectangle coordinates to simplify display of info
  {0, top_y + 0*BYTE_HEIGHT, LCD_WIDTH, top_y + 1*BYTE_HEIGHT},
  {start_xLH, top_y + 2*BYTE_HEIGHT, LCD_WIDTH/2, top_y + 3*BYTE_HEIGHT},   // left side values
  {start_xLH, top_y + 3*BYTE_HEIGHT, LCD_WIDTH/2, top_y + 4*BYTE_HEIGHT},   // left side values
  {start_xLH, top_y + 4*BYTE_HEIGHT, LCD_WIDTH/2, top_y + 5*BYTE_HEIGHT},   // left side values
  {start_xLH, top_y + 5*BYTE_HEIGHT, LCD_WIDTH/2, top_y + 6*BYTE_HEIGHT},   // left side values
  {start_xRH, top_y + 2*BYTE_HEIGHT, LCD_WIDTH, top_y + 3*BYTE_HEIGHT},     // right side values
  {start_xRH, top_y + 3*BYTE_HEIGHT, LCD_WIDTH, top_y + 4*BYTE_HEIGHT},     // right side values
  {start_xRH, top_y + 4*BYTE_HEIGHT, LCD_WIDTH, top_y + 5*BYTE_HEIGHT},     // right side values
  {start_xRH, top_y + 5*BYTE_HEIGHT, LCD_WIDTH, top_y + 6*BYTE_HEIGHT},     // right side values
  {start_xLH, top_y + 6*BYTE_HEIGHT, LCD_WIDTH/2, top_y + 7*BYTE_HEIGHT},   // left side values
  {0, top_y + 8*BYTE_HEIGHT, LCD_WIDTH, top_y + 9*BYTE_HEIGHT},             // radio buttons for status byte
  };

  mustStoreCmd("M7989 R\n");          //TG 12/23/22 send cmd to Marlin to read VFDpresent flag
  TFTtoMARLIN_wait(comp_7989);        //wait for response
  
  mustStoreCmd("M7988 R\n");          //TG 12/23/22 send cmd to Marlin to read S/W ver., S/N, F164, and F165
  TFTtoMARLIN_wait(comp_7988);        //wait for response

  if(VFDpresent==false) 
  {
    popupErrorOK((uint8_t *)"VFD not responding", (uint8_t *)"Is it powered on?");
    GUI_Clear(infoSettings.bg_color);
    GUI_SetColor(GRAY); 
  }


  // display VFD info page
  while (infoMenu.menu[infoMenu.cur] == menuVFD)
  { //draw top header string all the way across width
    GUI_SetColor(0xDB40);   // orange
    sprintf(buf, "  S/W Ver:%2.2f CPU:%2.2f  F164:%1d F165:%1d", (sw_ver/100), (cpu_ver/100), f164, f165);
    GUI_DispStringInPrectEOL(&version[0], (uint8_t *)buf);
    
    // draw a gray seperator line all the way across width
    GUI_SetColor(GRAY);
    GUI_HLine(0, (BYTE_HEIGHT*1), LCD_WIDTH);

    //draw LH side titles
    GUI_DispString(12, version[1].y0, (uint8_t *)"Out Freq :");
    GUI_DispString(12, version[2].y0, (uint8_t *)"Set Freq :");
    GUI_DispString(12, version[3].y0, (uint8_t *)"Current  :");
    GUI_DispString(12, version[4].y0, (uint8_t *)"Out RPM  :");
    GUI_DispString(12, version[9].y0, (uint8_t *)"Run Hours:");

    //draw LH side info, all values come from parseAck.c
    GUI_SetColor(0xDB40);
    GUI_DispStringInPrectEOL(&version[1], (uint8_t*)toStr(inputReg.freq_out/vfdP,1));  
    GUI_DispStringInPrectEOL(&version[2], (uint8_t*)toStr(inputReg.freq_set/vfdP,1));
    GUI_DispStringInPrectEOL(&version[3], (uint8_t*)toStr(inputReg.current_out/(vfdP*10),1));
    GUI_DispStringInPrectEOL(&version[4], (uint8_t*)toStr(inputReg.speed_out * 8,0));
    GUI_DispStringInPrectEOL(&version[9], (uint8_t*)toStr(inputReg.total_hours,0));

    //draw RH side titles
    GUI_SetColor(GRAY);
    GUI_DispString(12+LCD_WIDTH/2, version[5].y0, (uint8_t *)"DC Voltage :");
    GUI_DispString(12+LCD_WIDTH/2, version[6].y0, (uint8_t *)"AC Voltage :");
    GUI_DispString(12+LCD_WIDTH/2, version[7].y0, (uint8_t *)"Temperature:");
    GUI_DispString(12+LCD_WIDTH/2, version[8].y0, (uint8_t *)"Last Fault :");
    
    //draw RH side info, all values come from parseAck.c
    GUI_SetColor(0xDB40);
    GUI_DispStringInPrectEOL(&version[5], (uint8_t*)toStr(inputReg.dc_voltage/vfdP,1));
    GUI_DispStringInPrectEOL(&version[6], (uint8_t*)toStr(inputReg.ac_voltage/vfdP,1));
    GUI_DispStringInPrectEOL(&version[7], (uint8_t*)toStr(inputReg.temperature/vfdP,1));
    GUI_DispStringInPrectEOL(&version[8], (uint8_t*)toStr(inputReg.fault_code,0));

    // draw a gray seperator line all the way across width
    GUI_SetColor(GRAY);
    GUI_HLine(0, LCD_HEIGHT - (BYTE_HEIGHT*6), LCD_WIDTH);

    char *sFlags[] = {"RUN","JOG","F/R","OPR","JOG","F/R","BRK","TRK"}; // array for names of the VFD status bits
    uint16_t color;
    for (uint8_t i=0;i<8;i++)
    {
      if ((vfdStatus & (1 << i))==0)
        color = BLACK;    // off
      else
        color = GREEN;      // on
      // draw a psuedo-LED with one of the names from the array above, BLACK is OFF, RED is ON
      RADIO_Create_Single(version[10].x0 + (2+i*5) * BYTE_WIDTH, version[10].y0, color, (char*)sFlags[i], BOTTOM);
    }

    // draw a gray seperator line all the way across width, followed by 'touch any key to exit' message
    GUI_HLine(0, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH);
    GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));

    if (!isPress()) 
      loopBackEnd();    // do background stuff while this page is displayed
    
    if (isPress())      // if the screen is pressed, restore the default color and return to previous menu 
    {
      GUI_RestoreColorDefault();
      infoMenu.cur--;
    }
  }
}

// convert a float to a string and return ptr to the string
// note: the returned string is a static array and will change every time this function gets called.
// This function then is suitable for a single call within another function, but not suitable if
// a function calls this more than once i.e. sprintf("%s %s %s", toStr(10), toStr(20), toStr(30)),
// since as the compiler calls each occurence in processing the sprintf, succesive calls to toStr()
// overwrite the previous result, and you end up with "30 30 30" instead of "10 20 30"
char* toStr(float value, uint8_t dec_places)
{
  static char str[7];
  char* fmt[1] = {"     "};
  switch (dec_places)
  {
    case 0:
      fmt[0] = "%5.0f";
      break;
    case 1:
      fmt[0] = "%5.1f";
      break;
    case 2:
      fmt[0] = "%5.2f";
      break;    
  }
  sprintf(&str[0], fmt[0], value);
  return &str[0];
}

// waits for the passed retmsg to be seen by ParseAck(). Returns 1=success or 0=timeout, not received
uint8_t TFTtoMARLIN_wait(msgcodes retmsg)
{
  uint16_t waitPeriod;
  waitPeriod = isPrinting() ? 3000 : 2000;      // allow extra time for responses if printing
  uint32_t start = OS_GetTimeMs() + waitPeriod; // timeout 2sec for response (3sec if printing);
  msg_complete = 0;                             // clear the response flag (sets in parseAck.c)
  
  while ((msg_complete & retmsg) == 0)          // until we get the retmsg we expect
  {
    loopProcess();
    if(OS_GetTimeMs() >= start){                // exceeded timeout with no response? 
      return 1;                                 // timed out
    }
  }
  return 0;                                     // response completed, no errors
}

//TG function to display a popup with Confirm/Cancel keys over existing menu and wait for response
//returns to original menu once a key was pressed with the global CancelFlag = 0(confirm) or 1(cancel)
void clrCancel(){ CancelFlag = 0; }
void setCancel(){ CancelFlag = 1; }

void popupErrorOK(uint8_t* title, uint8_t* msg)
{
  CancelFlag = 0;
  setDialogText(title, msg, LABEL_CONFIRM, LABEL_CANCEL);     // sets the strings
  showDialog(DIALOG_TYPE_ERROR, setCancel, NULL, NULL);  // draws the dialog box
  loopProcess();                      // allows loop popup() to be called and set menu ptr ahead
  (*infoMenu.menu[infoMenu.cur])();   // switch the menu to the showDialog menu
}
#endif // #ifdef USING_VFD_CONTROLLER