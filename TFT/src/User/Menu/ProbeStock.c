// TG 12/26/22 added new menu page for Probing Stock on CNC
// Homes to 0,0,0 first, with Z0 being max upward travel of Z carriage, it then probes down from home
// towards the machine's base (or spoilboard) to obtain the maximum downward travel distance with no
// workpiece(stock) in place. This represents what would be the bottom surface of the stock.
// Next it re-homes Z to Z0 and then again probes down from home towards the machine's base (or spoilboard)
// with the workpiece in place to obtain the top position of the workpiece(stock).
// The thickness of a plate used for probing can be optionally included or excluded in the measurement.

#include "Home.h"
#include "includes.h"
const MENUITEMS probeStockItems = {        //tg 1/12/20 added the new MENUITEMS cncHomeItems
//   title
LABEL_HOME,
// icon                       label
 {{ICON_NULL,           LABEL_NULL},
  {ICON_NULL,           LABEL_NULL},
  {ICON_NULL,           LABEL_NULL},
  {ICON_NULL,           LABEL_NULL},
  {ICON_SSTART,               LABEL_SSTART},
  {ICON_NULL,           LABEL_NULL},
  {ICON_SSTOP,                LABEL_SSTOP},
  {ICON_BACK,                 LABEL_BACK},}
};

char tempstr[40];             // sprintf buffer
float spoilboardZ;            // holds the maximum neagtive travel distance to the surface the workpiece rests on
float probe_thickness;        // holds the thickness of a probe plate if used

void menuProbeStock(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&probeStockItems);        //TG 12/26/22 new
  displayProgress((char*)"Press START to begin and home all axes");       
  while(infoMenu.menu[infoMenu.cur] == menuProbeStock)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
        case KEY_ICON_0:break;
        case KEY_ICON_1:break;
        case KEY_ICON_2:break;
        case KEY_ICON_3:break;
        case KEY_ICON_4: doProbeStock(); updateProbeStockDisplay(); break;
        case KEY_ICON_5:break;
        case KEY_ICON_6: stopProbeStock(); updateProbeStockDisplay(); break;
        case KEY_ICON_7: infoMenu.cur--; break;
        default:break;
    }
    loopBackEnd();
  }
}

