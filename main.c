#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "ip_arp_udp_tcp.h"
#include "ENC28J60.H"
#include "net.h"
#include "loopback.h"
#include "socket.h"

#define PSTR(s) s
/* Function prototype declaration */
void SYS_Init(void);
void SPI_Init(void);

uint8_t ARP[42] = {0xff , 0xff, 0xff, 0xff, 0xff, 0xff,//Destination MAC set as boardcast
										0x74, 0x69, 0x69, 0x2d, 0x30, 0x36,//Source MAC 
										0x08, 0x06, 0x00, 0x01, //Ethernettype(0x0806=ARP)/HTYPE(0x01 Eth)
										0x08, 0x00, 0x06, 0x04, 0x00, 0x01,//PTYPE(0x08IP)/HLEN(0x0604=42)/Oper(0x01)Request
										0x74, 0x69, 0x69, 0x2d, 0x30, 0x36, //SenderMAC = Source MAC
										0xc0, 0xa8, 0x01, 119,//SenderIP
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//TargetMAC
										0xc0, 0xa8, 0x01, 1};//PADING

int main()
{
	uint8_t khanh[5]={'k','h','b','n','h'};
	/* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();
		GPIO_SetMode(PA,BIT14,GPIO_PMD_OUTPUT);
    /* Init SPI */
    SPI_Init();
//	ENC28J60_CSL();
//	//PA14=0;
//	
//	wizchip_spi_writebyte('a');
//	wizchip_spi_writeburst(khanh,5) ;	
//	
//	//PA14=1;
//	ENC28J60_CSH();
	printf("hi khanh");
	uint8_t destip[4] = 	{192, 168, 1, 103};
  uint16_t destport = 	5001;
	printf("hi khanh");
  loopback_tcpc(1, khanh,destip,destport);
	printf("hi khanh");
	while(1)
	{
		
	}
	return 0;
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
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, 2000000);

    /* Enable the automatic hardware slave select function. Select the SS pin and configure as low-active. */
    //SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS_ACTIVE_LOW);
		SPI_DisableAutoSS(SPI0);
}
/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/