// TG 3/31/23 revised new menu page for Measuring heights of Stock, Spoilboard, CNC Table and Tool-Stickout
// Homes to 0,0,0 first, with Z_MAX_POS being max upward position of Spindle, MACHINE_Z_MIN is specified in
// the Configuration.h file (measured from the Z_MAX_POS to the CNC Table. For home UP (CNC), MACHINE_Z_MIN
// will be negative. The current MPCNC design limits the Spindle travel to 70mm max (set by UP endstop max
// to compressed spring height min). The mounting of the Spindle prevents it's lowest point(shaft w/collet removed)
// from reaching the CNC Table (which is physically -98.0mm from Z_MAX_POS). By using a 30mm block, we can measure
// Table surface as far as 100mm (70+30) downward.
// The thickness of a plate used for probing can be optionally included or excluded in the measurements.
// Z_MAX_POS is used as a limit when testing results, anything greater than Z_MAX_POS is an error


#include "Home.h"
#include "includes.h"
MENUITEMS probeStockItems = {        //TG 12/26/22 added the new MENUITEMS probeStockItems
//   title
LABEL_HOME,
// icon                       label
 {{ICON_NULL,                 LABEL_NULL},
  {ICON_NULL,                 LABEL_NULL},
  {ICON_NULL,                 LABEL_NULL},
  {ICON_NULL,                 LABEL_NULL},
  {ICON_P_SPOILBOARD,         LABEL_P_SPOILBOARD},
  {ICON_P_TOOL_LENGTH,        LABEL_P_TOOL_LENGTH},
  {ICON_P_STOCK_HEIGHT,       LABEL_P_STOCK_HEIGHT},
  {ICON_BACK,                 LABEL_BACK},}
};

char tempstr[80];             // sprintf buffer
float tool_stickout = 0;      // always a logical val
float stock_position = 0;     // native val
float stock_pos_logical = 0;  // logical val
float zmin_abs_logical = 0;   // logical val
float spoilboard_logical = 0; // logical val 
bool needs_home = true;       //TG 3/26/23 added

void menuProbeStock(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&probeStockItems);        //TG 12/26/22 new
  
  // zmin_absolute, spoilboard_absolute, and probeThickness are maintained in the infoSettings structure and
  // are saved / recalled from EEPROM so they persist even after power down. Their logical equivalents are
  // calculated only when this menu runs, so let's get them prepared to show on initial entry. probeThickness
  // is always a logical value so nothing needs to be done for it.   
  // pre-calculate these quantities so they can display something if they've been previously measured
  if(infoSettings.zmin_absolute != Z_MAX_POS)                       // anything other than initial empty value?
  {
    zmin_abs_logical = infoSettings.zmin_absolute - MACHINE_Z_MIN;
    if(infoSettings.spoilboard_absolute != Z_MAX_POS)               // anything other than initial empty value? 
      spoilboard_logical = infoSettings.spoilboard_absolute - infoSettings.zmin_absolute;
  }
  
  // tool_stickout, stock_position, and needs_home are not remebered if we leave this menu, so they will reset
  // to empty each time this menu is newly entered.
  tool_stickout = 0;  
  stock_position = 0; 
  needs_home = true;    // reset to require Homing

  updateProbeStockDisplay();                                        // show the results page in upper half of screen 
  while(infoMenu.menu[infoMenu.cur] == menuProbeStock)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
        case KEY_ICON_0:break;
        case KEY_ICON_1:break;
        case KEY_ICON_2:break;
        case KEY_ICON_3:break;
        case KEY_ICON_4:
          doProbeSpoilBoard(false,false); 
          updateProbeStockDisplay(); 
          break;
        case KEY_ICON_5:
          doProbeToolLength(false); 
          updateProbeStockDisplay();
          break;
        case KEY_ICON_6:
          doProbeStock(false,false,false); 
          updateProbeStockDisplay(); 
          break;
        case KEY_ICON_7: 
          infoMenu.cur--; 
          break;
        default:break;
    }
    loopBackEnd();
  }
}


