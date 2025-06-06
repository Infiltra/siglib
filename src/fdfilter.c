
/**************************************************************************
File Name               : FDFILTER.C    | Author        : JOHN EDWARDS
Siglib Library Version  : 10.50         |
----------------------------------------+----------------------------------
Compiler  : Independent                 | Start Date    : 23/10/1999
Options   :                             | Latest Update : 17/11/2020
---------------------------------------------------------------------------
Support for SigLib is available via Email: support@numerix-dsp.com

This file may be modified, in any way, providing that this header remains
within the file and none of the header contents are removed or modified.

SigLib is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License V2, as
published by the Free Software Foundation.

SigLib is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA

This sofware is also available with a commercial license, for use in
proprietary, research, government or commercial applications.
Please contact Delta Numerix for further details :
https://www.numerix-dsp.com
support@.numerix-dsp.com

Copyright (c) 2024 Delta Numerix All rights reserved.
---------------------------------------------------------------------------
Description: SigLib DSP library frequency domain FIR filter, convolution
and correlation functions.

****************************************************************************/

#define SIGLIB_SRC_FILE_FDFILTER 1    // Defines the source file that this code is being used in

#include <siglib.h>    // Include SigLib header file

/********************************************************
 * Function: SIF_FirOverlapAdd
 *
 * Parameters:
 *  const SLData_t * SIGLIB_PTR_DECL pTimeDomainCoeffs,
 *  SLData_t * SIGLIB_PTR_DECL pRealFreqDomainCoeffs,
 *  SLData_t * SIGLIB_PTR_DECL pImagFreqDomainCoeffs,
 *  SLData_t * SIGLIB_PTR_DECL pOverlapArray,
 *  SLData_t * SIGLIB_PTR_DECL pFFTCoeffs,  - FFT coefficient pointer
 *  SLArrayIndex_t * SIGLIB_PTR_DECL pBitReverseAddressTable, - Bit reverse
 *mode flag / Pointer to bit reverse address table SLData_t * pInverseFFTLength,
 *  const SLArrayIndex_t FFTLength,
 *  const SLArrayIndex_t Log10FFTLength,
 *  const SLArrayIndex_t filterLength
 *
 * Return value: void
 *
 * Description: Initialise the overlap and add frequency
 *  domain filter.
 *
 ********************************************************/

void SIGLIB_FUNC_DECL SIF_FirOverlapAdd(const SLData_t* SIGLIB_PTR_DECL pTimeDomainCoeffs, SLData_t* SIGLIB_PTR_DECL pRealFreqDomainCoeffs,
                                        SLData_t* SIGLIB_PTR_DECL pImagFreqDomainCoeffs, SLData_t* SIGLIB_PTR_DECL pOverlapArray,
                                        SLData_t* SIGLIB_PTR_DECL pFFTCoeffs, SLArrayIndex_t* SIGLIB_PTR_DECL pBitReverseAddressTable,
                                        SLData_t* pInverseFFTLength, const SLArrayIndex_t FFTLength, const SLArrayIndex_t Log10FFTLength,
                                        const SLArrayIndex_t filterLength)
{
  SIF_Fft(pFFTCoeffs, pBitReverseAddressTable, FFTLength);    // Initialise FFT
  SDA_Zeros(pOverlapArray,
            (SLArrayIndex_t)(filterLength - 1));    // Clear overlap storage
  SDA_Copy(pTimeDomainCoeffs, pRealFreqDomainCoeffs,
           filterLength);    // Copy coefficients
  SDA_Zeros(pRealFreqDomainCoeffs + filterLength,
            (SLArrayIndex_t)(FFTLength - filterLength));                                                                     // Zero pad
  SDA_Rfft(pRealFreqDomainCoeffs, pImagFreqDomainCoeffs, pFFTCoeffs, pBitReverseAddressTable, FFTLength, Log10FFTLength);    // Perform FFT

  *pInverseFFTLength = SIGLIB_ONE / FFTLength;
}    // End of  SIF_FirOverlapAdd()

