#include "STM_ENC28_J60.h"
#include <stdio.h>
#include "NUC131.h"

static uint8_t Enc28_Bank;
uint8_t dataWatch8;
uint16_t dataWatch16;

static void delay(uint32_t time);
/*
Use with Read Control Register 	-RCR
*/
uint8_t ENC28_readOp(uint8_t oper, uint8_t addr)
{
		SPI0->CNTRL |= 16<<SPI_CNTRL_TX_BIT_LEN_Pos;
		PA14=0;

  /* Write to TX register */
  SPI_WRITE_TX(SPI0, ((oper<<5)| (addr))<<8);
  /* Trigger SPI data transfer */
  SPI_TRIGGER(SPI0);
  /* Check SPI0 busy status */
  while(SPI_IS_BUSY(SPI0));
PA14=1;
	return SPI_READ_RX(SPI0);
}


void ENC28_writeOp(uint8_t oper, uint8_t addr, uint8_t data)
{
PA14=0;
		SPI0->CNTRL |= 16<<SPI_CNTRL_TX_BIT_LEN_Pos;
		/* Write to TX register */
    SPI_WRITE_TX(SPI0, (oper<<5|addr)<<8|data);
    /* Trigger SPI data transfer */
    SPI_TRIGGER(SPI0);
    /* Check SPI0 busy status */
    while(SPI_IS_BUSY(SPI0));
PA14=1;
}
uint8_t ENC28_readReg8(uint8_t addr)
{
	ENC28_setBank(addr);
	return ENC28_readOp(ENC28_READ_CTRL_REG, addr);
}

void ENC28_writeReg8(uint8_t addr, uint8_t data)
{
	//ENC28_setBank(addr);
	ENC28_writeOp(ENC28_WRITE_CTRL_REG, addr, data);
}

uint16_t ENC28_readReg16( uint8_t addr)
{
	
	return ENC28_readReg8(addr) + (ENC28_readReg8(addr+1) << 8);
}
void ENC28_writeReg16(uint8_t addrL, uint16_t data)
{
	ENC28_writeReg8(addrL, (data));
	ENC28_writeReg8(addrL+1, (data >> 8));
}

void ENC28_setBank(uint8_t addr)
{

		ENC28_writeOp(ENC28_BIT_FIELD_CLR, ECON1, ECON1_BSEL1|ECON1_BSEL0);
		Enc28_Bank = addr & BANK_MASK;
    ENC28_writeOp(ENC28_BIT_FIELD_SET, ECON1, Enc28_Bank>>5);
}

void ENC28_writePhy(uint8_t addr, uint16_t data)
{
	ENC28_writeOp(0x05, ECON1, 0x03);//setbit
	ENC28_writeOp(0x04, ECON1, 0x02);//clearbit
	
	//ENC28_writeReg8(MIREGADR, addr);
	ENC28_writeOp(0x02, MIREGADR, addr);
	//ENC28_writeReg8(MIWR, data);
	ENC28_writeOp(0x02, MIWR, data);
	
	ENC28_writeOp(0x05, ECON1, 0x03);//setbit
	ENC28_writeOp(0x04, ECON1, 0x03);//clearbit
	
	//while (ENC28_readOp(0x00, MISTAT) & MISTAT_BUSY);
	
	SPI0->CNTRL |= 24<<SPI_CNTRL_TX_BIT_LEN_Pos;
  /* Write to TX register */
  SPI_WRITE_TX(SPI0, ((0x00<<5)| (MISTAT))<<16);
  /* Trigger SPI data transfer */
  SPI_TRIGGER(SPI0);
  /* Check SPI0 busy status */
  while(SPI_IS_BUSY(SPI0));

	while( (SPI_READ_RX(SPI0)&0xff00)&MISTAT_BUSY);
	

}

