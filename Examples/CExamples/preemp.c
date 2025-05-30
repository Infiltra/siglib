// SigLib - Pre-emphasis / De-emphasis Filter Example
// Copyright (c) 2024 Delta Numerix All rights reserved.

// Include files
#include <stdio.h>
#include <stdlib.h>
#include <siglib.h>             // SigLib DSP library
#include <gnuplot_c.h>          // Gnuplot/C
#include "plot_fd/plot_fd.h"    // Frequency domain plots

// Define constants
#define SAMPLE_LENGTH 256
#define FILTER_COEFFICIENT 0.95    // Filter coefficient

// Declare global variables and arrays

int main(void)
{
  h_GPC_Plot* h2DPlot;    // Plot object

  SLData_t PreEmphasisFilterState, DeEmphasisFilterState;

  SLData_t* pData = SUF_VectorArrayAllocate(SAMPLE_LENGTH);

  h2DPlot =                                               // Initialize plot
      gpc_init_2d("Pre-emphasis / De-emphasis Filter",    // Plot title
                  "Time",                                 // X-Axis label
                  "Magnitude",                            // Y-Axis label
                  GPC_AUTO_SCALE,                         // Scaling mode
                  GPC_SIGNED,                             // Sign mode
                  GPC_KEY_ENABLE);                        // Legend / key mode
  if (NULL == h2DPlot) {
    printf("\nPlot creation failure.\n");
    exit(-1);
  }

  SDA_Impulse(pData,             // Pointer to destination array
              SAMPLE_LENGTH);    // Output dataset length

  SIF_PreEmphasisFilter(&PreEmphasisFilterState);    // Pointer to filter state
  SDA_PreEmphasisFilter(pData,                       // Pointer to source array
                        pData,                       // Pointer to destination array
                        FILTER_COEFFICIENT,          // Filter coefficient
                        &PreEmphasisFilterState,     // Pointer to filter state
                        SAMPLE_LENGTH);              // Dataset length

  plot_frequency_domain(pData, SIGLIB_FLAT_TOP_FOURIER, "Pre-emphasis Filter Response", SAMPLE_LENGTH, SAMPLE_LENGTH);
  printf("Please hit <Carriage Return> to continue . . .\n");
  getchar();

  SIF_DeEmphasisFilter(&DeEmphasisFilterState);    // Pointer to filter state
  SDA_DeEmphasisFilter(pData,                      // Pointer to source array
                       pData,                      // Pointer to destination array
                       FILTER_COEFFICIENT,         // Filter coefficient
                       &DeEmphasisFilterState,     // Pointer to filter state
                       SAMPLE_LENGTH);             // Dataset length

  gpc_plot_2d(h2DPlot,                                   // Graph handle
              pData,                                     // Dataset
              SAMPLE_LENGTH,                             // Dataset length
              "Time Domain Combined Filter Response",    // Dataset title
              SIGLIB_ZERO,                               // Minimum X value
              (double)(SAMPLE_LENGTH - 1),               // Maximum X value
              "lines",                                   // Graph type
              "blue",                                    // Colour
              GPC_NEW);                                  // New graph
  printf("\nTime Domain Combined Filter Response\nPlease hit <Carriage Return> "
         "to continue . . .");
  getchar();

  plot_frequency_domain(pData, SIGLIB_FLAT_TOP_FOURIER, "Combined Filter Response", SAMPLE_LENGTH, SAMPLE_LENGTH);

  printf("\nHit <Carriage Return> to continue ....\n");
  getchar();    // Wait for <Carriage Return>
  gpc_close(h2DPlot);

  SUF_MemoryFree(pData);    // Free memory

  return (0);
}