/********************************************************
 * Function: SDA_FirOverlapAdd
 *
 * Parameters:
 *  const SLData_t * SIGLIB_PTR_DECL pSrc
 *  SLData_t * SIGLIB_PTR_DECL pDst
 *  const SLData_t * SIGLIB_PTR_DECL pRealFreqDomainCoeffs,
 *  const SLData_t * SIGLIB_PTR_DECL pImagFreqDomainCoeffs,
 *  SLData_t * SIGLIB_PTR_DECL pOverlapArray,
 *  SLData_t * SIGLIB_PTR_DECL pTempArray,
 *  SLData_t * SIGLIB_PTR_DECL pFFTCoeffs,  - FFT coefficient pointer
 *  SLArrayIndex_t * SIGLIB_PTR_DECL pBitReverseAddressTable, - Bit reverse
 *mode flag / Pointer to bit reverse address table const SLData_t
 *InverseFFTLength, const SLArrayIndex_t FFTLength, const SLArrayIndex_t
 *Log10FFTLength, const SLArrayIndex_t filterLength, const SLArrayIndex_t
 *DataLength
 *
 * Return value: void
 *
 * Description: Apply the overlap and add frequency
 *  domain filter.
 *
 ********************************************************/

void SIGLIB_FUNC_DECL SDA_FirOverlapAdd(const SLData_t* SIGLIB_PTR_DECL pSrc, SLData_t* SIGLIB_PTR_DECL pDst,
                                        const SLData_t* SIGLIB_PTR_DECL pRealFreqDomainCoeffs,
                                        const SLData_t* SIGLIB_PTR_DECL pImagFreqDomainCoeffs, SLData_t* SIGLIB_PTR_DECL pOverlapArray,
                                        SLData_t* SIGLIB_PTR_DECL pTempArray, SLData_t* SIGLIB_PTR_DECL pFFTCoeffs,
                                        SLArrayIndex_t* SIGLIB_PTR_DECL pBitReverseAddressTable, const SLData_t InverseFFTLength,
                                        const SLArrayIndex_t FFTLength, const SLArrayIndex_t Log10FFTLength, const SLArrayIndex_t filterLength,
                                        const SLArrayIndex_t DataLength)
{
  SDA_Copy(pSrc, pDst, DataLength);    // Copy data to destination
  SDA_Zeros(pDst + DataLength,
            (SLArrayIndex_t)(FFTLength - DataLength));    // Zero pad

  SDA_Rfft(pDst, pTempArray, pFFTCoeffs, pBitReverseAddressTable, FFTLength,
           Log10FFTLength);    // Perform FFT
  SDA_ComplexMultiply2(pDst, pTempArray, pRealFreqDomainCoeffs, pImagFreqDomainCoeffs, pDst, pTempArray, FFTLength);
  SDA_Cifft(pDst, pTempArray, pFFTCoeffs, pBitReverseAddressTable, FFTLength,
            Log10FFTLength);                                // Perform inverse FFT
                                                            // Imaginary output ignored
  SDA_Multiply(pDst, InverseFFTLength, pDst, FFTLength);    // Scale FFT results
  SDA_Add2(pDst, pOverlapArray, pDst,
           (SLArrayIndex_t)(filterLength - 1));    // Add overlap

  SDA_Copy(pDst + DataLength, pOverlapArray,
           (SLArrayIndex_t)(filterLength - 1));    // Copy remainder of output for next iteration
}    // End of  SDA_FirOverlapAdd()

/********************************************************
 * Function: SIF_FirOverlapSave
 *
 * Parameters:
 *  const SLData_t * SIGLIB_PTR_DECL pTimeDomainCoeffs,
 *  SLData_t * SIGLIB_PTR_DECL pRealFreqDomainCoeffs,
 *  SLData_t * SIGLIB_PTR_DECL pImagFreqDomainCoeffs,
 *  SLData_t * SIGLIB_PTR_DECL pOverlapArray,
 *  SLData_t * SIGLIB_PTR_DECL pFFTCoeffs,  - FFT coefficient pointer
 *  SLArrayIndex_t * SIGLIB_PTR_DECL pBitReverseAddressTable, - Bit reverse
 *mode flag / Pointer to bit reverse address table SLData_t * pInverseFFTLength,
 *  const SLArrayIndex_t FFTLength,
 *  const SLArrayIndex_t Log10FFTLength,
 *  const SLArrayIndex_t filterLength
 *
 * Return value: void
 *
 * Description: Initialise the overlap and add frequency
 *  domain filter.
 *
 ********************************************************/

