/*
 * debug.h
 *
 *  Created on: Dec 10, 2019
 *      Author: kychu
 */

#ifndef _LOG_H_
#define _LOG_H_

#include "logconfig.h"
#include "SysConfig.h"

#define DEBUG_ENABLE                             (1)

#ifdef DEBUG_ENABLE
#define CONFIG_DEBUG_ALERT
#define CONFIG_DEBUG_ERROR
#define CONFIG_DEBUG_WARN
#define CONFIG_DEBUG_INFO
#endif /* defined(DEBUG_ENABLE) */

typedef status_t (*log_put_t)(const char *);

#ifdef DEBUG_ENABLE
status_t log_init(log_put_t ptx);
status_t log_write(const char *format, ...);
#endif /* defined(DEBUG_ENABLE) */

#ifndef CONFIG_DEBUG_ALERT
#  define ky_alert                               (void)
#else
#  define ky_alert(format, ...)                  log_write(format, ##__VA_ARGS__)
# endif

#ifndef CONFIG_DEBUG_ERROR
#  define ky_err                                 (void)
#else
#  define ky_err(format, ...)                    log_write(format, ##__VA_ARGS__)
#endif

#ifndef CONFIG_DEBUG_WARN
#  define ky_warn                                (void)
#else
#  define ky_warn(format, ...)                   log_write(format, ##__VA_ARGS__)
#endif

#ifndef CONFIG_DEBUG_INFO
#  define ky_info                                (void)
#else
#  define ky_info(format, ...)                   log_write(format, ##__VA_ARGS__)
#endif

/* This determines the importance of the message. The levels are, in order
 * of decreasing importance:
 */

//#define LOG_EMERG     0  /* System is unusable */
//#define LOG_ALERT     1  /* Action must be taken immediately */
//#define LOG_CRIT      2  /* Critical conditions */
//#define LOG_ERR       3  /* Error conditions */
//#define LOG_WARNING   4  /* Warning conditions */
//#define LOG_NOTICE    5  /* Normal, but significant, condition */
//#define LOG_INFO      6  /* Informational message */
//#define LOG_DEBUG     7  /* Debug-level message */

#endif /* _LOG_H_ */
