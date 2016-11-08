/*
 * definitions.h
 *
 *  Created on: Nov 1, 2016
 *      Author: Matheus
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_


#define UNIT_ID 2

#define SERVER_IP 	String("10.0.0.218")
#define SERVER_PORT 8080
#define SERVER_PATH	"/sample"

#define AP_SSID "The Pod Wifi Free"
#define AP_PW	""


#define NUM_AVG_PEAKS 3
#define VOLT_REF 212
#define INPUT_GAIN 1.63


#define FREQ_SAMPLE 1000
#define DELAY_MS_SAMPLES 1


#define MEAN_PW_INTERVAL_S 3


#define SEND_THRESHOLD (FREQ_SAMPLE * MEAN_PW_INTERVAL_S)


class ESP8266; // Forward implementation
extern ESP8266 esp;

#endif /* DEFINITIONS_H_ */
