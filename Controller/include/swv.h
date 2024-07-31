/*
 * swv.h
 *
 *  Created on: 2024. 7. 15.
 *      Author: homedory
 *
 *  Header file for debugging purposes using Serial Wire Viewer (SWV).
 *  This file declares the swv_write function used for SWV-based debugging.
 */

#ifndef SWV_H_
#define SWV_H_

int swv_write(char *ptr, int len);

#endif /* SWV_H_ */
