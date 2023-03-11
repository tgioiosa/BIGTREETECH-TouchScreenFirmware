//TG MODIFIED*****
#include "includes.h"
#ifndef NO_RRFSupport   //TG 3/2/23 added global switch for RepRap support
  #include "RRFParseACK.hpp"
#endif
#include "menu.h"
#include "parseACK.h"

typedef enum  // popup message types available to display an echo message
{
  ECHO_NOTIFY_NONE = 0,  // ignore the echo message
  ECHO_NOTIFY_TOAST,     // show a non invasive toast on the title bar for a preset duration.
  ECHO_NOTIFY_DIALOG,    // show a window to notify the user and alow interaction.
} ECHO_NOTIFY_TYPE;

typedef struct
{
  ECHO_NOTIFY_TYPE notifyType;
  const char * const msg;
} ECHO;

// notify or ignore messages starting with following text
const ECHO knownEcho[] = {
  {ECHO_NOTIFY_NONE, "busy: paused for user"},
  {ECHO_NOTIFY_NONE, "busy: processing"},
  {ECHO_NOTIFY_NONE, "Now fresh file:"},
  {ECHO_NOTIFY_NONE, "Now doing file:"},
  //{ECHO_NOTIFY_NONE, "Probe Offset"},
  //{ECHO_NOTIFY_NONE, "enqueueing \"M117\""},
  {ECHO_NOTIFY_NONE, "Flow:"},
  {ECHO_NOTIFY_NONE, "echo:;"},                   // M503
  {ECHO_NOTIFY_NONE, "echo:  G"},                 // M503
  {ECHO_NOTIFY_NONE, "echo:  M"},                 // M503
  {ECHO_NOTIFY_TOAST, "echo:Active Mesh"},        // M503
  {ECHO_NOTIFY_TOAST, "echo:EEPROM can"},         // M503
  {ECHO_NOTIFY_NONE, "Cap:"},                     // M115
  {ECHO_NOTIFY_NONE, "Config:"},                  // M360
  {ECHO_NOTIFY_TOAST, "Settings Stored"},         // M500
  {ECHO_NOTIFY_TOAST, "echo:Bed"},                // M420
  {ECHO_NOTIFY_TOAST, "echo:Fade"},               // M420
  {ECHO_NOTIFY_TOAST, "echo:Active Extruder"},    // Tool Change
  {ECHO_NOTIFY_NONE, "Unknown command: \"M150"},  // M150
};

const char magic_error[] = "Error:";
const char magic_echo[] = "echo:";
const char magic_warning[] = "Warning:";  // RRF warning
const char magic_message[] = "message";   // RRF message in Json format

#define ACK_CACHE_SIZE 512  // including ending character '\0'

char ack_cache[ACK_CACHE_SIZE];             // buffer where read ACK messages are stored
uint16_t ack_len;                           // length of data currently present in ack_cache
uint16_t ack_index;
SERIAL_PORT_INDEX ack_port_index = PORT_1;  // index of target serial port for the ACK message (related to originating gcode)
bool hostDialog = false;

struct HOST_ACTION
{
  char prompt_begin[30];
  char prompt_button[2][20];
  bool prompt_show;         // show popup reminder or not
  uint8_t button;           // number of buttons
} hostAction;

void setHostDialog(bool isHostDialog)
{
  hostDialog = isHostDialog;
}

bool getHostDialog(void)
{
  return hostDialog;
}

void setCurrentAckSrc(SERIAL_PORT_INDEX portIndex)
{
  ack_port_index = portIndex;
}

static bool ack_starts_with(const char * str)  // checks if the cache starts with the given parameter
{
  uint16_t i = 0;

  while (str[i] == ack_cache[i])
  {
    if (str[++i] == '\0')
    {
      ack_index = i;
      return true;
    }
  }

  return false;
}

static bool ack_seen(const char * str)
{ // searches the first appearance of the given string from the start
  // of the cache, on success sets the current index of the cache
  // ("ack_index") next to the position where the found string ended
  uint16_t i;

  for (ack_index = 0, i = 0; ack_cache[ack_index] != '\0'; ack_index++, i = 0)
  {
    while (str[i] == ack_cache[ack_index + i])
    {
      if (str[++i] == '\0')
      {
        ack_index += i;
        return true;
      }
    }
  }

  return false;
}

static bool ack_continue_seen(const char * str)
{ // unlike "ack_seen()", this starts the search from the current index, where previous
  // search left off and retains "ack_index" if the searched string is not found
  uint16_t tmp_index;
  uint16_t i;

  for (tmp_index = ack_index, i = 0; ack_cache[tmp_index] != '\0'; tmp_index++, i = 0)
  {
    while (str[i] == ack_cache[tmp_index + i])
    {
      if (str[++i] == '\0')
      {
        ack_index = tmp_index + i;
        return true;
      }
    }
  }

  return false;
}

static float ack_value()  // returns the decimal ack value
{
  return (strtod(&ack_cache[ack_index], NULL));
}

// read the value after "/", if any
static float ack_second_value()
{
  char * secondValue = strchr(&ack_cache[ack_index], '/');

  if (secondValue != NULL)
    return (strtod(secondValue + 1, NULL));
  else
    return -0.5;
}

void ack_values_sum(float * data)
{
  while (((ack_cache[ack_index] < '0') || (ack_cache[ack_index] > '9')) && ack_cache[ack_index] != '\n')
  {
    ack_index++;
  }

  *data += ack_value();

  while ((((ack_cache[ack_index] >= '0') && (ack_cache[ack_index] <= '9')) ||
          (ack_cache[ack_index] == '.')) && (ack_cache[ack_index] != '\n'))
  {
    ack_index++;
  }

  if (ack_cache[ack_index] != '\n')
    ack_values_sum(data);
}

void ackPopupInfo(const char * info)
{
  bool show_dialog = true;

  if (MENU_IS(menuTerminal) || (MENU_IS(menuStatus) && info == magic_echo))
    show_dialog = false;

  // play notification sound if buzzer for ACK is enabled
  if (info == magic_error)
    BUZZER_PLAY(SOUND_ERROR);
  else if (info == magic_echo && infoSettings.ack_notification == 1)
    BUZZER_PLAY(SOUND_NOTIFY);

  // set echo message in status screen
  if (info == magic_echo || info == magic_message)
  {
    // ignore all messages if parameter settings is open
    if (MENU_IS(menuParameterSettings))
      return;

    // show notification based on notificaiton settings
    if (infoSettings.ack_notification == 1)
      addNotification(DIALOG_TYPE_INFO, (char *)info, (char *)ack_cache + ack_index, show_dialog);
    else if (infoSettings.ack_notification == 2)
      addToast(DIALOG_TYPE_INFO, ack_cache);  // show toast notificaion if turned on
  }
  else
  {
    addNotification(DIALOG_TYPE_ERROR, (char *)info, (char *)ack_cache + ack_index, show_dialog);
  }
  //TG 2/18/21 added to do some resetting after a printer kill is received
  if((info == magic_error) && (strstr(ack_cache, "kill()") != NULL))
  {
      initSettings();                               //TG reset all infoSettings vars
      vacuumState = 0;                              //TG reset vacuum on/off and auto functions cause this is not stored in infoSettings
      OPEN_MENU(menuStatus);                        //TG reset LCD to status menu
  }
}

bool processKnownEcho(void)  // returns true for known echo msgs found in knownEcho array
{
  bool isKnown = false;
  uint8_t i;

  for (i = 0; i < COUNT(knownEcho); i++)
  {
    if (strstr(ack_cache, knownEcho[i].msg))
    {
      isKnown = true;
      break;
    }
  }

  // display the busy indicator
  busyIndicator(SYS_STATUS_BUSY);

  if (isKnown)
  {
    if (knownEcho[i].notifyType == ECHO_NOTIFY_NONE)
      return isKnown;

    if (knownEcho[i].notifyType == ECHO_NOTIFY_TOAST)
    {
      addToast(DIALOG_TYPE_INFO, ack_cache);
    }
    else if (knownEcho[i].notifyType == ECHO_NOTIFY_DIALOG)
    {
      BUZZER_PLAY(SOUND_NOTIFY);
      addNotification(DIALOG_TYPE_INFO, (char *)magic_echo, (char *)ack_cache + ack_index, true);
    }
  }

  return isKnown;
}

bool dmaL1NotEmpty(uint8_t port)
{
  return dmaL1Data[port].rIndex != dmaL1Data[port].wIndex;
}

void syncL2CacheFromL1(uint8_t port)  // copies dmaL1Data to ack_cache until newline character
{
  uint16_t i = 0;

  while (dmaL1NotEmpty(port))
  {
    ack_cache[i] = dmaL1Data[port].cache[dmaL1Data[port].rIndex];
    dmaL1Data[port].rIndex = (dmaL1Data[port].rIndex + 1) % dmaL1Data[port].cacheSize;
    if (ack_cache[i++] == '\n') break;
  }
  ack_cache[i] = 0;  // End character
}