//TG function to display a popup with Modify or Keep keys over any existing menu and wait for response
//returns with global popupResp = KEY_POPUP once a key was pressed, then redraws original menu
float getProbeThickness(){
  sprintf(tempstr, "Current probe thickness:\n[%6.3f]",infoSettings.probeThickness);
  popupThreeKeys((uint8_t *)"Probe Thickness?\n", (uint8_t*)tempstr, (uint8_t*)"Modify",(uint8_t*)"Keep", NULL);
  
  if (popupResp == KEY_POPUP_CONFIRM) { //Modify
    retry:
    infoSettings.probeThickness = numPadFloat((uint8_t*)"Probe Thickness",infoSettings.probeThickness,
                                             infoSettings.probeThickness,true);
  
    if (!WITHIN(infoSettings.probeThickness,0,15)){
      BUZZER_PLAY(SOUND_ERROR);
      goto retry;
    }
  }
  else if (popupResp == KEY_POPUP_CONFIRM)
  {// do nothing just continue
  }
  menuDrawPage(&probeStockItems);                       // after response, redraw this page to clear dialog box
  return infoSettings.probeThickness;                   // always should be a positive number
}

// Display a message and probe to Target with Marlin G38, get response from Marlin (adjusted for probe thickness)
// execute the probe downwards toward target (use gcodeSendAndWaitForOK to wait for Marlin "ok" after each step) don't
// combine multiple gcodes in one string since gcodeSendAndWaitForOK would return too soon, after the first code's "ok"
float probeDowntoTarget(float target, char* tname, char* title, char* msg){
  if (*msg != 0){
    popupConfirmCancel((uint8_t*)title, (uint8_t*)msg );// if there is a message, show it
    if(popupResp==KEY_POPUP_CANCEL) return Z_MAX_POS;   // if user cancelled
  }
  
  //start probing for maximum downward travel distance with no stock now
  menuDrawPage(&probeStockItems);                       //after response, redraw this page to clear dialog box
  sprintf(tempstr, "Probing to %s %6.3f", tname, target);
  displayProgress((char*)tempstr);
  sprintf(tempstr, "G38.3 F400 Z%6.3f\n", target);      // send some gcodes and wait for Marlin "ok" response to each
  gcodeSendAndWaitForOK(tempstr,20000);                 // probe down towards bottom (82mm max if no spoilboard)
  gcodeSendAndWaitForOK("M400\n",10000);                // wait to complete and flush motion cmds
  
  //TG 3/28/23 do not append 'P' to M7986 cause we don't want an //action:prompt from it
  storeCmd("M7986 R%5.3f\n", infoSettings.probeThickness); //send Marlin the probe thickness, it will return z-position as probed_Z_pos
  if(TFTtoMARLIN_wait(comp_7986)==1)                    //Ask Marlin to report current Z, Marlin adjusts for probe thickness
    {marlinError(); return Z_MAX_POS;}
  return probed_Z_pos;                                  //return probed height
}

