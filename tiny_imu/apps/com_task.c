#include "com_task.h"

static CommPackageDef *pRx;
static CommPackageDef TxPacket;
static Quat_T AttQ;

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
	if(_Get_Millis() - tx_stamp > 20) {
		tx_stamp = _Get_Millis();
		AttQ = get_est_q();
		TxPacket.Packet.msg_id = TYPE_ATT_QUAT_Resp;
		TxPacket.Packet.length = sizeof(Quat_T);
		for(uint32_t idx = 0; idx < TxPacket.Packet.length; idx ++) {
			TxPacket.Packet.PacketData.pData[idx] = ((uint8_t *)&AttQ)[idx];
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