//TG This entire routine has been changed from V26 to V27. It should now
//process M0 message strings correctly. If there are problems with it not
//working refer to this same code (hostActionCommands(void)) in V26 where
//I modified it to work with V26.
//Get here from parseAck when "//action:" has been seen
void hostActionCommands(void)
{
  if (ack_seen(":notification "))
  {
    uint16_t index = ack_index;  // save the current index for further usage

    if (ack_continue_seen("Time Left"))  // parsing printing time left
    {
      // format: Time Left <XX>h<YY>m<ZZ>s (e.g. Time Left 02h04m06s)
      parsePrintRemainingTime((char *)ack_cache + ack_index);
    }
    else if (ack_continue_seen("Layer Left"))  // parsing printing layer left
    {
      // format: Layer Left <XXXX>/<YYYY> (e.g. Layer Left 51/940)
      setPrintLayerNumber(ack_value());
      setPrintLayerCount(ack_second_value());
    }
    else if (ack_continue_seen("Data Left"))  // parsing printing data left
    {
      // format: Data Left <XXXX>/<YYYY> (e.g. Data Left 123/12345)
      setPrintProgressData(ack_value(), ack_second_value());
    }
    else
    {
      statusScreen_setMsg((uint8_t *)magic_echo, (uint8_t *)ack_cache + index);  // always display the notification on status screen

      if (!ack_continue_seen("Ready."))  // avoid to display unneeded/frequent useless notifications (e.g. "My printer Ready.")
      {
        if (MENU_IS_NOT(menuStatus))  // don't show it when in menuStatus
          addToast(DIALOG_TYPE_INFO, ack_cache + index);

        if (infoSettings.notification_m117 == ENABLED)
          addNotification(DIALOG_TYPE_INFO, (char *)magic_echo, (char *)ack_cache + index, false);
      }
    }
  }
  else if (ack_seen(":print_start"))  // print started from remote host (e.g. OctoPrint etc...)
  {
    startPrintingFromRemoteHost(NULL);  // start print originated and hosted by remote host and open Printing menu
  }
  else if (ack_seen(":print_end"))  // print ended from remote host (e.g. OctoPrint etc...)
  {
    endPrint();
  }
  else if (ack_seen(":pause") || ack_seen(":paused"))
  {
    if (infoMachineSettings.firmwareType == FW_MARLIN && ack_seen(":paused"))
    { // if paused with ADVANCED_PAUSE_FEATURE enabled in Marlin (:paused),
      // disable Resume/Pause button in the Printing menu
      hostDialog = true;
    }

    setPrintPause(HOST_STATUS_PAUSING, PAUSE_EXTERNAL);

    if (ack_continue_seen("filament_runout"))
      setRunoutAlarmTrue();
  }
  else if (ack_seen(":resume") || ack_seen(":resumed"))
  {
    hostDialog = false;  // enable Resume/Pause button in the Printing menu

    setPrintResume(HOST_STATUS_RESUMING);
  }
  else if (ack_seen(":cancel"))  // to be added to Marlin abortprint routine
  {
    setPrintAbort();
  }
  //TG was else if
  if (ack_seen(":prompt_begin "))
  {
    strcpy(hostAction.prompt_begin, ack_cache + ack_index);
    hostAction.button = 0;
    hostAction.prompt_show = true;

    if (ack_continue_seen("Nozzle Parked"))
    {
      setPrintPause(HOST_STATUS_PAUSING, PAUSE_EXTERNAL);
    }
    else if (ack_continue_seen("Resuming"))  // resuming from TFT media or (remote) onboard media
    {
      setPrintResume(HOST_STATUS_RESUMING);

      hostAction.prompt_show = false;
      sendEmergencyCmd("M876 S0\n");  // auto-respond to a prompt request that is not shown on the TFT
    }
    else if (ack_continue_seen("Reheating"))
    {
      hostAction.prompt_show = false;
      sendEmergencyCmd("M876 S0\n");  // auto-respond to a prompt request that is not shown on the TFT
    }
  }
  else if (ack_seen(":prompt_button "))
  {
    strcpy(hostAction.prompt_button[hostAction.button++], ack_cache + ack_index);
  }
  //TG was else if
  if (ack_seen(":prompt_show") && hostAction.prompt_show)
  {
    BUZZER_PLAY(SOUND_NOTIFY);

    switch (hostAction.button)
    {
      case 0:
        popupDialog(DIALOG_TYPE_ALERT, (uint8_t *)"Message", (uint8_t *)hostAction.prompt_begin,
                    LABEL_CONFIRM, LABEL_NULL, setRunoutAlarmFalse, NULL, NULL);
        break;

      case 1:
        popupDialog(DIALOG_TYPE_ALERT, (uint8_t *)"Action command", (uint8_t *)hostAction.prompt_begin,
                    (uint8_t *)hostAction.prompt_button[0], LABEL_NULL, breakAndContinue, NULL, NULL);
        break;

      case 2:
        popupDialog(DIALOG_TYPE_ALERT, (uint8_t *)"Action command", (uint8_t *)hostAction.prompt_begin,
                    (uint8_t *)hostAction.prompt_button[0], (uint8_t *)hostAction.prompt_button[1], resumeAndPurge, resumeAndContinue, NULL);
        break;
    }
  }
}

