#include "com_task.h"

static kyLinkPackageDef *pRxUart;
static kyLinkPackageDef TxPacket;
KYLINK_CORE_HANDLE UartPortHandle;
KYLINK_CORE_HANDLE CDC_PortHandle;
static IMU_UNIT imu_unit;

static uint8_t tx_cnt = 0;
static uint32_t tx_stamp = 0;

void com_task_init(void)
{
	kyLinkInit(&UartPortHandle, uart2_TxBytesDMA);
	kyLinkInit(&CDC_PortHandle, USB_CDC_SendBufferFast);
	kyLinkTxEnable(&UartPortHandle);
	pRxUart = GetRxPackage(&UartPortHandle);
	TxPacket.FormatData.stx1 = kySTX1;
	TxPacket.FormatData.stx2 = kySTX2;
	TxPacket.FormatData.dev_id = HARD_DEV_ID;
}
//extern DebugFloatDef DBGDATA;
//static uint8_t rx_data;
static uint8_t read_len;
static uint8_t read_buf[8];
void com_task(void)
{
	if(_Get_Millis() - tx_stamp > 10) {
		tx_stamp = _Get_Millis();
		tx_cnt ++;
		if(tx_cnt % 2 == 0) {
			TxPacket.FormatData.msg_id = TYPE_ATT_QUAT_Resp;
			TxPacket.FormatData.length = sizeof(Quat_T);
			TxPacket.FormatData.PacketData.TypeData.AttitudeQuat = get_est_q();
		} else {// if(tx_cnt % 3 == 1)
			TxPacket.FormatData.msg_id = TYPE_IMU_INFO_Resp;
			TxPacket.FormatData.length = sizeof(IMU_INFO_DEF);
			imu_unit = get_imu_unit();
			TxPacket.FormatData.PacketData.TypeData.IMU_InfoData.accX = imu_unit.AccData.accX;
			TxPacket.FormatData.PacketData.TypeData.IMU_InfoData.accY = imu_unit.AccData.accY;
			TxPacket.FormatData.PacketData.TypeData.IMU_InfoData.accZ = imu_unit.AccData.accZ;
			TxPacket.FormatData.PacketData.TypeData.IMU_InfoData.gyrX = imu_unit.GyrData.gyrX;
			TxPacket.FormatData.PacketData.TypeData.IMU_InfoData.gyrY = imu_unit.GyrData.gyrY;
			TxPacket.FormatData.PacketData.TypeData.IMU_InfoData.gyrZ = imu_unit.GyrData.gyrZ;
//		} else {
//			TxPacket.FormatData.msg_id = TYPE_DEBUG_DATA_Resp;
//			TxPacket.FormatData.length = sizeof(DebugFloatDef);
//			TxPacket.FormatData.PacketData.TypeData.DebugData = DBGDATA;
		}
		SendTxPacket(&UartPortHandle, &TxPacket);
		SendTxPacket(&CDC_PortHandle, &TxPacket);
	}

	while((read_len = uart2_pullBytes(read_buf, 8)) > 0) {
//	while(uart2_pullByte(&rx_data) != 0) {
		for(uint8_t idx = 0; idx < read_len; idx ++) {
			kylink_decode(&UartPortHandle, read_buf[idx]);
		}
//		kyLink_DecodeProcess(rx_data);
		if(kyLinkCheckUpdate(&UartPortHandle)) {
			USER_LED_TOG();
			if(pRxUart->FormatData.msg_id == TYPE_UPGRADE_REQUEST && pRxUart->FormatData.length == 16 &&
			   pRxUart->FormatData.PacketData.TypeData.FileInfo.Enc_Type == ENC_TYPE_PLAIN) {
				__disable_irq();
				NVIC_SystemReset();
			}
		}
	}
}
