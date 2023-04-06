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

//=====================  file all new  AFP 09-01-22


/*****
  Purpose: to send a Morse code dit

  Paramter list:
    void

  Return value:
    void
*****/
void KeyTipOn()
{
  if (digitalRead(KEYER_DIT_INPUT_TIP) == LOW && xmtMode == CW_MODE ) {
    keyPressedOn = 1;
  }
}

/*****
  Purpose: CW Key interrupt //AFP 09-01-22

  Parameter list:

  Return value;
  voidKeyRingOn(

*****/
void KeyRingOn() //AFP 09-25-22
{
  if (keyType == 1) {
    if (digitalRead(KEYER_DAH_INPUT_RING) == LOW && xmtMode == CW_MODE ) {
      keyPressedOn = 1;
    }
  }
}
/*****


  Purpose: Create I and Q signals from Mic input

  Parameter list:

  Return value;
    void
    Notes:
    There are several actions in this function
    1.  Read in the data from the ADC into the Left Channel at 192KHz
    2.  Format the L data and Decimate (downsample and filter)the sampled data by x8
          - the new effective sampling rate is now 24KHz
    3.  Process the L data through the 7 EQ filters and combine to a single data stream
    4.  Copy the L channel to the R channel
    5.  Process the R and L through two Hilbert Transformers - L 0deg phase shift and R 90 deg ph shift
          - This create the I (L) and Q(R) channels
    6.  Interpolate 8x (upsample and filter) the data stream to 192KHz sample rate
    7.  Output the data stream thruogh the DACs at 192KHz
*****/
void CW_ExciterIQData() //AFP 08-20-22
{
  uint32_t N_BLOCKS_EX = N_B_EX;
 
  arm_scale_f32 (cosBuffer2, 0.127, float_buffer_L_EX, 256);  // AFP 10-13-22 Use pre-calculated sin & cos instead of Hilbert
  arm_scale_f32 (sinBuffer2, 0.127, float_buffer_R_EX, 256);  // AFP 10-13-22
  /**********************************************************************************
            Additional scaling, if nesessary to compensate for down-stream gain variations
   **********************************************************************************/

  //============================== AFP 10-21-22  Begin new

  if (bands[currentBand].mode == DEMOD_LSB) {
    arm_scale_f32 (float_buffer_L_EX, IQXAmpCorrectionFactor[currentBandA], float_buffer_L_EX, 256);  //Adjust level of L buffer
    IQPhaseCorrection(float_buffer_L_EX, float_buffer_R_EX, IQXPhaseCorrectionFactor[currentBandA], 256);  // Adjust phase
  } else {
    if (bands[currentBand].mode == DEMOD_USB) {
      arm_scale_f32 (float_buffer_L_EX, -IQXAmpCorrectionFactor[currentBandA], float_buffer_L_EX, 256);
      IQPhaseCorrection(float_buffer_L_EX, float_buffer_R_EX, IQXPhaseCorrectionFactor[currentBandA], 256);
    }
  }
    /**********************************************************************************
              Interpolate (upsample the data streams by 8X to create the 192KHx sample rate for output
              Requires a LPF FIR 48 tap 10KHz and 8KHz
     **********************************************************************************/
    //24KHz effective sample rate here
    arm_fir_interpolate_f32(&FIR_int1_EX_I, float_buffer_L_EX, float_buffer_LTemp, 256);
    arm_fir_interpolate_f32(&FIR_int1_EX_Q, float_buffer_R_EX, float_buffer_RTemp, 256);

    // interpolation-by-4,  48KHz effective sample rate here
    arm_fir_interpolate_f32(&FIR_int2_EX_I, float_buffer_LTemp, float_buffer_L_EX, 512);
    arm_fir_interpolate_f32(&FIR_int2_EX_Q, float_buffer_RTemp, float_buffer_R_EX, 512);

    //  192KHz effective sample rate here
    arm_scale_f32(float_buffer_L_EX, 20, float_buffer_L_EX, 2048); //Scale to compensate for losses in Interpolation
    arm_scale_f32(float_buffer_R_EX, 20, float_buffer_R_EX, 2048);

    /**********************************************************************************  AFP 12-31-20
      CONVERT TO INTEGER AND PLAY AUDIO
    **********************************************************************************/

    for (unsigned  i = 0; i < N_BLOCKS_EX; i++) {  //N_BLOCKS_EX=16  BUFFER_SIZE=128 16x128=2048
      sp_L2 = Q_out_L_Ex.getBuffer();
      sp_R2 = Q_out_R_Ex.getBuffer();
      arm_float_to_q15 (&float_buffer_L_EX[BUFFER_SIZE * i], sp_L2, BUFFER_SIZE);
      arm_float_to_q15 (&float_buffer_R_EX[BUFFER_SIZE * i], sp_R2, BUFFER_SIZE);
      Q_out_L_Ex.playBuffer(); // play it !
      Q_out_R_Ex.playBuffer(); // play it !
    }
}
