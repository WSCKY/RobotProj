/*
 * compass.h
 *
 *  Created on: Jan 8, 2020
 *      Author: kychu
 */

#ifndef __COMPASS_H
#define __COMPASS_H

#include "drivers.h"

#include "algorithm.h"

void magnetics_task(void const *argument);

int magnetics_take(Vector3D *data);

#endif /* __COMPASS_H */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/

