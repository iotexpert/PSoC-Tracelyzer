/*******************************************************************************
* File Name: main.c
*
* Version:   1.0
*
* Description:
*  This example shows how to use DMA to transfer data from a RAM array to
*  the UART TX buffer, and shows how to use DMA to transfer data from the
*  UART RX buffer to a RAM array.
*  UART receives and buffers characters from a serial program such as 
*  HyperTerminal. Once 16 characters are received, UART transmits (echoes) them
*  back to HyperTerminal.
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <project.h>

#define DESCR0              0
#define DESCR1              1
#define BUFFER_SIZE         16

/* Function prototypes */
static void RxDmaSetConfig(void);
static void RxDmaInterrupt(void);
static void TxDmaSetConfig(void);

/* Global variables */
volatile uint8 rxDmaComplete = 0u;

/* UART data buffers */
static uint8 uartBuffer0[BUFFER_SIZE];
static uint8 uartBuffer1[BUFFER_SIZE];


/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  This function sets an interrupt callback function for the receive DMA, starts
*  the UART communication, configures and starts DMA channels. 
*  Once 16 characters are received (rxDmaComplete flag is set), points the
*  transmit DMA to the buffer which contains the received data and enables the
*  transfer.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
int main()
{
    /* Array of pointers to UART data buffers. */
    static void * const txBufferAddress[] = {uartBuffer0, uartBuffer1};

    /* Indicates which buffer is being currently transmitted. */
    static uint8 txBufferIdx = 0u;

    /* Enable interrupts. */
    CyIntEnable(CYDMA_INTR_NUMBER);
    CyGlobalIntEnable;

    /* Start UART communication. */
    UART_Start();
    UART_UartPutString("*** PSoC 4 DMA UART Example Project ***\n\r\0");
    UART_UartPutString("Enter the characters to transmit\n\r\0");
    
    /* Configure receive and transmit DMA channels. */
    RxDmaSetConfig();
    TxDmaSetConfig();

    for(;;)
    {
        /* Wait for rxDmaComplete to be set by RxDmaInterrupt. */ 
        if(0u != rxDmaComplete)
        {
            rxDmaComplete = 0u;
            
            /* Point transmit DMA to buffer which contains received data
            * and enable transfer.
            */
            TxDma_SetSrcAddress(DESCR0, txBufferAddress[txBufferIdx]);
            TxDma_ValidateDescriptor(DESCR0);
            TxDma_ChEnable();
            
            /* Switch to other buffer. */
            txBufferIdx ^= 1u;
        }
    }
}


/*******************************************************************************
* Function Name: RxDmaInterrupt
********************************************************************************
*
* Summary:
*  Handles the receive interrupt. Informs the main code when 16 characters are
*  received. 
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
static void RxDmaInterrupt(void)
{
    rxDmaComplete = 1u;
}


/*******************************************************************************
* Function Name: RxDmaSetConfig
********************************************************************************
*
* Summary:
*  Configures the receive DMA to transfer from UART to RAM.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
static void RxDmaSetConfig(void)
{
    /* Set callback function to be called for receive DMA interrupt. */
    RxDma_SetInterruptCallback(&RxDmaInterrupt);

    /* Set source destination locations. Validate descriptor 1. 
    * Descriptor 0 is configured and validated by RxDma_Start().
    */
    RxDma_SetSrcAddress(DESCR1, (void *)UART_RX_FIFO_RD_PTR);
    RxDma_SetDstAddress(DESCR1, (void *)uartBuffer1);
    RxDma_ValidateDescriptor(DESCR1);
    
    RxDma_Start((void *)UART_RX_FIFO_RD_PTR, (void *)uartBuffer0);
}


/*******************************************************************************
* Function Name: TxDmaSetConfig
********************************************************************************
*
* Summary:
*  Configures the transmit DMA to transfer data from RAM to UART. 
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
static void TxDmaSetConfig(void)
{   
    /* There is nothing to transmit before the first 16 characters are received.
    * The transmit DMA channel is initialized now and left disabled until then.
    * The source address is set in the main loop every time 16 characters are
    * received to point to the last received data.
    */
    TxDma_Init();
    TxDma_SetDstAddress(DESCR0, (void *)UART_TX_FIFO_WR_PTR);
}


/* [] END OF FILE */