// execute the probe downwards toward top surface of stock (use gcodeSendAndWaitForOK to wait for Marlin "ok" after each step)
// don't combine multiple gcodes in one string since gcodeSendAndWaitForOK would return too soon, after the first code's "ok"
void doProbeStock(void)
{
  // Wait for user to press OK in this popup dialog to begin process, show popup msg with OK only
  //popupInfoOKOnly((uint8_t *)"Begin Probe sequence", (uint8_t *)"Press OK to home all axes");
  //menuDrawPage(&probeStockItems);                       //after response, clear away dialog box redraw this page 
  
  // to begin we home all axes
  displayProgress((char*)"Homing Z Axis .....\n");      //display a progress message in status area
  // send some gcodes and wait for Marlin "ok" response to each
  gcodeSendAndWaitForOK("G53\n",1000);                  //back to native WCS first!
  gcodeSendAndWaitForOK("G28 Z0\n",20000);              //home Z, allow up to 20 secs for Marlin's "ok" response
  
  displayProgress((char*)"Homing X,Y Axes .....\n");    // display a progress message in status area
  gcodeSendAndWaitForOK("G28 X0 Y0\n",40000);           //home x,y, allow up to 40 secs
  gcodeSendAndWaitForOK("M400\n",5000);                 //wait to complete and flush motion cmds
  
  //the stock origin should be at X=40.00mm,Y=40.00mm, so go there plus a 10mm margin
  //if return code equals 1, there was an error so exit at this point
  if (gcodeSendAndWaitForOK("G1 X40 Y40 F4800\n",5000)==1)   //move in over stock + 10mm margin
    {marlinError(); return;}
  
  // Ask if we should probe for spoilboard surface (no stock in place)
  popupQuestionOK((uint8_t *)"Probe sequence...\n", (uint8_t *)"Probe for the spoilboard surface?\n");
  menuDrawPage(&probeStockItems);                       //after response, clear away dialog box redraw this page
  if(CancelFlag==0)                                     //yes response probe spoilboard
  {
    // first probe is to see where the actual table surface(spoilboard surface) is at, show Yes-No popup dialog
    popupQuestionOK((uint8_t *)"Probe sequence...\n", (uint8_t *)"Remove Stock, Attach Probe\n \nUse 0.5mm for probe plate?\n");
    menuDrawPage(&probeStockItems);                       //after response, clear away dialog box redraw this page
    // process user response
    if(CancelFlag==0) {probe_thickness = 0.5;}            //yes response probe thickness is 0.5mm
    else {probe_thickness = 0.0;}                         //no response set probe thickness to 0.0mm

    //start probing for maximum downward travel distance with no stock now
    displayProgress((char*)"Probing down to machine table .....\n");
    // send some gcodes and wait for Marlin "ok" response to each 
    sprintf(tempstr, "G38.3 F400 Z-%3.1f\n", (float)Z_MAX_POS);
    gcodeSendAndWaitForOK(tempstr,20000);                 //probe for true table bottom (82mm max if no spoilboard)
    gcodeSendAndWaitForOK("M400\n",10000);                //wait to complete and flush motion cmds
    storeCmd("M7986 R%3.1f\n", probe_thickness);          //send Marlin the probe thickness, it will return z-position as stockTopZaxis
    if(TFTtoMARLIN_wait(comp_7986)==1)                    //Ask Marlin to report current Z, Marlin adjusts for probe thickness
      {marlinError(); return;}
    spoilboardZ = stockTopZaxis;                          //save spoilboard height with no stock in place
    displayProgress((char*)"Resetting to Z0 pos .....\n");
    if(gcodeSendAndWaitForOK("G1 Z0 F600\n",20000)==1)    //quick back to Z0 again, exit if error
      {marlinError(); return;}
    displayProgress((char *)"Spoilboard surface found...\n");
  }

  // second probe is with stock in place to find top surface of stock, show popup msg with OK only
  popupSuccessOKOnly((uint8_t *)"Probe sequence...\n", (uint8_t *)"Ready to probe for Stock Top\nBe sure the work piece is\nin position!\n");
  menuDrawPage(&probeStockItems);                       //after response, clear away dialog box redraw this page
  
  // send some gcodes and wait for Marlin "ok" response to each 
  displayProgress((char*)"Probing down for top of stock .....\n");
  sprintf(tempstr, "G38.3 F400 Z-%3.1f\n", (float)Z_MAX_POS);//probe for top of stock material to machine table at most
  gcodeSendAndWaitForOK(tempstr,20000);                 
  gcodeSendAndWaitForOK("M400\n",10000);                //wait to complete and flush motion cmds
  storeCmd("M7986 R%3.1f\n", probe_thickness);          //send Marlin the probe thickness, it will return z-position as stockTopZaxis
  if(TFTtoMARLIN_wait(comp_7986)==1)                    //Ask Marlin to report current Z, Marlin adjusts for probe thickness
    {marlinError(); return;}
  
  // display results in status area
  updateProbeStockDisplay();
  if(gcodeSendAndWaitForOK("G1 Z0 F600\n",20000)==1)    //quick back to Z0 again, exit if error
    {marlinError(); return;}
  // show completion popup msg with OK only
  popupSuccessOKOnly((uint8_t *)"Stock Top found...", (uint8_t *)"Finished...remove Probe\n");
  menuDrawPage(&probeStockItems);                       //after response, clear away dialog box redraw this page
  updateProbeStockDisplay();
}

void marlinError(void)
{
  popupErrorOK((uint8_t *)"Error...", (uint8_t *)"Marlin didn't respond!\n"); // msg with OK only
  menuDrawPage(&probeStockItems);                       //after response, clear away dialog box redraw this page
}

void stopProbeStock(void)
{
  popupErrorOK((uint8_t *)"Warning", (uint8_t *)"Reset Probing?\n"); // msg with OK only
  menuDrawPage(&probeStockItems);                       // after response, clear away dialog box redraw this page
  clearCmdQueue();                                      //flush all pending moves
  displayProgress((char*)"Resetting to Z0 pos .....\n");
  if(gcodeSendAndWaitForOK("G28 Z0\n",20000)==1)        //Home Z;
    {marlinError(); return;}
  gcodeSendAndWaitForOK("M400\n",10000);                //wait to complete and flush motion cmds
}

// displays msg in the status area, middle screen
void displayProgress(char* msg)
{
  const GUI_RECT r1 = {2 * BYTE_WIDTH, 3*BYTE_HEIGHT,LCD_WIDTH, 4*BYTE_HEIGHT};
  GUI_ClearPrect(&r1);
  GUI_DispStringInPrect(&r1, (uint8_t*)msg);
}

