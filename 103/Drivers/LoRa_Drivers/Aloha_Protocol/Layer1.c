/*
 * Layer1.c
 *
 *  Created on: Nov 18, 2021
 *      Author: Admin
 */


#include "Layer1.h"
#include "Config.h"
#include "String.h"

#define MSG_PAYLOAD			0x01
#define MSG_ACK				0x02

Std_ReturnType Receive_Handle(uint8_t *payload);
Std_ReturnType LoRa_BufferHandle();
uint32_t mac[3];
extern uint8_t ACK_Check[10];
extern uint8_t ACK_Send[10];



extern packet_ringbuffer DataBuffer;

extern slotted_time_struct slot;


void CadDoneIrqHandle(bool channelDectectted )
{
	if( channelDectectted )
	{
		slot.send_enable = 0;
		slot.timer++;
	}
	else
	{
		slot.send_enable = 1;
	}
}


void TxDoneIrqHandle()
{
	Radio.Rx(10000);
}
void RxDoneIrqHandle(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
	//Receive_Handle(payload);
	LoRa_BufferHandle(payload);
}

void RxTimeOutIrqHandle()
{

}

RadioEvents_t RadioCallBack = {
	.TxDone = TxDoneIrqHandle,
	.RxDone = RxDoneIrqHandle,
	.TxTimeout = NULL,
	.RxTimeout = NULL,
	.RxError = NULL,
	.CadDone = CadDoneIrqHandle
};






void L1_Init()
{
	mac[0] = *(uint32_t *)(0x1FFFF7E8); // high address
	mac[1] = *(uint32_t  *)(0x1FFFF7E8 + 4); //
	mac[2] = *(uint32_t *)(0x1FFFF7E8 + 8); //  Low address
	/*
	RadioEvents_t RadioEvents = {
		.TxDone = NULL,
		.RxDone = NULL,
		.TxTimeout = NULL,
		.RxTimeout = NULL,
		.RxError = NULL,
		.CadDone = NULL
	};

	Radio.Standby();
	Radio.Init( &RadioEvents );

	Radio.SetChannel( RF_FREQUENCY );

	Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                     LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                     LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                     true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
	Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                     LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                     LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                     0, true, 0, 0, LORA_IQ_INVERSION_ON, false );
	SX126xSetCadParams(0x02, 23, 10, 0, 00);
	*/
}


uint8_t LoRa_WriteBuffer(User_ID receiver,uint8_t msg_type, uint8_t *payload)
{
	packet_ringbuffer *buff = &DataBuffer;
	if(buff->TxHead != buff->TxTail)
	{
		if((buff->TxHead + 1) == buff->TxTail)
		{
			return 0;
		}
	}

	buff->TxBuffer[buff->TxHead].receiver = receiver;
	buff->TxBuffer[buff->TxHead].sender = NODEID;
	buff->TxBuffer[buff->TxHead].msg_type = msg_type;
	buff->TxBuffer[buff->TxHead].macaddr = mac[2];
	if(msg_type == MSG_PAYLOAD)
	{
		ACK_Check[receiver] = 1;
	}

	buff->TxBuffer[buff->TxHead].payload_ptr = payload;
	buff->TxHead = (buff->TxHead + 1) % (MAX_BUFFER_SIZE - 1);
	return E_OK;
}
static Std_ReturnType Send_Packet()
{
	packet_ringbuffer *buff = &DataBuffer;
	if(buff->TxHead == buff->RxTail)
	{
		return E_NOT_OK;
	}
	uint8_t TxSend[10];
	memcpy(TxSend,buff[buff->TxTail].TxBuffer,10);
	Radio.Send(TxSend,10);
	buff->TxTail = (buff->TxTail+1) % MAX_BUFFER_SIZE;
	return E_OK;
}








