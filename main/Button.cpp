/*
 *  TEENSY CONVOLUTION SDR
 * 
 *  Copyright (C) Frank Dziock DD4WH,
 *                Jack Purdum W8TEE, 
 *                Al Peter AC8GY,
 *                Contributors.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*
 *  This comment block must appear in the load page 
 *  (e.g., main() or setup()) in any source code that uses 
 *  code presented as whole or part of the T41-EP source code.
 *
 *  (c) Frank Dziock, DD4WH, 2020_05_8
 *   "TEENSY CONVOLUTION SDR" 
 *  substantially modified by Jack Purdum, W8TEE, and Al Peter, AC8GY
 *
 *  This software is made available under the GNU GPL v3 license agreement.
 *
 *  If commercial use of this software is planned, we would appreciate it 
 *  if the interested parties get written approval from Jack Purdum, W8TEE,
 *  and Al Peter, AC8GY.
 *
 *  Any and all other commercial uses, written or implied, are forbidden 
 *  without written permission from from Jack Purdum, W8TEE, 
 *  and Al Peter, AC8GY.
*/

#include "SDT.h"

/*****
  Purpose: Determine which UI button was pressed

  Parameter list:
    int valPin            the ADC value from analogRead()

  Return value;
    int                   -1 if not valid push button, index of push button if valid
*****/
int ProcessButtonPress(int valPin)
{
  int switchIndex;

  if (valPin == BOGUS_PIN_READ) {                  // Not valid press
    return -1;
  }
  if (valPin == MENU_OPTION_SELECT && menuStatus == NO_MENUS_ACTIVE) {
    NoActiveMenu();
    return -1;
  }
  for (switchIndex = 0; switchIndex < NUMBER_OF_SWITCHES; switchIndex++)
  {
    if (abs(valPin - EEPROMData.switchValues[switchIndex]) < WIGGLE_ROOM)    // ...because ADC does return exact values every time
    {
      return switchIndex;
    }
  }
  return -1;                                              // Really should never do this
}

/*****
  Purpose: Check for UI button press. If pressed, return the ADC value

  Parameter list:
    int vsl               the value from analogRead in loop()\

  Return value;
    int                   -1 if not valid push button, ADC value if valid
*****/
int ReadSelectedPushButton()
{
  minPinRead        = 0;
  int buttonReadOld = 1023;

  while (abs(minPinRead - buttonReadOld) > 3) {                   // do averaging to smooth out the button response
    minPinRead = analogRead(BUSY_ANALOG_PIN);
    buttonRead = .1 * minPinRead + (1 - .1) * buttonReadOld;      // See expected values in next function.
    buttonReadOld = buttonRead;
  }
  if (buttonRead > EEPROMData.switchValues[0] + WIGGLE_ROOM) {   //AFP 10-29-22 per Jack Wilson
    return -1;
  }
  minPinRead = buttonRead;
  MyDelay(100L);
  return minPinRead;
}