void SIGLIB_FUNC_DECL SIF_FirOverlapSave(const SLData_t* SIGLIB_PTR_DECL pTimeDomainCoeffs, SLData_t* SIGLIB_PTR_DECL pRealFreqDomainCoeffs,
                                         SLData_t* SIGLIB_PTR_DECL pImagFreqDomainCoeffs, SLData_t* SIGLIB_PTR_DECL pOverlapArray,
                                         SLData_t* SIGLIB_PTR_DECL pFFTCoeffs, SLArrayIndex_t* SIGLIB_PTR_DECL pBitReverseAddressTable,
                                         SLData_t* pInverseFFTLength, const SLArrayIndex_t FFTLength, const SLArrayIndex_t Log10FFTLength,
                                         const SLArrayIndex_t filterLength)
{
  SIF_Fft(pFFTCoeffs, pBitReverseAddressTable, FFTLength);    // Initialise FFT
  SDA_Zeros(pOverlapArray, FFTLength);                        // Clear overlap storage
  SDA_Copy(pTimeDomainCoeffs, pRealFreqDomainCoeffs,
           filterLength);    // Copy coefficients
  SDA_Zeros(pRealFreqDomainCoeffs + filterLength,
            (SLArrayIndex_t)(FFTLength - filterLength));                                                                     // Zero pad
  SDA_Rfft(pRealFreqDomainCoeffs, pImagFreqDomainCoeffs, pFFTCoeffs, pBitReverseAddressTable, FFTLength, Log10FFTLength);    // Perform FFT

  *pInverseFFTLength = SIGLIB_ONE / FFTLength;
}    // End of  SIF_FirOverlapSave()

/********************************************************
 * Function: SDA_FirOverlapSave
 *
 * Parameters:
 *  const SLData_t * SIGLIB_PTR_DECL pSrc
 *  SLData_t * SIGLIB_PTR_DECL pDst
 *  const SLData_t * SIGLIB_PTR_DECL pRealFreqDomainCoeffs,
 *  const SLData_t * SIGLIB_PTR_DECL pImagFreqDomainCoeffs,
 *  SLData_t * SIGLIB_PTR_DECL pOverlapArray,
 *  SLData_t * SIGLIB_PTR_DECL pTempArray,
 *  SLData_t * SIGLIB_PTR_DECL pFFTCoeffs,  - FFT coefficient pointer
 *  SLArrayIndex_t * SIGLIB_PTR_DECL pBitReverseAddressTable, - Bit reverse
 *mode flag / Pointer to bit reverse address table const SLData_t
 *InverseFFTLength, const SLArrayIndex_t FFTLength, const SLArrayIndex_t
 *Log10FFTLength, const SLArrayIndex_t filterLength, const SLArrayIndex_t
 *DataLength
 *
 * Return value: void
 *
 * Description: Apply the overlap and add frequency
 *  domain filter.
 *
 *  Notes:
 *  The data array length must be greater than or equal
 *  to the length on the filter.
 *
 *  The overlap array is used to hold the real data for
 *  the FFT This optimises the use of memory.
 *
 ********************************************************/

void SIGLIB_FUNC_DECL SDA_FirOverlapSave(const SLData_t* SIGLIB_PTR_DECL pSrc, SLData_t* SIGLIB_PTR_DECL pDst,
                                         const SLData_t* SIGLIB_PTR_DECL pRealFreqDomainCoeffs,
                                         const SLData_t* SIGLIB_PTR_DECL pImagFreqDomainCoeffs, SLData_t* SIGLIB_PTR_DECL pOverlapArray,
                                         SLData_t* SIGLIB_PTR_DECL pTempArray, SLData_t* SIGLIB_PTR_DECL pFFTCoeffs,
                                         SLArrayIndex_t* SIGLIB_PTR_DECL pBitReverseAddressTable, const SLData_t InverseFFTLength,
                                         const SLArrayIndex_t FFTLength, const SLArrayIndex_t Log10FFTLength, const SLArrayIndex_t filterLength,
                                         const SLArrayIndex_t DataLength)
{
  SDA_Copy(pSrc, pOverlapArray + (FFTLength - DataLength),
           DataLength);    // Copy source data

  SDA_Rfft(pOverlapArray, pTempArray, pFFTCoeffs, pBitReverseAddressTable, FFTLength, Log10FFTLength);    // Perform FFT
  SDA_ComplexMultiply2(pOverlapArray, pTempArray, pRealFreqDomainCoeffs, pImagFreqDomainCoeffs, pOverlapArray, pTempArray, FFTLength);
  SDA_Cifft(pOverlapArray, pTempArray, pFFTCoeffs, pBitReverseAddressTable, FFTLength, Log10FFTLength);    // Perform inverse FFT
                                                                                                           // Imaginary output ignored
  SDA_Multiply(pOverlapArray, InverseFFTLength, pOverlapArray,
               FFTLength);    // Scale FFT results

  SDA_Copy(pOverlapArray + FFTLength - DataLength, pDst,
           DataLength);    // Copy output data to destination

  SDA_Copy(pSrc + DataLength - (filterLength - 1), pOverlapArray + FFTLength - DataLength - (filterLength - 1),
           (SLArrayIndex_t)(filterLength - 1));    // Copy and shift overlap from input
                                                   // array for next iteration
  SDA_Zeros(pOverlapArray,
            (SLArrayIndex_t)(FFTLength - DataLength - (filterLength - 1)));    // Clear start of overlap array
}    // End of  SDA_FirOverlapSave()