/********************************************************************************************************************
   Look for any incoming message in ack_cache(RAM) and then parse each token found (ending with \n) until the
   UART Rx DMA buffer is empty. Error msgs will be handled if the word "Error:" is seen, but none of the other
   parsed tokens is processed yet till one these cases are detected: "@" and "T:" or  "@" and "B:" or just "T0:"
   After pasring, if the source of the message was not the printer(SERIAL_PORT], echo the ack_cache on to all  
   other active serial ports (up to _UART_CNT which is currently 6).
*********************************************************************************************************************/
void parseACK(void)  // ***** this is the main msg parser for RECEIVED serial data from host SERIAL_PORT
{
  while ((ack_len = Serial_Get(SERIAL_PORT, ack_cache, ACK_CACHE_SIZE)) != 0)  // if some data have been retrieved
  {
    #if defined(SERIAL_DEBUG_PORT) && defined(DEBUG_SERIAL_COMM)
      // dump raw serial data received to debug port
      Serial_Put(SERIAL_DEBUG_PORT, "<<");
      Serial_Put(SERIAL_DEBUG_PORT, ack_cache);
    #endif

    bool avoid_terminal = false;

    if (infoHost.connected == false)  // not connected to printer
    {
      // parse error information even though not connected to printer
      if (ack_seen(magic_error)) ackPopupInfo(magic_error);

      //the first response should be such as "T0:25/50\n", Marlin sends only "T:25/50\n" when only 1 hotend
      //TG 1/2/2020 added the !ack_seen("B:") term so that we acknowledge printer with 0 extruders, 0 hotends
      // were any of these below seen in the msg?  If not skip ahead to parse_end
      // skip to parse end till we see  "@" and "T:"  or  "@" and "B:"   or just "T0:"
      if (!(ack_seen("@") && (ack_seen("T:") || ack_seen("B:"))) && !ack_seen("T0:"))  goto parse_end;

      // find hotend count and setup heaters
      uint8_t i;
      //TG 1/9/20 redid this calc to allow zero extruders, added && !(ack_seen("T:") && i==0) term
      // and change formula for infoSettings.hotend_count after the for loop
      for (i = TOOL0; i < MAX_SPINDLE_COUNT; i++)
      {
        if(!ack_seen(heaterID[i]) && !(ack_seen("T:") && i==0)) break;  // didn't see "Tn:", "T:" at start of line?
      }

      infoSettings.hotend_count = i;     

      if (infoSettings.ext_count < infoSettings.hotend_count) infoSettings.ext_count = infoSettings.hotend_count;
      if (ack_seen(heaterID[BED])) infoSettings.bed_en = ENABLED;
      if (ack_seen(heaterID[CHAMBER])) infoSettings.chamber_en = ENABLED;

      updateNextHeatCheckTime();

      if (!ack_seen("@"))  // it's RepRapFirmware
      {
        storeCmd("M92\n");
        storeCmd("M115\n");  // as last command to identify the FW type!
        coordinateQuerySetWait(true);
      }
      else if (infoMachineSettings.firmwareType == FW_NOT_DETECTED)  // if never connected to the printer since boot
      {
        storeCmd("M503\n");  // query detailed printer capabilities
        storeCmd("M92\n");   // steps/mm of extruder is an important parameter for Smart filament runout
                             // avoid can't getting this parameter due to disabled M503 in Marlin
        storeCmd("M211\n");  // retrieve the software endstops state
        storeCmd("M115\n");  // as last command to identify the FW type!
      }

      infoHost.connected = true;
      requestCommandInfo.inJson = false;
    }

    /* onboard media gcode command response start */
    // Process Onboard sd Gcode command response
    //TG only if using resetRequestCommandInfo() to wait for a response
    if (requestCommandInfo.inWaitResponse)
    {
      if (ack_seen(requestCommandInfo.startMagic))
      {
        requestCommandInfo.inResponse = true;
        requestCommandInfo.inWaitResponse = false;
      }
      else if ((requestCommandInfo.error_num > 0 && ack_seen(requestCommandInfo.magic_error[0])) ||
               (requestCommandInfo.error_num > 1 && ack_seen(requestCommandInfo.magic_error[1])) ||
               (requestCommandInfo.error_num > 2 && ack_seen(requestCommandInfo.magic_error[2])))
      { // parse onboard media error
        requestCommandInfo.done = true;
        requestCommandInfo.inResponse = false;
        requestCommandInfo.inError = true;
        requestCommandInfo.inWaitResponse = false;

        if (requestCommandInfo.stream_handler != NULL)
        {
          clearRequestCommandInfo();  // unused if the streaming handler is involved
          requestCommandInfo.stream_handler(ack_cache);
        }
        else
        {
          strcpy(requestCommandInfo.cmd_rev_buf, ack_cache);
        }

        BUZZER_PLAY(SOUND_ERROR);
        goto parse_end;
      }

      requestCommandInfo.inJson = false;
    }

    if (requestCommandInfo.inResponse)
    {
      if (requestCommandInfo.stream_handler != NULL)
      {
        clearRequestCommandInfo();  // unused if the streaming handler is involved
        requestCommandInfo.stream_handler(ack_cache);

        if (ack_seen(requestCommandInfo.stopMagic))
        {
          requestCommandInfo.done = true;
          requestCommandInfo.inResponse = false;
        }
      }
      else if (strlen(requestCommandInfo.cmd_rev_buf) + strlen(ack_cache) < CMD_MAX_REV)
      {
        strcat(requestCommandInfo.cmd_rev_buf, ack_cache);

        if (ack_seen(requestCommandInfo.stopMagic))
        {
          requestCommandInfo.done = true;
          requestCommandInfo.inResponse = false;
        }
      }
      else
      {
        requestCommandInfo.done = true;
        requestCommandInfo.inResponse = false;
        ackPopupInfo(magic_error);
      }

      infoHost.wait = false;
      requestCommandInfo.inJson = false;
      goto parse_end;
    }
    /* onboard media gcode command response end */

    /* RepRap response handle */
    if (!requestCommandInfo.inWaitResponse && !requestCommandInfo.inResponse && infoMachineSettings.firmwareType == FW_REPRAPFW)
    {
      if (strchr(ack_cache, '{') != NULL)
        requestCommandInfo.inJson = true;
    }

    if (requestCommandInfo.inJson)
    {
      if (ack_seen(magic_warning))
        ackPopupInfo(magic_warning);
      #ifndef NO_RRFSupport   //TG 3/2/23 added
      else
        rrfParseACK(ack_cache);
      #endif

      infoHost.wait = false;
      goto parse_end;
    }
    /* RepRap response handle end */

    if (ack_starts_with("ok"))  // handle "ok" response
    { // it is checked first (and not later on) because it is the most frequent response during printing
      infoHost.wait = false;

      // if regular "ok\n" response or ADVANCED_OK response (Marlin) (e.g. "ok N10 P15 B3\n")
      if ((ack_cache[ack_index] == '\n') || (ack_continue_seen("P") && ack_continue_seen("B")))
        goto parse_end;  // there's nothing else to check for, don't parse
    }

    if (ack_starts_with("wait"))  // suppress "wait" from terminal
    { // it is checked second (and not later on) because it is the most frequent response during printer idle
      avoid_terminal = !infoSettings.terminal_ack;
    }

    //----------------------------------------
    // Pushed / polled / on printing parsed responses
    //----------------------------------------

    // parse and store temperatures
    else if ((ack_seen("@") && ack_seen("T:")) || ack_seen("T0:") || ack_seen("B:") || ack_seen("S0:"))   //TG 1/9/20 added B: for when extruders=0
    {
       //TG 1/9/20 depending on # extruders/hotends we will see from Marlin:
       //  0 hotends = no T: or T0:      1 hotend = T: but not T0:     >1 hotend = T: and T0:, T1:, T2:,......
       //  B: or C: if they are defined will be present
       //  The heaterID[] array will be adjusted according to HOTEND_NUM size in Configuration.h

      uint8_t heaterIndex = TOOL0;
      if (infoSettings.hotend_count == 1)   //TG 2/24/23 handles 1 hotend case and heaterIndex=0
      {
        heatSetCurrentTemp(heaterIndex, ack_value() + 0.5f);
        heatSetTargetTemp(heaterIndex, ack_second_value() + 0.5f, FROM_HOST);
        heaterIndex = BED;
      }

      while (heaterIndex < MAX_HEATER_COUNT)
      { //TG 2/24/23 handle more heaterIndex cases if heaterID was valid and seen
        if (heaterDisplayIsValid(heaterIndex) && (ack_seen(heaterID[heaterIndex])))
        {
          heatSetCurrentTemp(heaterIndex, ack_value() + 0.5f);
          heatSetTargetTemp(heaterIndex, ack_second_value() + 0.5f, FROM_HOST);
        }
        heaterIndex++;
      }

      //TG 2/20/21 added this to read actual speed from Marlin RPM sensor
      if (ack_seen("S0:")) {   
          spindleSetCurSpeed(0,ack_second_value() + 0.5f);
          //drawSingleLiveIconLine();  //TG 2/21/21 update the StatusScreen spindle speed immediately
      }

      //TG 10/4/22 - parse and store WCS from Temperature AutoReport - modified Marlin AutoReportTemp::report()
      // to append this at end of line (can also use G39 to get active workspace and G39 T to get all worspaces).      
      if (ack_seen("WCS:"))
      {
        if (infoMachineSettings.active_workspace != ack_value())
        { 
          infoMachineSettings.active_workspace = ack_value();
          nextWCSupdate = true;
          //infoHost.wait = false;
        }
      }

      //TG 2/28/23 - parse and store the status of the VFD connection - modified Marlin AutoReportTemp::report()
      // to append this at end of line 
      if (ack_seen("VFD:"))
      {
        VFDpresent = ack_value();
      }
      
      avoid_terminal = !infoSettings.terminal_ack;
      updateNextHeatCheckTime();
    }

    // parse and store M114, current position
    else if (ack_starts_with("X:") || ack_seen("C: X:"))  // Smoothieware axis position starts with "C: X:"
    {
      coordinateSetAxisActual(X_AXIS, ack_value());
      if (ack_continue_seen("Y:"))
      {
        coordinateSetAxisActual(Y_AXIS, ack_value());
        if (ack_continue_seen("Z:"))
        {
          coordinateSetAxisActual(Z_AXIS, ack_value());
          if (ack_continue_seen("E:"))
          {
            coordinateSetAxisActual(E_AXIS, ack_value());
          }
        }
      }

      coordinateQuerySetWait(false);
    }
    // parse and store M114 E, extruder position. Required "M114_DETAIL" in Marlin
    else if (ack_seen("Count E:"))
    {
        // Parse actual extruder position, response of "M114 E\n", required "M114_DETAIL" in Marlin
        coordinateSetExtruderActualSteps(ack_value());
    }
    // parse and store feed rate percentage
    else if (ack_seen("FR:"))
    {
      speedSetCurPercent(0, ack_value());
      speedQuerySetWait(false);
    }
    // parse and store flow rate percentage
    else if (ack_seen("Flow: "))
    {
      speedSetCurPercent(1, ack_value());
      speedQuerySetWait(false);
    }
    // parse and store feed rate percentage in case of Smoothieware
    else if ((infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) && ack_seen("Speed factor at "))
    {
      speedSetCurPercent(0, ack_value());
      speedQuerySetWait(false);
    }
    // parse and store flow rate percentage in case of Smoothieware
    else if ((infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) && ack_seen("Flow rate at "))
    {
      speedSetCurPercent(1, ack_value());
      speedQuerySetWait(false);
    }
    // parse and store M106, fan speed
    else if (ack_starts_with("M106"))
    {
      fanSetCurSpeed(ack_continue_seen("P") ? ack_value() : 0, ack_seen("S") ? ack_value() : 100);
    }
    // parse and store M710, controller fan
    else if (ack_starts_with("M710"))
    {
      if (ack_seen("S"))
        fanSetCurSpeed(MAX_COOLING_FAN_COUNT, ack_value());
      if (ack_seen("I"))
        fanSetCurSpeed(MAX_COOLING_FAN_COUNT + 1, ack_value());

      ctrlFanQuerySetWait(false);
    }
    // parse pause message
    else if (!infoMachineSettings.promptSupport && ack_seen("paused for user"))
    {
      popupDialog(DIALOG_TYPE_QUESTION, (uint8_t *)"Printer is Paused", (uint8_t *)"Paused for user\ncontinue?",
                    LABEL_CONFIRM, LABEL_NULL, breakAndContinue, NULL, NULL);
    }
    // parse host action commands. Required "HOST_ACTION_COMMANDS" and other settings in Marlin
    else if (ack_starts_with("//action:"))
    {
      hostActionCommands();
    }
    // parse and store M118, filament data update
    else if (ack_seen("filament_data"))
    {
      if (ack_continue_seen("L:"))
        ack_values_sum(&infoPrintSummary.length);
      else if (ack_continue_seen("W:"))
        ack_values_sum(&infoPrintSummary.weight);
      else if (ack_continue_seen("C:"))
        ack_values_sum(&infoPrintSummary.cost);

      infoPrintSummary.hasFilamentData = true;
    }
    // parse and store M23, select SD file
    else if (infoMachineSettings.onboardSD == ENABLED && ack_starts_with("File opened:"))
    {
      // NOTE: this block is not reached in case of printing from onboard media because startPrint() in Printing.c will
      //       call request_M23_M36() that will be managed in parseAck() by the block "Onboard media response handling"
      //       provided at the beginning of this funtion

      // parse file name.
      // Format: "File opened: <file name> Size: <YYYY>" (e.g. "File opened: 1A29A~1.GCO Size: 6974")
      //
      char file_name[MAX_PATH_LEN];
      char * end_string = " Size:";

      uint16_t start_index = ack_index;
      uint16_t end_index = ack_continue_seen(end_string) ? (ack_index - strlen(end_string)) : start_index;
      uint16_t path_len = MIN(end_index - start_index, MAX_PATH_LEN - 1);

      memcpy(file_name, ack_cache + start_index, path_len);
      file_name[path_len] = '\0';

      // start print originated by remote host but hosted by onboard
      // (print from remote onboard media) and open Printing menu
      startPrintingFromRemoteHost(file_name);
    }
    // parse and store M24, M27 and M73, if printing from (remote) onboard media
    else if (infoMachineSettings.onboardSD == ENABLED && WITHIN(infoFile.source, FS_ONBOARD_MEDIA, FS_ONBOARD_MEDIA_REMOTE) &&
             (ack_starts_with("Done printing file") || ack_seen("SD printing") || ack_starts_with("echo: M73")))
    {
      // NOTE FOR "M73": Required "SET_PROGRESS_MANUALLY" and "M73_REPORT" settings in Marlin

      // parse and store M24, received "Done printing file" (printing from (remote) onboard media completed)
      if (ack_starts_with("Done"))
      {
        endPrint();

      }
      // parse and store M27, received "SD printing byte" or "Not SD printing"
      else if (ack_seen("SD"))
      {
        if (infoHost.status == HOST_STATUS_RESUMING)
          setPrintResume(HOST_STATUS_PRINTING);
        else if (infoHost.status == HOST_STATUS_PAUSING)
          setPrintPause(HOST_STATUS_PAUSED, PAUSE_EXTERNAL);

        if (infoHost.status == HOST_STATUS_PRINTING)
        {
          if (ack_continue_seen("byte"))  // received "SD printing byte"
          {
            // parse file data progress.
            // Format: "SD printing byte <XXXX>/<YYYY>" (e.g. "SD printing byte 123/12345")
            //
            setPrintProgressData(ack_value(), ack_second_value());
          }
          else  // received "Not SD printing"
          {
            setPrintAbort();
          }
        }
      }
      // parse and store M73
      else
      {
        // parse progress percentage and remaining time.
        // Format: "M73 Progress: <XX>%; Time left: <YY>m;" (e.g. "M73 Progress: 40%; Time left: 2m;")

        if (ack_seen("Progress:"))
        {
          setPrintProgressSource(PROG_SLICER);
          setPrintProgressPercentage(ack_value());
        }

        if (ack_seen("Time left:"))
        {
          setPrintRemainingTime(ack_value() * 60);
          setTimeFromSlicer(true);  // disable parsing remaning time from gcode comments

          if (getPrintProgressSource() < PROG_TIME && infoSettings.prog_source == 1)
            setPrintProgressSource(PROG_TIME);
        }
      }
    }

    //----------------------------------------
    // Tuning parsed responses
    //----------------------------------------

    // parse and store build volume size
    else if (ack_seen("work:"))
    {
      if (ack_continue_seen("min:"))
      {
        if (ack_continue_seen("x:")) infoSettings.machine_size_min[X_AXIS] = ack_value();
        if (ack_continue_seen("y:")) infoSettings.machine_size_min[Y_AXIS] = ack_value();
        if (ack_continue_seen("z:")) infoSettings.machine_size_min[Z_AXIS] = ack_value();
      }

      if (ack_continue_seen("max:"))
      {
        if (ack_continue_seen("x:")) infoSettings.machine_size_max[X_AXIS] = ack_value();
        if (ack_continue_seen("y:")) infoSettings.machine_size_max[Y_AXIS] = ack_value();
        if (ack_continue_seen("z:")) infoSettings.machine_size_max[Z_AXIS] = ack_value();
      }
    }
    // parse M48, repeatability test
    else if (ack_starts_with("Mean:"))
    {
      char tmpMsg[100];

      strcpy (tmpMsg, "Mean: ");
      sprintf (&tmpMsg[strlen(tmpMsg)], "%0.5f", ack_value());

      if (ack_continue_seen("Min: "))
        sprintf(&tmpMsg[strlen(tmpMsg)], "\nMin: %0.5f", ack_value());
      if (ack_continue_seen("Max: "))
        sprintf(&tmpMsg[strlen(tmpMsg)], "\nMax: %0.5f", ack_value());
      if (ack_continue_seen("Range: "))
        sprintf(&tmpMsg[strlen(tmpMsg)], "\nRange: %0.5f", ack_value());

      popupReminder(DIALOG_TYPE_INFO, (uint8_t *)"Repeatability Test", (uint8_t *)tmpMsg);
    }
    // parse M48, standard deviation
    else if (ack_seen("Standard Deviation: "))
    {
      char tmpMsg[100];

      if (memcmp((char *)getDialogMsgStr(), "Mean: ", 6) == 0)
      {
        levelingSetProbedPoint(-1, -1, ack_value());  // save probed Z value
        sprintf(tmpMsg, "%s\nStandard Deviation: %0.5f", (char *)getDialogMsgStr(), ack_value());

        popupReminder(DIALOG_TYPE_INFO, (uint8_t *)"Repeatability Test", (uint8_t *)tmpMsg);
      }
    }
    // parse and store M211 or M503, software endstops state (e.g. from Probe Offset, MBL, Mesh Editor menus)
    else if (ack_starts_with("M211") || ack_seen("Soft endstops"))
    {
      uint8_t curValue = infoMachineSettings.softwareEndstops;
      infoMachineSettings.softwareEndstops = ack_continue_seen("ON");

      if (curValue != infoMachineSettings.softwareEndstops)  // send a notification only if status is changed
        addToast(DIALOG_TYPE_INFO, ack_cache);
    }
    //TG 2/14/21 removed Pid.c for CNC, so commented out
    // parse M303, PID autotune finished message
    /*
    else if (ack_starts_with("PID Autotune finished"))
    {
      pidUpdateStatus(PID_SUCCESS);
    }
    // parse M303, PID autotune failed message
    else if (ack_starts_with("PID Autotune failed"))
    {
      pidUpdateStatus(PID_FAILED);
    }
    // parse M303, PID autotune finished message in case of Smoothieware
    else if ((infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) && ack_seen("PID Autotune Complete!"))
    {
      //ack_index += 84; -> need length check
      pidUpdateStatus(PID_SUCCESS);
    }
    // parse M303, PID autotune failed message in case of Smoothieware
    else if ((infoMachineSettings.firmwareType == FW_SMOOTHIEWARE) && ack_seen("// WARNING: Autopid did not resolve within"))
    {
      pidUpdateStatus(PID_FAILED);
    }
    // parse M306, model predictive temperature control tuning end message (interrupted or finished)
    else if (ack_seen("MPC Autotune"))
    {
      if (ack_continue_seen("finished"))
        setMpcTuningResult(FINISHED);
      else if (ack_continue_seen("interrupted"))
        setMpcTuningResult(INTERRUPTED);
    }
    */
    
    // parse and store M355, case light message
    #ifndef NO_CASE_LIGHT  //TG 3/2/23 added global flag to exclude this code
    else if (ack_seen("Case light:"))
    {
      if (ack_continue_seen("OFF"))
      {
        caseLightSetState(false);
      }
      else
      {
        caseLightSetState(true);
        caseLightSetBrightness(ack_value());
      }
    }
    #endif
    
    //TG 7/17/22 Removed MeshTuner.c and MeshEditor.c, so following commented out
    /*
    // parse and store M420 V1 T1, mesh data (e.g. from Mesh Editor menu)
    //
    // IMPORTANT: It must be placed before the following keywords:
    //            1) echo:Bed Leveling
    //            2) mesh. Z offset:
    //
    else if (meshIsWaitingData())
    {
      meshUpdateData(ack_cache);  // update mesh data
    }
    */
    
    // parse and store M420 V1 T1 or G29 S0 (mesh. Z offset:) or M503 (G29 S4 Zxx), MBL Z offset value (e.g. from Babystep menu)
    else if (ack_seen("mesh. Z offset:") || ack_seen("G29 S4 Z"))
    {
      setParameter(P_MBL_OFFSET, 0, ack_value());
    }
    // parse and store M290 (Probe Offset) or M503 (M851), probe offset value (e.g. from Babystep menu) and
    // X an Y probe offset for LevelCorner position limit
    else if (ack_seen("Probe Offset") || ack_starts_with("M851"))
    {
      if (ack_seen("X")) setParameter(P_PROBE_OFFSET, AXIS_INDEX_X, ack_value());
      if (ack_seen("Y")) setParameter(P_PROBE_OFFSET, AXIS_INDEX_Y, ack_value());
      if (ack_seen("Z") || (ack_seen("Z:"))) setParameter(P_PROBE_OFFSET, AXIS_INDEX_Z, ack_value());
    }
    // parse G29 (ABL) + M118, ABL completed message (ABL, BBL, UBL) (e.g. from ABL menu)
    #ifndef NO_ABL  //TG 3/2/23 added global flag to exclude this code
    else if (ack_seen("ABL Completed"))
    {
      ablUpdateStatus(true);
    }
    #endif
    // parse G29 (MBL), MBL completed message (e.g. from MBL menu)
    else if (ack_seen("Mesh probing done"))
    {
      #ifndef NO_UNIFIED_HEAT_MENU  //TG 3/2/23 added global flag to exclude this code
      mblUpdateStatus(true);
      #endif
    }
    // parse G30, feedback to get the 4 corners Z value returned by Marlin for LevelCorner menu
    else if (ack_seen("Bed X: "))
    {
      float x = ack_value();
      float y = 0;

      if (ack_continue_seen("Y: ")) y = ack_value();
      if (ack_continue_seen("Z: ")) levelingSetProbedPoint(x, y, ack_value());  // save probed Z value
    }
    #if DELTA_PROBE_TYPE != 0
      // parse and store Delta calibration settings
      else if (ack_seen("Calibration OK"))
      {
        BUZZER_PLAY(SOUND_SUCCESS);

          if (infoMachineSettings.EEPROM == 1)
          {
            popupDialog(DIALOG_TYPE_SUCCESS, LABEL_DELTA_CONFIGURATION, LABEL_EEPROM_SAVE_INFO,
                        LABEL_CONFIRM, LABEL_CANCEL, saveEepromSettings, NULL, NULL);
          }
          else
          {
            popupReminder(DIALOG_TYPE_SUCCESS, LABEL_DELTA_CONFIGURATION, LABEL_PROCESS_COMPLETED);
          }
        }
      #endif

    //----------------------------------------
    // Parameter / M503 / M115 parsed responses
    //----------------------------------------

    // parse and store filament diameter
    else if (ack_starts_with("M200"))
    {
      if (ack_starts_with("M200 S") || ack_seen("D0")) setParameter(P_FILAMENT_DIAMETER, 0, ack_value());

      uint8_t i = (ack_seen("T")) ? ack_value() : 0;

      if (ack_seen("D")) setParameter(P_FILAMENT_DIAMETER, 1 + i, ack_value());

      if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
      {
        // filament_diameter > 0.01 to enable volumetric extrusion. Otherwise (<= 0.01), disable volumetric extrusion
        setParameter(P_FILAMENT_DIAMETER, 0, getParameter(P_FILAMENT_DIAMETER, 1) > 0.01f ? 1 : 0);
      }
    }
    // parse and store axis steps-per-unit (steps/mm) (M92), max acceleration (units/s2) (M201) and max feedrate (units/s) (M203)
    else if (ack_starts_with("M92 ") || ack_starts_with("M201") || ack_starts_with("M203"))  // "M92 " to not trigger if "M928" received
    {
      PARAMETER_NAME param = P_STEPS_PER_MM;  // default value

      // using consecutive "if" instead of "if else if" on the following two lines just to reduce code
      // instead of optimizing performance (code typically not executed during a print)
      if (ack_starts_with("M201")) param = P_MAX_ACCELERATION;
      if (ack_starts_with("M203")) param = P_MAX_FEED_RATE;

      if (ack_seen("X")) setParameter(param, AXIS_INDEX_X, ack_value());
      if (ack_seen("Y")) setParameter(param, AXIS_INDEX_Y, ack_value());
      if (ack_seen("Z")) setParameter(param, AXIS_INDEX_Z, ack_value());

      uint8_t i = (ack_seen("T")) ? ack_value() : 0;

      if (ack_seen("E")) setParameter(param, AXIS_INDEX_E0 + i, ack_value());
    }
    // parse and store acceleration (units/s2)
    else if (ack_starts_with("M204 P"))
    {
                         setParameter(P_ACCELERATION, 0, ack_value());
      if (ack_seen("R")) setParameter(P_ACCELERATION, 1, ack_value());
      if (ack_seen("T")) setParameter(P_ACCELERATION, 2, ack_value());
    }
    // parse and store advanced settings
    else if (ack_starts_with("M205"))
    {
      if (ack_seen("X")) setParameter(P_JERK, AXIS_INDEX_X, ack_value());
      if (ack_seen("Y")) setParameter(P_JERK, AXIS_INDEX_Y, ack_value());
      if (ack_seen("Z")) setParameter(P_JERK, AXIS_INDEX_Z, ack_value());
      if (ack_seen("E")) setParameter(P_JERK, AXIS_INDEX_E0, ack_value());
      if (ack_seen("J")) setParameter(P_JUNCTION_DEVIATION, 0, ack_value());
    }
    // parse and store home offset (M206) and hotend offset (M218)
    else if (ack_starts_with("M206 X") || ack_starts_with("M218 T1 X"))
    {
      PARAMETER_NAME param = ack_starts_with("M206") ? P_HOME_OFFSET : P_HOTEND_OFFSET;

      if (ack_seen("X")) setParameter(param, AXIS_INDEX_X, ack_value());
      if (ack_seen("Y")) setParameter(param, AXIS_INDEX_Y, ack_value());
      if (ack_seen("Z")) setParameter(param, AXIS_INDEX_Z, ack_value());
    }
    // parse and store FW retraction (M207) and FW recover (M208)
    else if (ack_starts_with("M207 S") || ack_starts_with("M208 S"))
    {
      PARAMETER_NAME param = ack_starts_with("M207") ? P_FWRETRACT : P_FWRECOVER;

      if (ack_seen("S")) setParameter(param, 0, ack_value());
      if (ack_seen("W")) setParameter(param, 1, ack_value());
      if (ack_seen("F")) setParameter(param, 2, ack_value());

      if (param == P_FWRETRACT)
      {
        if (ack_seen("Z")) setParameter(param, 3, ack_value());
      }
      else  // P_FWRECOVER
      {
        if (ack_seen("R")) setParameter(param, 3, ack_value());
      }
    }
    // parse and store auto retract
    else if (ack_starts_with("M209 S"))
    {
      setParameter(P_AUTO_RETRACT, 0, ack_value());
    }
    // parse and store hotend PID (M301), bed PID (M304)
    else if (ack_starts_with("M301") || ack_starts_with("M304"))
    {
      PARAMETER_NAME param = ack_starts_with("M301") ? P_HOTEND_PID : P_BED_PID;

      if (ack_seen("P")) setParameter(param, 0, ack_value());
      if (ack_seen("I")) setParameter(param, 1, ack_value());
      if (ack_seen("D")) setParameter(param, 2, ack_value());
    }
    // parse and store model predictive temperature control (only for Marlin)
    else if (ack_starts_with("M306") && infoMachineSettings.firmwareType == FW_MARLIN)
    { /* //TG 2/26/23 removed for CNC
      if (ack_continue_seen("E"))
      {
        uint8_t index = ack_value();

        if (ack_continue_seen("P"))
          setMpcHeaterPower(index, ack_value());

        if (ack_continue_seen("H"))
          setMpcFilHeatCapacity(index, ack_value());
      }*/
    }
    // parse and store input shaping parameters (only for Marlin)
    else if (ack_starts_with("M593") && infoMachineSettings.firmwareType == FW_MARLIN)
    {
      // M593 accepts its parameters in any order,
      // if both X and Y axis are missing than the rest
      // of the parameters are referring to each axis

      enum
      {
        SET_NONE = 0B00,
        SET_X = 0B01,
        SET_Y = 0B10,
        SET_BOTH = 0B11
      } setAxis = SET_NONE;

      float pValue;

      if (ack_seen("X")) setAxis |= SET_X;
      if (ack_seen("Y")) setAxis |= SET_Y;
      if (setAxis == SET_NONE) setAxis = SET_BOTH;

      if (ack_seen("F"))
      {
        pValue = ack_value();

        if (setAxis & SET_X)
            setParameter(P_INPUT_SHAPING, 0, pValue);
        if (setAxis & SET_Y)
            setParameter(P_INPUT_SHAPING, 2, pValue);
      }

      if (ack_seen("D"))
      {
        pValue = ack_value();

        if (setAxis & SET_X)
            setParameter(P_INPUT_SHAPING, 1, pValue);
        if (setAxis & SET_Y)
            setParameter(P_INPUT_SHAPING, 3, pValue);
      }
    }
    // parse and store Delta configuration / Delta tower angle (M665) and Delta endstop adjustments (M666)
    //
    // IMPORTANT: It must be placed before the following keywords:
    //            1) M420 S
    //
    else if (ack_starts_with("M665") || ack_starts_with("M666"))
    {
      PARAMETER_NAME param = ack_starts_with("M665") ? P_DELTA_TOWER_ANGLE : P_DELTA_ENDSTOP;

      if (param < P_DELTA_ENDSTOP)  // options not supported by M666
      {
        if (ack_seen("H")) setParameter(P_DELTA_CONFIGURATION, 0, ack_value());
        if (ack_seen("S")) setParameter(P_DELTA_CONFIGURATION, 1, ack_value());
        if (ack_seen("R")) setParameter(P_DELTA_CONFIGURATION, 2, ack_value());
        if (ack_seen("L")) setParameter(P_DELTA_CONFIGURATION, 3, ack_value());
        if (ack_seen("A")) setParameter(P_DELTA_DIAGONAL_ROD, AXIS_INDEX_X, ack_value());
        if (ack_seen("B")) setParameter(P_DELTA_DIAGONAL_ROD, AXIS_INDEX_Y, ack_value());
        if (ack_seen("C")) setParameter(P_DELTA_DIAGONAL_ROD, AXIS_INDEX_Z, ack_value());
      }

      if (ack_seen("X")) setParameter(param, AXIS_INDEX_X, ack_value());
      if (ack_seen("Y")) setParameter(param, AXIS_INDEX_Y, ack_value());
      if (ack_seen("Z")) setParameter(param, AXIS_INDEX_Z, ack_value());
    }
    // parse and store ABL on/off state & Z fade value on M503
    else if (ack_starts_with("M420 S"))
    {
                                  setParameter(P_ABL_STATE, 0, ack_value());
      if (ack_continue_seen("Z")) setParameter(P_ABL_STATE, 1, ack_value());
    }
    // parse and store TMC stepping mode
    else if (ack_seen("Driver stepping mode:"))  // poll stelthchop settings separately
    {
      storeCmd("M569\n");
    }
    else if (ack_seen("driver mode:"))
    {
      float isStealthChop = ack_continue_seen("stealthChop");  // boolean type value also casted to float type
      STEPPER_INDEX stepperIndex = 0;

      if (ack_seen("X")) stepperIndex = STEPPER_INDEX_X;
      else if (ack_seen("Y")) stepperIndex = STEPPER_INDEX_Y;
      else if (ack_seen("Z")) stepperIndex = STEPPER_INDEX_Z;
      else if (ack_seen("E")) stepperIndex = STEPPER_INDEX_E0;

      if (stepperIndex < STEPPER_INDEX_E0)  // if "X", "X1", "X2", "Y", "Y1", "Y2", "Z", "Z1", "Z2", "Z3", "Z4"
      {
        if (ack_value() > 0)  // if "X"->0, "X1"->0, "X2"->1, "Y"->2, "Y1"->2, "Y2"->3, "Z"->4, "Z1"->4, "Z2"->5, "Z3"->6, "Z4"->7
          stepperIndex += ack_value() - 1;
      }

      setParameter(P_STEALTH_CHOP, stepperIndex, isStealthChop);
    }
    // parse and store linear advance factor
    else if (ack_starts_with("M900"))
    {
      uint8_t i = (ack_seen("T")) ? ack_value() : 0;

      if (ack_seen("K")) setParameter(P_LIN_ADV, i, ack_value());
    }
    // parse and store stepper motor current (M906), TMC hybrid threshold speed (M913) and TMC bump sensitivity (M914)
    else if (ack_starts_with("M906") || ack_starts_with("M913") || ack_starts_with("M914"))
    {
      PARAMETER_NAME param = P_CURRENT;  // default value

      // using consecutive "if" instead of "if else if" on the following two lines just to reduce code
      // instead of optimizing performance (code typically not executed during a print)
      if (ack_starts_with("M913")) param = P_HYBRID_THRESHOLD;
      if (ack_starts_with("M914")) param = P_BUMPSENSITIVITY;

      int8_t stepperIndex = (ack_seen("I")) ? ack_value() : 0;

      // if index is missing or set to -1 (meaning all indexes) then it must be converted to 0
      // to make sure array index is never negative
      if (stepperIndex < 0)
        stepperIndex = 0;

      // for M913 and M914, provided index is:
      //   1->"X1", 2->"X2", 1->"Y1", 2->"Y2", 1->"Z1", 2->"Z2", 3->"Z3", 4->"Z4"
      // and it must be converted to:
      //   0->"X1", 1->"X2", 0->"Y1", 1->"Y2", 0->"Z1", 1->"Z2", 2->"Z3", 3->"Z4"
      // to make sure array index is properly accessed
      if (param > P_CURRENT && stepperIndex > 0)
        stepperIndex--;

      if (ack_seen("X")) setParameter(param, STEPPER_INDEX_X + stepperIndex, ack_value());
      if (ack_seen("Y")) setParameter(param, STEPPER_INDEX_Y + stepperIndex, ack_value());
      if (ack_seen("Z")) setParameter(param, STEPPER_INDEX_Z + stepperIndex, ack_value());

      if (param < P_BUMPSENSITIVITY)  // T and E options not supported by M914
      {
        stepperIndex = (ack_seen("T")) ? ack_value() : 0;

        // if index is missing or set to -1 (meaning all indexes) then it must be converted to 0
        // to make sure array index is never negative
        if (stepperIndex < 0)
          stepperIndex = 0;

        if (ack_seen("E")) setParameter(param, STEPPER_INDEX_E0 + stepperIndex, ack_value());
      }
    }
    // parse and store ABL type if auto-detect is enabled
    #if BED_LEVELING_TYPE == 1
      else if (ack_seen("Auto Bed Leveling"))
      {
        infoMachineSettings.leveling = BL_ABL;
      }
      else if (ack_seen("Unified Bed Leveling"))
      {
        infoMachineSettings.leveling = BL_UBL;
      }
      else if (ack_seen("Mesh Bed Leveling"))
      {
        infoMachineSettings.leveling = BL_MBL;
      }
    #endif
    // parse M115 capability report
    else if (ack_seen("FIRMWARE_NAME:"))
    {
      uint8_t * string = (uint8_t *)&ack_cache[ack_index];
      uint16_t string_start = ack_index;
      uint16_t string_end = string_start;

      if (ack_continue_seen("Marlin"))
        setupMachine(FW_MARLIN);
      else if (ack_continue_seen("RepRapFirmware"))
        setupMachine(FW_REPRAPFW);
      else if (ack_continue_seen("Smoothieware"))
        setupMachine(FW_SMOOTHIEWARE);
      else
        setupMachine(FW_UNKNOWN);

      if (ack_seen("FIRMWARE_URL:"))  // For Smoothieware
        string_end = ack_index - sizeof("FIRMWARE_URL:");   
      else if (ack_seen("SOURCE_CODE_URL:"))  // For Marlin
        string_end = ack_index - sizeof("SOURCE_CODE_URL:");  //TG set string_end ptr to start of "SOURCE_CODE_URL:"
      else if ((infoMachineSettings.firmwareType == FW_REPRAPFW) && ack_seen("ELECTRONICS"))  // For RepRapFirmware
        string_end = ack_index - sizeof("ELECTRONICS");

      infoSetFirmwareName(string, string_end - string_start);   // Set firmware name
        
      //TG 2/24/23 added new code to obtain source code folder name from Marlin
      string = (uint8_t *)&ack_cache[ack_index];
      string_start = ack_index;
      if (ack_seen("PROTOCOL_VERSION:"))
      { //TG set string_end ptr to start of "SOURCE_CODE_URL:" 
        string_end = ack_index - sizeof("PROTOCOL_VERSION:");     
        infoSetSourceCodeURL(string, string_end - string_start);  // Set src code folder name
      }


      if (ack_seen("MACHINE_TYPE:"))
      {
        string = (uint8_t *)&ack_cache[ack_index];
        string_start = ack_index;

        if (ack_seen("EXTRUDER_COUNT:"))
        {
          if (MIXING_EXTRUDER == 0)
            infoSettings.ext_count = ack_value();

          string_end = ack_index - sizeof("EXTRUDER_COUNT:");
        }

        infoSetMachineType(string, string_end - string_start);  // set firmware name
      }
    }
    else if (ack_starts_with("Cap:"))  //TG prior version ran all if tests under here all the time
    {
      if (ack_continue_seen("EEPROM:"))
      {
        infoMachineSettings.EEPROM = ack_value();
      }
      else if (ack_continue_seen("AUTOREPORT_TEMP:"))
      {
        infoMachineSettings.autoReportTemp = ack_value();

        if (infoMachineSettings.autoReportTemp)
          storeCmd("M155 S%u\n", heatGetUpdateSeconds());
      }
      else if (ack_continue_seen("AUTOREPORT_POS:"))
      {
        infoMachineSettings.autoReportPos = ack_value();
      }
      else if (ack_continue_seen("AUTOLEVEL:") && infoMachineSettings.leveling == BL_DISABLED)
      {
        infoMachineSettings.leveling = ack_value();
      }
      else if (ack_continue_seen("Z_PROBE:"))
      {
        infoMachineSettings.zProbe = ack_value();
      }
      else if (ack_continue_seen("LEVELING_DATA:"))
      {
        infoMachineSettings.levelingData = ack_value();
      }
      else if (ack_continue_seen("SOFTWARE_POWER:"))
      {
        infoMachineSettings.softwarePower = ack_value();
      }
      else if (ack_continue_seen("TOGGLE_LIGHTS:"))
      {
        infoMachineSettings.toggleLights = ack_value();
      }
      else if (ack_continue_seen("CASE_LIGHT_BRIGHTNESS:"))
      {
        infoMachineSettings.caseLightsBrightness = ack_value();
      }
      else if (ack_continue_seen("EMERGENCY_PARSER:"))
      {
        infoMachineSettings.emergencyParser = ack_value();
      }
      else if (ack_continue_seen("PROMPT_SUPPORT:"))
      {
        infoMachineSettings.promptSupport = ack_value();
      }
      else if (ack_continue_seen("SDCARD:") && infoSettings.onboard_sd == AUTO)
      {
        infoMachineSettings.onboardSD = ack_value();
      }
      else if (ack_continue_seen("MULTI_VOLUME:"))
      {
        infoMachineSettings.multiVolume = ack_value();
      }
      else if (ack_continue_seen("AUTOREPORT_SD_STATUS:"))
      {
        infoMachineSettings.autoReportSDStatus = ack_value();
      }
      else if (ack_continue_seen("LONG_FILENAME:") && infoSettings.long_filename == AUTO)
      {
        infoMachineSettings.longFilename = ack_value();
      }
      else if (ack_continue_seen("BABYSTEPPING:"))
      {
        infoMachineSettings.babyStepping = ack_value();
      }
      else if (ack_continue_seen("BUILD_PERCENT:"))  // M73 support. Required "SET_PROGRESS_MANUALLY" in Marlin
      {
        infoMachineSettings.buildPercent = ack_value();
      }
      else if (ack_continue_seen("CHAMBER_TEMPERATURE:") && infoSettings.chamber_en == DISABLED)  // auto-detect only if set to disabled
      {
        infoSettings.chamber_en = ack_value();
      }
    }

    //----------------------------------------
    // Error / echo parsed responses
    //----------------------------------------

    // parse error messages
    else if (ack_seen(magic_error))
    {
      ackPopupInfo(magic_error);
    }
    // parse echo messages
    else if (ack_starts_with(magic_echo))
    {
      // parse and store M401 H, BLTouch HighSpeed mode
      if (ack_continue_seen("BLTouch HS mode"))
      { //TG 2/26/23 BL Touch is removed
        //setHSmode(ack_continue_seen("ON") ? HS_ON : HS_OFF);
      }
      // parse and store M420 V1 T1 or M420 Sxx, ABL state (e.g. from Bed Leveling menu)
      else if (ack_continue_seen("Bed Leveling"))
      {
        setParameter(P_ABL_STATE, 0, ack_continue_seen("ON") ? ENABLED : DISABLED);
      }
      else if (ack_continue_seen("Fade Height"))
      {
        setParameter(P_ABL_STATE, 1, ack_value());
      }
      // newer Marlin (e.g. 2.0.9.3) returns this ACK for M900 command
      else if (ack_continue_seen("Advance K="))
      {
        setParameter(P_LIN_ADV, heatGetCurrentTool(), ack_value());
      }
      else if (!processKnownEcho())  // if no known echo was found and processed, then popup the echo message
      {
        ackPopupInfo(magic_echo);
      }
    }
      
    else if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
    {
      if (ack_seen("ZProbe triggered before move"))  // smoothieboard ZProbe triggered before move, aborting command
      {
        ackPopupInfo("ZProbe triggered before move.\nAborting Print!");
      }
      // parse and store volumetric extrusion M200 response of Smoothieware
      else if (ack_seen("Volumetric extrusion is disabled"))
      {
        setParameter(P_FILAMENT_DIAMETER, 0, 0);
        setParameter(P_FILAMENT_DIAMETER, 1, 0.0f);
      }
      // parse and store volumetric extrusion M200 response of Smoothieware
      else if (ack_seen("Filament Diameter:"))
      {
        setParameter(P_FILAMENT_DIAMETER, 1, ack_value());
        // filament_diameter > 0.01 to enable volumetric extrusion. Otherwise (<= 0.01), disable volumetric extrusion
        setParameter(P_FILAMENT_DIAMETER, 0, getParameter(P_FILAMENT_DIAMETER, 1) > 0.01f ? 1 : 0);
      }
    }
    /*else if (infoMachineSettings.firmwareType == FW_REPRAPFW) //TG this was removed in this new version
      { // keep it here and parse it the latest
        if (ack_seen(warningmagic))
        {
          ackPopupInfo(warningmagic);
        }
        else if (ack_seen(messagemagic))
        {
          ackPopupInfo(messagemagic);
        }
        else if (ack_seen("access point "))
        {
          uint8_t *string = (uint8_t *)&dmaL2Cache[ack_index];
          uint16_t string_start = ack_index;
          uint16_t string_end = string_start;
          if (ack_seen(","))
            string_end = ack_index - 1 ;

          infoSetAccessPoint(string, string_end - string_start);  // Set access poing

          if (ack_seen("IP address "))
          {
            string = (uint8_t *)&dmaL2Cache[ack_index];
            string_start = ack_index;
            if (ack_seen("\n"))
              string_end = ack_index - 1;
            infoSetIPAddress(string, string_end - string_start);  // Set IP address
          }
        }
      }*/

  //===========================================================================================================================  
  //TG=============================== CUSTOM ADDED G Codes ====================================================================
  //===========================================================================================================================
  //TG 9/24/21 - check for custom message from Marlin's report_spindle_speed(), it's used to sync menuSpindle() in 
  //Spindle.c when Marlin gets a Spindle M3/4/5 command from it's USB serial port (like with Repetier Host or Pronterface).
  //Marlin has been modified at set_spindle_speed() so that report_spindle_speed() outputs to serial ports the message
  // "Spindle Pn A:actualspeed T:targetspeed" on an M3/4/5 receipt.
  //This handles speed changes immediately instead of waiting for the speed in M155 Autoreport which only comes every 3 secs.
    else if (ack_seen("Spindle")){                  
      if (ack_seen("T:")){
        actTarget = ack_value() + 0.5f;                                     // get the new target speed
        spindleState = actTarget > 0 ? 1 : 0;                               // set state correctly
        
        // set speed in marlin units from actTarget var, spindle will update when loopBackEnd() calls loopSpindle()
        spindleState==0 ?spindleSetSpeed(0, 0):spindleSetSpeed(0, actTarget);
        lastSetSpindleSpeed[0] = actTarget;                                 // needed by loopSpindle() to know of speed change
        
        if(infoMenu.menu[infoMenu.cur] == menuSpindle){                     // update screen if in Spindle Menu
          spindleItems.items[KEY_ICON_6] = itemSpindleONOFF[spindleState];  // update icon/label in menu
          menuDrawItem(&spindleItems.items[KEY_ICON_6], KEY_ICON_6);        // redraw the menu with updates
          updateSpeedStatusDisplay(0, false);
        }

        // Handle auto vacuum mode
        if(actTarget>0){
          if((vacuumState & 2) == 2)                                        // turn vacuum on if in auto mode (state bit 1 is set)
            vacuum_set(255);                                                // set vacuum on
        }                                   
        else{                                   
          if((vacuumState & 2) == 2)                                        // turn vacuum off if in auto mode (state bit 1 is set)
            vacuum_set(0);                                                  // set vacuum off 
        }
        
      } // end of 9/24/21 TG addition 
    }

    /** //TG MODIFIED BY T.GIOIOSA  - adds M79xx for Spindle control
    * original 10/3/21
    * updated  12/24/22
    *
    * M codes for data exchange between Marlin and TFT35 screen over UART
    * The direction is from the TFT point of view
    * 
    *  gcode   function            TFT SEND to Marlin         Marlin RETURN to TFT       Description                                             If CMD issued from REMOTE USB host
    * -------  --------------    ---------------------      ------------------------     ------------------------------------                    -------------------------------------------------
    * M7900    AVRBlockInfo(-PID) use M7900 F....                    M7900 R             receive/send AVRInfoBlock.PIDFLAG, 
    *                                                                                    AVRInfoBlock.Reset_Flag, AVRInfoBlock.Display_Page, 
    *                                                                                    AVRInfoBlock.PID_Speed,  AVRInfoBlock.Update_EEPROM,
    *                                                                                    AVRInfoBlock.EE_chksum, AVRInfoBlock.dummy_pad_byte     *will echo to REMOTE SERIAL
    * M7979    PID flag           use M7979 Sx                       M7900 (blockinfo)   spindle_use_pid flag, pid on/off                        *also echoes to REMOTE SERIAL
    * M7980    RESET AVR flag     use M7980                          none                send reset AVR cmd to Marlin                            *will reset the AVR, echo ok only
    * M7981    PID Kp,Ki,Kd       use M7981 Px Ix Dx                 M7981 R             receive/send P,I,D constants Kp, Ki, Kd                 *also echoes to REMOTE SERIAL
    * M7982    AVR Display Page   use M7982 Px                       M7900 (blockinfo)   send AVR LCD display page # to Marlin                   *will change page, echo ok only
    * M7983    AVR PID speed      use M7983 Sx (0, 1, 2)             M7900 (blockinfo)   change the selected AVR PID speed                       *will change preset, echo ok only
    * M7984    AVR PID Reload     use M7984 Sx (1=current 2=default) M7900 (blockinfo)   reload a pid speed - TFT never needs to read back       *will reload, echo ok only
    * M7985    Vacuum Enable      happens via M42 P122               see M42()           Vacuum Enable state changed in Marlin                   *no response (must use M42 cmd)
    * M7986    Stock Top Z-axis   from printing M7986 Rx             M7986 Tx Zx         Sent during Printing Stock Top Z-axis value             *instructs Marlin to:
    *                                                                                        R=Get Stock_Top from current Z and subtract probe plate thickess(value after R)
    *                                                                                        S=Get Stock_Top from print gcode (val after S)(already corrected for probe thickness)
    *                                                                                        *Results are echoed only to the TFT SERIAL_PORT!
    * M7987    VFD Input Registers nothing	                          auto-sent           Marlin sends every 2s (10s when printing)(see vfd.cpp)    
    * M7988    VFD sw and comm     nothing	                          M7988 R             returns VFD sw_ver, cpu_ver, baudrate, format         
    * M7989    TFT print state     M7989 Px 		                      M7989 ok            TFT print state sent to Marlin (0=printing,1=printing)
    *
    *
    *
    * The info sent from TFT to Marlin is stored in Marlin's AVRInfoBlock struct and avrpid[]
    * which can be exchanged with the AVR Triac controller via I2C commands.
    *  
    */     
    #ifdef USING_AVR_TRIAC_CONTROLLER
        // receive entire AVRInfoBlock
        else if(ack_seen("M7900")){
          if(ack_seen("F")) {
                              AVRInfoBlock.PIDFLAG = ack_value(); 
            if(ack_seen("R")) AVRInfoBlock.Reset_Flag = ack_value(); 
            if(ack_seen("N")) AVRInfoBlock.Display_Page = ack_value(); 
            if(ack_seen("S")) AVRInfoBlock.PID_Speed= ack_value();
            if(ack_seen("U")) AVRInfoBlock.Update_EEPROM = ack_value(); 
            if(ack_seen("C")) AVRInfoBlock.EE_chksum = ack_value(); 
            if(ack_seen("B")) AVRInfoBlock.Reload_Preset = ack_value();
            if(ack_seen("D")) AVRInfoBlock.Data_Interval = ack_value();
            if(ack_seen("P")) AVRInfoBlock.PID_Interval = ack_value();
          }
          msg_complete = comp_7900;  // set flag on any M7900 ack_seen
        }
        else if(ack_seen("M7979 OK")){
          msg_complete = comp_7979;
        }
        
        else if(ack_seen("M7980 OK")){
          msg_complete = comp_7980;
        }

        // respond to a M7981 PID constants msg from Marlin (this is Marlin's response to a M7981 R(equest) msg from TFT)
        // the M7981 R msgs are generated in the avrTriac.c menu system
        else if(ack_seen("M7981")){
          if(ack_seen("P")) {
                              AVRInfoBlock.K[0] = ack_value();
            if(ack_seen("I")) AVRInfoBlock.K[1] = ack_value();
            if(ack_seen("D")) AVRInfoBlock.K[2] = ack_value();
          }
          msg_complete = comp_7981;    // set flag on any M7981 ack_seen
        } 
              
        else if(ack_seen("M7982 OK")){
            msg_complete = comp_7982;
        }
        
        else if(ack_seen("M7983 OK")){
            msg_complete = comp_7983;   
        }

        else if(ack_seen("M7984 OK")){
            msg_complete = comp_7984;   
        }

        else if(ack_seen("M7985")){
          if(ack_seen("S")){
            volatile uint8_t vv = ack_value();
            vacuumState = vv>0 ? (vacuumState | 1) : (vacuumState & ~1); // adjust state
          }
        }
    #endif
      // this one is ALWAYS needed for spindle in ALL contoller cases
      else if(ack_seen("M7986")){   
      if(ack_seen("T")){
        stockTopZaxis = ack_value();  // get value
        if(ack_seen("Z")){
          Marlin_ZMAX_POS = ack_value();  // get value
          msg_complete = comp_7986;
        }
      }
    }

    #ifdef USING_VFD_CONTROLLER
      else if(ack_seen("M7987")){   //TG 12/23/22 added for receiving VFD Input Registers, status, and decimal precision 
        if(ack_seen("OF")) {
                            inputReg.freq_out = ack_value(); 
        if(ack_seen("SF")) inputReg.freq_set = ack_value(); 
        if(ack_seen("OC")) inputReg.current_out = ack_value(); 
        if(ack_seen("OR")) inputReg.speed_out= ack_value();
        if(ack_seen("DC")) inputReg.dc_voltage = ack_value(); 
        if(ack_seen("AC")) inputReg.ac_voltage = ack_value(); 
        if(ack_seen("TP")) inputReg.temperature = ack_value();
        if(ack_seen("LF")) inputReg.fault_code = ack_value();
        if(ack_seen("RH")) inputReg.total_hours= ack_value();
        if(ack_seen("ST")) vfdStatus = ack_value();
        if(ack_seen("DP")) vfdP = ack_value();
        }
        // no msg_complete set here since M7987 is never requested, it comes automatically from Marlin
      }

      else if(ack_seen("M7988")){   //TG 12/23/22 added added for receiving VFD S/W ver, CPU ver, F164, and F165
      if(ack_seen("SW")) {                            
                          sw_ver = ack_value();  
      if(ack_seen("CP")) cpu_ver = ack_value(); 
      if(ack_seen("BR")) f164 = ack_value(); 
      if(ack_seen("FT")) f165 = ack_value();
      }
      msg_complete = comp_7988;  // this is here since TFT requests for an M7988 from Marlin
    }

      else if(ack_seen("M7989")){   //TG 2/13/23  added added for TFT print state acknowledged
        msg_complete = comp_7989;
      }
    
    #endif  // #ifdef USING_VFD_CONTROLLER

  //===========================================================================================================================
  //TG======================= END CUSTOM ADDED G Codes ========================================================================
  //===========================================================================================================================


  parse_end:
    // should we copy the msg to the Gcode Terminal cache?
    if (!avoid_terminal && MENU_IS(menuTerminal))
      terminalCache(ack_cache, ack_len, ack_port_index, SRC_TERMINAL_ACK);

    #ifdef SERIAL_PORT_2
      if (ack_port_index == PORT_1) //TG - WiFi port is considered a supplementary port
      {
        if (infoHost.wait == false && !ack_starts_with("ok"))
        { // if the ACK message is not related to a gcode originated by the TFT and it is not "ok", it is a spontaneous
          // ACK message so pass it to all the supplementary serial ports (since these messages came unrequested)
          Serial_Forward(SUP_PORTS, ack_cache);
        }
      }
      else
      { // if the ACK message is related to a gcode originated by a supplementary serial port,
        // forward the message to that supplementary serial port
        Serial_Forward(ack_port_index, ack_cache);
        // if "ok" has been received, reset ACK port index to avoid wrong relaying (in case no more commands will
        // be sent by interfaceCmd) of any successive spontaneous ACK message
        if (infoHost.wait == false)
          ack_port_index = PORT_1;
      }
    #endif
  } //while ((ack_len = Serial_Get(SERIAL_PORT, ack_cache, ACK_CACHE_SIZE)) != 0)
} //parseAck() 