//TG 3/28/23 - the MACHINE_Z_MIN defined in Configuration.h gives the machine's absolute lowest Z point (table surface) with 
// respect to the Z_MAX_POS(0 for a CNC). Still, you can elect to measure this value by probing down from Z_MAX_POS. However
// the present MPCNC design has limited Z-travel of 70mm (due to Z-endstop location and compressed spring height limit), so
// the table surface can't be reached by the spindle shaft's bottom surface. It can be reached if there is a tool inserted
// (with a known tool-stickout) or by using a pre-known spacer block on the table surface. Here we use a spacer block
// of exactly 30mm and then account for that in the measurement. You can also directly enter a value measured by hand using
// the numpad. The value is stored as infoSettings.zmin_absolute and is saved/recalled from EEPROM.
// The logical value displayed should ideally be zero since it's the difference of measured value and config MACHINE_Z_MIN.
// But if the measured value deviates from pre-defined MACHINE_Z_MIN, the logical value will show the difference. 
float getMIN_Z_Position(){
  uint8_t title[] = "Machine Zmin position\n";
  float val = 0;
  sprintf(tempstr, "Current Zmin is:\n%6.3f  [%6.3f]", infoSettings.zmin_absolute, zmin_abs_logical);
  popupThreeKeys(title, (uint8_t*)tempstr, (uint8_t*)"Measure", (uint8_t*)"Modify",(uint8_t*)"Keep");
  
  // SPOILBOARD and TABLE Z_MIN are measured from Spindle Shaft with the 30mm calibrated block!
  if (popupResp != KEY_POPUP_EXTRA)       // if NOT Keep, just redraw page and return
  {
    if (popupResp == KEY_POPUP_CONFIRM){  // Measure
      sprintf(tempstr, "To measure absolute table min\nplace 30mm block on surface\nPress OK when ready...");
      popupInfoOKOnly(title, (uint8_t*)tempstr);
      sprintf(tempstr, "Probing for table Zmin...", infoSettings.zmin_absolute);
      val = probeDowntoTarget(MACHINE_Z_MIN-2 ,(char*)"TABLE Z_MIN", tempstr, (char*)"Make sure Spoilboard\nis REMOVED!"); //allow extra 2mm for tolerances
      if (val >= Z_MAX_POS)   // skip the rest if user cancelled during probeDowntoTarget()
        {
          menuDrawPage(&probeStockItems);                 // after response, redraw this page to clear dialog box
          return Z_MAX_POS;                               // indicate user cancelled
        }
      val = val - 30;   // compensate for 30mm block (used to reach table surface due to limited machine Z-travel)
      displayProgress((char*)"Resetting to Z0 pos .....\n");
      if(gcodeSendAndWaitForOK("G1 Z0 F600\n",15000)==1)  // quick back to Z0 again, exit if error
       {marlinError(); return Z_MAX_POS;}                 // indicate error
    }
    else if (popupResp == KEY_POPUP_CANCEL) // Modify
    {
retry1:
      val = numPadFloat((uint8_t*)"CNC Table Zmin",infoSettings.zmin_absolute,
                                     infoSettings.zmin_absolute,true);
      if (!WITHIN(val,MACHINE_Z_MIN-2,Z_MAX_POS)){        // if out of range beep and retry, allow MACHINE_Z_MIN extra 2mm for tolerances
        BUZZER_PLAY(SOUND_ERROR);
        goto retry1;
      }
    }
    val = NOBEYOND(MACHINE_Z_MIN-2, val, Z_MAX_POS);      // keep the value within limits
    infoSettings.zmin_absolute = val;                     // negative if CNC(home up) positive if 3D printer(home down)
  
    zmin_abs_logical = infoSettings.zmin_absolute - MACHINE_Z_MIN;  // calc logical value by referencing to MACHINE_Z_MIN
    sprintf(tempstr, "New position at:\n  %6.3f native\n   %6.3f logical", infoSettings.zmin_absolute, zmin_abs_logical);
    popupSuccessOKOnly(title, (uint8_t*)tempstr);    
    gcodeSendAndWaitForOK("M400\n",10000);                //wait to complete and flush motion cmds
  }
  menuDrawPage(&probeStockItems);                         //after response, redraw this page to clear dialog box
  return infoSettings.zmin_absolute;
}

// SPOILBOARD and TABLE Z_MIN are measured from Spindle Shaft with the 30mm calibrated block!
// Probe for the spoilboard, it's always and only referenced to table Zmin.
float doProbeSpoilBoard(bool skipProbeThk, bool skipMinZ){
  float val = 0;
  uint8_t title[] = "Spoilboard position\n";
  if(needs_home) doHomeAll();
  if (!skipProbeThk) getProbeThickness();

  // Min Z must be known to proceed with spoilboard probe
  if (getMIN_Z_Position() >= Z_MAX_POS)   // check or get zmin_absolute if not already set
    return Z_MAX_POS;                     // exit if user cancelled

  sprintf(tempstr, "Current Spoilboard Top:\n%6.3f  [%6.3f]", infoSettings.spoilboard_absolute, spoilboard_logical);
  popupThreeKeys(title, (uint8_t*)tempstr, (uint8_t*)"Measure", (uint8_t*)"Modify", (uint8_t*)"Keep");
  
  if (popupResp != KEY_POPUP_EXTRA)       // if NOT Keep, just redraw page and return
  {
    if (popupResp == KEY_POPUP_CONFIRM){  // Measure
      sprintf(tempstr, "Probing to %6.3f...", infoSettings.zmin_absolute); // now probe for spoilboard top
      val = probeDowntoTarget(infoSettings.zmin_absolute, (char*)"TABLE Z_MIN", tempstr, (char*)"Make sure Spoilboard\nis in place!\n\nUse 30mm block!!");
      if (val >= Z_MAX_POS)
      {
        menuDrawPage(&probeStockItems);   // redraw this page to clear dialog box
        return Z_MAX_POS;                 // return - user cancelled or error 
      }
      infoSettings.spoilboard_absolute = NOBEYOND(infoSettings.zmin_absolute, val, Z_MAX_POS) - 30;
      displayProgress((char*)"Resetting to Z0 pos .....\n");
      if(gcodeSendAndWaitForOK("G1 Z0 F600\n",15000)==1)    //quick back to Z0 again, exit if error
        {marlinError(); return Z_MAX_POS;}  
    }
    else if (popupResp == KEY_POPUP_CANCEL) // Modify
    {
retry1:
        infoSettings.spoilboard_absolute = numPadFloat((uint8_t*)"Spoilboard height",infoSettings.spoilboard_absolute,
                                       infoSettings.spoilboard_absolute,true);
        if (!WITHIN(infoSettings.spoilboard_absolute,infoSettings.zmin_absolute,Z_MAX_POS)){ // if out of range beep and retry
          BUZZER_PLAY(SOUND_ERROR);
          goto retry1;
        }
    }
    spoilboard_logical = infoSettings.spoilboard_absolute - infoSettings.zmin_absolute;  // calc logical value by referencing infoSettings.zmin_absolute
    sprintf(tempstr, "New position at:\n  %6.3f native\n   %6.3f logical", infoSettings.spoilboard_absolute, infoSettings.spoilboard_absolute - infoSettings.zmin_absolute);
    popupSuccessOKOnly(title, (uint8_t*)tempstr);
  }
  menuDrawPage(&probeStockItems);                         // redraw this page to clear dialog box
  return infoSettings.spoilboard_absolute;
}

