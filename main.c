/******************************************************************************
 * @file     main.c
 * @version  V2.00
 * $Revision: 8 $
 * $Date: 15/01/16 1:45p $
 * @brief
 *           Implement SPI Master loop back transfer.
 *           This sample code needs to connect SPI0_MISO0 pin and SPI0_MOSI0 pin together.
 *           It will compare the received data with transmitted data.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NUC131.h"
#include "STM_ENC28_J60.h"
#define TEST_COUNT             64
void delay1(uint32_t time);
uint32_t g_au32SourceData[TEST_COUNT];
uint32_t g_au32DestinationData[TEST_COUNT];
uint32_t result[2];
/* Function prototype declaration */
void SYS_Init(void);
void SPI_Init(void);

uint32_t debug;
uint8_t debug1;
uint16_t dataWatch17;

/* ------------- */
/* Main function */
/* ------------- */
int main(void)
{
    uint32_t u32DataCount, u32TestCount, u32Err;
		
    /* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();
		GPIO_SetMode(PA, BIT14, GPIO_PMD_OUTPUT);
		//PB13 = 1;
    /* Lock protected registers */
    SYS_LockReg();

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);

    /* Init SPI */
    SPI_Init();
////////////////////////////////////////////
//intETH
////////////////////////////////////////////	
	// (1): Disable the chip CS pin
	
	// (2): Perform soft reset to the ENC28J60 module
		ENC28_writeOp(ENC28_SOFT_RESET, 0, ENC28_SOFT_RESET);
		
	// (3): Wait untill Clock is ready
	while(!ENC28_readOp(0x00, 0x1D) & ESTAT_CLKRDY);
	
		// (6): MAC Control Register 1
	//ENC28_writeReg8(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS|MACON1_PASSALL);
	ENC28_writeOp(0x05, ECON1, 0x03);//setbit
	ENC28_writeOp(0x04, ECON1, 0x02);//clearbit
	ENC28_writeOp(0x02, MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS|MACON1_PASSALL);
	
	//dataWatch8 = ENC28_readReg8(ERXFCON);
	// (7): MAC Control Register 3
	ENC28_writeOp(0x04, MACON3,MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
	
//		debug = ENC28_readOp(0x00,0x1f);
//		ENC28_writeOp(0x04, ECON1, 0x03);
//		debug = ENC28_readOp(0x00,0x1f);
//		ENC28_writeOp(0x05, ECON1, 0x03);
//		debug = ENC28_readOp(0x00,0x1f);
//    ENC28_writeOp(0x04, ECON1, 0x03);
//		//ENC28_writeOp(0x02, ECON1, 0x03);
//		debug = ENC28_readOp(0x00,0x1f);
//		
//		ENC28_writeReg16(ETXND, TXSTOP_INIT);
//		
//		dataWatch17 = ENC28_readReg16(ERXND);
//		//ENC28_writeOp(0x02, 0x1F, 0x01);
//		debug = ENC28_readOp(0x00,0x1f);
//		ENC28_writeOp(ENC28_SOFT_RESET,0x1f, 0x00);
//		debug = ENC28_readOp(0x00,0x1f);
		
			// (10): Set the MAC address of the device
				ENC28_writeOp(0x05, ECON1, 0x03);//setbit
				ENC28_writeOp(0x04, ECON1, 0x03);//clearbit
				
	//ENC28_writeReg8(0x04, MAC_1);
		ENC28_writeOp(0x02, 0x04,MAC_1);
	//ENC28_writeReg8(MAADR2, MAC_2);
		ENC28_writeOp(0x02, 0x05,MAC_2);
	//ENC28_writeReg8(MAADR3, MAC_3);
		ENC28_writeOp(0x02, 0x02,MAC_3);
	//ENC28_writeReg8(MAADR4, MAC_4);
		ENC28_writeOp(0x02, 0x03,MAC_4);
	//ENC28_writeReg8(MAADR5, MAC_5);
		ENC28_writeOp(0x02, 0x00,MAC_5);
	//ENC28_writeReg8(MAADR6, MAC_6);
		ENC28_writeOp(0x02, 0x01,MAC_6);
	
//	dataWatch8 = ENC28_readReg8(MAADR1);
//	dataWatch8 = ENC28_readReg8(MAADR2);
//	dataWatch8 = ENC28_readReg8(MAADR3);
//	dataWatch8 = ENC28_readReg8(MAADR4);
//	dataWatch8 = ENC28_readReg8(MAADR5);
//	dataWatch8 = ENC28_readReg8(MAADR6);

	debug = ENC28_readOp(0x00, 0x04);
	debug = ENC28_readOp(0x00, 0x05);
	debug = ENC28_readOp(0x00, 0x02);
	debug = ENC28_readOp(0x00, 0x03);
	debug = ENC28_readOp(0x00, 0x00);
	debug = ENC28_readOp(0x00, 0x01);
	
		/*
		//test read/write 16b but faild
		debug = ENC28_readReg16(ERXST);
		ENC28_writeReg16(ERXST, RXSTOP_INIT);
		debug = ENC28_readReg16(ERXST);
		debug = ENC28_readReg16(ERXRDPT);
		debug = ENC28_readReg16(ERXWRPT);
		*/
		
		/*
		ENC28_Init();
    */
		
		/* Close SPI0 */
    SPI_Close(SPI0);

    while(1)
			{
			}
			
}

void SYS_Init(void)
{

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable external 12MHz XTAL */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Switch HCLK clock source to HXT and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT, CLK_CLKDIV_HCLK(1));

    /* Select HXT as the clock source of UART0 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /* Select HCLK as the clock source of SPI0 */
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL1_SPI0_S_HCLK, MODULE_NoMsk);

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(SPI0_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

    /* Setup SPI0 multi-function pins */
    SYS->GPC_MFP &= ~(SYS_GPC_MFP_PC0_Msk | SYS_GPC_MFP_PC1_Msk | SYS_GPC_MFP_PC2_Msk | SYS_GPC_MFP_PC3_Msk);
    SYS->GPC_MFP |= SYS_GPC_MFP_PC0_SPI0_SS0 | SYS_GPC_MFP_PC1_SPI0_CLK | SYS_GPC_MFP_PC2_SPI0_MISO0 | SYS_GPC_MFP_PC3_SPI0_MOSI0;
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PC0_Msk | SYS_ALT_MFP_PC1_Msk | SYS_ALT_MFP_PC2_Msk | SYS_ALT_MFP_PC3_Msk);
    SYS->ALT_MFP |= SYS_ALT_MFP_PC0_SPI0_SS0 | SYS_ALT_MFP_PC1_SPI0_CLK | SYS_ALT_MFP_PC2_SPI0_MISO0 | SYS_ALT_MFP_PC3_SPI0_MOSI0;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();
}

void SPI_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Configure as a master, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 2MHz */
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 16, 2000000);

    /* Enable the automatic hardware slave select function. Select the SS pin and configure as low-active. */
    SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS_ACTIVE_LOW);
		//SPI_DisableAutoSS(SPI0);
}
void delay1(uint32_t time)
	{
		while(time--);
	}
/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/

