#include "com_task.h"

static CommPackageDef *pRx;
static CommPackageDef TxPacket;
static IMU_UNIT imu_unit;

static uint8_t tx_cnt = 0;
static uint32_t tx_stamp = 0;

void com_task_init(void)
{
	pRx = GetRxPacket();
	TxPacket.Packet.stx1 = kySTX1;
	TxPacket.Packet.stx2 = kySTX2;
	TxPacket.Packet.dev_id = HARD_DEV_ID;
}

//static uint8_t rx_data;
static uint8_t read_len;
static uint8_t read_buf[8];
void com_task(void)
{
	if(_Get_Millis() - tx_stamp > 10) {
		tx_stamp = _Get_Millis();
		tx_cnt ++;
		if(tx_cnt % 2 == 0) {
			TxPacket.Packet.msg_id = TYPE_ATT_QUAT_Resp;
			TxPacket.Packet.length = sizeof(Quat_T);
			TxPacket.Packet.PacketData.AttitudeQuat = get_est_q();
		} else {
			TxPacket.Packet.msg_id = TYPE_IMU_INFO_Resp;
			TxPacket.Packet.length = sizeof(IMU_INFO_DEF);
			imu_unit = get_imu_unit();
			TxPacket.Packet.PacketData.IMU_InfoData.accX = imu_unit.AccData.accX;
			TxPacket.Packet.PacketData.IMU_InfoData.accY = imu_unit.AccData.accY;
			TxPacket.Packet.PacketData.IMU_InfoData.accZ = imu_unit.AccData.accZ;
			TxPacket.Packet.PacketData.IMU_InfoData.gyrX = imu_unit.GyrData.gyrX;
			TxPacket.Packet.PacketData.IMU_InfoData.gyrY = imu_unit.GyrData.gyrY;
			TxPacket.Packet.PacketData.IMU_InfoData.gyrZ = imu_unit.GyrData.gyrZ;
		}
		SendTxPacket(&TxPacket);
	}

	while((read_len = uart2_pullBytes(read_buf, 8)) > 0) {
//	while(uart2_pullByte(&rx_data) != 0) {
		for(uint8_t idx = 0; idx < read_len; idx ++) {
			kyLink_DecodeProcess(read_buf[idx]);
		}
//		kyLink_DecodeProcess(rx_data);
		if(GotNewData()) {
			USER_LED_TOG();
			if(pRx->Packet.msg_id == TYPE_UPGRADE_REQUEST && pRx->Packet.length == 16 &&
			   pRx->Packet.PacketData.FileInfo.Enc_Type == ENC_TYPE_PLAIN) {
				__disable_irq();
				NVIC_SystemReset();
			}
		}
	}
}