/********************************************************
 * Function: SIF_FftConvolvePre
 *
 * Parameters:
 *  const SLData_t *,       - Pointer to filter coefficients
 *  SLData_t *,             - Pointer to real frequency domain filter
 *coefficients SLData_t *,             - Pointer to imag frequency domain filter
 *coefficients SLData_t *,             - Pointer to FFT coefficients
 *  SLArrayIndex_t *,       - Pointer to bit reverse address table
 *  const SLArrayIndex_t,   - Filter length
 *  const SLArrayIndex_t,   - FFT length
 *  const SLArrayIndex_t);  - Log 2 FFT length
 *
 * Return value:
 *  void
 *
 * Description:
 *  Initialize frequency domain filtering function
 *
 ********************************************************/

void SIGLIB_FUNC_DECL SIF_FftConvolvePre(const SLData_t* SIGLIB_PTR_DECL Coeffs, SLData_t* SIGLIB_PTR_DECL pRealCoeffs,
                                         SLData_t* SIGLIB_PTR_DECL pImagCoeffs, SLData_t* SIGLIB_PTR_DECL pFFTCoeffs,
                                         SLArrayIndex_t* SIGLIB_PTR_DECL pBitReverseAddressTable, const SLArrayIndex_t FilterSize,
                                         const SLArrayIndex_t FFTLength, const SLArrayIndex_t Log2FFTLength)
{
  // Initialise FFT
  SIF_Fft(pFFTCoeffs,                 // Pointer to FFT coefficients
          pBitReverseAddressTable,    // Bit reverse mode flag / Pointer to bit
                                      // reverse address table
          FFTLength);                 // FFT length

  // Generate frequency domain coefficients
  SDA_Copy(Coeffs, pRealCoeffs, FilterSize);
  SDA_Zeros(pRealCoeffs + FilterSize, (SLArrayIndex_t)(FFTLength - FilterSize));
  SDA_Rfft(pRealCoeffs, pImagCoeffs, pFFTCoeffs, pBitReverseAddressTable, FFTLength, Log2FFTLength);    // Coefficients FFT
}

/********************************************************
 * Function: SDA_FftConvolvePre
 *
 * Parameters:
 *  SLData_t *,             - Pointer to real time domain source data
 *  SLData_t *,             - Pointer to imag time domain source data
 *  const SLData_t *,       - Pointer to real frequency domain filter
 *coefficients const SLData_t *,       - Pointer to imag frequency domain filter
 *coefficients SLData_t *,             - Pointer to destination array const
 *SLData_t *,       - Pointer to FFT coefficients const SLArrayIndex_t *, -
 *Pointer to bit reverse address table const SLArrayIndex_t,   - Source length
 *  const SLArrayIndex_t,   - Filter length
 *  const SLArrayIndex_t,   - FFT length
 *  const SLArrayIndex_t,   - Log 2 FFT length
 *  const SLData_t);        - Inv FFT length
 *
 * Return value:
 *  void
 *
 * Description:
 *  Frequency domain filtering function
 *
 ********************************************************/

