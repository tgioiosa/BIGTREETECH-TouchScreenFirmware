#ifndef _POPUP_H_
#define _POPUP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"
#include "GUI.h"

enum
{
  KEY_POPUP_CONFIRM = 0,
  KEY_POPUP_CANCEL,
  KEY_POPUP_EXTRA,                      //TG 3/29/23 added for 3-button popup
};

extern const GUI_RECT doubleBtnRect[];
extern BUTTON bottomDoubleBtn[];
extern const GUI_RECT tripleBtnRect[];  //TG 3/29/23 added for 3-button popup
extern BUTTON bottomTripleBtn[];        //TG 3/29/23 added for 3-button popup

void _setDialogTitleStr(uint8_t * str);
void _setDialogMsgStr(uint8_t * str);
uint8_t *getDialogMsgStr(void);
void _setDialogOkTextStr(uint8_t * str);
void _setDialogCancelTextStr(uint8_t * str);
void _setDialogExtraTextStr(uint8_t * str);   //TG 3/29/23 added for 3-button popup

void _setDialogTitleLabel(int16_t index);
void _setDialogMsgLabel(int16_t index);
void _setDialogOkTextLabel(int16_t index);
void _setDialogCancelTextLabel(int16_t index);
void _setDialogExtraTextLabel(int16_t index); //TG 3/29/23 added for 3-button popup
uint8_t *getDialogTitle(void);

#define setDialogTitle(x) _Generic(((x+0)), const uint8_t*: _setDialogTitleStr, \
                                                  uint8_t*: _setDialogTitleStr, \
                                                   default: _setDialogTitleLabel)(x)
#define setDialogMsg(x) _Generic(((x+0)), const uint8_t*: _setDialogMsgStr, \
                                                uint8_t*: _setDialogMsgStr, \
                                                 default: _setDialogMsgLabel)(x)
#define setDialogOkText(x) _Generic(((x+0)), const uint8_t*: _setDialogOkTextStr, \
                                                   uint8_t*: _setDialogOkTextStr, \
                                                    default: _setDialogOkTextLabel)(x)
#define setDialogCancelText(x) _Generic(((x+0)), const uint8_t*: _setDialogCancelTextStr, \
                                                       uint8_t*: _setDialogCancelTextStr, \
                                                        default: _setDialogCancelTextLabel)(x)
//TG 3/29/23 added for 3-button popup
#define setDialogExtraText(x) _Generic(((x+0)), const uint8_t*: _setDialogExtraTextStr, \
                                                      uint8_t*: _setDialogExtraTextStr, \
                                                        default: _setDialogExtraTextLabel)(x)


//set text from LABEL index or pointer (uint8_t*)  //TG 3/29/23 added extratext for 3-button popup
#define setDialogText(title, msg, oktext, canceltext, extratext) \
  {                                                   \
    setDialogTitle(title);                            \
    setDialogMsg(msg);                                \
    setDialogOkText(oktext);                          \
    setDialogCancelText(canceltext);                  \
    setDialogExtraText(extratext);                    \
  }

void popupDrawPage(DIALOG_TYPE type, BUTTON * btn, const uint8_t * title, const uint8_t * context, const uint8_t * yes,          //TG 3/29/23 added extra for 3-button popup
                    const uint8_t * no, const uint8_t * extra);
void menuDialog(void);
void showDialog(DIALOG_TYPE type, void (*ok_action)(), void (*cancel_action)(), void (*extra_action)(), void (*loop_action)());  //TG 3/29/23 added extra for 3-button popup
void loopPopup(void);

/**
 * @brief Displays a popup for a dialog, it needs user interaction to close it.
 *
 * @param type the type of the dialog (alert, question, error, etc)
 * @param title title of the message box
 * @param msg the body of the dialog
 * @param oktext the text to be displayed on the "OK" button
 * @param canceltext the text to be displayed on the "cancel" button
 * @param extratext the text to be displayed on the "extra" button
 * @param ok_action the action to be taken if "OK" button is pressed
 * @param cancel_action the action to be taken if "Cancel" button is pressed
 * @param extra_action the action to be taken if "Extra" button is pressed    //TG 3/29/23 added for 3-button popup
 * @param loop_action the action to be taken while the dialog is active (visible/not answered)
 * 
 */
//TG 3/29/23 modified for 3-button popup
#define popupDialog(_type, _title, _msg, _oktext, _canceltext, _extratext, _ok_action, _cancel_action, _extra_action, _loop_action) \
  {                                                                                                      \
    setDialogText(_title, _msg, _oktext, _canceltext, _extratext);                                       \
    showDialog(_type, _ok_action, _cancel_action, _loop_action, _extra_action);                          \
  }


/**
 * @brief Displays a popup for a reminder, it needs user confirmation to close it.
 *
 * @param type the type of the reminder (info, alert, error, etc)
 * @param title title of the message box
 * @param msg the body of the message/reminder to be displayed
 */
//TG 3/29/23 added NULL's for 3-button popup
#define popupReminder(_type, _title, _msg)                  \
  {                                                         \
    setDialogText(_title, _msg, LABEL_CONFIRM, LABEL_NULL, LABEL_NULL); \
    showDialog(_type, NULL, NULL, NULL, NULL);                    \
  }

/**
 * @brief Displays a popup with a message, the user cannot close it.
 *
 * @param type the type of the message (info, alert, error, etc)
 * @param title title of the message box
 * @param msg the body of the message/reminder to be displayed
 */
//TG 3/29/23 added NULL's for 3-button popup
#define popupSplash(_type, _title, _msg)                 \
  {                                                      \
    setDialogText(_title, _msg, LABEL_NULL, LABEL_NULL, LABEL_NULL); \
    showDialog(_type, NULL, NULL, NULL, NULL);                 \
  }


#ifdef __cplusplus
}
#endif

#endif
