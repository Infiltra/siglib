// SigLib Octal Phase Shift Keying (8-PSK) Example
//     Data rate - 1800 bps
//     Baud rate - 600 Baud
//     Sample rate - 9600 Hz
//     Carrier freq. - 2400 Hz
//
// To see how to apply a scrambler to the sequence
// (e.g. polynomial: 1 + x-14 + x-17 used in the ITU-T
// recommendations), please refer to example tstqam16.c.
// Copyright (c) 2024 Delta Numerix All rights reserved.

// Include files
#include <stdio.h>
#include <siglib.h>             // SigLib DSP library
#include <gnuplot_c.h>          // Gnuplot/C
#include "plot_fd/plot_fd.h"    // Frequency domain plots
#include <dpchar.h>

// Define constants
#define RRCF_ENABLE 1                     // Root raised cosine filter on Tx and Rx
#define DIFFERENTIAL_ENCODING_ENABLE 1    // Differential encoding / decoding

#define TX_ALPHABET 1       // Set to '1' to Tx the alphabet or '0' to Tx 0xFFs
#define DEBUG_LOG_FILE 0    // Set to '1' to enable logging to debug.log

// Select one of the following display modes
#define DISPLAY_TIME_DOMAIN 0      // Set to '1' to display the time domain output from the transmitter
#define DISPLAY_FREQ_DOMAIN 0      // Set to '1' to display the frequency domain output
#define DISPLAY_EYE_DIAGRAM 0      // Set to '1' to display the eye diagram output
#define DISPLAY_TRIGGER 0          // Set to '1' to display the trigger and the delayed costas loop LPF outputs
#define DISPLAY_CONSTELLATION 1    // Set to '1' to display the constellation diagram

#define NUMBER_OF_LOOPS 6    // Number of loops

// Basic application definitions
#define SAMPLE_LENGTH 512    // Number of samples in array

#define SAMPLE_RATE 9600.     // Sample rate
#define SYMBOL_RATE 600.      // Baud rate
#define CARRIER_FREQ 2400.    // Frequency of carrier signal - a multiple of the sine table frequency

#define SYMBOL_LENGTH 16          // Symbol Period = SAMPLE_RATE / SYMBOL_RATE
#define CARRIER_CYCLE_LENGTH 4    // Carrier Period = SAMPLE_RATE / CARRIER_FREQ

#if RRCF_ENABLE
#  define RRCF_PERIOD SYMBOL_LENGTH    // RRCF Period
#  define RRCF_ROLL_OFF 0.75           // Root raised cosine filter roll off factor
#  define RRCF_LENGTH 81               // Root raised cosine filter length
#  if DIFFERENTIAL_ENCODING_ENABLE
#    define TX_STARTUP_PREFILL \
      1    // Txr startup pre-fill (# symbols) to allow correct synchronization with
           // receiver
#    define RX_STARTUP_DELAY \
      0    // Rxr startup delay (# symbols) to allow correct synchronization with
           // transmitter
#  else
#    define TX_STARTUP_PREFILL \
      4    // Txr startup pre-fill (# symbols) to allow correct synchronization with
           // receiver
#    define RX_STARTUP_DELAY \
      1    // Rxr startup delay (# symbols) to allow correct synchronization with
           // transmitter
#  endif
#else
#  define RRCF_PERIOD SIGLIB_ZERO      // Dummy value - RRCF Period
#  define RRCF_ROLL_OFF SIGLIB_ZERO    // Dummy value - Root raised cosine filter roll off factor
#  define RRCF_LENGTH 1                // Dummy value - Root raised cosine filter length
#  if DIFFERENTIAL_ENCODING_ENABLE
#    define TX_STARTUP_PREFILL \
      1    // Txr startup pre-fill (# symbols) to allow correct synchronization with
           // receiver
#    define RX_STARTUP_DELAY \
      0    // Rxr startup delay (# symbols) to allow correct synchronization with
           // transmitter
#  else
#    define TX_STARTUP_PREFILL \
      4    // Txr startup pre-fill (# symbols) to allow correct synchronization with
           // receiver
#    define RX_STARTUP_DELAY \
      1    // Rxr startup delay (# symbols) to allow correct synchronization with
           // transmitter
#  endif
#endif

#define GAUSSIAN_NOISE_VARIANCE SIGLIB_ZERO    // Injected noise parameters
#define GAUSSIAN_NOISE_OFFSET SIGLIB_ZERO

// Derived application definitions
#define SYMBOLS_PER_LOOP ((SLArrayIndex_t)(SAMPLE_LENGTH / SYMBOL_LENGTH))    // Number of symbols per loop for graph

#define TX_CARRIER_TABLE_FREQ 100.    // Frequency of sine wave in table
#define TX_CARRIER_TABLE_SIZE \
  ((SLArrayIndex_t)(SAMPLE_RATE / TX_CARRIER_TABLE_FREQ))                                      // Number of samples in each of cos
                                                                                               // and sine table
#define TX_CARRIER_TABLE_INCREMENT ((SLArrayIndex_t)(CARRIER_FREQ / TX_CARRIER_TABLE_FREQ))    // Carrier frequency

#define EYE_DIAGRAM_SIZE (2 * SYMBOL_LENGTH)    // Size of eye diagram graph - Two complete symbol periods

#define MAX_CONSTELLATION_POINTS 40    // Maximum number of constellation points

// Declare global variables and arrays

#if TX_ALPHABET
// Transmit string.
static const char TxString[] = "Hello World - "
                               "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
