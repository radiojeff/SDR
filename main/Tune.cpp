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

/***** //AFP 10-11-22 all new
  Purpose:  Reset tuning to center

  Parameter list:
  void

  Return value;
  void
*****/
void ResetTuning() {
  if (activeVFO == VFO_A) {
    currentFreqA = centerFreq + NCOFreq;
    NCOFreq = 0L;
    TxRxFreq = centerFreq = currentFreqA;  //AFP 10-28-22
  } else {
    currentFreqB = centerFreq + NCOFreq;
    NCOFreq = 0L;
    TxRxFreq = centerFreq = currentFreqB;  //AFP 10-28-22
  }

  DrawBandWidthIndicatorBar();
  BandInformation();
  ShowFrequency();
  centerTuneFlag = 1;
}
// ===== End AFP 10-11-22

/*****
  Purpose: SetFrequency

  Parameter list:
    void

  Return value;
    void

  CAUTION: SI5351_FREQ_MULT is set in the si5253.h header file and is 100UL
*****/
void SetFreq() {  //AFP 09-22-22

  // NEVER USE AUDIONOINTERRUPTS HERE: that introduces annoying clicking noise with every frequency change
  // SI5351_FREQ_MULT is 100ULL, MASTER_CLK_MULT is 4;

  if (xmtMode == SSB_MODE) {
    Clk2SetFreq = (((centerFreq)*SI5351_FREQ_MULT) + IFFreq * SI5351_FREQ_MULT) * MASTER_CLK_MULT;  // AFP 09-27-22
    Clk1SetFreq = (TxRxFreq * SI5351_FREQ_MULT) * MASTER_CLK_MULT;                                  // AFP 09-27-22
  } else {
    // =========================  CW Xmit
    if (bands[currentBand].mode == DEMOD_LSB) {                                                                       // Which sideband?
      Clk2SetFreq = (((centerFreq + CWFreqShift) * SI5351_FREQ_MULT) + IFFreq * SI5351_FREQ_MULT) * MASTER_CLK_MULT;  // AFP 09-27-22
      Clk1SetFreq = (((TxRxFreq - CWFreqShift - calFreqShift) * SI5351_FREQ_MULT)) * MASTER_CLK_MULT;                 // AFP 09-27-22;
        } else {                                                              //  DEMOD_USB
      Clk2SetFreq = (((centerFreq + CWFreqShift) * SI5351_FREQ_MULT) + IFFreq * SI5351_FREQ_MULT) * MASTER_CLK_MULT;  // AFP 09-27-22
      Clk1SetFreq = (((TxRxFreq + CWFreqShift + calFreqShift) * SI5351_FREQ_MULT)) * MASTER_CLK_MULT;                 // AFP 10-01-22;
    }
  }
  // =========================  End CW Xmit

  if ((digitalRead(PTT) == LOW) && (currentBand == BAND_10M)) {
    Clk2SetFreq = (((7000000) * SI5351_FREQ_MULT) + IFFreq * SI5351_FREQ_MULT) * MASTER_CLK_MULT;  //AFP 08-22-22
  } else {
    Clk2SetFreq = (((centerFreq)*SI5351_FREQ_MULT) + IFFreq * SI5351_FREQ_MULT) * MASTER_CLK_MULT;
  }
  //=====================  AFP 10-03-22 =================
  si5351.set_freq(Clk2SetFreq, SI5351_CLK2);

  if (xrState == RECEIVE_STATE) {     // Turn CLK1 off during Receive and on for Transmit  AFP 10-02-22
    si5351.set_freq(0, SI5351_CLK1);  // CLK1 off during receive to prevent birdies
  } else {
    si5351.set_freq(Clk1SetFreq, SI5351_CLK1);  // TRANSMIT_STATE
  }
  //=====================  AFP 10-03-22 =================
  DrawFrequencyBarValue();
}



