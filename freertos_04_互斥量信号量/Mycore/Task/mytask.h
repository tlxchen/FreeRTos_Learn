#ifndef __MYTASK_H__
#define __MYTASK_H__
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "oled.h"

void start_task(void* arg);
#endif
