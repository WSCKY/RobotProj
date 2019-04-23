#ifndef __NAV_TASK_H
#define __NAV_TASK_H

#include "MAX_M8Q.h"

#define NAV_TASK_PRIORITY              osPriorityNormal
#define NAV_TASK_STACK_SIZE            configMINIMAL_STACK_SIZE

void NAV_Thread(void const *argument);

#endif /* __NAV_TASK_H */
