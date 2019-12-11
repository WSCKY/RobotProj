#include "mesg.h"
#include "kyLink.h"
#include "gnss.h"
#include <math.h>

static KYLINK_CORE_HANDLE kylink_msg;
static kyLinkPackageDef tx_packet;

/* Semaphore to signal incoming packets */
osSemaphoreId msg_xSemaphore = NULL;

static ubx_npvts_t npvts_pack;
static ubx_npvts_t npvts_pack_1;

static double computeAzimuth(double lat1, double lon1, double lat2, double lon2);
double test_val = 0;
void mesg_send_task(void const *argument)
{
  if(cdcif_init() != status_ok) {
    ky_err("usb cdc init failed.\n");
    ky_err("mesg module start failed, exit!\n");
    vTaskDelete(NULL);
  }

  ky_info("mesg module started.\n");

  kyLinkInit(&kylink_msg);
  kyLinkConfigTxFunc(&kylink_msg, cdcif_tx_bytes);
  kyLinkInitPackage(&tx_packet);

  tx_packet.FormatData.msg_id = TYPE_PVTS_Info_Resp;
  tx_packet.FormatData.length = sizeof(ubx_npvts_t);

  osSemaphoreDef(SEM);
  msg_xSemaphore = osSemaphoreCreate(osSemaphore(SEM), 1 );

  for(;;) {
    if(osSemaphoreWait( msg_xSemaphore, osWaitForever)==osOK) {
      npvts_pack = *get_npvts_a();
      npvts_pack_1 = *get_npvts_b();
      test_val = computeAzimuth(npvts_pack.lat, npvts_pack.lon, npvts_pack_1.lat, npvts_pack_1.lon);
      npvts_pack.head_veh = test_val * 1e5;
      tx_packet.FormatData.PacketData.TypeData.pvts = npvts_pack;
      SendTxPacket(&kylink_msg, &tx_packet);
    }
  }
}

#define DEG_TO_RAD 0.017453292519943295769236907684886f
#define RAD_TO_DEG 57.295779513082320876798154814105f

//static double computeAzimuth(double lat1, double lon1, double lat2, double lon2) {
//    double result = 0.0;

//    int ilat1 = (int) (0.50 + lat1 * 360000.0);
//    int ilat2 = (int) (0.50 + lat2 * 360000.0);
//    int ilon1 = (int) (0.50 + lon1 * 360000.0);
//    int ilon2 = (int) (0.50 + lon2 * 360000.0);

//    lat1 = lat1 * DEG_TO_RAD;
//    lon1 = lon1 * DEG_TO_RAD;
//    lat2 = lat2 * DEG_TO_RAD;
//    lon2 = lon2 * DEG_TO_RAD;

//    if ((ilat1 == ilat2) && (ilon1 == ilon2)) {
//        return result;
//    } else if (ilon1 == ilon2) {
//        if (ilat1 > ilat2)
//            result = 180.0;
//    } else {
//        double c = acosf(sinf(lat2) * sinf(lat1) + cosf(lat2) * cosf(lat1) * cosf((lon2 - lon1)));
//        double A = asinf(cosf(lat2) * sinf((lon2 - lon1)) / sinf(c));
//        result = A * RAD_TO_DEG;
//        if ((ilat2 > ilat1) && (ilon2 > ilon1)) {
//        } else if ((ilat2 < ilat1) && (ilon2 < ilon1)) {
//            result = 180.0 - result;
//        } else if ((ilat2 < ilat1) && (ilon2 > ilon1)) {
//            result = 180.0 - result;
//        } else if ((ilat2 > ilat1) && (ilon2 < ilon1)) {
//            result += 360.0;
//        }
//    }
//    return result;
//}

// direction: P1 ----> P2
static double computeAzimuth(double lat1, double lon1, double lat2, double lon2) {
  if(lon2 == lon1) {
    if(lat2 > lat1)
      return 0.0f;
    else
      return 180.0f;
  }
  float alpha = 90.0f - atanf((lat2 - lat1) / (lon2 - lon1)) * RAD_TO_DEG;
  if(lon2 > lon1) return alpha;
  return (180.0f + alpha);
}
