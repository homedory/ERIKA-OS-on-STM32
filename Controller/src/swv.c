/*
 * swv.c
 *
 *  Created on: 2024. 7. 15.
 *      Author: homedory
 *
 *   Source code for debugging purposes using Serial Wire Viewer (SWV).
 *   This file contains the implementation of the swv_write function
 */

#include "misc.h"
#include "swv.h"

int swv_write(char *ptr, int len)
{
  int DataIdx;
  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    ITM_SendChar(*ptr++);
  }

  return 0;
}