// Spoilboard and CNC Table measurements are referenced from the bottom of the Spindle shaft, with collet removed.
// Tool displacement (stickout) is an additional length from the spindle shaft bottom to the tip of the tool.
// To get that displacement, we put in the tool and tighten it, then do a new probe to the spoilboard to get a
// 2nd spoilboard height with the tool in place. Subtracting this 2nd spoilboard height from the initial spoilboard
// height gives us the tool displacement. The same can be done using the Table (infoSettings.zmin_absolute) instead.
void doProbeToolLength(bool skipProbeThk){
  float val = 0;
  float baseline = 0;
  uint8_t title[] = "Tool Length (stick-out)\n";
  char* tnp;
  if(needs_home) doHomeAll();
  if(!skipProbeThk) getProbeThickness();        // only if true

  // first validate the known baseline measured from spindle shaft measurement without the tool 
  sprintf(tempstr, "Current Stick-out: %6.3f\n\nChoose where to reference Tool stickout measurement:", tool_stickout);
  popupThreeKeys(title, (uint8_t*)tempstr, (uint8_t*)"To Spoilboard", (uint8_t*)"To CNC  Table", (uint8_t*)"Keep    Current");
  
  if (popupResp != KEY_POPUP_EXTRA)             // if Keep Current, just redraw page and return
  {
    if (popupResp == KEY_POPUP_CONFIRM)         // Yes
      {
        if(infoSettings.spoilboard_absolute>=Z_MAX_POS) doProbeSpoilBoard(true,false); // if spoilboard position NOT known, get it now
        baseline = infoSettings.spoilboard_absolute; 
        tnp = (char*)"SPOILBOARD";
      }
    else
    {
        if(infoSettings.zmin_absolute >= Z_MAX_POS) getMIN_Z_Position(); // if CNC Table position NOT known, get it now
        baseline = infoSettings.zmin_absolute;
        tnp = (char*)"TABLE Z_MIN";  
    }  
    
    if (baseline >= Z_MAX_POS)          // if returned baseline from above is not valid, exit now
    {
      menuDrawPage(&probeStockItems);   // redraw this page to clear dialog box
      return;                           // return - user cancelled or error 
    }

    // we have the 1st baseline measurement, now get a second measurement with the tool in place
    sprintf(tempstr, "INSERT tool and tighten collet\n\nOK to start probe...");
    val = probeDowntoTarget(baseline, (char*)tnp, (char*)"Measuring Tool Stickout...\n", tempstr);
    if (val < Z_MAX_POS) // skip the rest if user cancelled during probeDowntoTarget()
    {
      tool_stickout = val - baseline;
      displayProgress((char*)"Resetting to Z0 pos .....\n");
      if(gcodeSendAndWaitForOK("G1 Z0 F600\n",15000)==1)  //quick back to Z0 again, exit if error
        {marlinError(); return;}
      sprintf(tempstr, "Tool Stickout is:\n  %6.3f \n", tool_stickout);
      popupSuccessOKOnly(title, (uint8_t*)tempstr);
    }
  }  
  menuDrawPage(&probeStockItems);                         //after response, redraw this page to clear dialog box
}