uint16_t ENC28_readPhy(uint8_t addr)
{
	//ENC28_writeReg8(MIREGADR, addr);							// pass the PHY address to the MII
	ENC28_writeOp(0x05, ECON1, 0x03);//setbit
	ENC28_writeOp(0x04, ECON1, 0x02);//clearbit
	ENC28_writeOp(0x02, MIREGADR, addr);
	
	//ENC28_writeReg8(MICMD, MICMD_MIIRD);					// Enable Read bit
	ENC28_writeOp(0x02, MICMD, MICMD_MIIRD);
	
	//while (ENC28_readReg8(MISTAT) & MISTAT_BUSY);	// Poll for end of reading
	ENC28_writeOp(0x05, ECON1, 0x03);//setbit
	ENC28_writeOp(0x04, ECON1, 0x03);//clearbit
	//while (ENC28_readOp(0x00,MISTAT) & MISTAT_BUSY);
		SPI0->CNTRL |= 24<<SPI_CNTRL_TX_BIT_LEN_Pos;
  /* Write to TX register */
  SPI_WRITE_TX(SPI0, ((0x00<<5)| (MISTAT))<<16);
  /* Trigger SPI data transfer */
  SPI_TRIGGER(SPI0);
  /* Check SPI0 busy status */
  while(SPI_IS_BUSY(SPI0));

	while( (SPI_READ_RX(SPI0)&0xff00)&MISTAT_BUSY);
	
	
	//ENC28_writeReg8(MICMD, 0x00);									// Disable MII Read
	ENC28_writeOp(0x02, MICMD, 0x00);//clearbit
	
	//return ENC28_readReg8(MIRD) + (ENC28_readReg8(MIRD+1) << 8);
	ENC28_writeOp(0x05, ECON1, 0x03);//setbit
	ENC28_writeOp(0x04, ECON1, 0x02);//clearbit
	
	uint8_t h;
	uint8_t l;
	
	//return ENC28_readOp(0x00,MIRD) + (ENC28_readOp(0x00,MIRD+1) << 8);
		SPI0->CNTRL |= 24<<SPI_CNTRL_TX_BIT_LEN_Pos;
		
  /* Write to TX register */
  SPI_WRITE_TX(SPI0, ((0x00<<5)| (MIRD))<<16);
  /* Trigger SPI data transfer */
  SPI_TRIGGER(SPI0);
  /* Check SPI0 busy status */
  while(SPI_IS_BUSY(SPI0));
	h=SPI_READ_RX(SPI0);
  /* Write to TX register */
  SPI_WRITE_TX(SPI0, ((0x00<<5)| (MIRD)+1)<<16);
  /* Trigger SPI data transfer */
  SPI_TRIGGER(SPI0);
  /* Check SPI0 busy status */
  while(SPI_IS_BUSY(SPI0));
	l=SPI_READ_RX(SPI0);
	return l;
}

