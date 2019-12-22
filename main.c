#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "ip_arp_udp_tcp.h"
#include "ENC28J60.H"
#include "net.h"
#include "loopback.h"
#include "socket.h"
#include <stdio.h>

#define PSTR(s) s
/* Function prototype declaration */
void SYS_Init(void);
void SPI_Init(void);

void cs_sel();
void cs_desel();
void 	wb(uint8_t wb);
uint8_t rb(void);

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
	uint8_t khanh[20]={'B','a','t','t','l','e','S','h','i','p','Q','u','a','n','K','h','a','n','h','\n'};
	int32_t revbuf[10];
	uint16_t distport = 5000;
	uint8_t distIP[] = {192,168,1,103};
  uint8_t retVal, sockStatus;
  int16_t rcvLen;
  uint8_t rcvBuf[20], bufSize[] = {2, 2, 2, 2};
	/* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();
		GPIO_SetMode(PA,BIT14,GPIO_PMD_OUTPUT);
    /* Init SPI */
    SPI_Init();
	
		reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
		reg_wizchip_spi_cbfunc(rb, wb);
	
		wizchip_init(bufSize, bufSize);
	  wiz_NetInfo netInfotest = { .mac 	= {0x00, 0, 0, 0, 0, 0},	// Mac address
                          .ip 	= {0, 0, 0, 0},					// IP address
                          .sn 	= {0, 0, 0, 0},					// Subnet mask
                          .gw 	= {0, 0, 0, 0}};					// Gateway address
	
  wiz_NetInfo netInfo = { .mac 	= {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},	// Mac address
                          .ip 	= {192, 168, 1, 125},					// IP address
                          .sn 	= {255, 255, 255, 0},					// Subnet mask
                          .gw 	= {192, 168, 1, 100}};					// Gateway address
	
  wizchip_setnetinfo(&netInfo);
  wizchip_getnetinfo(&netInfotest);
	
	socket(0, Sn_MR_TCP, 5000, 0);
													
	connect(0, distIP, distport);
	while(1)
		{				
				send(0, khanh, 20);
				recv(0, (uint32_t*)revbuf, 10);								
		}
		disconnect(0);
//	reconnect:
//  /* Open socket 0 as TCP_SOCKET with port 5000 */
//  if((retVal = socket(0, Sn_MR_TCP, 5000, 0)) == 0) {
//	  /* Put socket in LISTEN mode. This means we are creating a TCP server */
//	  if((retVal = listen(0)) == SOCK_OK) {
//		  /* While socket is in LISTEN mode we wait for a remote connection */
//		  while(sockStatus = getSn_SR(0) == SOCK_LISTEN)
//			  //HAL_Delay(100);
//		  /* OK. Got a remote peer. Let's send a message to it */
//		  while(1) {
//			  /* If connection is ESTABLISHED with remote peer */
//			  if(sockStatus = getSn_SR(0) == SOCK_ESTABLISHED) {
//				  uint8_t remoteIP[4];
//				  uint16_t remotePort;
//				  /* Retrieving remote peer IP and port number */
//				  getsockopt(0, SO_DESTIP, remoteIP);
//				  getsockopt(0, SO_DESTPORT, (uint8_t*)&remotePort);
//				  //sprintf(msg, CONN_ESTABLISHED_MSG, remoteIP[0], remoteIP[1], remoteIP[2], remoteIP[3], remotePort);
//				  //PRINT_STR(msg);
//				  /* Let's send a welcome message and closing socket */
////				  if(retVal = send(0, GREETING_MSG, strlen(GREETING_MSG)) == (int16_t)strlen(GREETING_MSG))
////					  //PRINT_STR(SENT_MESSAGE_MSG);
////				  else { /* Ops: something went wrong during data transfer */
////					  sprintf(msg, WRONG_RETVAL_MSG, retVal);
////					  //PRINT_STR(msg);
////				  }
//				  break;
//			  }
//			  else { /* Something went wrong with remote peer, maybe the connection was closed unexpectedly */
//				  //sprintf(msg, WRONG_STATUS_MSG, sockStatus);
//				  //PRINT_STR(msg);
//				  break;
//			  }
//		  }

//	  } //else /* Ops: socket not in LISTEN mode. Something went wrong */
//		  //PRINT_STR(LISTEN_ERR_MSG);
//  } else { /* Can't open the socket. This means something is wrong with W5100 configuration: maybe SPI issue? */
//	  //sprintf(msg, WRONG_RETVAL_MSG, retVal);
//	  //PRINT_STR(msg);
//  }

//  /* We close the socket and start a connection again */
//  disconnect(0);
//  close(0);
//  goto reconnect;
													
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

uint8_t rb(void)        
{
	SPI_WRITE_TX(SPI0, (uint8_t)0xff);
	/* Trigger SPI data transfer */
	SPI_TRIGGER(SPI0);
	/* Check SPI0 busy status */
	while(SPI_IS_BUSY(SPI0));
	return SPI_READ_RX(SPI0);
}

/**
 * @brief Default function to write in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
//void 	wizchip_spi_writebyte(uint8_t wb) {};
void 	wb(uint8_t wb)
 {
	/* Write to TX register */
  SPI_WRITE_TX(SPI0, (uint8_t)wb);
  /* Trigger SPI data transfer */
  SPI_TRIGGER(SPI0);
  /* Check SPI0 busy status */
  while(SPI_IS_BUSY(SPI0));
 }
 
void cs_sel() {
	PA14=0; //CS LOW
}
 
void cs_desel() {
	PA14=1; //CS HIGH
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/