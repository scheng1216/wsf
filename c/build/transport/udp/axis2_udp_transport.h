/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AXIS2_UDP_TRANSPORT_H
#define AXIS2_UDP_TRANSPORT_H

#include <axutil_utils.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Default values */
#define AXIS2_UDP_TRANSPORT_UNI_REPEAT		2
#define AXIS2_UDP_TRANSPORT_UNI_MIN_DELAY	50
#define AXIS2_UDP_TRANSPORT_UNI_MAX_DELAY	250
#define AXIS2_UDP_TRANSPORT_UNI_UPPER_DELAY	500

#define AXIS2_UDP_TRANSPORT_MUL_REPEAT		4
#define AXIS2_UDP_TRANSPORT_MUL_MIN_DELAY	50
#define AXIS2_UDP_TRANSPORT_MUL_MAX_DELAY	250
#define AXIS2_UDP_TRANSPORT_MUL_UPPER_DELAY	500

/* Following constants are used to represent the values in the axis2 configuration */
#define AXIS2_UDP_TRANSPORT_UNI_REPEAT_STR		"uniRepeat"
#define AXIS2_UDP_TRANSPORT_UNI_MIN_DELAY_STR	"uniMinDelay"
#define AXIS2_UDP_TRANSPORT_UNI_MAX_DELAY_STR	"uniMaxDelay"
#define AXIS2_UDP_TRANSPORT_UNI_UPPER_DELAY_STR	"uniUpperDelay"

#define AXIS2_UDP_TRANSPORT_MUL_REPEAT_STR		"mulRepeat"
#define AXIS2_UDP_TRANSPORT_MUL_MIN_DELAY_STR	"mulMinDelay"
#define AXIS2_UDP_TRANSPORT_MUL_MAX_DELAY_STR	"mulMaxDelay"
#define AXIS2_UDP_TRANSPORT_MUL_UPPER_DELAY_STR	"mulUpperDelay"

#define AXIS2_UDP_TRANSPORT_MULTICAST			"multicast"

#define AXIS2_PORT_STRING "port"

#define AXIS2_UDP_PACKET_MAX_SIZE		16384
#define AXIS2_UDP_TRANSPORT_DEFAULT_RECV_PORT	13000


typedef struct axis2_udp_transport_params
{
	int udp_repeat;
	int udp_min_delay;
	int udp_max_delay;
	int udp_upper_delay;
} axis2_udp_transport_params_t;

/* properties */
/* Property value: Integer */
#define AXIS2_UDP_TRANSPORT_RECEIVER_PORT		"AXIS2_UDP_TRANSPORT_RECEIVER_PORT"
/* Property value: A string */
#define AXIS2_UDP_TRANSPORT_RECEIVER_ADDRESS	"AXIS2_UDP_TRANSPORT_RECEIVER_ADDRESS"
/* Property value: a integer */
#define AXIS2_UDP_TRANSPORT_IS_MULTICAST		"AXIS2_UDP_TRANPORT_IS_MULTICAST"

#define AXIS2_UDP_TRANSPORT_MULTICAST_GROUP		"multicastGroup"

#ifdef __cpluspluss
}
#endif

#endif   