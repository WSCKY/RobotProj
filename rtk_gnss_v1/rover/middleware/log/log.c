/*
 * debug.c
 *
 *  Created on: Dec 10, 2019
 *      Author: kychu
 */

#include "log.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef DEBUG_ENABLE

static char *log_cache = NULL;
static log_put_t log_put_func = NULL;
static osMutexId logMutex = NULL;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: _alert, _err, _warn, and _info
 *
 * Description:
 *  If the cross-compiler's pre-processor does not support variable
 *  length arguments, then these additional APIs will be built.
 *
 ****************************************************************************/

status_t log_init(log_put_t ptx)
{
  log_cache = kmm_alloc(128);
  if(log_cache == NULL) return status_nomem;

  /* Create the mutex  */
  osMutexDef(LOGMutex);
  logMutex = osMutexCreate(osMutex(LOGMutex));
  if(logMutex == NULL) return status_error;

  if(ptx == NULL) return status_error;
  log_put_func = ptx;
  return status_ok;
}

status_t log_write(const char *format, ...)
{
  if(log_cache == NULL) return status_error;
  va_list ap;

  va_start(ap, format);
  osMutexWait(logMutex, osWaitForever);
  vsprintf(log_cache, format, ap);
  log_put_func((const char *)log_cache);
  osMutexRelease(logMutex);
  va_end(ap);
  return status_ok;
}

#endif /* DEBUG_ENABLE */
