/*
 * response.h
 *
 *  Created on: 2024. 7. 11.
 *      Author: homedory
 */

#ifndef RESPONSE_H_
#define RESPONSE_H_

typedef struct {
    int response_data;
    int response_ready;
} Response;

extern Response move_motor_response;
extern Response display_text_response;
extern Response terminate_rpc_response;

#endif /* RESPONSE_H_ */
