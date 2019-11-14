#include <stdio.h>
#include "NUC131.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define PLL_CLOCK   50000000


void vTask1( void *pvParameters );
void vTask2( void *pvParameters );
void vTask3( void *pvParameters );


const char *pvTask1  = "Task1 is running.";
const char *pvTask2  = "Task2 is running.";
const char *pvTask3  = "Task3 is running.";
SemaphoreHandle_t xSemaphore = NULL; // ##############Add this line if use semaphore


void delay(int itime){
         
   int icompTime = 0;

   for(icompTime=itime*1000;icompTime!=0;icompTime--);
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Enable UART module clock */
    //CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source */
    //CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD(PB.0) and TXD(PB.1) */
    //SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    //SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

}

//void UART0_Init()
//{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART0 module */
//    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
//    UART_Open(UART0, 115200);
//}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();
		//GPIO_SetMode(PB, BIT13, GPIO_PMD_OUTPUT);
	
		//PB->PMD |= 0x44000000; // PB13,15 PUSHPULL OUTPUT , All the other pins INPUT
		//PA->PMD |= 0x1000000; // PA12 PUSHPULL OUTPUT , All the other pins INPUT
		
		GPIO_SetMode(PB, BIT13, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PB, BIT15, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PA, BIT13, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PA, BIT14, GPIO_PMD_INPUT);
		// Blinking slowly when don't press the button
	
	xTaskCreate(vTask1, /* Pointer to the function that implements the task. */
	"Task 1", /* Text name for the task.  This is to facilitate debugging only. */
        configMINIMAL_STACK_SIZE, /* Stack depth in words. */
	(void*)pvTask1,		/* We are not using the task parameter. */
	1,			/* This task will run at priority 1. */
	NULL );		/* We are not using the task handle. */
	
		xTaskCreate( vTask2, "Task 2", configMINIMAL_STACK_SIZE, (void*)pvTask2, 2, NULL );
		xTaskCreate( vTask3, "Task 3", configMINIMAL_STACK_SIZE, (void*)pvTask3, 3, NULL );

	xSemaphore = xSemaphoreCreateBinary();
        xSemaphoreGive( xSemaphore);

	
//    // Start RTOS scheduler
    vTaskStartScheduler();
	
				
			return 0;
}


void vTask1( void *pvParameters )
{
		while(1)
		{
			//xSemaphoreTake(xSemaphore,(TickType_t) portMAX_DELAY);
        PB13 ^= 1;
			//xSemaphoreGive(xSemaphore);
        vTaskDelay(1000/portTICK_RATE_MS);
    }
}
/*-----------------------------------------------------------*/
void vTask2( void *pvParameters )
{
		while(1)
		{//xSemaphoreGive(xSemaphore);
			//xSemaphoreTake(xSemaphore,(TickType_t) portMAX_DELAY);
        PA13 = 1;
			//xSemaphoreGive(xSemaphore);
        //vTaskDelay(2000/portTICK_RATE_MS);
    }
} 

void vTask3( void *pvParameters )
{

    while(1)
		{//xSemaphoreGive(xSemaphore);
			//xSemaphoreTake(xSemaphore,(TickType_t) portMAX_DELAY);
        while(PA14 == 0)
					{
						PA13=0;
					};
			//xSemaphoreGive(xSemaphore);
        //vTaskDelay(2000/portTICK_RATE_MS);
    }

}