// Probe for the Stock top, to do this we would want a tool in place, so first we have to find the displacement
// that extends below the spindle shaft (which was used to obtain infoSettings.zmin_absolute and spoilboard heights)
// To get that dimension, we put in the tool and tighten it, then do a probe to the spoilboard(already known) to
// get a spoilboard height with the tool in place. Subtracting spoilboard height(with tool) from initial spoilboard
// height gives us the tool displacement. The same can be done using the Table (infoSettings.zmin_absolute) instead.
//
// Knowing the tool displacement, we can then probe for the Stock Top with the tool in place.
float doProbeStock(bool skipProbeThk, bool skipMinZ, bool skipTool){
  float val = 0;
  float baseline = 0;
  uint8_t title[] = "Stock position\n";  if(needs_home) doHomeAll();
  char msg[48] = {'\0'};
  char tname[12] = {};
  char* tnp = &tname[0];

  if (!skipProbeThk) getProbeThickness();
retry2:  
  popupQuestionYesNo(title, (uint8_t*)"Stock measurement requires a tool in the spindle to be measured first!\nIs the tool inserted?");
  if (popupResp == KEY_POPUP_CONFIRM){              // OK - continue
    doProbeToolLength(true);                        // measure tool, skipProbeThickness
    if(tool_stickout == 0) goto retry2;             // if tool measurmenet is not valid try again

    // Tool is good, Stock position will always be measured, nut prev value can be kept
    sprintf(tempstr, "Current Stock Top:\n %6.3f  [%6.3f]\n\nStock is placed on:", stock_position,stock_pos_logical);

    popupThreeKeys(title, (uint8_t*)tempstr, (uint8_t*)"On SpoilBoard", (uint8_t*)"On CNC  Table", (uint8_t*)"Keep    Current");
    if (popupResp != KEY_POPUP_EXTRA)               // if NOT Keep Current
    {
      if (popupResp == KEY_POPUP_CONFIRM){          // probe to SpoilBoard
        if(infoSettings.spoilboard_absolute>=Z_MAX_POS) doProbeSpoilBoard(true,false); // if spoilboard position NOT known, get it now
        baseline = infoSettings.spoilboard_absolute;       
        sprintf(msg, (char*)"Make sure Stock is\nin place on Spoilboard!\n\nand Tool inserted!!");
        tnp = (char*)"SPOILBOARD";
      }
      else if (popupResp == KEY_POPUP_CANCEL)       // probe to CNC Table
      {
        if(infoSettings.zmin_absolute >= Z_MAX_POS) getMIN_Z_Position(); // if CNC Table position NOT known, get it now
        baseline = infoSettings.zmin_absolute;  
        sprintf(msg, (char*)"Make sure Stock is\nin place on CNC table!\n\nand Tool inserted!!");
        tnp = (char*)"TABLE Z_MIN";
      }

      if (baseline < Z_MAX_POS)                     // proceed only if no cancel or no error getting baseline
        val = probeDowntoTarget(infoSettings.zmin_absolute,(char*)tnp,tempstr, msg);

      // do a check of return val and baseline from above
      if ((val >= Z_MAX_POS) || (baseline >= Z_MAX_POS)) // if user cancelled or during probeDowntoTarget()
      {
        menuDrawPage(&probeStockItems);             // redraw this page to clear dialog box
        return Z_MAX_POS;                           // user cancelled or error
      }
      
      stock_position = NOBEYOND(infoSettings.zmin_absolute, val, Z_MAX_POS);  // keep within limits
      stock_position = stock_position - tool_stickout;        //always adjust for tool
      displayProgress((char*)"Resetting to Z0 pos .....\n");
      if(gcodeSendAndWaitForOK("G1 Z0 F600\n",15000)==1)      // quick back to Z0 again
        {marlinError(); return Z_MAX_POS;}                    // exit if error  

      stock_pos_logical = stock_position - baseline;
      sprintf(tempstr, "New position at at:\n  %6.3f native\n   %6.3f logical", stock_position, stock_pos_logical);
      popupSuccessOKOnly(title, (uint8_t*)tempstr);
    }
  }
  menuDrawPage(&probeStockItems);                    //after response, redraw this page to clear dialog box
  return stock_position;
}



