/*
 * time.h
 *
 *  Created on: 23 Mar 2018
 *      Author: peter
 */

#ifndef TIME_H_
#define TIME_H_

#include <stdint.h>

void time_init();
uint32_t time_now();

static inline int64_t time_diff(uint32_t timestamp)
{
	return (int64_t)time_now() - (int64_t)timestamp;
}

static inline int time_elapsed(uint32_t timestamp)
{
	return time_diff(timestamp) >= 0;
}

static inline uint32_t time_getOffsetTimestamp(uint32_t origin, uint32_t offset)
{
	return origin + offset;
}

static inline uint32_t time_getOffset(uint32_t offset)
{
	return time_getOffsetTimestamp(time_now(), offset);
}

static inline int time_elapsedOffset(uint32_t timestamp, uint32_t offset)
{
	return time_elapsed(time_getOffsetTimestamp(timestamp,offset));
}

#endif /* TIME_H_ */