void SIGLIB_FUNC_DECL SDA_FftConvolvePre(SLData_t* SIGLIB_PTR_DECL pRealData, SLData_t* SIGLIB_PTR_DECL pImagData,
                                         const SLData_t* SIGLIB_PTR_DECL pRealCoeffs, const SLData_t* SIGLIB_PTR_DECL pImagCoeffs,
                                         SLData_t* SIGLIB_PTR_DECL pConvResults, const SLData_t* SIGLIB_PTR_DECL pFFTCoeffs,
                                         const SLArrayIndex_t* SIGLIB_PTR_DECL pBitReverseAddressTable, const SLArrayIndex_t SrcLength,
                                         const SLArrayIndex_t CoeffsLength, const SLArrayIndex_t FFTLength, const SLArrayIndex_t Log2FFTLength,
                                         const SLData_t InvFFTLength)
{
  SDA_Zeros(pRealData + SrcLength,
            (SLArrayIndex_t)(FFTLength - SrcLength));    // Ensure zero padded samples

  // Frequency domain convolution
  SDA_Rfft(pRealData, pImagData, pFFTCoeffs, pBitReverseAddressTable, FFTLength,
           Log2FFTLength);    // Source 1 data FFT

  // Complex cross multiply
  SDA_ComplexMultiply2(pRealData, pImagData, pRealCoeffs, pImagCoeffs, pRealData, pImagData, FFTLength);

  SDA_Cifft(pRealData, pImagData, pFFTCoeffs, pBitReverseAddressTable, FFTLength, Log2FFTLength);    // IFFT

  SDA_Multiply(pRealData, InvFFTLength, pConvResults,
               SrcLength + CoeffsLength - 1);    // Scale and copy output results
}    // End of SDA_FftConvolvePre()

/********************************************************
 * Function: SDA_FftConvolveArb
 *
 * Parameters:
 *  SLData_t *,             - Pointer to real time domain source data 1
 *  SLData_t *,             - Pointer to imag time domain source data 1
 *  SLData_t *,             - Pointer to real time domain source data 2
 *  SLData_t *,             - Pointer to imag time domain source data 2
 *  SLData_t *,             - Pointer to destination array
 *  const SLData_t *,       - Pointer to FFT coefficients
 *  const SLArrayIndex_t *, - Pointer to bit reverse address table
 *  const SLArrayIndex_t,   - Source 1 length
 *  const SLArrayIndex_t,   - Source 2 length
 *  const SLArrayIndex_t,   - FFT length
 *  const SLArrayIndex_t,   - Log 2 FFT length
 *  const SLData_t);        - Inv FFT length
 *
 * Return value:
 *  void
 *
 * Description:
 *  Frequency domain convolution function
 *
 ********************************************************/

void SIGLIB_FUNC_DECL SDA_FftConvolveArb(SLData_t* SIGLIB_PTR_DECL pRealSrc1, SLData_t* SIGLIB_PTR_DECL pImagSrc1,
                                         SLData_t* SIGLIB_PTR_DECL pRealSrc2, SLData_t* SIGLIB_PTR_DECL pImagSrc2,
                                         SLData_t* SIGLIB_PTR_DECL pConvResults, const SLData_t* SIGLIB_PTR_DECL pFFTCoeffs,
                                         const SLArrayIndex_t* SIGLIB_PTR_DECL pBitReverseAddressTable, const SLArrayIndex_t Src1Length,
                                         const SLArrayIndex_t Src2Length, const SLArrayIndex_t FFTLength, const SLArrayIndex_t Log2FFTLength,
                                         const SLData_t InvFFTLength)
{
  SDA_Zeros(pRealSrc1 + Src1Length,
            (SLArrayIndex_t)(FFTLength - Src1Length));    // Ensure zero padded samples
  SDA_Zeros(pRealSrc2 + Src2Length, (SLArrayIndex_t)(FFTLength - Src2Length));

  // Frequency domain convolution
  SDA_Rfft(pRealSrc1, pImagSrc1, pFFTCoeffs, pBitReverseAddressTable, FFTLength,
           Log2FFTLength);    // Source 1 data FFT
  SDA_Rfft(pRealSrc2, pImagSrc2, pFFTCoeffs, pBitReverseAddressTable, FFTLength,
           Log2FFTLength);    // Source 2 data FFT

  // Complex cross multiply
  SDA_ComplexMultiply2(pRealSrc1, pImagSrc1, pRealSrc2, pImagSrc2, pRealSrc1, pImagSrc1, FFTLength);

  SDA_Cifft(pRealSrc1, pImagSrc1, pFFTCoeffs, pBitReverseAddressTable, FFTLength, Log2FFTLength);    // IFFT

  SDA_Multiply(pRealSrc1, InvFFTLength, pConvResults,
               Src1Length + Src2Length - 1);    // Scale and copy output results
}    // End of SDA_FftConvolveArb()

