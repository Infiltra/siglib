// SigLib - Band pass filter length estimation functions
// Copyright (c) 2023 Delta Numerix All rights reserved.

#include <siglib.h>    // SigLib DSP library
#include <stdio.h>

int main(void)
{

  printf("est (10k, 1k, 10, 100) = %d\n\n", SUF_EstimateBPFirFilterLength(10000.0, 1000.0, 10, 100));
  printf("est (10k, 1k001, 10, 100) = %d\n\n", SUF_EstimateBPFirFilterLength(10000.0, 1001.0, 10, 100));
  printf("est (6k, 1k, 10, 20) = %d\n\n", SUF_EstimateBPFirFilterLength(6000.0, 1000.0, 10, 20));
  printf("est (8k, 1k, 10, 20) = %d\n\n", SUF_EstimateBPFirFilterLength(8000.0, 1000.0, 10, 20));
  printf("est (3k, 1k, 10, 20) = %d\n\n", SUF_EstimateBPFirFilterLength(3000.0, 1000.0, 10, 20));
  printf("est (4k, 1k, 10, 20) = %d\n\n", SUF_EstimateBPFirFilterLength(4000.0, 1000.0, 10, 20));
  printf("est (10k, 1k, 8, 12) = %d\n\n", SUF_EstimateBPFirFilterLength(10000.0, 1000.0, 8, 12));

  return (0);
}
