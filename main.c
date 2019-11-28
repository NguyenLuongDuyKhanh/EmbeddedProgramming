#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "ip_arp_udp_tcp.h"
#include "ENC28J60.H"
#include "net.h"

#define PSTR(s) s
/* Khai bao cac bien*/
/*************************************************************************************/
static uint8_t mymac[6] ={0x00,0x1b,0x77,0x71,0x48,0xf0}; 			//dia chi mac enc28j60
static uint8_t myip[4] ={192, 168, 1, 125};                    //dia chi IP cua enc28j60

static uint8_t dis_mac[6] = {0x1C,0x65,0x9D,0x58,0x97,0xF3}; // dia chi mac may tinh // "ipconfig /all" roi tim cai MAC cua card WIfi ay
static uint8_t dis_ip[4] = {192,168,1,116};                   // IP may tinh

#define BUFFER_SIZE 500//400
static uint8_t buf[BUFFER_SIZE + 1];
uint8_t ARP[42] = {0xff , 0xff, 0xff, 0xff, 0xff, 0xff,//Destination MAC set as boardcast
										0x74, 0x69, 0x69, 0x2d, 0x30, 0x36,//Source MAC 
										0x08, 0x06, 0x00, 0x01, //Ethernettype(0x0806=ARP)/HTYPE(0x01 Eth)
										0x08, 0x00, 0x06, 0x04, 0x00, 0x01,//PTYPE(0x08IP)/HLEN(0x0604=42)/Oper(0x01)Request
										0x74, 0x69, 0x69, 0x2d, 0x30, 0x36, //SenderMAC = Source MAC
										0xc0, 0xa8, 0x01, 119,//SenderIP
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//TargetMAC
										0xc0, 0xa8, 0x01, 1};//PADING
/**************************************************************************************/

/* Function prototype declaration */
void SYS_Init(void);
void SPI_Init(void);

int main(void)
{
		uint8_t a;

    /* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);
		
    /* Init SPI */
    SPI_Init();

	GPIO_SetMode(PB,BIT13,GPIO_PMD_OUTPUT);
	GPIO_SetMode(PA,BIT14,GPIO_PMD_OUTPUT);
	
	
	//Uncomment code duoi day no se bi loop hoai trong ham Init/PhyWrite : while (enc28j60Read(MISTAT) & MISTAT_BUSY)
	// Block1
	
		enc28j60Init(mymac);
		delay(1500);
		enc28j60PhyWrite(PHLCON, 0x476);
   //enc28j60clkout(2); // Thay doi clkout 6.25MHz den 12.5MHz
		
		delay(1500);// giu cham de khi ngat nguon va bat lai khong phai an nut NRST
	
		init_ip_arp_udp_tcp(mymac, myip);


    while(1)
			{			
		
				PB13^=1; // Nay de check xem no co chay vao main de blink k			
		// Block2, sau khi uncomment block 1 thi block 2 no se gui goi UDP den dia chi IP may tinh (192.168.1.16)
				
//		buf[UDP_DATA_P]='X';
//		buf[UDP_DATA_P+1]='I';
//		buf[UDP_DATA_P+2]='U';
//		send_udp_prepare(buf,5001,dis_ip,5001,dis_mac);	
//		send_udp_transmit(buf,3);
		
			enc28j60PacketSend(42, ARP);
				delay(5000);
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
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, 2000000);

    /* Enable the automatic hardware slave select function. Select the SS pin and configure as low-active. */
    //SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS_ACTIVE_LOW);
		SPI_DisableAutoSS(SPI0);
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/

