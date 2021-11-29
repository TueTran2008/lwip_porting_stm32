/*
 * App_Test.c
 *
 *  Created on: Oct 25, 2021
 *      Author: Admin
 */

#include "App_Test.h"
#include "Layer1.h"
#include "Types.h"



 packet_ringbuffer DataBuffer;

bool EnableMaster=true;//Ö÷´ÓÑ¡Ôñ

#define RF_FREQUENCY                                434000000 // Hz
#define TX_OUTPUT_POWER                             22        // 22 dBm
#define LORA_BANDWIDTH                              1         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       10         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false



#define PING_MESSAGE			0x01
#define PONG_MESSAGE			0x02



#define SESSION_DEFAULT			0x00
#define SESSION_RESPONSE		0x01

typedef void(*Session_Ptr)(void);
void Session_Default();
void Session_Response();
void Session_Receive();
Session_Ptr Session_Current[2] = { Session_Default,Session_Response};
uint8_t SessionID = 0;









uint32_t slotcount = 0;

uint8_t ACK_Check[10];
uint8_t ACK_Send[10];
uint8_t ack_enable ;


void Payload_Handle();
void LoRa_IRQHandle();



slotted_time_struct slot;







#define ACK_MESSAGE_T			0x03




uint8_t TransmitID = 0;

//uint8_t* data[100];
/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;
//LoRa_Buffer_t DataBuffer;

#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 64 // Define the payload size here


#define POS_USER_ID				5
#define POS_RANDOMKEY			4

#define		LORA_BUFFER_LENGHT			24



void OnCaDDetectted(bool channelActivityDetected )
{

	if( channelActivityDetected )
	{
		slot.send_enable = 0;
		slot.timer++;
	}
	else
	{
		slot.send_enable = 1;
	}
}
void OnTxDone( void )
{

	SessionID = SESSION_DEFAULT;
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
	Radio.Standby();
	if(ack_enable == 1)
	{
		Radio.Rx(500000);
	}
}
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
	/*memcpy(RxBuffer,payload,24);
	if(strncmp(RxBuffer,"START",5) == 0)
	{
		//enable_send = 1;
		SessionID = SESSION_RESPONSE;
		TransmitID = ACK_MESSAGE_T;
		Radio.Standby();
		memset(RxBuffer,0,5);
	}
	else
	{
		Radio.Rx(1);
		SessionID = SESSION_DEFAULT;
		memset(RxBuffer,0,5);
	}*/
}




void OnRxTimeout( void )
{
	Radio.Standby();


}
void OnRxError( void )
{
}



void App_TestMain()
{
	//RxContinuous = true;
	//Radio.Rx(0xFFFFFF);
	RadioEvents_t RadioEvents = {
		.TxDone = OnTxDone,
		.RxDone = OnRxDone,
		.TxTimeout = NULL,
		.RxTimeout = OnRxTimeout,
		.RxError = OnRxError,
		.CadDone = OnCaDDetectted
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

	SessionID = SESSION_RESPONSE;
	TransmitID = ACK_MESSAGE_T;
	HAL_ADC_Start_IT(&hadc1);
	HAL_TIM_Base_Start_IT(&htim6);

	while(1)
	{
		//Session_Current[SessionID]();
		LoRa_WriteBuffer(User_Node_0,0x01,"3");
		//LoRa_SynTransmit();
	}
}






void LoRa_IRQHandle()
{
	Radio.IrqProcess( );
}

void Session_Response()
{
	while(1)
	{
		if(SessionID != SESSION_RESPONSE)
		{
			break;
		}

	HAL_Delay(1);
	}
}


void Session_Default()
{
	Radio.Rx(1000);
	while(1)
	{
		if(SessionID != SESSION_DEFAULT)
		{
			break;
		}

		HAL_Delay(1);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);
  if(htim->Instance == TIM2)
  	  Radio.IrqProcess();
  if(htim->Instance == TIM6)
  {
	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_3);
	  Timer_Callback();
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);
  if(hadc->Instance == ADC1)
  {
		  uint16_t temp = HAL_ADC_GetValue(&hadc1);
		  slot.rnd_num = ((( temp & 0x0003) ^ (SysTick->VAL))&(0x0F));
  }
}