Std_ReturnType LoRa_ASynTransmit(User_ID receiver,uint8_t msg_type, uint8_t *payload)
{
	if(slot.timer >= slot.rnd_num)
	{
		Radio.StartCad();
		HAL_ADC_Start_IT(&hadc1);
		slot.rnd_num = slot.timer;
	}
	if(slot.send_enable == true)
	{
		if(LoRa_WriteBuffer(receiver,msg_type,payload) == E_NOT_OK)
		{
			return E_NOT_OK;
		}
		Send_Packet();
		slot.timer= 0;
		ACK_Send[DataBuffer.TxBuffer[DataBuffer.TxHead].receiver] = 0;
		slot.send_enable = 0;
	}
	return E_NOT_OK;
}






Std_ReturnType LoRa_SynTransmit(User_ID receiver,uint8_t msg_type, uint8_t *payload)
{
	if(ACK_Check[DataBuffer.TxBuffer[DataBuffer.TxHead].receiver] == 1)
	{
		if(slot.timer >= slot.rnd_num)
		{
			Radio.StartCad();
			HAL_ADC_Start_IT(&hadc1);
			slot.timer = 0;
		}
		if(slot.send_enable == true)
		{
			if(LoRa_WriteBuffer(receiver,msg_type,payload) == E_NOT_OK)
			{
				return E_NOT_OK;
			}
			Send_Packet();
			slot.timer= 0;
			slot.send_enable = 0;
		}
		return E_NOT_OK;
	}
	else
	{
		return E_OK;
	}
}



Std_ReturnType Receive_Handle(uint8_t *payload)
{
	uint8_t data[3] = {0};
	if(payload == NULL)
	{
		return E_NOT_OK;
	}
	if(DataBuffer.RxHead != DataBuffer.RxTail)
	{
		if(DataBuffer.RxHead++ == DataBuffer.RxTail)
		{
			return E_NOT_OK;
		}
	}
	DataBuffer.RxBuffer[DataBuffer.RxHead].sender = *payload;
	DataBuffer.RxBuffer[DataBuffer.RxHead].receiver = *(payload+1);
	DataBuffer.RxBuffer[DataBuffer.RxHead].msg_type = *(payload+2);
	DataBuffer.RxBuffer[DataBuffer.RxHead].macaddr = *(payload+3);
	memcpy(DataBuffer.RxBuffer[DataBuffer.RxHead].payload_ptr,payload+4,3);
	DataBuffer.RxHead = (DataBuffer.RxHead + 1) % MAX_BUFFER_SIZE;
	return E_OK;
}

Std_ReturnType LoRa_BufferHandle(uint8_t *payload)
{
	if( Receive_Handle(payload) == E_NOT_OK )
	{
		return E_NOT_OK;
	}
	if(DataBuffer.RxHead == DataBuffer.RxTail)
	{
		return E_NOT_OK;
	}
	if(DataBuffer.RxBuffer[DataBuffer.TxTail].sender == NODEID ||
	   DataBuffer.RxBuffer[DataBuffer.TxTail].receiver != NODEID ||
	   DataBuffer.RxBuffer[DataBuffer.TxTail].receiver != 0xFF )
	{
		DataBuffer.RxTail = (DataBuffer.RxTail+1) % MAX_BUFFER_SIZE;
		return E_NOT_OK;
	}

	if(DataBuffer.RxBuffer[DataBuffer.TxTail].msg_type == MSG_ACK)
	{
		ACK_Check[DataBuffer.RxBuffer[DataBuffer.TxTail].sender] = 0;
	}
	else if( DataBuffer.RxBuffer[DataBuffer.TxTail].msg_type == MSG_PAYLOAD)
	{
		//
	}
	DataBuffer.RxTail = (DataBuffer.RxTail+1) % MAX_BUFFER_SIZE;
	return E_OK;
}






void LoRa_Polling()
{


}

















void Timer_Callback()
{
	if(slot.synchro_enable == false)
	{
		slot.timer++;
	}
}













































