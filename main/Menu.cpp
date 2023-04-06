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

/*
const char *topMenus[]      = {"CW Options", "Spectrum Set", "AGC",      "NR Set",   "IQ Manual",
                               "EQ Rec Set", "EQ Xmt Set",   "Mic Comp", "Calibrate Freq", "Noise Floor",
                               "RF Set",     "VFO Select",   "EEPROM",   
                              };

int (*functionPtr[])()      = {&CWOptions, &SpectrumOptions, &AGCOptions, &NROptions, &IQOptions,
                               &EqualizerRecOptions, &EqualizerXmtOptions, &MicOptions, &CalibrateFrequency, &ButtonSetNoiseFloor,
                               &RFOptions, &VFOSelect, &EEPROMOptions
                              };
*/
/*****
  Purpose: void ShowMenu()

  Parameter list:
    char *menuItem          pointers to the menu
    int where               0 is a primary menu, 1 is a secondary menu

  Return value;
    void
*****/
void ShowMenu(const char *menu[], int where)
{
  tft.setFontScale( (enum RA8875tsize) 1);  

  if (menuStatus == NO_MENUS_ACTIVE)                                        // No menu selected??
     NoActiveMenu();
     
  if (where == PRIMARY_MENU) {                                              // Should print on left edge of top line
    tft.fillRect(PRIMARY_MENU_X, MENUS_Y, 300, CHAR_HEIGHT, RA8875_BLUE);   // Top-left of display
    tft.setCursor(5, 0);
    tft.setTextColor(RA8875_WHITE);
    tft.print(*menu);                                                       // Primary Menu
  } else {
    tft.fillRect(SECONDARY_MENU_X, MENUS_Y, 300, CHAR_HEIGHT, RA8875_GREEN);// Right of primary display
    tft.setCursor(35, 0);
    tft.setTextColor(RA8875_WHITE);
    tft.print(*menu);                                                       // Secondary Menu
  }
  return;
}
