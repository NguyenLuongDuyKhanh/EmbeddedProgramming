# EmbeddedProgramming
Hi, this tutorial to guide you guys to make a game Ship Shooter, for image, please watch video...

## Over view
This project make a couple board with the same element: a keyboard, NUC131 as MCU, LCD display, ENC28J60 as the connector to ethernet.
About the software, it had implemented FreeRTOS - a customize port to deal with 3 threads: UDP packages sent, keyboard scan and LCD display.

## RTOS

## Enthernet
We use the ENC28J60 to sent UDP packages throuth ethernet.
**Note:** These capture in this section due by Salease Analyzer 24Mhz
ENC28J60 has no protocol supported by itself, 8KByte transmit/recive buffer (share) and communicate with MCU via SPI protocol.

If you have not known about SPI protocol, please get it [SPI]
To communicate with ENC28J60 you must to connect him with MCU via 4 wires SPI at frist (of course =))). Note that the SPI must to config **exacly** the requirement (at 4.1 ENC28J60 datasheet) and the tranfers data is the copy of instruction set - which you will see later.

At the certain, we config the SPI protocol like below

```
* Configure as a master, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
/* Set IP clock divider. SPI clock rate = 2MHz */
SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, 2000000);
```

Be carefull with auto select slave mode (if your MCU has) because some command need 16 clocks or 24 clock so it is a big deal if we use the auto select slave mode.
With the SPI, we can sent each of 6 commands (4.2 datasheet) to ENC28J60 

ENC28J60 has 3 kind of programable memory:
**Control register**
This is 4 bank 8 bit register control the ENC28J60.
We can use 5 command (exclude Soft reset command effect the whole system) with those register.
For example, the below capture show the result of signal on SPI bus when excute the Read Control Register (CRC) with ERDPTL register.
![](https://github.com/NguyenLuongDuyKhanh/EmbeddedProgramming/blob/master/img/readecon.PNG)
MOSI pin write 0x00 that mean the CRC command (000) to ERDPTL address (00000000).
At the reset status, the defaul value of ERDPTL is 1111 1010 so the output signal we recive is exacly 1111 1010.
You can see the min address for each bank is 0x00 and the max address is 0x1F, that mean the 3 MSB nerver used, so we use them with mask to detect bank number and MAC/MIL register.
For example, in source code MICON register address defined like this
```
      #define MICON            (0x11|0x40|0x80)
```
The frist 5 bit is the address of this register, the next 2 bit is use for the bank number, the last bit mean the register is MAC/MIL if high, and the opposite is not (it sound helpfull after a few line).
In the certain case (0b 1101 0001) the address is 0x11 in bank 0x04 and be a MIL register.



**PHY Register**
Is a group 16 bit control the physical layer of ENC28J60.
There is no command work with 16 bit register in ENC28J60 instruction set. So the way is use 5 basic command with MIL register in the control register (that why we need MAC/MIL mask in register address)
We have a n example on read/write MAC/MIL register for you.
![](https://github.com/NguyenLuongDuyKhanh/EmbeddedProgramming/blob/master/img/readMac.PNG)

![](https://github.com/NguyenLuongDuyKhanh/EmbeddedProgramming/blob/master/img/readMadfdfc.PNG)

**Ethernet Buffer**
Is the exact bit by bit signal write on the RJ45 bus when active. If capture the package by using WireShark, we will recive the acxacly value of this buffer.
It have 8Kbyte while the maximum size of a [TCP/IP buffer] transfer frame is 8KB.

## UDP protocol
14 byte for Ethernet header
20 byte for IP header
8 byte for UDP header
8 byte for data payload
...that all we need in a data package between 2 board

First of all, we add the IP and MAC to transfer buffer, these only wrote one because IP and MAC nerver change when playing
```
static uint8_t mymac[6] ={0x00,0x1b,0x77,0x71,0x48,0xf0}; 			//dia chi mac enc28j60
static uint8_t myip[4] ={192, 168, 1, 125};                    //dia chi IP cua enc28j60

static uint8_t dis_mac[6] = {0x1C,0x65,0x9D,0x58,0x97,0xF3}; // dia chi mac may tinh // "ipconfig /all" roi tim cai MAC cua card WIfi ay
static uint8_t dis_ip[4] = {192,168,1,116}; 
```
After each transmit/recive sequence, exclude the data we must to compute the checksum to, for the detail see [check sum]
##LCD and GPIO

[check sum]: https://www.netfor2.com/checksum.html
[UDP header]: https://tailamblog.wordpress.com/2017/08/10/cau-truc-ip-packet-header/
[Ether Type]: https://en.wikipedia.org/wiki/EtherType
[UDP protocol]: https://vi.wikipedia.org/wiki/UDP
[IpV4]: https://vi.wikipedia.org/wiki/IPv4
[SPI]: https://en.wikipedia.org/wiki/Serial_Peripheral_Interface
[TCP/IP buffer]: https://www.ibm.com/support/knowledgecenter/SSAW57_8.5.5/com.ibm.websphere.nd.multiplatform.doc/ae/tprf_tunetcpip.html
[KhanhNguyen](https://github.com/NguyenLuongDuyKhanh/)
[QuanTongAnh](https://github.com/phungorquan)