/********************************************************
 * Function: SIF_FftCorrelatePre
 *
 * Parameters:
 *  const SLData_t *,       - Pointer to filter coefficients
 *  SLData_t *,             - Pointer to real frequency domain filter
 *coefficients SLData_t *,             - Pointer to imag frequency domain filter
 *coefficients SLData_t *,             - Pointer to FFT coefficients
 *  SLArrayIndex_t *,       - Pointer to bit reverse address table
 *  const SLArrayIndex_t,   - Filter length
 *  const SLArrayIndex_t,   - FFT length
 *  const SLArrayIndex_t);  - Log 2 FFT length
 *
 * Return value:
 *  void
 *
 * Description:
 *  Initialize frequency domain filtering function
 *
 ********************************************************/

void SIGLIB_FUNC_DECL SIF_FftCorrelatePre(const SLData_t* SIGLIB_PTR_DECL Coeffs, SLData_t* SIGLIB_PTR_DECL pRealSrc2,
                                          SLData_t* SIGLIB_PTR_DECL pImagSrc2, SLData_t* SIGLIB_PTR_DECL pFFTCoeffs,
                                          SLArrayIndex_t* SIGLIB_PTR_DECL pBitReverseAddressTable, const SLArrayIndex_t FilterSize,
                                          const SLArrayIndex_t FFTLength, const SLArrayIndex_t Log2FFTLength)
{

  // Initialise FFT
  SIF_Fft(pFFTCoeffs,                 // Pointer to FFT coefficients
          pBitReverseAddressTable,    // Bit reverse mode flag / Pointer to bit
                                      // reverse address table
          FFTLength);                 // FFT length

  // Generate frequency domain coefficients
  SDA_Copy(Coeffs, pRealSrc2, FilterSize);
  SDA_Zeros(pRealSrc2 + FilterSize, (SLArrayIndex_t)(FFTLength - FilterSize));
  SDA_Rfft(pRealSrc2, pImagSrc2, pFFTCoeffs, pBitReverseAddressTable, FFTLength,
           Log2FFTLength);                        // Coefficients FFT
  SDA_Negate(pImagSrc2, pImagSrc2, FFTLength);    // Conjugate input 2
}

/********************************************************
 * Function: SDA_FftCorrelatePre
 *
 * Parameters:
 *  SLData_t *,             - Pointer to real time domain source data 1
 *  SLData_t *,             - Pointer to imag time domain source data 1
 *  SLData_t *,             - Pointer to real time domain source data 2
 *  SLData_t *,             - Pointer to imag time domain source data 2
 *  SLData_t *,             - Pointer to destination array
 *  const SLData_t *,       - Pointer to FFT coefficients
 *  const SLArrayIndex_t *, - Pointer to bit reverse address table
 *  const SLArrayIndex_t,   - Source 1 length
 *  const SLArrayIndex_t,   - Source 2 length
 *  const SLArrayIndex_t,   - FFT length
 *  const SLArrayIndex_t,   - Log 2 FFT length
 *  const SLData_t);        - Inv FFT length
 *
 * Return value:
 *  void
 *
 * Description:
 *  Frequency domain filtering function
 *
 ********************************************************/

void SIGLIB_FUNC_DECL SDA_FftCorrelatePre(SLData_t* SIGLIB_PTR_DECL pRealSrc1, SLData_t* SIGLIB_PTR_DECL pImagSrc1,
                                          SLData_t* SIGLIB_PTR_DECL pRealSrc2, SLData_t* SIGLIB_PTR_DECL pImagSrc2,
                                          SLData_t* SIGLIB_PTR_DECL pCorrResults, const SLData_t* SIGLIB_PTR_DECL pFFTCoeffs,
                                          const SLArrayIndex_t* SIGLIB_PTR_DECL pBitReverseAddressTable, const SLArrayIndex_t Src1Length,
                                          const SLArrayIndex_t Src2Length, const SLArrayIndex_t FFTLength, const SLArrayIndex_t Log2FFTLength,
                                          const SLData_t InvFFTLength)
{
  SDA_Zeros(pRealSrc1 + Src1Length,
            (SLArrayIndex_t)(FFTLength - Src1Length));    // Ensure zero padded samples

  // Frequency domain convolution
  SDA_Rfft(pRealSrc1, pImagSrc1, pFFTCoeffs, pBitReverseAddressTable, FFTLength,
           Log2FFTLength);    // Source 1 data FFT

  // Complex cross multiply
  SDA_ComplexMultiply2(pRealSrc1, pImagSrc1, pRealSrc2, pImagSrc2, pRealSrc1, pImagSrc1, FFTLength);

  SDA_Cifft(pRealSrc1, pImagSrc1, pFFTCoeffs, pBitReverseAddressTable, FFTLength, Log2FFTLength);    // IFFT

  SDA_Rotate(pRealSrc1, pImagSrc1, Src2Length - SIGLIB_AI_ONE,
             FFTLength);    // Rotate correlation results

  SDA_Multiply(pImagSrc1, InvFFTLength, pCorrResults,
               Src1Length + Src2Length - 1);    // Scale and copy output results
}    // End of SDA_FftCorrelatePre()