/*****
  Purpose: Function is designed to route program control to the proper execution point in response to
           a button press.

  Parameter list:
    int vsl               the value from analogRead in loop()

  Return value;
    void
*****/
void ExecuteButtonPress(int val)
{
  if (val == MENU_OPTION_SELECT && menuStatus == NO_MENUS_ACTIVE) {          // Pressed Select with no primary/secondary menu selected
    NoActiveMenu();
    return;
  } else {
    menuStatus = PRIMARY_MENU_ACTIVE;
  }
  switch (val) {
    case MENU_OPTION_SELECT:                                     // 0

      if (menuStatus == PRIMARY_MENU_ACTIVE) {                             // Doing primary menu
        ErasePrimaryMenu();
        secondaryMenuChoiceMade = functionPtr[mainMenuIndex]();            // These are processed in MenuProcessing.cpp
        menuStatus = SECONDARY_MENU_ACTIVE;
        secondaryMenuIndex = -1;                                  // Reset secondary menu
      } else {
        if (menuStatus == SECONDARY_MENU_ACTIVE) {                         // Doing primary menu
          menuStatus = PRIMARY_MENU_ACTIVE;
          mainMenuIndex = 0;
        }
      }
      EraseMenus();
      break;

    case MAIN_MENU_UP:                                            // 1
      ButtonMenuIncrease();                                       // This makes sure the increment does go outta range
      if (menuStatus != NO_MENUS_ACTIVE) {                        // Doing primary menu
        ShowMenu(&topMenus[mainMenuIndex], PRIMARY_MENU);
      }
      break;

    case BAND_UP:                                                 // 2 Now calls ProcessIQData and Encoders calls
      EraseMenus();
      digitalWrite(bandswitchPins[currentBand], LOW);
      ButtonBandIncrease();
      digitalWrite(bandswitchPins[currentBand], HIGH);
      BandInformation();
      NCOFreq = 0L;
      DrawBandWidthIndicatorBar(); // AFP 10-20-22
      //FilterOverlay();   // AFP 10-20-22
      SetFreq();
      ShowSpectrum();
      break;

    case ZOOM:                                                    // 3
      menuStatus = PRIMARY_MENU_ACTIVE;
      EraseMenus();
      ButtonZoom();
      break;

    case MAIN_MENU_DN:                                            // 4
      ButtonMenuDecrease();
      if (menuStatus != NO_MENUS_ACTIVE) {                        // Doing primary menu
        ShowMenu(&topMenus[mainMenuIndex], PRIMARY_MENU);
      }
      break;

    case BAND_DN:                                                 // 5
      EraseMenus();
      ShowSpectrum();                                           //Now calls ProcessIQData and Encoders calls
      digitalWrite(bandswitchPins[currentBand], LOW);
      ButtonBandDecrease();
      digitalWrite(bandswitchPins[currentBand], HIGH);
      BandInformation();
      NCOFreq = 0L;
      DrawBandWidthIndicatorBar();  //AFP 10-20-22
      //FilterOverlay();            // AFP 10-20-22
      break;

    case FILTER:                                                  // 6
      EraseMenus();
      ButtonFilter();
      break;

    case DEMODULATION:                                            // 7
      EraseMenus();
      ButtonDemodMode();
      break;

    case SET_MODE:                                                // 8
      ButtonMode();
      ShowSpectrumdBScale();
      break;

    case NOISE_REDUCTION:                                         // 9
      ButtonNR();
      break;

    case NOTCH_FILTER:                                            // 10
      ButtonNotchFilter();
      UpdateNotchField();
      break;

    case NOISE_FLOOR:                                             // 11
      ButtonSetNoiseFloor();
      break;

    case FINE_TUNE_INCREMENT:                                     // 12
      if (stepFT == 50) {
        stepFT = 500;
      } else {
        stepFT = 50;
      }
      UpdateIncrementField();
      break;

    case DECODER_TOGGLE:                                          // 13
      //      decoderFlag = !decoderFlag;

      if (decoderFlag == DECODE_OFF) {
        decoderFlag = DECODE_ON;
      } else {
        decoderFlag = DECODE_OFF;
      }

      UpdateDecoderField();
      break;

    case MAIN_TUNE_INCREMENT:                                     // 14
      ButtonFreqIncrement();
      break;

    case RESET_TUNING:                                            // 15   AFP 10-11-22
      ResetTuning();                                              // AFP 10-11-22
      break;                                                      // AFP 10-11-22

    case UNUSED_1:                                                // 16
      if (calOnFlag == 0) {
        ButtonFrequencyEntry();
      }
      break;

    case UNUSED_2:                                                // 17  // AFP 10-11-22
#ifdef SD_CARD_PRESENT
      int buttonIndex, doneViewing, valPin;
    
      MyDelay(100L);
      DrawKeyboard();
      CaptureKeystrokes();
      BearingHeading(keyboardBuffer);
      bmpDraw( (char *) MAP_FILE_NAME, IMAGE_CORNER_X, IMAGE_CORNER_Y); // MAP_FILE_NAME = "HomeLocationOriginalResizeBy4.bmp"
      doneViewing = false;
      while (true) {
        valPin = ReadSelectedPushButton();                        // Poll UI push buttons
        MyDelay(100L);
        if (valPin != BOGUS_PIN_READ) {                           // If a button was pushed...
          buttonIndex = ProcessButtonPress(valPin);               // Winner, winner...chicken dinner!

          switch (buttonIndex) {
            case UNUSED_2:
              doneViewing = true;
              break;
            default:
              break;
          }
        }

        if (doneViewing == true) {
          //  tft.clearMemory();          // Need to clear overlay too
          //  tft.writeTo(L2);
          //  tft.fillWindow();
          break;
        }
      }
      RedrawDisplayScreen();
      ShowFrequency();
      DrawFrequencyBarValue();
#else
      tft.setCursor(80, 2);
      tft.setTextColor(RA8875_RED);
      tft.print("No SD card");
      return;
#endif
      break;

    default:
      break;
  }
}


/*****
  Purpose: To process a band decrease button push

  Parameter list:
    void

  Return value:
    void
*****/
void ButtonFreqIncrement()
{
  tuneIndex--;
  if (tuneIndex < 0)
    tuneIndex = MAX_FREQ_INDEX - 1;
  freqIncrement = incrementValues[tuneIndex];
  UpdateIncrementField();
}


/*****
  Purpose: Error message if Select button pressed with no Menu active

  Parameter list:
    void

  Return value;
    void
*****/
void NoActiveMenu()
{
  tft.setFontScale( (enum RA8875tsize) 1);
  tft.setTextColor(RED);
  tft.setCursor(10, 0);
  tft.print("No menu selected");

  menuStatus         = NO_MENUS_ACTIVE;
  mainMenuIndex      = 0;
  secondaryMenuIndex = 0;
}
