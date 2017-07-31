
#include "project.h"
#include <stdio.h>

// A flag to trigger the DMA
volatile int myFlag=0;
  
static const char myArray[]="asdf1234asdfadsfasdfadsfqwerasdfqwerqwer9\n";

CY_ISR(sw_handler)
{
    myFlag = 1;
    SW_ClearInterrupt();
}

// This is called TWICE at the end of the DMA transaction
CY_ISR(myDMA)
{
    BLUE_Write(~BLUE_Read());
}
int main(void)
{
    CyIntEnable(CYDMA_INTR_NUMBER);
    CyGlobalIntEnable; /* Enable global interrupts. */

    char c;
    
    UART_Start();
    UART_UartPutString("Started\n");
  
    isr_1_StartEx(sw_handler);
  
    CyDmaEnable();
    
    DMA_Init();
    DMA_SetDstAddress(0, (void *)UART_TX_FIFO_WR_PTR);
    DMA_SetInterruptCallback(myDMA);

    while(1)
    {
        c = UART_UartGetChar();
        switch(c)
        {       
            case 's':
                myFlag = 1;
            break;
        }
        
        // This turns on the DMA so that the string will go to the UART.
        if(myFlag && CyDmaGetActiveChannels() == 0)
        {
            DMA_SetSrcAddress(0, (void *)myArray);
            DMA_SetNumDataElements(0,strlen(myArray)-1);
            DMA_ValidateDescriptor(0);
            DMA_ChEnable();
            myFlag=0;
        }
    }
}