// displays the results of measurements in the status are of the screen
void updateProbeStockDisplay(void)
{
  // Show/draw values in upper center status area
  const uint16_t top_y = 0;                    // start at top of LCD
  const uint16_t start_xLH = 2 * BYTE_WIDTH;  // where the Left column values will start, first 2, then 19
  const uint16_t start_xRH = 19 * BYTE_WIDTH;  // where the Right column values will start, first 2, then 19
  const GUI_RECT zdata[10] = {  //an array of rectangle coordinates to simplify display of info
  {start_xLH, top_y + 1.5*BYTE_HEIGHT, LCD_WIDTH, top_y + 2.5*BYTE_HEIGHT},   // left side values
  {start_xLH, top_y + 3*BYTE_HEIGHT, LCD_WIDTH/2, top_y + 4*BYTE_HEIGHT},   // left side values
  {start_xLH, top_y + 4*BYTE_HEIGHT, LCD_WIDTH/2, top_y + 5*BYTE_HEIGHT},   // left side values
  {start_xLH, top_y + 5*BYTE_HEIGHT, LCD_WIDTH/2, top_y + 6*BYTE_HEIGHT},   // left side values
  {start_xLH, top_y + 6*BYTE_HEIGHT, LCD_WIDTH/2, top_y + 7*BYTE_HEIGHT},   // left side values
  {start_xRH, top_y + 1.5*BYTE_HEIGHT, LCD_WIDTH, top_y + 2.5*BYTE_HEIGHT},     // right side values
  {start_xRH, top_y + 3*BYTE_HEIGHT, LCD_WIDTH, top_y + 4*BYTE_HEIGHT},     // right side values
  {start_xRH, top_y + 4*BYTE_HEIGHT, LCD_WIDTH, top_y + 5*BYTE_HEIGHT},     // right side values
  {start_xRH, top_y + 5*BYTE_HEIGHT, LCD_WIDTH, top_y + 6*BYTE_HEIGHT},     // right side values
  {start_xRH, top_y + 6*BYTE_HEIGHT, LCD_WIDTH, top_y + 7*BYTE_HEIGHT},     // right side values
  };

    // draw a gray seperator line all the way across width
  GUI_SetColor(GRAY);
  GUI_HLine(0, (BYTE_HEIGHT*1.4), LCD_WIDTH);
  
  GUI_SetBkColor(infoSettings.title_bg_color);
  GUI_DispString(17*BYTE_WIDTH, zdata[0].y0, (uint8_t *)"WCS units  Native units");
   //draw LH side titles
  GUI_DispString(zdata[1].x0, zdata[1].y0, (uint8_t *)"Maximum Z limit:");
  GUI_DispString(zdata[2].x0, zdata[2].y0, (uint8_t *)"Stock(top)     :");
  GUI_DispString(zdata[3].x0, zdata[3].y0, (uint8_t *)"Stock(bottom)  :");
  GUI_DispString(zdata[4].x0, zdata[4].y0, (uint8_t *)"Probe Thickness:");

  // draw a gray seperator line all the way across width
  GUI_SetColor(GRAY);
  GUI_HLine(0, (BYTE_HEIGHT*2.8), LCD_WIDTH);

  //draw LH side info, all values come from parseAck.c
  GUI_SetColor(0xDB40);
  sprintf(tempstr,"% 5.1f      % 5.1f\n", Marlin_ZMAX_POS, Marlin_ZMAX_POS + Z_MAX_POS);
  GUI_DispStringInPrectEOL(&zdata[6], (uint8_t *)tempstr);
  sprintf(tempstr,"% 5.1f      % 5.1f\n", stockTopZaxis, stockTopZaxis + Z_MAX_POS);
  GUI_DispStringInPrectEOL(&zdata[7], (uint8_t *)tempstr);
  sprintf(tempstr,"% 5.1f      % 5.1f\n", spoilboardZ, spoilboardZ + Z_MAX_POS);
  GUI_DispStringInPrectEOL(&zdata[8], (uint8_t *)tempstr);  
  sprintf(tempstr,"% 5.1f      % 5.1f\n", probe_thickness, probe_thickness);
  GUI_DispStringInPrectEOL(&zdata[9], (uint8_t *)tempstr);  

  GUI_RestoreColorDefault();
}




















