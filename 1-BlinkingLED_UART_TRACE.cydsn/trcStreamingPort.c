// An adataption of the Percepio USB_CDC Port to PSoC Serial
#include "trcRecorder.h"
#include "task.h"

#if (TRC_USE_TRACEALYZER_RECORDER == 1)
#if(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include "stdint.h"

int32_t PSoC_Receive(void *data, uint32_t size, int32_t *numOfBytesReceived)
{
  
    uint8_t *myData= (uint8_t *)data;
    
    *numOfBytesReceived = 0;
    while( (*numOfBytesReceived < (int32_t)size) && UART_SpiUartGetRxBufferSize())
    {
            myData[*numOfBytesReceived] = UART_SpiUartReadRxData();
            *numOfBytesReceived += 1;
    }
  
	return 0; // Doesnt matter what you return... i dont think that it is checked
}


int32_t PSoC_Transmit(void* data, uint32_t size, int32_t *numOfBytesSent )
{
    TickType_t time;
    
    timing_Write(1);
    
    time = xTaskGetTickCount();
    UART_SpiUartPutArray((uint8_t *)data,size);
    time = xTaskGetTickCount() - time;
    vTracePrintF(0,"%d",time);
    timing_Write(0);
    *numOfBytesSent=size;
    
    return 0; // Doesnt matter what you return... i dont think that it is checked
}

#endif	/*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/
#endif  /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

