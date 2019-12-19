//#include "mbed.h"
#include "WIZnetInterface.h"
 
#define USE_DHCP    1
 
#define LOOPBACKPORT    5000
 
const char * IP_Addr    = "192.168.11.194";
const char * IP_Subnet  = "255.255.255.0";
const char * IP_Gateway = "192.168.11.1";
unsigned char MAC_Addr[6] = {0x00,0x08,0xDC,0x12,0x34,0x56};
 
DigitalOut myled1(LED1);
Serial pc(USBTX, USBRX);
 
#ifdef TARGET_LPC11U68
SPI spi(P0_9,P0_8,P1_29);
WIZnetInterface ethernet(&spi,P0_2,P1_13);
#endif
 
int main() {
    
    //Set serial port baudrate speed: 115200
    pc.baud(115200);
    
    pc.printf("Start\r\n");
 
    char buffer[256];
    
    while(1)
    {
        #if USE_DHCP
        int ret = ethernet.init(MAC_Addr);
        #else
        int ret = ethernet.init(MAC_Addr,IP_Addr,IP_Subnet,IP_Gateway);
        #endif
        
        if (!ret) {
            pc.printf("Initialized, MAC: %s\r\n", ethernet.getMACAddress());
            ret = ethernet.connect();
            if (!ret) {
                pc.printf("IP: %s, MASK: %s, GW: %s\r\n",
                          ethernet.getIPAddress(), ethernet.getNetworkMask(), ethernet.getGateway());
            } else {
                pc.printf("Error ethernet.connect() - ret = %d\r\n", ret);
                exit(0);
            }
        } else {
            pc.printf("Error ethernet.init() - ret = %d\r\n", ret);
            exit(0);
        }
        
        TCPSocketServer server;
        server.bind(LOOPBACKPORT);
        server.listen();
        
        while (1) {
            pc.printf("\nWait for new connection...\r\n");
            TCPSocketConnection client;
            server.accept(client);
            client.set_blocking(false, 0); // Timeout=0.
            pc.printf("Connection from: %s\r\n", client.get_address());
            while (client.is_connected() == true) {
                int n = client.receive(buffer, sizeof(buffer));
                if(n > 0)
                    client.send_all(buffer, n);
                if(client.is_fin_received())
                    client.close();
            }
            pc.printf("Disconnected.\r\n");
        }
    }
}