/********************************************************
 * Function: SDA_FftCorrelateArb
 *
 * Parameters:
 *  SLData_t *,             - Pointer to real time domain source data 1
 *  SLData_t *,             - Pointer to imag time domain source data 1
 *  SLData_t *,             - Pointer to real time domain source data 2
 *  SLData_t *,             - Pointer to imag time domain source data 2
 *  SLData_t *,             - Pointer to destination array
 *  const SLData_t *,       - Pointer to FFT coefficients
 *  const SLArrayIndex_t *, - Pointer to bit reverse address table
 *  const SLArrayIndex_t,   - Source 1 length
 *  const SLArrayIndex_t,   - Source 2 length
 *  const SLArrayIndex_t,   - FFT length
 *  const SLArrayIndex_t,   - Log 2 FFT length
 *  const SLData_t);        - Inv FFT length
 *
 * Return value:
 *  void
 *
 * Description:
 *  Frequency domain convolution function
 *
 ********************************************************/

void SIGLIB_FUNC_DECL SDA_FftCorrelateArb(SLData_t* SIGLIB_PTR_DECL pRealSrc1, SLData_t* SIGLIB_PTR_DECL pImagSrc1,
                                          SLData_t* SIGLIB_PTR_DECL pRealSrc2, SLData_t* SIGLIB_PTR_DECL pImagSrc2,
                                          SLData_t* SIGLIB_PTR_DECL pCorrResults, const SLData_t* SIGLIB_PTR_DECL pFFTCoeffs,
                                          const SLArrayIndex_t* SIGLIB_PTR_DECL pBitReverseAddressTable, const SLArrayIndex_t Src1Length,
                                          const SLArrayIndex_t Src2Length, const SLArrayIndex_t FFTLength, const SLArrayIndex_t Log2FFTLength,
                                          const SLData_t InvFFTLength)
{
  SDA_Zeros(pRealSrc1 + Src1Length,
            (SLArrayIndex_t)(FFTLength - Src1Length));    // Ensure zero padded samples
  SDA_Zeros(pRealSrc2 + Src2Length, (SLArrayIndex_t)(FFTLength - Src2Length));

  // Frequency domain convolution
  SDA_Rfft(pRealSrc1, pImagSrc1, pFFTCoeffs, pBitReverseAddressTable, FFTLength,
           Log2FFTLength);    // Source 1 data FFT
  SDA_Rfft(pRealSrc2, pImagSrc2, pFFTCoeffs, pBitReverseAddressTable, FFTLength,
           Log2FFTLength);    // Source 2 data FFT

  SDA_Negate(pImagSrc2, pImagSrc2, FFTLength);    // Conjugate input 2

  // Complex cross multiply
  SDA_ComplexMultiply2(pRealSrc1, pImagSrc1, pRealSrc2, pImagSrc2, pRealSrc1, pImagSrc1, FFTLength);

  SDA_Cifft(pRealSrc1, pImagSrc1, pFFTCoeffs, pBitReverseAddressTable, FFTLength, Log2FFTLength);    // IFFT

  SDA_Rotate(pRealSrc1, pImagSrc1, Src2Length - SIGLIB_AI_ONE,
             FFTLength);    // Rotate correlation results

  SDA_Multiply(pImagSrc1, InvFFTLength, pCorrResults,
               Src1Length + Src2Length - 1);    // Scale and copy output results
}    // End of SDA_FftCorrelateArb()

/********************************************************
 * Function: SDA_RfftConvolve
 *
 * Parameters:
 *  SLData_t *,             - Pointer to real time domain source data 1
 *  SLData_t *,             - Pointer to imag time domain source data 1
 *  SLData_t *,             - Pointer to real time domain source data 2
 *  SLData_t *,             - Pointer to imag time domain source data 2
 *  SLData_t *,             - Pointer to real destination array
 *  SLData_t *,             - Pointer to imag destination array
 *  const SLData_t *,       - Pointer to FFT coefficients
 *  const SLArrayIndex_t *, - Pointer to bit reverse address table
 *  const enum SLFftConvolveModeType_t, - Result mode
 *  const SLArrayIndex_t,   - Source 1 length
 *  const SLArrayIndex_t,   - Source 2 length
 *  const SLArrayIndex_t,   - FFT length
 *  const SLArrayIndex_t,   - Log 2 FFT length
 *  const SLData_t);        - Inv FFT length
 *
 * Return value:
 *  Result length
 *
 * Description:
 *  Frequency domain convolution function, with selectable result modes
 *
 ********************************************************/