/*****
  Purpose: Places the Fast Tune cursor in the center of the spectrum display

  Parameter list:

  Return value;
    void
*****/
void CenterFastTune() {
  tft.drawFastVLine(oldCursorPosition, SPECTRUM_TOP_Y + 20, SPECTRUM_HEIGHT - 27, RA8875_BLACK);
  tft.drawFastVLine(newCursorPosition, SPECTRUM_TOP_Y + 20, SPECTRUM_HEIGHT - 27, RA8875_RED);
}

/*****
  Purpose: Purpose is to sety VFOa to receive frequency and VFOb to the transmit frequency

  Parameter list:
    void

  Return value;
    int           the offset as an int

  CAUTION: SI5351_FREQ_MULT is set in the si5253.h header file and is 100UL
*****/
int DoSplitVFO() {
  char freqBuffer[15];
  int val;
  long chunk = SPLIT_INCREMENT;
  long splitOffset;

  tft.drawRect(INFORMATION_WINDOW_X - 10, INFORMATION_WINDOW_Y - 2, 260, 200, RA8875_MAGENTA);
  tft.fillRect(INFORMATION_WINDOW_X - 8, INFORMATION_WINDOW_Y, 250, 185, RA8875_BLACK);  // Clear volume field
  tft.setFontScale((enum RA8875tsize)1);
  tft.setCursor(INFORMATION_WINDOW_X + 10, INFORMATION_WINDOW_Y + 5);
  tft.print("xmit offset: ");

  splitOffset = chunk;  // Set starting offset to 500Hz
  tft.setTextColor(RA8875_GREEN);
  tft.setCursor(INFORMATION_WINDOW_X + 60, INFORMATION_WINDOW_Y + 90);
  tft.print(splitOffset);
  tft.print("Hz  ");

  while (true) {
    if (filterEncoderMove != 0) {  // Changed encoder?
      splitOffset += filterEncoderMove * chunk;
      tft.fillRect(INFORMATION_WINDOW_X + 60, INFORMATION_WINDOW_Y + 90, 150, 50, RA8875_BLACK);
      tft.setCursor(INFORMATION_WINDOW_X + 60, INFORMATION_WINDOW_Y + 90);
      tft.print(splitOffset);
      tft.print("Hz  ");
    }
    filterEncoderMove = 0L;

    val = ReadSelectedPushButton();  // Read pin that controls all switches
    val = ProcessButtonPress(val);
    MyDelay(150L);
    if (val == MENU_OPTION_SELECT) {  // Make a choice??
      Clk1SetFreq += splitOffset;     // New transmit frequency // AFP 09-27-22
      UpdateInfoWindow();
      filterEncoderMove = 0L;
      break;
    }
  }
  currentFreqB = currentFreqA + splitOffset;
  FormatFrequency(currentFreqB, freqBuffer);
  tft.fillRect(FREQUENCY_X_SPLIT, FREQUENCY_Y - 12, VFOB_PIXEL_LENGTH, FREQUENCY_PIXEL_HI, RA8875_BLACK);
  tft.setCursor(FREQUENCY_X_SPLIT, FREQUENCY_Y);
  tft.setFont(&FreeMonoBold24pt7b);
  tft.setTextColor(RA8875_GREEN);
  tft.print(freqBuffer);  // Show VFO_A

  tft.setFont(&FreeMonoBold18pt7b);
  FormatFrequency(currentFreqA, freqBuffer);
  tft.setTextColor(RA8875_LIGHT_GREY);
  tft.setCursor(FREQUENCY_X, FREQUENCY_Y + 6);
  tft.print(freqBuffer);  // Show VFO_A

  tft.useLayers(1);  //mainly used to turn on layers!
  tft.layerEffect(OR);
  tft.writeTo(L2);
  tft.clearMemory();
  tft.writeTo(L1);

  tft.setFont(&FreeMono9pt7b);
  tft.setTextColor(RA8875_RED);
  tft.setCursor(FILTER_PARAMETERS_X + 180, FILTER_PARAMETERS_Y + 6);
  tft.print("Split Active");

  tft.setFontDefault();
  return (int)splitOffset;  // Can be +/-
}
