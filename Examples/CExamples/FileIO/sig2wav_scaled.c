// .SIG to .WAV file conversion program
// Use when the signal data magnitude is +/- 1.0 and the .wav magnitude is +/-
// 32767.0 Copyright (c) 2024 Delta Numerix All rights reserved.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <siglib.h>    // SigLib DSP library

#define SAMPLE_SIZE 128

SLWavFileInfo_s wavInfo;

int main(int argc, char** argv)
{
  SLArrayIndex_t inputSampleCount;
  SLArrayIndex_t outputSampleCount = 0;
  FILE *fpInputFile, *fpOutputFile;

  char SigFileName[256];
  char WavFileName[256];

  if (argc != 2) {
    printf("\nUsage:\nsig2wav_scaled filename (no extension)\n\n");
    exit(-1);    // Exit - usage error
  }

  if (argc != 3) {
    printf("Useage: sig2wav filename (no extension) samplerate\nSample rates: "
           "8000, 22050, 44100 etc.");
    exit(-1);
  }

  strcpy(SigFileName, argv[1]);
  strcat(SigFileName, ".dat");

  strcpy(WavFileName, argv[1]);
  strcat(WavFileName, ".wav");

  printf("dat filename: %s\n", SigFileName);
  printf("Wav filename: %s\n", WavFileName);

  SLData_t* pDataArray = SUF_VectorArrayAllocate(SAMPLE_SIZE);

  if ((fpInputFile = fopen(SigFileName, "rb")) == NULL) {    // Note this file is binary
    printf("Error opening input file %s\n", SigFileName);
    exit(-1);
  }

  if ((fpOutputFile = fopen(WavFileName, "wb")) == NULL) {    // Note this file is binary
    printf("Error opening output file %s\n", WavFileName);
    fclose(fpInputFile);
    exit(-1);
  }

  wavInfo = SUF_WavSetInfo(atol(argv[2]), 0, 1, 16, 2, 1);
  //  wavInfo = SUF_WavSetInfo (8000, 0, 1, 16, 2, 1);
  //  wavInfo = SUF_WavSetInfo (21050, 0, 1, 16, 2, 1);

  SUF_WavWriteHeader(fpOutputFile,
                     wavInfo);    // Write dummy header to output file

  while ((inputSampleCount = SUF_SigReadData(pDataArray, fpInputFile, SAMPLE_SIZE)) != 0) {    // Successively read arrays of upto 128 samples
    for (int i = 0; i < SAMPLE_SIZE; ++i) {
      pDataArray[i] *= 32767.;
    }
    outputSampleCount += inputSampleCount;
    SUF_WavWriteData(pDataArray, fpOutputFile, wavInfo, inputSampleCount);
  }
  // Write last block of data
  printf("Total number of samples read: %d\n", outputSampleCount);

  wavInfo.NumberOfSamples = outputSampleCount;    // Set total data length
  rewind(fpOutputFile);                           // Rewind pointer to start of file
  SUF_WavWriteHeader(fpOutputFile, wavInfo);      // Overwrite the header information

  fclose(fpInputFile);
  fclose(fpOutputFile);

  free(pDataArray);    // Free memory

  return (0);
}
