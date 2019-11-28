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
uint8_t ARP[42] = {0xff , 0xff, 0xff, 0xff, 0xff, 0xff,//Destination MAC set as boardcast
										0x74, 0x69, 0x69, 0x2d, 0x30, 0x36,//Source MAC 
										0x08, 0x06, 0x00, 0x01, //Ethernettype(0x0806=ARP)/HTYPE(0x01 Eth)
										0x08, 0x00, 0x06, 0x04, 0x00, 0x01,//PTYPE(0x08IP)/HLEN(0x0604=42)/Oper(0x01)Request
										0x74, 0x69, 0x69, 0x2d, 0x30, 0x36, //SenderMAC = Source MAC
										0xc0, 0xa8, 0x01, 119,//SenderIP
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//TargetMAC
										0xc0, 0xa8, 0x01, 1};//PADING
/* ------------- */
/* Main function */
/* ------------- */
int main(void)
{
		uint32_t debug8;
	uint32_t delay;
    uint32_t u32DataCount, u32TestCount, u32Err;
		
    /* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();
		GPIO_SetMode(PA, BIT14, GPIO_PMD_OUTPUT);
	GPIO_SetMode(PB, BIT13, GPIO_PMD_OUTPUT);
	PB13=0;
	PA14=1;
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
	
		// (4): Initialise RX and TX buffer size
	ENC28_writeOp(0x05, ECON1, 0x03);//clear
	ENC28_writeOp(0x04, ECON1, 0x00);//set
	
	//ENC28_writeReg16(ERXST, RXSTART_INIT);
	ENC28_writeOp(0x02, ERXND, RXSTART_INIT&0xff);
	ENC28_writeOp(0x02, ERXND+1, RXSTART_INIT>>8);
	//ENC28_writeReg16(ERXND, RXSTOP_INIT);
	ENC28_writeOp(0x02, ERXND, RXSTOP_INIT&0xff);
	ENC28_writeOp(0x02, ERXND+1, RXSTOP_INIT>>8);
	

	//ENC28_writeReg16(ETXST, TXSTART_INIT);
	ENC28_writeOp(0x02, ETXST, TXSTART_INIT&0xff);
	ENC28_writeOp(0x02, ETXST+1, TXSTART_INIT>>8);
  //ENC28_writeReg16(ETXND, TXSTOP_INIT);
	ENC28_writeOp(0x02, ETXND, TXSTOP_INIT&0xff);
	ENC28_writeOp(0x02, ETXND+1, TXSTOP_INIT>>8);
	
	//ENC28_writeReg16(ERXRDPT, RXSTART_INIT);
	ENC28_writeOp(0x02, ERXRDPT, RXSTART_INIT&0xff);
	ENC28_writeOp(0x02, ERXRDPT+1, RXSTART_INIT>>8);
	//ENC28_writeReg16(ERXWRPT, RXSTART_INIT);
	ENC28_writeOp(0x02, ERXWRPT, RXSTART_INIT&0xff);
	ENC28_writeOp(0x02, ERXWRPT+1, RXSTART_INIT>>8);
	

		// (5): Reviece buffer filters
	ENC28_writeOp(0x05, ECON1, 0x03);//clear
	ENC28_writeOp(0x04, ECON1, 0x01);//setbit
	//ENC28_writeReg8(ERXFCON, ERXFCON_UCEN|ERXFCON_ANDOR|ERXFCON_CRCEN);
	ENC28_writeOp(0x02, ERXFCON, ERXFCON_UCEN|ERXFCON_ANDOR|ERXFCON_CRCEN);

		// (6): MAC Control Register 1
	//ENC28_writeReg8(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS|MACON1_PASSALL);
	ENC28_writeOp(0x05, ECON1, 0x03);//setbit
	ENC28_writeOp(0x04, ECON1, 0x02);//clearbit
	//ENC28_writeOp(0x02, MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS|MACON1_PASSALL);
	ENC28_writeOp(0x02, 0x00, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS|MACON1_PASSALL);
	
	// (7): MAC Control Register 3
	//ENC28_writeOp(0x04, MACON3,MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
	ENC28_writeOp(0x04, 0x02,MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_HFRMEN);
	
	  // (8): NON/Back to back gap
	ENC28_writeOp(0x05, ECON1, 0x03);//setbit
	ENC28_writeOp(0x04, ECON1, 0x02);//clearbit
	
	//ENC28_writeReg16(MAIPG, 0x0C12);  // NonBackToBack gap
	ENC28_writeOp(0x02, MAIPG, 0x0C12&0xff);
	ENC28_writeOp(0x02, MAIPG+1, 0x0C12>>8);
	
	//ENC28_writeReg8(MABBIPG, 0x12);  // BackToBack gap
	ENC28_writeOp(0x02, MABBIPG, ERXFCON_UCEN|ERXFCON_ANDOR|ERXFCON_CRCEN);
	
	// (9): Set Maximum framelenght
	ENC28_writeOp(0x05, ECON1, 0x03);//setbit
	ENC28_writeOp(0x04, ECON1, 0x02);//clearbit
	
	//ENC28_writeReg16(MAMXFL, MAX_FRAMELEN);	// Set Maximum frame length (any packet bigger will be discarded)
	ENC28_writeOp(0x02, MAMXFL, MAX_FRAMELEN&0xff);
	ENC28_writeOp(0x02, MAMXFL+1, MAX_FRAMELEN>>8);
	
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

	debug = ENC28_readOp(0x00, 0x04);
	debug = ENC28_readOp(0x00, 0x05);
	debug = ENC28_readOp(0x00, 0x02);
	debug = ENC28_readOp(0x00, 0x03);
	debug = ENC28_readOp(0x00, 0x00);
	debug = ENC28_readOp(0x00, 0x01);


	//**********Advanced Initialisations************//
	//ENC28_writePhy(PHLCON, PHLCON_LED);
	/*
		ENC28_writeReg8(MIREGADR, addr);
	ENC28_writeReg16(MIWR, data);
	while (ENC28_readReg8(MISTAT) & MISTAT_BUSY);
	*/
					ENC28_writeOp(0x05, ECON1, 0x03);//setbit
				ENC28_writeOp(0x04, ECON1, 0x02);//clearbit
				
		ENC28_writeOp(0x02,0x14, PHLCON);//MIREGADR
		ENC28_writeOp(0x02, 0x16, PHLCON_LED&0xff);//MIWR
		ENC28_writeOp(0x02, 0x17, PHLCON_LED>>8);
		delay1(100000);
		
	//ENC28_writePhy(PHLCON, PHLCON_LED);
	/*
		ENC28_writeReg8(MIREGADR, addr);
	ENC28_writeReg16(MIWR, data);
	while (ENC28_readReg8(MISTAT) & MISTAT_BUSY);
	*/
		ENC28_writeOp(0x02,0x14, PHCON2);
		ENC28_writeOp(0x02, 0x16, PHCON2_HDLDIS&0xff);
		ENC28_writeOp(0x02, 0x17, PHCON2_HDLDIS>>8);
		delay1(100000);
		
	ENC28_writeOp(0x04, ECON1, ECON1_RXEN);
	
		ENC28_writeOp(0x04, EIE, EIE_INTIE|EIE_PKTIE);
		ENC28_writeOp(0x04, EIR, EIR_PKTIF);

//	ENC28_writeOp(0x00,MIREGADR, addr);							// pass the PHY address to the MII
//	ENC28_writeOp(MICMD, MICMD_MIIRD);					// Enable Read bit
//	while (ENC28_readReg8(MISTAT) & MISTAT_BUSY);	// Poll for end of reading
//	ENC28_writeReg8(MICMD, 0x00);									// Disable MII Read
//	return ENC28_readReg8(MIRD) + (ENC28_readReg8(MIRD+1) << 8);

//////////////////////////////////testreadphy
//		//	ENC28_writeReg8(MIREGADR, addr);							// pass the PHY address to the MII
//		ENC28_writeOp(0x05, ECON1, 0x03);//clear
//		ENC28_writeOp(0x04, ECON1, 0x02);//setbit
//		
//		ENC28_writeOp(0x02, 0x14, 0x02);							// MIREGADRpass the PHY address to the MII
//		
//		//ENC28_writeOp(0x02, MICMD, MICMD_MIIRD);					// Enable Read bit
//		ENC28_writeOp(0x02, 0x12, MICMD_MIIRD);
//		delay1(10000);
//		ENC28_writeOp(0x02, 0x12, 0x00);	
//		delay1(10000);
//		
//		//readmistat
//		ENC28_writeOp(0x05, ECON1, 0x03);//clear
//		ENC28_writeOp(0x04, ECON1, 0x02);//setbit
//		
//		SPI0->CNTRL |= 24<<SPI_CNTRL_TX_BIT_LEN_Pos;
//		PA14=0;
//		/* Write to TX register */
//    SPI_WRITE_TX(SPI0, (0x00<<5|0x18)<<16);
//    /* Trigger SPI data transfer */
//    SPI_TRIGGER(SPI0);
//    /* Check SPI0 busy status */
//    while(SPI_IS_BUSY(SPI0));
//		result[0] = SPI_READ_RX(SPI0);
//		PA14=1;
//		//readmistat
//		
//				//readmistat
//		ENC28_writeOp(0x05, ECON1, 0x03);//clear
//		ENC28_writeOp(0x04, ECON1, 0x02);//setbit
//		
//		SPI0->CNTRL |= 24<<SPI_CNTRL_TX_BIT_LEN_Pos;
//		PA14=0;
//		/* Write to TX register */
//    SPI_WRITE_TX(SPI0, (0x00<<5|0x19)<<16);
//    /* Trigger SPI data transfer */
//    SPI_TRIGGER(SPI0);
//    /* Check SPI0 busy status */
//    while(SPI_IS_BUSY(SPI0));
//						result[1] =SPI_READ_RX(SPI0);
//		PA14=1;
//		//readmistat

//////////////////////////////////testreadphy

//////////////////////////////////testreadphy
//		//	ENC28_writeReg8(MIREGADR, addr);							// pass the PHY address to the MII
//		ENC28_writeOp(0x05, ECON1, 0x03);//clear
//		ENC28_writeOp(0x04, ECON1, 0x02);//setbit
//		
//		ENC28_writeOp(0x02, 0x14, 0x14);							// MIREGADRpass the PHY address to the MII
//		
//		//ENC28_writeOp(0x02, MICMD, MICMD_MIIRD);					// Enable Read bit
//		ENC28_writeOp(0x02, 0x12, MICMD_MIIRD);
//		delay1(10000);
//		ENC28_writeOp(0x02, 0x12, 0x00);	
//		delay1(10000);
//		
//		//readmistat
//		ENC28_writeOp(0x05, ECON1, 0x03);//clear
//		ENC28_writeOp(0x04, ECON1, 0x02);//setbit
//		
//		SPI0->CNTRL |= 24<<SPI_CNTRL_TX_BIT_LEN_Pos;
//		PA14=0;
//		/* Write to TX register */
//    SPI_WRITE_TX(SPI0, (0x00<<5|0x18)<<16);
//    /* Trigger SPI data transfer */
//    SPI_TRIGGER(SPI0);
//    /* Check SPI0 busy status */
//    while(SPI_IS_BUSY(SPI0));
//		result[0] = SPI_READ_RX(SPI0);
//		PA14=1;
//		//readmistat
//		
//				//readmistat
//		ENC28_writeOp(0x05, ECON1, 0x03);//clear
//		ENC28_writeOp(0x04, ECON1, 0x02);//setbit
//		
//		SPI0->CNTRL |= 24<<SPI_CNTRL_TX_BIT_LEN_Pos;
//		PA14=0;
//		/* Write to TX register */
//    SPI_WRITE_TX(SPI0, (0x00<<5|0x19)<<16);
//    /* Trigger SPI data transfer */
//    SPI_TRIGGER(SPI0);
//    /* Check SPI0 busy status */
//    while(SPI_IS_BUSY(SPI0));
//		result[1] =SPI_READ_RX(SPI0);
//		PA14=1;
//		//readmistat

//////////////////////////////////testreadphy
		/*
		ENC28_Init();
    */
		
		/* Close SPI0 */
    //SPI_Close(SPI0);

	debug8= ENC28_readOp(0x00,0x1c)&0xff;

    while(1)
			{
				//delay1(50000);
					uint32_t debug5 = ENC28_readOp(0x00,0x00);
						ENC28_packetSend(42,ARP);
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
    //SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS_ACTIVE_LOW);
		SPI_DisableAutoSS(SPI0);
}
void delay1(uint32_t time)
	{
		while(time--);
	}
/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/