// home Z, then X,Y and go to X50 Y50 for probing
void doHomeAll(){
  displayProgress((char*)"Homing X,Y,Z Axes .....\n");  // display a progress message in status area
  // send some gcodes and wait for Marlin "ok" response to each
  gcodeSendAndWaitForOK("G53\n",1000);                  //back to native WCS first!
  gcodeSendAndWaitForOK("G28 Z0\n",20000);              //home Z, allow up to 20 secs for Marlin's "ok" response
  gcodeSendAndWaitForOK("G28 X0 Y0\n",40000);           //home x,y, allow up to 40 secs
  gcodeSendAndWaitForOK("M400\n",5000);                 //wait to complete and flush motion cmds
  
  // the stock origin should be at X=40.00mm,Y=40.00mm, so go there plus a 10mm margin
  // if return code equals 1, there was an error so exit at this point
  if (gcodeSendAndWaitForOK("G1 X50 Y50 F4800\n",5000)==1)   //move in over stock + 10mm margin
    {marlinError(); return;} 
  menuDrawPage(&probeStockItems);                       //redraw to clear message
  needs_home = false;
}

// displays error message, abandons any task in effect, and redraws page
void marlinError(void)
{
  popupErrorOK((uint8_t *)"Error...", (uint8_t *)"Marlin didn't respond!\n"); // msg with OK only
  needs_home = true;                     //assume we have to re-home after an error
  menuDrawPage(&probeStockItems);        //after response, clear away dialog box redraw this page
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
  const uint16_t top_y = 0;                     // start at top of LCD
  const uint16_t start_xLH = 2 * BYTE_WIDTH;    // where the Left column values will start, first 2, then 19
  const uint16_t start_xRH = 19 * BYTE_WIDTH;   // where the Right column values will start, first 2, then 19
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

  
  saveSettings(); //TG 3/28/23 update any settings to EEPROM that may have changed
  
    // draw a gray seperator line all the way across width
  GUI_SetColor(GRAY);
  GUI_HLine(0, (BYTE_HEIGHT*1.4), LCD_WIDTH);
  
  GUI_SetBkColor(infoSettings.title_bg_color);
  sprintf(tempstr,"Plate: %4.2fmm      Native     Logical", infoSettings.probeThickness);
  GUI_DispString(1*BYTE_WIDTH, zdata[0].y0, (uint8_t *)tempstr);
   //draw LH side titles
  GUI_DispString(zdata[1].x0, zdata[1].y0, (uint8_t *)"Minimum Z limit:");
  GUI_DispString(zdata[2].x0, zdata[2].y0, (uint8_t *)"Spoilboard top :");
  GUI_DispString(zdata[3].x0, zdata[3].y0, (uint8_t *)"Stock top      :");
  GUI_DispString(zdata[4].x0, zdata[4].y0, (uint8_t *)"Tool Length    :");

  // draw a gray seperator line all the way across width
  GUI_SetColor(GRAY);
  GUI_HLine(0, (BYTE_HEIGHT*2.8), LCD_WIDTH);

  //draw LH side info, all values come from parseAck.c
  GUI_SetColor(0xDB40);
  sprintf(tempstr,"% 8.3f   % 8.3f\n", infoSettings.zmin_absolute, zmin_abs_logical);
  GUI_DispStringInPrectEOL(&zdata[6], (uint8_t *)tempstr);
  sprintf(tempstr,"% 8.3f   % 8.3f\n", infoSettings.spoilboard_absolute,spoilboard_logical);
  GUI_DispStringInPrectEOL(&zdata[7], (uint8_t *)tempstr);
  sprintf(tempstr,"% 8.3f   % 8.3f\n", stock_position, stock_pos_logical);
  GUI_DispStringInPrectEOL(&zdata[8], (uint8_t *)tempstr);  
  sprintf(tempstr,"% 8.3f   % 8.3f\n", tool_stickout, tool_stickout);
  GUI_DispStringInPrectEOL(&zdata[9], (uint8_t *)tempstr);  

  GUI_RestoreColorDefault();
}




