SLArrayIndex_t SIGLIB_FUNC_DECL SDA_RfftConvolve(SLData_t* SIGLIB_PTR_DECL pSrc1Real, SLData_t* SIGLIB_PTR_DECL pSrc1Imag,
                                                 SLData_t* SIGLIB_PTR_DECL pSrc2Real, SLData_t* SIGLIB_PTR_DECL pSrc2Imag,
                                                 SLData_t* SIGLIB_PTR_DECL pResultReal, SLData_t* SIGLIB_PTR_DECL pResultImag,
                                                 const SLData_t* SIGLIB_PTR_DECL pFFTCoeffs,
                                                 const SLArrayIndex_t* SIGLIB_PTR_DECL pBitReverseAddressTable,
                                                 const enum SLFftConvolveModeType_t mode, SLArrayIndex_t src1Length, SLArrayIndex_t src2Length,
                                                 const SLArrayIndex_t fftLength, const SLArrayIndex_t log2FFTLength,
                                                 const SLData_t InverseFFTLength)
{
  if (src2Length > src1Length) {
    return -1;
  }

  SDA_Zeros(pSrc1Real + src1Length, fftLength - src1Length);    // Ensure input arrays are zero padded
  SDA_Zeros(pSrc2Real + src1Length, fftLength - src1Length);

  // Perform forward FFTs
  SDA_Rfft(pSrc1Real,                  // Pointer to real array
           pSrc1Imag,                  // Pointer to imaginary array
           pFFTCoeffs,                 // Pointer to FFT coefficients
           pBitReverseAddressTable,    // Bit reverse mode flag / Pointer to bit
                                       // reverse address table
           fftLength,                  // FFT length
           log2FFTLength);             // log2 FFT length
  SDA_Rfft(pSrc2Real,                  // Pointer to real array
           pSrc2Imag,                  // Pointer to imaginary array
           pFFTCoeffs,                 // Pointer to FFT coefficients
           pBitReverseAddressTable,    // Bit reverse mode flag / Pointer to bit
                                       // reverse address table
           fftLength,                  // FFT length
           log2FFTLength);             // log2 FFT length

  // Frequency domain multiplication
  for (SLArrayIndex_t i = 0; i < fftLength; i++) {
    pResultReal[i] = pSrc1Real[i] * pSrc2Real[i] - pSrc1Imag[i] * pSrc2Imag[i];
    pResultImag[i] = pSrc1Real[i] * pSrc2Imag[i] + pSrc1Imag[i] * pSrc2Real[i];
  }

  // Perform inverse FFT
  SDA_Cifft(pResultReal,                // Pointer to real array
            pResultImag,                // Pointer to imaginary array
            pFFTCoeffs,                 // Pointer to FFT coefficients
            pBitReverseAddressTable,    // Bit reverse mode flag / Pointer to bit
                                        // reverse address table
            fftLength,                  // FFT length
            log2FFTLength);             // log2 FFT length

  for (SLArrayIndex_t i = 0; i < fftLength; i++) {    // Normalize by dividing by the FFT length
    pResultReal[i] *= InverseFFTLength;
  }

  // Determine output size based on mode
  SLArrayIndex_t resultLength;
  if (mode == SIGLIB_FFT_CONVOLVE_MODE_SAME) {
    int start = (src2Length - 1) / 2;
    for (SLArrayIndex_t i = 0; i < src1Length; i++) {
      pResultReal[i] = pResultReal[i + start];
    }

    resultLength = src1Length;
  } else if (mode == SIGLIB_FFT_CONVOLVE_MODE_VALID) {
    for (SLArrayIndex_t i = 0; i < src1Length - src2Length + 1; i++) {
      pResultReal[i] = pResultReal[i + src2Length - 1];
    }
    resultLength = src1Length - src2Length + 1;
  } else {    // Default mode == SIGLIB_FULL
    resultLength = src1Length + src2Length - 1;
  }
  return (resultLength);
}    // End of SDA_RfftConvolve()
