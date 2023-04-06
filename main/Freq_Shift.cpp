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
  Purpose: void FreqShift1()
          AFP 12-31-20
        Frequency translation by Fs/4 without multiplication from Lyons (2011): chapter 13.1.2 page 646
        together with the savings of not having to shift/rotate the FFT_buffer, this saves
        about 1% of processor use

        This is for +Fs/4 [moves receive frequency to the left in the spectrum display]
           float_buffer_L contains I = real values
           float_buffer_R contains Q = imaginary values
           xnew(0) =  xreal(0) + jximag(0)
               leave first value (DC component) as it is!
           xnew(1) =  - ximag(1) + jxreal(1)
  Parameter list:
    void

  Return value:
    void
*****/
void FreqShift1()
{
  for (unsigned i = 0; i < BUFFER_SIZE * N_BLOCKS; i += 4) {
    hh1 = - float_buffer_R[i + 1];  // xnew(1) =  - ximag(1) + jxreal(1)
    hh2 =   float_buffer_L[i + 1];
    float_buffer_L[i + 1] = hh1;
    float_buffer_R[i + 1] = hh2;
    hh1 = - float_buffer_L[i + 2];
    hh2 = - float_buffer_R[i + 2];
    float_buffer_L[i + 2] = hh1;
    float_buffer_R[i + 2] = hh2;
    hh1 =   float_buffer_R[i + 3];
    hh2 = - float_buffer_L[i + 3];
    float_buffer_L[i + 3] = hh1;
    float_buffer_R[i + 3] = hh2;
  }
  for (unsigned i = 0; i < BUFFER_SIZE * N_BLOCKS; i ++) {
    float_buffer_L_3[i] = float_buffer_L[i];
    float_buffer_R_3[i] = float_buffer_R[i];
  }
  // this is for -Fs/4 [moves receive frequency to the right in the spectrumdisplay]
}

/*****
  Purpose: Shift Receive frequency by an arbitray amount

  Parameter list:
    void

  Return value;
    void
    Notes:  Routine includes checks to ensure the frequency selection stays within the bounds of the
    displayed spectrum
    Also included a variable frequency step, depending on how fast the encoder id turned.  Step varies from 50Hz/step to 10KHz/step

    freq_conv2()

    FREQUENCY CONVERSION USING A SOFTWARE QUADRATURE OSCILLATOR (NCO)

    THIS VERSION calculates the COS AND SIN WAVE on the fly AND IS SLOW

    MAJOR ADVANTAGE: frequency conversion can be done for any frequency !

    large parts of the code taken from the mcHF code by Clint, KA7OEI, thank you!
      see here for more info on quadrature oscillators:
    Wheatley, M. (2011): CuteSDR Technical Manual Ver. 1.01. - http://sourceforge.net/projects/cutesdr/
    Lyons, R.G. (2011): Understanding Digital Processing. – Pearson, 3rd edition.
    Requires 4 complex multiplies and two adds per data point within the time domain buffer.  Applied after the data
    stream is sent to the Zoom FFT , but befor decimation.
*****/
void FreqShift2()
{
  uint i;
  long currentFreqAOld;
  int sideToneShift = 0;

  if (fineTuneEncoderMove != 0L) {
    SetFreq();           //AFP 10-04-22
    ShowFrequency();
    DrawBandWidthIndicatorBar();

    // ); //AFP 10-04-22
    // EncoderFineTune();      //AFP 10-04-22

    if (NCOFreq > 40000L) {
      NCOFreq = 40000L;
    }
    // centerFreq += freqIncrement;
    currentFreqA = centerFreq + NCOFreq;
    //SetFreq(); //AFP 10-04-22
    ShowFrequency();
  }

  encoderStepOld = fineTuneEncoderMove;
  currentFreqAOld = TxRxFreq;
  TxRxFreq = centerFreq + NCOFreq;

#warning bugbug jdw there is a signed/unsigned error in this condition

  if (  (currentFreqAOld != TxRxFreq)   // bugbug jdw - fail fast
    	 && ( abs(currentFreqAOld - TxRxFreq) < (9 * stepFT) ) ) { // AFP 10-30-22
           ShowFrequency();
           DrawBandWidthIndicatorBar();
  }
  if (xmtMode == SSB_MODE ) {
    sideToneShift = 0;
  } else {
    if (xmtMode == CW_MODE ) {
      if (bands[currentBand].mode == 1) {
        sideToneShift = CWFreqShift;
      } else {
        if (bands[currentBand].mode == 0) {
          sideToneShift = -CWFreqShift;
        }
      }
    }
  }
  NCO_INC = 2.0 * PI * (NCOFreq + sideToneShift) / 192000.0; //192000 SPS is the actual sample rate used in the Receive ADC

  OSC_COS = cos (NCO_INC);
  OSC_SIN = sin (NCO_INC);

  for (i = 0; i < BUFFER_SIZE * N_BLOCKS; i++) {
    // generate local oscillator on-the-fly:  This takes a lot of processor time!
    Osc_Q = (Osc_Vect_Q * OSC_COS) - (Osc_Vect_I * OSC_SIN);  // Q channel of oscillator
    Osc_I = (Osc_Vect_I * OSC_COS) + (Osc_Vect_Q * OSC_SIN);  // I channel of oscillator
    Osc_Gain = 1.95 - ((Osc_Vect_Q * Osc_Vect_Q) + (Osc_Vect_I * Osc_Vect_I));  // Amplitude control of oscillator

    // rotate vectors while maintaining constant oscillator amplitude
    Osc_Vect_Q = Osc_Gain * Osc_Q;
    Osc_Vect_I = Osc_Gain * Osc_I;
    //
    // do actual frequency conversion
    float freqAdjFactor = 1.1;
    float_buffer_L[i] = (float_buffer_L_3[i] * freqAdjFactor * Osc_Q) + (float_buffer_R_3[i] * freqAdjFactor * Osc_I); // multiply I/Q data by sine/cosine data to do translation
    float_buffer_R[i] = (float_buffer_R_3[i] * freqAdjFactor * Osc_Q) - (float_buffer_L_3[i] * freqAdjFactor * Osc_I);
  }
}
