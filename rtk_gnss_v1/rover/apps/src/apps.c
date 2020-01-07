#include "apps.h"
#include "test_case.h"

static void led_thread(void const *argument);

#define STR1(R) #R
#define STR2(R) STR1(R)

static const char SystemInfo[] =
"\n  .--,       .--,"
"\n ( (  \\.---./  ) )"
"\n  '.__/o   o\\__.'"
"\n     {=  ^  =}"               "\t\t\t"   "Integrated Navigation System based on SINS/GPS"
"\n      >  -  <"                "\t\t\t"   "Author:  kyChu<kychu@qq.com>"
"\n     /       \\"              "\t\t\t"   "Version: " STR2(__VERSION_STR__)
"\n    //       \\\\"            "\t\t\t"   "Date:    " __DATE__
"\n   //|   .   |\\\\"             "\t\t"   "Time:    " __TIME__
"\n   \"'\\       /'\"_.-~^`'-."     "\t"   "Board:   Deepblue RTK NAV Board(2019 V1)"
"\n      \\  _  /--'         `"      "\t"   "ALL RIGHTS RESERVED BY kyChu<kychu@qq.com>"
"\n    ___)( )(___"
"\n   (((__) (__)))"
"\n"
;

#define TEST_CASE_TASK_ENABLE                    (0)

void APP_StartThread(void const *argument)
{
  /* Initialize LEDs */
  leds_init();

  comif_init();
  log_init(comif_tx_string_util);
  osDelay(500);
  ky_alert("!!!KERNEL START!!!\n");
  comif_tx_string_util(SystemInfo);

  // mount FATFS logic driver
  if(fatfs_mount() != status_ok) {
    ky_err("ERROR: FS INIT FAIL! EXIT!\n");
    vTaskDelete(NULL);
  }

  osThreadDef(SINS, att_est_q_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 8); // stack size = 1KB
  osThreadDef(GNSS, gnss_navg_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2); // stack size = 256B
  osThreadDef(RTCM, rtcm_transfer_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2); // stack size = 256B
#if (TEST_CASE_TASK_ENABLE)
  osThreadDef(TEST, test_case_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 8); // stack size = 1KB
#endif /* (TEST_CASE_TASK_ENABLE) */
  osThreadDef(MESG, mesg_proc_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 4); // stack size = 512B
  osThreadDef(FILE, transfile_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 4); // stack size = 512B

  if(osThreadCreate(osThread(SINS), NULL) == NULL) ky_err("sins task create failed.\n");
  if(osThreadCreate(osThread(GNSS), NULL) == NULL) ky_err("gnss task create failed.\n");
  if(osThreadCreate(osThread(RTCM), NULL) == NULL) ky_err("rtcm task create failed.\n");
#if (TEST_CASE_TASK_ENABLE)
  if(osThreadCreate(osThread(TEST), NULL) == NULL) ky_err("test task create failed.\n");
#endif /* (TEST_CASE_TASK_ENABLE) */
  if(osThreadCreate(osThread(MESG), NULL) == NULL) ky_err("mesg task create failed.\n");
  if(osThreadCreate(osThread(FILE), NULL) == NULL) ky_err("file task create failed.\n");

  /* LED INDICATOR TASK */
  osThreadDef(LEDS, led_thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE); // stack size = 128B

  if(osThreadCreate(osThread(LEDS), NULL) == NULL) ky_err("leds task create failed.\n");

  vTaskDelete(NULL);
}

static void led_thread(void const *argument)
{
  (void) argument;

  for(;;) {
    led_toggle(LED_GREEN);

    osDelay(400);
  }
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