#else
static const unsigned char TxString[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif
static char RxString[80];

// Modem transmitter parameters and variables
static SLData_t txCarrierPhase;
static SLArrayIndex_t TxSampleClock;    // Used to keep track of the samples and symbols
static SLComplexRect_s TxMagn;          // Used to calculate the signal magnitude

static SLData_t TxIRRCState[RRCF_LENGTH];    // RRCF filter state arrays
static SLData_t TxQRRCState[RRCF_LENGTH];
static SLData_t RRCFCoeffs[RRCF_LENGTH];    // Filter coefficients

static SLArrayIndex_t TxRRCFIFilterIndex,
    TxRRCFQFilterIndex;    // RRCF filter indices

static SLData_t ModulatedSignal[SAMPLE_LENGTH];    // Modulated signal between Txr and Rxr

static SLData_t GaussianNoisePhase,
    GaussianNoiseValue;    // Variables for injecting noise

// Variable and arrays for Costas QAM demodulator output
static SLData_t pRealOutput[MAX_CONSTELLATION_POINTS], pImagOutput[MAX_CONSTELLATION_POINTS];

// Costas loop data
// Note: Costas loop LPF lengths are chosen so that there
// are at least two full cycles and an odd number - for an integer group delay
#define COSTAS_LP_LPF_LENGTH (((SLArrayIndex_t)(2.0 * CARRIER_CYCLE_LENGTH)) | 0x1)    // Costas loop LP LPF FIR filter length
#define COSTAS_LP_VCO_MODULATION_INDEX 0.00005                                         // Modulation index
#define COSTAS_LP_LOOP_FILTER_ALPHA 0.5                                                // Feedback coeff for one-pole loop filter
#define COSTAS_LP_VCO_TABLE_SIZE 1024                                                  // Look up table for fast sine calculation

static SLArrayIndex_t CostasLpLPF1Index;    // Costas loop inphase LPF filter index
static SLArrayIndex_t CostasLpLPF2Index;    // Costas loop quadrature phase LPF filter index
static SLData_t CostasLpState;              // Costas loop feedback state for next iteration

static SLData_t CostasLpLoopFilterState;    // Costas loop loop filter feedback coeff
static SLData_t CostasLpVCOPhase;           // Costas loop VCO phase

// Early-late gate symbol synchronizer data

#define ELG_EARLY_GATE_DELAY_LENGTH (SYMBOL_LENGTH >> 1)
#define ELG_LOOP_FILTER_LENGTH ((3 * SYMBOL_LENGTH) + 1)
#define ELG_LOOP_FILTER_FC (SYMBOL_RATE / SIGLIB_TWO)
#define ELG_PULSE_DETECTOR_THRESHOLD 0.2    // Threshold below which the timing is not detected
#define ELG_SYNCH_DELAY_ARRAY_LENGTH \
  (10 * SYMBOL_LENGTH)    // Length of array for accounting for delay through
                          // timing detector
                          // This dataset length must be long enough to
                          // account for the TED delay

static SLData_t ELGMatchedFilterSum;
static SLData_t pELGMatchedFilterState[SYMBOL_LENGTH];
static SLArrayIndex_t ELGMatchedFilterIndex;

static SLData_t pELGEarlyGateDelay[ELG_EARLY_GATE_DELAY_LENGTH];
static SLArrayIndex_t ELGEarlyGateDelayIndex;

static SLData_t pELGLoopFilterCoeffs[ELG_LOOP_FILTER_LENGTH];
static SLData_t pELGLoopFilterState[ELG_LOOP_FILTER_LENGTH];
static SLArrayIndex_t ELGLoopFilterIndex;
static SLFixData_t ELGPulseDetectorThresholdFlag;    // Flag to indicate signal level higher than
                                                     // noise level
static SLData_t ELGZeroCrossingPreviousSample;       // Previous sample used for zero
                                                     // crossing detector
static SLArrayIndex_t ELGTriggerLatency;             // Estimate of the latency through the ELG TED
static SLArrayIndex_t ELGTriggerCount;               // Variables for trigger reverberator
static SLFixData_t ELGTriggerDetectedFlag;
static SLFixData_t ELGTriggerUpdatedFlag;

static SLData_t pELGRealOutputSynchDelay[ELG_SYNCH_DELAY_ARRAY_LENGTH];    // ELG output real synchronization delay
static SLData_t pELGImagOutputSynchDelay[ELG_SYNCH_DELAY_ARRAY_LENGTH];    // ELG output imag synchronization delay
static SLArrayIndex_t ELGOutputSynchDelayIndex;

extern SLComplexRect_s siglib_numerix_OPSKRxConstellation[];    // Receiver constellation map

static SLData_t RxDemodErrorArray[SIGLIB_OPSK_NUMBER_OF_PHASES];    // Error calculation array

// Debug arrays
#if DISPLAY_CONSTELLATION
static SLComplexRect_s ConstellationPoints[MAX_CONSTELLATION_POINTS];    // Constellation diagram
                                                                         // array
#endif
#if (DISPLAY_TRIGGER) || (DISPLAY_EYE_DIAGRAM)
static SLArrayIndex_t TriggerIndexCount;
static SLData_t DebugRealFilterOutput[SAMPLE_LENGTH];
static SLData_t DebugImagFilterOutput[SAMPLE_LENGTH];
static SLData_t DebugELGTriggerOutput[SAMPLE_LENGTH];
#endif

int main(void)
{
#if (DISPLAY_TIME_DOMAIN) || (DISPLAY_FREQ_DOMAIN) || (DISPLAY_TRIGGER) || (DISPLAY_EYE_DIAGRAM)
  h_GPC_Plot* h2DPlot;    // Plot objects
#endif
#if DISPLAY_CONSTELLATION
  h_GPC_Plot* hConstellationDiagram;
#endif
#if DISPLAY_TIME_DOMAIN
  SLData_t TimeIndex = SIGLIB_ZERO;
#endif

  SLError_t SigLibErrorCode;
  SLArrayIndex_t TxStringIndex = 0;
  SLArrayIndex_t TxSourceWordPhase = 0;
  SLFixData_t TxTriBit = 0;
  SLFixData_t TxPreviousTxCharBits = 0;
  SLArrayIndex_t TxPreFillSymbolCount = 0;    // Tx pipeline pre-fill
  SLArrayIndex_t RxStringIndex = 0;
  SLArrayIndex_t RxSourceWordPhase = 0;
  SLFixData_t RxTriBit;
  SLFixData_t RxStartUpDelayCount = 0;    // Rx startup delay count
  SLFixData_t RxPreviousRxCharBits = 0;
  SLArrayIndex_t LoopCount;
  char TxCharTmpVariable = 0;    // Temporary variables - char type
  char RxCharTmpVariable = 0;
#if (DIFFERENTIAL_ENCODING_ENABLE)
  SLFixData_t PreviousTxTriBit = 0;    // Differential encoding variables
  SLFixData_t PreviousRxTriBit = 0;
#endif
  SLArrayIndex_t RxSymbolCount;    // Count of the number of output symbols
#if (DISPLAY_EYE_DIAGRAM)
  int FirstEyeDiagramFlag = 1;
#endif
#if (DISPLAY_CONSTELLATION)
  int FirstConstellationDiagramFlag = 1;
#endif

#if DEBUG_LOG_FILE
  SUF_ClearDebugfprintf();
  for (SLArrayIndex_t i = 0; i < 20; i++) {
    SUF_Debugfprintf("TxString[%d]", i);
    dpchar(TxString[i]);
  }
#endif
  // Allocate arrays
  SLData_t* pTxCarrierTable = SUF_OPSKCarrierArrayAllocate(TX_CARRIER_TABLE_SIZE);    // Overlapped cosine + sine look-up table
#if (DIFFERENTIAL_ENCODING_ENABLE)
  SLFixData_t* pDifferentialEncoderMap = SUF_DifferentialEncoderArrayAllocate(SIGLIB_OPSK_BITS_PER_SYMBOL);    // Differential encoding maps
  SLFixData_t* pDifferentialDecoderMap = SUF_DifferentialEncoderArrayAllocate(SIGLIB_OPSK_BITS_PER_SYMBOL);
#endif
  SLData_t* pCostasLpLPFCoeffs = SUF_VectorArrayAllocate(COSTAS_LP_LPF_LENGTH);    // Costas loop loop filter coefficient pointer
  SLData_t* pCostasLpLPF1State = SUF_VectorArrayAllocate(COSTAS_LP_LPF_LENGTH);
  SLData_t* pCostasLpLPF2State = SUF_VectorArrayAllocate(COSTAS_LP_LPF_LENGTH);
  SLData_t* pCostasLpVCOLookUpTable = SUF_CostasLoopVCOArrayAllocate(COSTAS_LP_VCO_TABLE_SIZE);    // VCO cosine look-up-table pointer

  if ((NULL == pTxCarrierTable) || (NULL == pCostasLpLPFCoeffs) || (NULL == pCostasLpLPF1State) ||
#if (DIFFERENTIAL_ENCODING_ENABLE)
      (NULL == pDifferentialEncoderMap) || (NULL == pDifferentialDecoderMap) ||
#endif
      (NULL == pCostasLpLPF2State) || (NULL == pCostasLpVCOLookUpTable)) {

    printf("Memory allocation failure\n");
    exit(-1);
  }

// Always initialise to largest size first
#if (DISPLAY_TIME_DOMAIN) || (DISPLAY_FREQ_DOMAIN)
  h2DPlot =                                           // Initialize plot
      gpc_init_2d("Quadrature Phase Shift Keying",    // Plot title
                  "Time",                             // X-Axis label
                  "Magnitude",                        // Y-Axis label
                  GPC_AUTO_SCALE,                     // Scaling mode
                  GPC_SIGNED,                         // Sign mode
                  GPC_KEY_DISABLE);                   // Legend / key mode
  if (NULL == h2DPlot) {
    printf("\nPlot creation failure.\n");
    exit(-1);
  }
#endif
#if (DISPLAY_TRIGGER) || (DISPLAY_EYE_DIAGRAM)
  h2DPlot =                                           // Initialize plot
      gpc_init_2d("Quadrature Phase Shift Keying",    // Plot title
                  "Time",                             // X-Axis label
                  "Magnitude",                        // Y-Axis label
                  (float)0.3,                         // Scaling mode
                  GPC_SIGNED,                         // Sign mode
                  GPC_KEY_DISABLE);                   // Legend / key mode
  if (NULL == h2DPlot) {
    printf("\nPlot creation failure.\n");
    exit(-1);
  }
#endif
#if DISPLAY_CONSTELLATION
  hConstellationDiagram =                           // Initialize plot
      gpc_init_xy("8-PSK Constellation Diagram",    // Plot title
                  "X-Axis",                         // X-Axis label
                  "Y-Axis",                         // Y-Axis label
                  1.1,                              // Dimension - this is square
                  GPC_KEY_DISABLE);                 // Legend / key mode
  if (NULL == hConstellationDiagram) {              // Graph creation failed
    printf("\nPlot creation failure.\n");
    exit(-1);
  }
#endif

  // Initialise OPSK functions
  SIF_OpskModulate(pTxCarrierTable,                        // Carrier table pointer
                   TX_CARRIER_TABLE_FREQ / SAMPLE_RATE,    // Carrier phase increment per sample (radians / 2π)
                   TX_CARRIER_TABLE_SIZE,                  // Carrier sine table size
                   &txCarrierPhase,                        // Carrier phase pointer
                   &TxSampleClock,                         // Sample clock pointer
                   &TxMagn,                                // Magnitude pointer
                   TxIRRCState,                            // RRCF Tx I delay pointer
                   &TxRRCFIFilterIndex,                    // RRCF Tx I Filter Index pointer
                   TxQRRCState,                            // RRCF Tx Q delay pointer
                   &TxRRCFQFilterIndex,                    // RRCF Tx Q Filter Index pointer
                   RRCFCoeffs,                             // RRCF Coefficients pointer
                   RRCF_PERIOD,                            // RRCF Period
                   RRCF_ROLL_OFF,                          // RRCF Roll off
                   RRCF_LENGTH,                            // RRCF size
                   RRCF_ENABLE);                           // RRCF enable / disable switch

  // Initialise the Costas loop QAM demodulator
  SigLibErrorCode = SIF_CostasQamDemodulate(&CostasLpVCOPhase,                   // VCO phase
                                            pCostasLpVCOLookUpTable,             // VCO look up table
                                            COSTAS_LP_VCO_TABLE_SIZE,            // VCO look up table size
                                            SYMBOL_RATE / SAMPLE_RATE,           // Low-pass filter cut-off frequency
                                            pCostasLpLPF1State,                  // Pointer to loop filter 1 state
                                            &CostasLpLPF1Index,                  // Pointer to loop filter 1 index
                                            pCostasLpLPF2State,                  // Pointer to loop filter 2 state
                                            &CostasLpLPF2Index,                  // Pointer to loop filter 2 index
                                            pCostasLpLPFCoeffs,                  // Pointer to loop filter coefficients
                                            COSTAS_LP_LPF_LENGTH,                // Loop filter length
                                            &CostasLpLoopFilterState,            // Pointer to loop filter state
                                            &CostasLpState,                      // Pointer to delayed sample
                                            pELGMatchedFilterState,              // Pointer to matched filter state array
                                            &ELGMatchedFilterIndex,              // Pointer to matched filter index
                                            &ELGMatchedFilterSum,                // Pointer to matched filter sum
                                            pELGEarlyGateDelay,                  // Pointer to early gate state array
                                            &ELGEarlyGateDelayIndex,             // Pointer to early gate delay index
                                            ELG_EARLY_GATE_DELAY_LENGTH,         // Early gate delay length
                                            pELGLoopFilterState,                 // Pointer to loop filter state array
                                            pELGLoopFilterCoeffs,                // Pointer to loop filter coefficients
                                            &ELGLoopFilterIndex,                 // Pointer to loop filter index
                                            ELG_LOOP_FILTER_LENGTH,              // Loop filter length
                                            ELG_LOOP_FILTER_FC / SAMPLE_RATE,    // Loop filter cut-off / centre frequency
                                            &ELGPulseDetectorThresholdFlag,      // Pointer to pulse detector threshold
                                                                                 // flag
                                            &ELGZeroCrossingPreviousSample,      // Pointer to zero crossing previous
                                                                                 // sample
                                            &ELGTriggerCount,                    // Pointer to trigger counter
                                            &ELGTriggerDetectedFlag,             // Pointer to trigger detected flag
                                            &ELGTriggerUpdatedFlag,              // Pointer to trigger updated flag
                                            &ELGTriggerLatency,                  // Pointer to ELG trigger latency
                                            SYMBOL_LENGTH,                       // Samples per symbol
                                            pELGRealOutputSynchDelay,            // Pointer to ELG real output synchronization
                                                                                 // delay state array
                                            pELGImagOutputSynchDelay,            // Pointer to ELG imaginary output
                                                                                 // synchronization delay state array
                                            &ELGOutputSynchDelayIndex);          // Pointer to ELG synchronization delay index

  if (SigLibErrorCode != SIGLIB_NO_ERROR) {
    printf("Error in SIF_CostasQAMDemodulate\n");
    exit(0);
  }

#if (DIFFERENTIAL_ENCODING_ENABLE)
  SIF_DifferentialEncoder(pDifferentialEncoderMap,         // Differential encoder LUT
                          pDifferentialDecoderMap,         // Differential decoder LUT
                          SIGLIB_OPSK_BITS_PER_SYMBOL);    // Bits per symbol
#endif

  // Main data processing loop
  for (LoopCount = 0; LoopCount < NUMBER_OF_LOOPS; LoopCount++) {
    for (SLArrayIndex_t i = 0; i < SYMBOLS_PER_LOOP; i++) {    // Modulate new symbol
      if (TxPreFillSymbolCount < TX_STARTUP_PREFILL) {         // Pre-fill the TxRx pipeline
        // In order to lock the TED we must use a signal that changes phase
        // every symbol period
        TxTriBit = (3 * TxPreFillSymbolCount) & SIGLIB_OPSK_BIT_MASK;
        TxPreFillSymbolCount++;
      } else {
        switch (TxSourceWordPhase) {
        case 0:
          TxCharTmpVariable = TxString[TxStringIndex++];
          TxTriBit = (SLArrayIndex_t)(TxCharTmpVariable >> 5) & SIGLIB_OPSK_BIT_MASK;
          TxSourceWordPhase = 1;
          break;
        case 1:
          TxTriBit = (SLArrayIndex_t)(TxCharTmpVariable >> 2) & SIGLIB_OPSK_BIT_MASK;
          TxPreviousTxCharBits = TxCharTmpVariable;
          TxSourceWordPhase = 2;
          break;
        case 2:
          TxCharTmpVariable = TxString[TxStringIndex++];
          TxTriBit = (SLArrayIndex_t)(((TxPreviousTxCharBits << 1) & 0x6) + ((TxCharTmpVariable >> 7) & 0x1));
          TxSourceWordPhase = 3;
          break;
        case 3:
          TxTriBit = (SLArrayIndex_t)(TxCharTmpVariable >> 4) & SIGLIB_OPSK_BIT_MASK;
          TxSourceWordPhase = 4;
          break;
        case 4:
          TxTriBit = (SLArrayIndex_t)(TxCharTmpVariable >> 1) & SIGLIB_OPSK_BIT_MASK;
          TxPreviousTxCharBits = TxCharTmpVariable;
          TxSourceWordPhase = 5;
          break;
        case 5:
          TxCharTmpVariable = TxString[TxStringIndex++];
          TxTriBit = (SLArrayIndex_t)(((TxPreviousTxCharBits << 2) & 0x4) + ((TxCharTmpVariable >> 6) & 0x3));
          TxSourceWordPhase = 6;
          break;
        case 6:
          TxTriBit = (SLArrayIndex_t)(TxCharTmpVariable >> 3) & SIGLIB_OPSK_BIT_MASK;
          TxSourceWordPhase = 7;
          break;
        case 7:
          TxTriBit = (SLArrayIndex_t)TxCharTmpVariable & SIGLIB_OPSK_BIT_MASK;
          TxSourceWordPhase = 0;
          break;
        }

#if (DIFFERENTIAL_ENCODING_ENABLE)
#  if DEBUG_LOG_FILE
        SUF_Debugfprintf("TxTriBit: 0x%x\n", (int)TxTriBit);
#  endif

        TxTriBit = SDS_DifferentialEncode(TxTriBit,                       // Tx tri-bit
                                          pDifferentialEncoderMap,        // Encoder LUT
                                          SIGLIB_OPSK_BITS_PER_SYMBOL,    // Bits per symbol
                                          SIGLIB_OPSK_BIT_MASK,           // Bit mask
                                          &PreviousTxTriBit);             // Previously transmitted word
#endif
      }

#if DEBUG_LOG_FILE
      SUF_Debugfprintf("Tx Modulate TriBit: 0x%x\n", (int)TxTriBit);
#endif

      SDA_OpskModulate(TxTriBit,                                 // Source data tri-bit
                       ModulatedSignal + (i * SYMBOL_LENGTH),    // Destination array
                       pTxCarrierTable,                          // Carrier table pointer
                       TX_CARRIER_TABLE_SIZE,                    // Carrier sine table size
                       &txCarrierPhase,                          // Carrier phase pointer
                       &TxSampleClock,                           // Sample clock pointer
                       &TxMagn,                                  // Magnitude pointer
                       TX_CARRIER_TABLE_INCREMENT,               // Carrier table increment
                       SYMBOL_LENGTH,                            // Samples per symbol
                       TxIRRCState,                              // RRCF Tx I delay pointer
                       &TxRRCFIFilterIndex,                      // RRCF Tx I Filter Index pointer
                       TxQRRCState,                              // RRCF Tx Q delay pointer
                       &TxRRCFQFilterIndex,                      // RRCF Tx Q Filter Index pointer
                       RRCFCoeffs,                               // RRCF Coefficients pointer
                       RRCF_LENGTH,                              // RRCF size
                       RRCF_ENABLE);                             // RRCF enable / disable switch
    }

#if DISPLAY_TIME_DOMAIN
    gpc_plot_2d(h2DPlot,                                                                                                      // Graph handle
                ModulatedSignal,                                                                                              // Dataset
                SAMPLE_LENGTH,                                                                                                // Dataset length
                "Modulated Signal",                                                                                           // Dataset title
                ((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount,                                                    // Minimum X value
                (((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount) + ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE),    // Maximum X value
                "lines",                                                                                                      // Graph type
                "blue",                                                                                                       // Colour
                GPC_NEW);                                                                                                     // New graph
    TimeIndex += (SLData_t)SAMPLE_LENGTH / SAMPLE_RATE;
    printf("\nModulated Signal\nPlease hit <Carriage Return> to continue . . .");
    getchar();
#endif

#if DISPLAY_FREQ_DOMAIN
    {
      SLError_t SigLibErrorCode;
      SigLibErrorCode = plot_frequency_domain(ModulatedSignal, SIGLIB_FLAT_TOP_FOURIER, "Output Frequency Spectrum", SAMPLE_LENGTH, SAMPLE_LENGTH);
      if (SigLibErrorCode != SIGLIB_NO_ERROR) {
        printf("SigLib Error Message:%s\n", SUF_StrError(SigLibErrorCode));
        exit(0);
      }
      printf("Please hit <Carriage Return> to continue . . .\n");
      getchar();
    }
#endif

    // Inject some noise into the signal
    GaussianNoisePhase = SIGLIB_ZERO;
    SDA_SignalGenerate(ModulatedSignal,            // Pointer to destination array
                       SIGLIB_GAUSSIAN_NOISE,      // Signal type - Gaussian noise
                       SIGLIB_ZERO,                // Signal peak level - Unused
                       SIGLIB_ADD,                 // Fill (overwrite) or add to existing array contents
                       SIGLIB_ZERO,                // Signal frequency - Unused
                       GAUSSIAN_NOISE_OFFSET,      // D.C. Offset
                       GAUSSIAN_NOISE_VARIANCE,    // Gaussian noise variance
                       SIGLIB_ZERO,                // Signal end value - Unused
                       &GaussianNoisePhase,        // Pointer to gaussian signal phase - should be
                                                   // initialised to zero
                       &GaussianNoiseValue,        // Gaussian signal second sample - should be
                                                   // initialised to zero
                       SAMPLE_LENGTH);             // Output dataset length

// Receiver - De-modulate new sample
#if (DISPLAY_TRIGGER) || (DISPLAY_EYE_DIAGRAM)
    RxSymbolCount = SDA_CostasQamDemodulateDebug(ModulatedSignal,                         // Source data pointer
                                                 pRealOutput,                             // Real destination data pointer
                                                 pImagOutput,                             // Imaginary destination data pointer
                                                 &CostasLpVCOPhase,                       // VCO phase
                                                 COSTAS_LP_VCO_MODULATION_INDEX,          // VCO modulation index
                                                 pCostasLpVCOLookUpTable,                 // VCO look up table
                                                 COSTAS_LP_VCO_TABLE_SIZE,                // VCO look up table size
                                                 CARRIER_FREQ / SAMPLE_RATE,              // Carrier frequency
                                                 pCostasLpLPF1State,                      // Pointer to loop filter 1 state
                                                 &CostasLpLPF1Index,                      // Pointer to loop filter 1 index
                                                 pCostasLpLPF2State,                      // Pointer to loop filter 2 state
                                                 &CostasLpLPF2Index,                      // Pointer to loop filter 2 index
                                                 pCostasLpLPFCoeffs,                      // Pointer to loop filter coefficients
                                                 COSTAS_LP_LPF_LENGTH,                    // Loop filter length
                                                 &CostasLpLoopFilterState,                // Pointer to loop filter state
                                                 COSTAS_LP_LOOP_FILTER_ALPHA,             // Loop filter coefficient
                                                 SIGLIB_COSTAS_LOOP_HARD_LIMITED_LOOP,    // Loop feedback mode
                                                 &CostasLpState,                          // Pointer to delayed sample
                                                 pELGMatchedFilterState,                  // Pointer to matched filter state array
                                                 &ELGMatchedFilterIndex,                  // Pointer to matched filter index
                                                 &ELGMatchedFilterSum,                    // Pointer to matched filter sum
                                                 pELGEarlyGateDelay,                      // Pointer to early gate state array
                                                 &ELGEarlyGateDelayIndex,                 // Pointer to early gate delay index
                                                 ELG_EARLY_GATE_DELAY_LENGTH,             // Early gate delay length
                                                 pELGLoopFilterState,                     // Pointer to loop filter state array
                                                 pELGLoopFilterCoeffs,                    // Pointer to loop filter coefficients
                                                 &ELGLoopFilterIndex,                     // Pointer to loop filter index
                                                 ELG_LOOP_FILTER_LENGTH,                  // Loop filter length
                                                 ELG_PULSE_DETECTOR_THRESHOLD,            // Noise threshold
                                                 &ELGPulseDetectorThresholdFlag,          // Pointer to pulse detector threshold
                                                                                          // flag
                                                 &ELGZeroCrossingPreviousSample,          // Pointer to zero crossing previous
                                                                                          // sample
                                                 &ELGTriggerCount,                        // Pointer to trigger counter
                                                 &ELGTriggerDetectedFlag,                 // Pointer to trigger detected flag
                                                 &ELGTriggerUpdatedFlag,                  // Pointer to trigger updated flag
                                                 SYMBOL_LENGTH,                           // Samples per symbol
                                                 pELGRealOutputSynchDelay,                // Pointer to ELG real output synchronization
                                                                                          // delay state array
                                                 pELGImagOutputSynchDelay,                // Pointer to ELG imaginary output
                                                                                          // synchronization delay state array
                                                 &ELGOutputSynchDelayIndex,               // Pointer to ELG synchronization delay index
                                                 ELGTriggerLatency,                       // ELG output synchronization delay length
                                                 SAMPLE_LENGTH,                           // Source dataset length
                                                 DebugRealFilterOutput,                   // Pointer to debug real filter output
                                                 DebugImagFilterOutput,                   // Pointer to debug imaginary filter output
                                                 DebugELGTriggerOutput);                  // Pointer to debug ELG trigger output
#else
    RxSymbolCount = SDA_CostasQamDemodulate(ModulatedSignal,                         // Source data pointer
                                            pRealOutput,                             // Real destination data pointer
                                            pImagOutput,                             // Imaginary destination data pointer
                                            &CostasLpVCOPhase,                       // VCO phase
                                            COSTAS_LP_VCO_MODULATION_INDEX,          // VCO modulation index
                                            pCostasLpVCOLookUpTable,                 // VCO look up table
                                            COSTAS_LP_VCO_TABLE_SIZE,                // VCO look up table size
                                            CARRIER_FREQ / SAMPLE_RATE,              // Carrier frequency
                                            pCostasLpLPF1State,                      // Pointer to loop filter 1 state
                                            &CostasLpLPF1Index,                      // Pointer to loop filter 1 index
                                            pCostasLpLPF2State,                      // Pointer to loop filter 2 state
                                            &CostasLpLPF2Index,                      // Pointer to loop filter 2 index
                                            pCostasLpLPFCoeffs,                      // Pointer to loop filter coefficients
                                            COSTAS_LP_LPF_LENGTH,                    // Loop filter length
                                            &CostasLpLoopFilterState,                // Pointer to loop filter state
                                            COSTAS_LP_LOOP_FILTER_ALPHA,             // Loop filter coefficient
                                            SIGLIB_COSTAS_LOOP_HARD_LIMITED_LOOP,    // Loop feedback mode
                                            &CostasLpState,                          // Pointer to delayed sample
                                            pELGMatchedFilterState,                  // Pointer to matched filter state array
                                            &ELGMatchedFilterIndex,                  // Pointer to matched filter index
                                            &ELGMatchedFilterSum,                    // Pointer to matched filter sum
                                            pELGEarlyGateDelay,                      // Pointer to early gate state array
                                            &ELGEarlyGateDelayIndex,                 // Pointer to early gate delay index
                                            ELG_EARLY_GATE_DELAY_LENGTH,             // Early gate delay length
                                            pELGLoopFilterState,                     // Pointer to loop filter state array
                                            pELGLoopFilterCoeffs,                    // Pointer to loop filter coefficients
                                            &ELGLoopFilterIndex,                     // Pointer to loop filter index
                                            ELG_LOOP_FILTER_LENGTH,                  // Loop filter length
                                            ELG_PULSE_DETECTOR_THRESHOLD,            // Noise threshold
                                            &ELGPulseDetectorThresholdFlag,          // Pointer to pulse detector threshold
                                                                                     // flag
                                            &ELGZeroCrossingPreviousSample,          // Pointer to zero crossing previous
                                                                                     // sample
                                            &ELGTriggerCount,                        // Pointer to trigger counter
                                            &ELGTriggerDetectedFlag,                 // Pointer to trigger detected flag
                                            &ELGTriggerUpdatedFlag,                  // Pointer to trigger updated flag
                                            SYMBOL_LENGTH,                           // Samples per symbol
                                            pELGRealOutputSynchDelay,                // Pointer to ELG real output synchronization
                                                                                     // delay state array
                                            pELGImagOutputSynchDelay,                // Pointer to ELG imaginary output
                                                                                     // synchronization delay state array
                                            &ELGOutputSynchDelayIndex,               // Pointer to ELG synchronization delay index
                                            ELGTriggerLatency,                       // ELG output synchronization delay length
                                            SAMPLE_LENGTH);                          // Source dataset length
#endif

    // Scale results
    SDA_Multiply(pRealOutput,                // Source array
                 SIGLIB_OPSK_DEMOD_SCALE,    // Multiplier
                 pRealOutput,                // Destination array
                 RxSymbolCount);             // Array length
    SDA_Multiply(pImagOutput,                // Source array
                 SIGLIB_OPSK_DEMOD_SCALE,    // Multiplier
                 pImagOutput,                // Destination array
                 RxSymbolCount);             // Array length

#if DEBUG_LOG_FILE
    SUF_Debugfprintf("RxSymbolCount: %d\n", RxSymbolCount);
#endif

#if DISPLAY_TRIGGER
    gpc_plot_2d(h2DPlot,                                                                                                      // Graph handle
                DebugRealFilterOutput,                                                                                        // Dataset
                SAMPLE_LENGTH,                                                                                                // Dataset length
                "Real Filter Output",                                                                                         // Dataset title
                ((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount,                                                    // Minimum X value
                (((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount) + ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE),    // Maximum X value
                "lines",                                                                                                      // Graph type
                "blue",                                                                                                       // Colour
                GPC_NEW);                                                                                                     // New graph

    gpc_plot_2d(h2DPlot,                                                                                                      // Graph handle
                DebugImagFilterOutput,                                                                                        // Dataset
                SAMPLE_LENGTH,                                                                                                // Dataset length
                "Imaginary Filter Output",                                                                                    // Dataset title
                ((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount,                                                    // Minimum X value
                (((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount) + ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE),    // Maximum X value
                "lines",                                                                                                      // Graph type
                "red",                                                                                                        // Colour
                GPC_ADD);                                                                                                     // New graph

    gpc_plot_2d(h2DPlot,                                                                                                      // Graph handle
                DebugELGTriggerOutput,                                                                                        // Dataset
                SAMPLE_LENGTH,                                                                                                // Dataset length
                "Early-Late-Gate Trigger",                                                                                    // Dataset title
                ((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount,                                                    // Minimum X value
                (((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount) + ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE),    // Maximum X value
                "lines",                                                                                                      // Graph type
                "green",                                                                                                      // Colour
                GPC_ADD);                                                                                                     // New graph
    printf("\nFilter Outputs And Early-Late-Gate Trigger\nPlease hit <Carriage "
           "Return> to continue . . .");
    getchar();
#endif

#if DISPLAY_CONSTELLATION
    SDA_CreateComplexRect(pRealOutput,            // In-phase constellation points
                          pImagOutput,            // Quadrature-phase constellation points
                          ConstellationPoints,    // Complex constellation points
                          RxSymbolCount);         // Array Length

    if (FirstConstellationDiagramFlag) {
      gpc_plot_xy(hConstellationDiagram,                  // Graph handle
                  (ComplexRect_s*)ConstellationPoints,    // Array of complex dataset
                  (int)RxSymbolCount,                     // Dataset length
                  "Constellation Diagram",                // Dataset title
                  "lines",                                // Graph type
                  "blue",                                 // Colour
                  GPC_NEW);                               // New graph

      FirstConstellationDiagramFlag = 0;
    } else {
      gpc_plot_xy(hConstellationDiagram,                  // Graph handle
                  (ComplexRect_s*)ConstellationPoints,    // Array of complex dataset
                  (int)RxSymbolCount,                     // Dataset length
                  "Constellation Diagram",                // Dataset title
                  "lines",                                // Graph type
                  "blue",                                 // Colour
                  GPC_ADD);                               // New graph
    }
#endif

    for (SLArrayIndex_t i = 0; i < RxSymbolCount; i++) {    // Scale and decode the receive symbols

      // #if DEBUG_LOG_FILE
      //       SUF_Debugfprintf ("Symbol position: ");
      //       SUF_DebugPrintComplex (pRealOutput[i], pImagOutput[i]);
      // #endif

      // Decode constellation - Find nearest constellation point
      for (int j = 0; j < SIGLIB_OPSK_NUMBER_OF_PHASES; j++) {
        RxDemodErrorArray[j] = SDS_EuclideanDistanceSquared(siglib_numerix_OPSKRxConstellation[j].real, siglib_numerix_OPSKRxConstellation[j].imag,
                                                            pRealOutput[i], pImagOutput[i]);
      }

      RxTriBit = SDA_AbsMinIndex(RxDemodErrorArray, SIGLIB_OPSK_NUMBER_OF_PHASES);

#if (DIFFERENTIAL_ENCODING_ENABLE)
#  if DEBUG_LOG_FILE
      SUF_Debugfprintf("Rx Demodulate TriBit: 0x%x\n", (int)RxTriBit);
#  endif

      RxTriBit = SDS_DifferentialDecode(RxTriBit,                       // Rx tri-bit
                                        pDifferentialDecoderMap,        // Encoder LUT
                                        SIGLIB_OPSK_BITS_PER_SYMBOL,    // Bits per symbol
                                        SIGLIB_OPSK_BIT_MASK,           // Bit mask
                                        &PreviousRxTriBit);             // Previously received word
#endif

#if DEBUG_LOG_FILE
      SUF_Debugfprintf("RxTriBit: 0x%x\n", (int)RxTriBit);
#endif

      if (RxStartUpDelayCount < RX_STARTUP_DELAY) {    // Flush pipeline before saving data
        RxStartUpDelayCount++;
      } else {
        switch (RxSourceWordPhase) {
        case 0:
          RxCharTmpVariable = (RxTriBit & SIGLIB_OPSK_BIT_MASK) << 5;
          RxSourceWordPhase = 1;
          break;
        case 1:
          RxCharTmpVariable += (RxTriBit & SIGLIB_OPSK_BIT_MASK) << 2;
          RxSourceWordPhase = 2;
          break;
        case 2:
          RxCharTmpVariable += (RxTriBit & SIGLIB_OPSK_BIT_MASK) >> 1;
          RxString[RxStringIndex++] = RxCharTmpVariable;
          RxPreviousRxCharBits = RxTriBit & 0x1;
          RxSourceWordPhase = 3;
          break;
        case 3:
          RxCharTmpVariable = (char)((RxPreviousRxCharBits << 7) + ((RxTriBit & SIGLIB_OPSK_BIT_MASK) << 4));
          RxSourceWordPhase = 4;
          break;
        case 4:
          RxCharTmpVariable += (char)((RxTriBit & SIGLIB_OPSK_BIT_MASK) << 1);
          RxSourceWordPhase = 5;
          break;
        case 5:
          RxCharTmpVariable += (char)((RxTriBit & SIGLIB_OPSK_BIT_MASK) >> 2);
          RxString[RxStringIndex++] = RxCharTmpVariable;
          RxPreviousRxCharBits = RxTriBit & 0x3;
          RxSourceWordPhase = 6;
          break;
        case 6:
          RxCharTmpVariable = (char)((RxPreviousRxCharBits << 6) + ((RxTriBit & SIGLIB_OPSK_BIT_MASK) << 3));
          RxSourceWordPhase = 7;
          break;
        case 7:
          RxString[RxStringIndex++] = (char)(RxCharTmpVariable + RxTriBit);
          RxSourceWordPhase = 0;
          break;
        }
      }
    }

#if DISPLAY_EYE_DIAGRAM
    for (SLArrayIndex_t i = 0; i < SAMPLE_LENGTH; i += EYE_DIAGRAM_SIZE) {
      if (FirstEyeDiagramFlag == 1) {
        gpc_plot_2d(h2DPlot,                                                                                                      // Graph handle
                    DebugRealFilterOutput + i,                                                                                    // Dataset
                    EYE_DIAGRAM_SIZE,                                                                                             // Dataset length
                    "Real data",                                                                                                  // Dataset title
                    ((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount,                                                    // Minimum X value
                    (((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount) + ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE),    // Maximum X value
                    "lines",                                                                                                      // Graph type
                    "blue",                                                                                                       // Colour
                    GPC_NEW);                                                                                                     // New graph
        gpc_plot_2d(h2DPlot,                                                                                                      // Graph handle
                    DebugImagFilterOutput + i,                                                                                    // Dataset
                    EYE_DIAGRAM_SIZE,                                                                                             // Dataset length
                    "Imaginary data",                                                                                             // Dataset title
                    ((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount,                                                    // Minimum X value
                    (((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount) + ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE),    // Maximum X value
                    "lines",                                                                                                      // Graph type
                    "red",                                                                                                        // Colour
                    GPC_NEW);                                                                                                     // New graph
        FirstEyeDiagramFlag = 0;
      } else {
        gpc_plot_2d(h2DPlot,                                                                                                      // Graph handle
                    DebugRealFilterOutput + i,                                                                                    // Dataset
                    EYE_DIAGRAM_SIZE,                                                                                             // Dataset length
                    "Real data",                                                                                                  // Dataset title
                    ((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount,                                                    // Minimum X value
                    (((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount) + ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE),    // Maximum X value
                    "lines",                                                                                                      // Graph type
                    "blue",                                                                                                       // Colour
                    GPC_ADD);                                                                                                     // New graph
        gpc_plot_2d(h2DPlot,                                                                                                      // Graph handle
                    DebugImagFilterOutput + i,                                                                                    // Dataset
                    EYE_DIAGRAM_SIZE,                                                                                             // Dataset length
                    "Imaginary data",                                                                                             // Dataset title
                    ((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount,                                                    // Minimum X value
                    (((double)SAMPLE_LENGTH / SAMPLE_RATE) * (double)LoopCount) + ((double)(SAMPLE_LENGTH - 1) / SAMPLE_RATE),    // Maximum X value
                    "lines",                                                                                                      // Graph type
                    "red",                                                                                                        // Colour
                    GPC_ADD);                                                                                                     // New graph
      }
    }
#endif
  }

#if DISPLAY_EYE_DIAGRAM
  printf("Eye diagram\n");
#endif

  printf("\nReceived string length: %d\n", (int)RxStringIndex);
#if TX_ALPHABET
  RxString[RxStringIndex] = 0;    // Terminate string for printf
  printf("Received string: %s", RxString);
#else
  for (SLArrayIndex_t i = 0; i < RxStringIndex; i++) {
    printf("0x%x, ", (int)RxString[i]);
  }
#endif

#if DEBUG_LOG_FILE
  for (SLArrayIndex_t i = 0; i < RxStringIndex; i++) {
    SUF_Debugfprintf("RxString[%d]", i);
    dpchar(RxString[i]);
  }
#endif

  printf("\n");
  printf("Bit Error Rate = %lf\n",
         SDA_BitErrorRate((SLChar_t*)TxString,                                                  // Source 1 pointer
                          (SLChar_t*)RxString,                                                  // Source 2 pointer
                          SIGLIB_ONE / (SLData_t)((RxStringIndex - 1) * SIGLIB_BYTE_LENGTH),    // Inverse of number of bits
                          RxStringIndex - 1));                                                  // Dataset length (ignore last character)

#if DISPLAY_TIME_DOMAIN || DISPLAY_FREQ_DOMAIN || DISPLAY_EYE_DIAGRAM || DISPLAY_CONSTELLATION
  printf("\nHit <Carriage Return> to continue ....\n");
  getchar();    // Wait for <Carriage Return>
#endif
#if DISPLAY_TIME_DOMAIN || DISPLAY_FREQ_DOMAIN || DISPLAY_EYE_DIAGRAM
  gpc_close(h2DPlot);
#endif
#if DISPLAY_CONSTELLATION
  gpc_close(hConstellationDiagram);
#endif

  SUF_MemoryFree(pTxCarrierTable);    // Free memory
#if (DIFFERENTIAL_ENCODING_ENABLE)
  SUF_MemoryFree(pDifferentialEncoderMap);
  SUF_MemoryFree(pDifferentialDecoderMap);
#endif
  SUF_MemoryFree(pCostasLpLPFCoeffs);
  SUF_MemoryFree(pCostasLpLPF1State);
  SUF_MemoryFree(pCostasLpLPF2State);
  SUF_MemoryFree(pCostasLpVCOLookUpTable);

  return (0);
}
