#include "com_task.h"

static uint32_t _init_flag = 0;

static CommPackageDef *pRx;
static CommPackageDef TxPacket;
static Quat_T AttQ;

static uint32_t tx_stamp = 0;

void com_tx_task(void)
{
	if(_init_flag == 0) {
		_init_flag = 1;

		pRx = GetRxPacket();
		TxPacket.Packet.stx1 = kySTX1;
		TxPacket.Packet.stx2 = kySTX2;
		TxPacket.Packet.dev_id = HARD_DEV_ID;
	} else {
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
	}
	if(GotNewData()) {
		if(pRx->Packet.msg_id == TYPE_UPGRADE_REQUEST && pRx->Packet.length == 16 &&
		   pRx->Packet.PacketData.FileInfo.Enc_Type == ENC_TYPE_PLAIN) {
			__disable_irq();
			NVIC_SystemReset();
		}
	}
}