/*TG 2/24/23 THIS MAY NOT BE NEEDED ANYMORE 
// ***********************************************************************************
// //TG           name          device    connected to      index
//               -----------   --------  ---------------   -----
//               SERIAL_PORT_2 _USART1   WiFi expansion    0
//               SERIAL_PORT   _USART2   printer(Marlin)   1  TFT
//               SERIAL_PORT_3 _USART3   ??                2
//               SERIAL_PORT_4 _UART4    ??                3
//                             _UART5    ??                4
// ***********************************************************************************

static bool SpuriousFlag = false;               //TG 2/14/23 added flag to prevent repeatedly displaying dialog box below
void setSpuriousFlag(){SpuriousFlag = true;}
void clrSpuriousFlag(){SpuriousFlag = false;}

char* SPort[] = {"SERIAL2 WiFi", "SERIAL1 Prtr", "SERIAL3", "SERIAL4", "SERIAL5", "Unknown"};
// get called here from loopBackend() to check for data received on other serial ports. The TFT uses SERIAL_PORT (USART2) for Marlin printer,
// but there is an ESP WiFi Board option on SERIAL_PORT_2 (USART1)!
// NOTE: Even without a WiFi board connected, some noise occasionally triggers SERIAL_PORT_2 and gets here with garbage bytes in the DMA buffer.
// This noise quickly overfills the input queue and hangs giving the "Busy processing....." message!
//TG modified the code below to popup a message if any data is received on SERIAL_PORT_2 (which should not normally happen)
//and prevent adding any SERIAL_PORT_2 data to infoCmd queue.
void parseRcvGcode(void)  
{
  #ifdef SERIAL_PORT_2    // the other serial port (USB) not the printer
    uint8_t i = 0;

    for (i = 0; i < _UART_CNT; i++)                                                       // loop thru all UARTS
    {
      if ((i != SERIAL_PORT) & (infoHost.rx_ok[i] == true))                               // if port is NOT TFT<>Printer and it's received flag is set 
      {
        infoHost.rx_ok[i] = false;                                                        // clear the received flag for the port
        
        if (SpuriousFlag == false)                                                        // don't allow multiple entries here if dialog box is already displayed
        {                                                                                 // the flg gets cleared when user presses OK to dismiss dialog box
          if (i == SERIAL_PORT_2)   // special handling for WiFi port                                             
          {
            float pct = infoCmd.count/CMD_QUEUE_SIZE;
            char TMsg[90]={"\0"};
            sprintf(TMsg, "Unexpected command from\n%s.....Ignored!\n \nCMD Queue %.2f%% full(%d/%d)", SPort[i], pct, infoCmd.count, CMD_QUEUE_SIZE);
            setDialogText((u8*)"ALERT!", (u8*)TMsg, LABEL_CONFIRM, LABEL_NULL);     // prepare dialg box for display
            setSpuriousFlag();                                                            // set flag to indicate that the dialog box has been displayed
            showDialog(DIALOG_TYPE_INFO, clrSpuriousFlag, clrSpuriousFlag, NULL);         // sets up for a dialog box //TG 12/26/22 modified
          }
          else                      // standard handling for all other ports
          {
            while (dmaL1NotEmpty(i))                                                      // add the port's data to the infoCmd queue
            {
              syncL2CacheFromL1(i);
              storeCmdFromUART(i, ack_cache);
            }
          }
        }
      }
    }
  #endif
}
*/

        
