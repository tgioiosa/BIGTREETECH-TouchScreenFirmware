#include "ProbeOffsetControl.h"
#include "includes.h"

static float z_offset_value = PROBE_Z_OFFSET_DEFAULT_VALUE;
static bool probe_offset_enabled = false;

/* //TG 2/28/23 - Probe Offset
  Sets the Z (optional XY) distance from the nozzle to the probe trigger-point.

  The easiest way to get the Z offset value is to:

  Home the Z axis.
  Raise Z and deploy the probe.
  Move Z down slowly until the probe triggers.
  Take the current Z value and negate it. (5.2 => -5.2)
  Set with M851 Z-5.2 and #define Z_PROBE_OFFSET_FROM_EXTRUDER -5.2.
  For X and Y you have to measure the distance.
*/

// Enable probe offset
void probeOffsetEnable(float shim)
{
  probe_offset_enabled = true;
  
  probeHeightEnable();  // temporary disable software endstops and save ABL state

  // Z offset gcode sequence start
  probeHeightHome();  // home, disable ABL and raise nozzle

  float probedZ = 0.0f;

  if (infoSettings.probing_z_offset)   // if homing without a probe (e.g. with a min endstop)
  {
    levelingProbePoint(LEVEL_CENTER);  // start Z-probe at center bed (G30) and set probedPoint = LEVEL_NO_POINT

    while (true)                       // levelingGetProbedPoint() in this loop reads probedPoint                       
    {
      loopProcess();
      // read probedPoint (which gets set along with probedZ by Marlin's response to G30 when probe is triggered)
      if (levelingGetProbedPoint() != LEVEL_NO_POINT)  // if probed Z is set, exit from loop and read probed Z
        break;
    }

    probedZ = levelingGetProbedZ();     // capture the probedZ value from Marlin
    levelingResetProbedPoint();         // reset probedPoint = LEVEL_NO_POINT to check for new updates
  }

  probeHeightRelative();                                            // set relative position mode
  mustStoreCmd("G1 X%.2f Y%.2f\n",
               getParameter(P_PROBE_OFFSET, AXIS_INDEX_X),
               getParameter(P_PROBE_OFFSET, AXIS_INDEX_Y));         // move nozzle to XY probing point
  probeHeightStart(probedZ - probeOffsetGetValue() + shim, false);  // lower nozzle to probing Z0 point + shim
  probeOffsetSetValue(probedZ);                                     // set Z offset (M851) to match probing Z0 point
  probeHeightRelative();                                            // set relative position mode
}

// Disable probe offset
void probeOffsetDisable(void)
{
  probe_offset_enabled = false;

  // Z offset gcode sequence stop
  probeHeightHome();      // home, disable ABL and raise nozzle
  probeHeightAbsolute();  // set absolute position mode

  probeHeightDisable();  // restore original software endstops state and ABL state
}

// Get probe offset status
bool probeOffsetGetStatus(void)
{
  return probe_offset_enabled;
}

// Set Z offset value
float probeOffsetSetValue(float value)
{ // send ("M851 Z%.2f\n", value")
  sendParameterCmd(P_PROBE_OFFSET, AXIS_INDEX_Z, value);  // send ("M851 Z%.2f\n", value")
  mustStoreCmd("M851\n");  // needed by probeOffsetGetValue() to read back the new value
  z_offset_value = value;

  return z_offset_value;
}

// Get current Z offset value
float probeOffsetGetValue(void)
{
  z_offset_value = getParameter(P_PROBE_OFFSET, AXIS_INDEX_Z);

  return z_offset_value;
}

// Reset Z offset value to default value
float probeOffsetResetValue(void)
{
  if (z_offset_value == PROBE_Z_OFFSET_DEFAULT_VALUE)  // if already default value, nothing to do
    return z_offset_value;

  float unit = z_offset_value - PROBE_Z_OFFSET_DEFAULT_VALUE;

  z_offset_value = PROBE_Z_OFFSET_DEFAULT_VALUE;
  sendParameterCmd(P_PROBE_OFFSET, AXIS_INDEX_Z, z_offset_value);  // set Z offset value
  mustStoreCmd("G1 Z%.2f\n", -unit);                               // move nozzle

  return z_offset_value;
}

// Update Z offset value
float probeOffsetUpdateValue(float unit, int8_t direction)
{
  float diff;

  if (direction < 0)
  {
    if (z_offset_value <= PROBE_Z_OFFSET_MIN_VALUE)
      return z_offset_value;

    diff = z_offset_value - PROBE_Z_OFFSET_MIN_VALUE;
  }
  else
  {
    if (z_offset_value >= PROBE_Z_OFFSET_MAX_VALUE)
      return z_offset_value;

    diff = PROBE_Z_OFFSET_MAX_VALUE - z_offset_value;
  }

  unit = ((diff > unit) ? unit : diff) * direction;
  z_offset_value += unit;
  sendParameterCmd(P_PROBE_OFFSET, AXIS_INDEX_Z, z_offset_value);  // send ("M851 Z%.2f\n", value")
  mustStoreCmd("G1 Z%.2f\n", unit);                                // move nozzle by "unit" amount

  return z_offset_value;
}