void ENC28_Init(void)
{
	//PA14=1;
	// (1): Disable the chip CS pin
	delay(100);
	// (2): Perform soft reset to the ENC28J60 module
	ENC28_writeOp(ENC28_SOFT_RESET, 0, ENC28_SOFT_RESET);
	delay(100);
	
	// (3): Wait untill Clock is ready
	while(!ENC28_readOp(ENC28_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY);
	
	// (4): Initialise RX and TX buffer size
	ENC28_writeReg16(ERXST, RXSTART_INIT);
	ENC28_writeReg16(ERXND, RXSTOP_INIT);

	ENC28_writeReg16(ETXST, TXSTART_INIT);
  ENC28_writeReg16(ETXND, TXSTOP_INIT);
	
	ENC28_writeReg16(ERXRDPT, RXSTART_INIT);
	ENC28_writeReg16(ERXWRPT, RXSTART_INIT);
	
	dataWatch16 = ENC28_readReg16(ERXND);
	
	// (5): Reviece buffer filters
	ENC28_writeReg8(ERXFCON, ERXFCON_UCEN|ERXFCON_ANDOR|ERXFCON_CRCEN);
	dataWatch8 = ENC28_readReg8(ERXFCON);
	// (6): MAC Control Register 1
	ENC28_writeReg8(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS|MACON1_PASSALL);
	dataWatch8 = ENC28_readReg8(ERXFCON);
	// (7): MAC Control Register 3
	ENC28_writeOp(ENC28_BIT_FIELD_SET, MACON3,MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
  // (8): NON/Back to back gap
	ENC28_writeReg16(MAIPG, 0x0C12);  // NonBackToBack gap
	ENC28_writeReg8(MABBIPG, 0x12);  // BackToBack gap
	// (9): Set Maximum framelenght
	ENC28_writeReg16(MAMXFL, MAX_FRAMELEN);	// Set Maximum frame length (any packet bigger will be discarded)
	// (10): Set the MAC address of the device
	ENC28_writeReg8(MAADR1, MAC_1);
	ENC28_writeReg8(MAADR2, MAC_2);
	ENC28_writeReg8(MAADR3, MAC_3);
	ENC28_writeReg8(MAADR4, MAC_4);
	ENC28_writeReg8(MAADR5, MAC_5);
	ENC28_writeReg8(MAADR6, MAC_6);
	
	dataWatch8 = ENC28_readReg8(MAADR1);
	dataWatch8 = ENC28_readReg8(MAADR2);
	dataWatch8 = ENC28_readReg8(MAADR3);
	dataWatch8 = ENC28_readReg8(MAADR4);
	dataWatch8 = ENC28_readReg8(MAADR5);
	dataWatch8 = ENC28_readReg8(MAADR6);
	
	//**********Advanced Initialisations************//
	// (1): Initialise PHY layer registers
	ENC28_writePhy(PHLCON, PHLCON_LED);
	ENC28_writePhy(PHCON2, PHCON2_HDLDIS);
	// (2): Enable Rx interrupt line
	ENC28_setBank(ECON1);
	ENC28_writeOp(ENC28_BIT_FIELD_SET, ECON1, ECON1_RXEN);
//	ENC28_writeOp(ENC28_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
//	ENC28_writeOp(ENC28_BIT_FIELD_SET, EIR, EIR_PKTIF);
}

void ENC28_packetSend(uint16_t len, uint8_t* dataBuf)
{

	uint8_t retry = 0;
	
	while(1)
	{
		ENC28_writeOp(0x04, ECON1, ECON1_TXRST);
    ENC28_writeOp(0x05, ECON1, ECON1_TXRST);
		ENC28_writeOp(0x05, EIR, EIR_TXERIF|EIR_TXIF);
		
		
		// prepare new transmission 
		if(retry == 0)
		{
			ENC28_writeOp(0x05, ECON1, 0x03);//Clear
			
			//ENC28_writeReg16(EWRPT, TXSTART_INIT);
			ENC28_writeOp(0x02, EWRPT, TXSTART_INIT&0xff);
			ENC28_writeOp(0x02, EWRPT+1, TXSTART_INIT>>8);
			
			//ENC28_writeReg16(ETXND, TXSTART_INIT+len);
			ENC28_writeOp(0x02, ETXND, TXSTART_INIT&0xff);
			ENC28_writeOp(0x02, ETXND+1, TXSTART_INIT>>8);
			
			//ENC28_writeOp(ENC28_WRITE_BUF_MEM, 0, 0);  //line 485 enc28j60.cpp
			ENC28_writeOp(0x03, 0, 0);  //line 485 enc28j60.cpp
			ENC28_writeBuf(len, dataBuf);
		}
		
		uint32_t debug2 = ENC28_readOp(0x00,EIR);
		// initiate transmission
		ENC28_writeOp(0x04, ECON1, ECON1_TXRTS);
		uint32_t debug3 = ENC28_readOp(0x00,EIR);
		uint16_t count = 0;
		
		//while ((ENC28_readOp(0x00,EIR) & (EIR_TXIF | EIR_TXERIF)) == 0);// && ++count < 0xffff);
		uint32_t debug4 = ENC28_readOp(0x00,EIR);
		if (!(ENC28_readOp(0x00,EIR) & EIR_TXERIF))// && count < 0xffff) 
		{
       // no error; start new transmission
			PB13=1;
       break;
    }
		// cancel previous transmission if stuck
		PB13=1;
    ENC28_writeOp(0x05, ECON1, ECON1_TXRTS);
		break;
	}

}

void ENC28_writeBuf(uint16_t len, uint8_t* data)
{
	SPI0->CNTRL |= 8<<SPI_CNTRL_TX_BIT_LEN_Pos;
		PA14=0;
		uint8_t de =0;
		/* Write to TX register */
    SPI_WRITE_TX(SPI0, 0x7a);
    /* Trigger SPI data transfer */
    SPI_TRIGGER(SPI0);
    /* Check SPI0 busy status */
    while(SPI_IS_BUSY(SPI0));
	uint8_t ARP[42] = {0xff , 0xff, 0xff, 0xff, 0xff, 0xff,//Destination MAC set as boardcast
										0x74, 0x69, 0x69, 0x2d, 0x30, 0x36,//Source MAC 
										0x08, 0x06, 0x00, 0x01, //Ethernettype(0x0806=ARP)/HTYPE(0x01 Eth)
										0x08, 0x00, 0x06, 0x04, 0x00, 0x01,//PTYPE(0x08IP)/HLEN(0x0604=42)/Oper(0x01)Request
										0x74, 0x69, 0x69, 0x2d, 0x30, 0x36, //SenderMAC = Source MAC
										0xc0, 0xa8, 0x01, 119,//SenderIP
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//TargetMAC
										0xc0, 0xa8, 0x01, 1};//PADING
	for(int i =0;i<42;i++)
	{		
		de++;
		/* Write to TX register */
    SPI_WRITE_TX(SPI0, ARP[i]);
    /* Trigger SPI data transfer */
    SPI_TRIGGER(SPI0);
    /* Check SPI0 busy status */
    while(SPI_IS_BUSY(SPI0));
		de++;
	}
		PA14=1;
}
void delay(uint32_t time)
	{
		while(time--);
	}