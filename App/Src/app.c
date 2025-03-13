#include "app.h"
#include "gpio.h"
#include <stdio.h>

#include <string.h>
#include <stdbool.h>
#include <w5500_spi.h>
#include <wizchip_conf.h>
#include <socket.h>
#include "ethernet.h"
//#include "my_net.h"

uint8_t server_ip[4]={192,168,0,100};//put IP address of your PC here
#define SERVER_PORT 5000

void app()
{
    int32_t result;//to store result(return value) of socket functions
    setbuf(stdout, NULL);//prevent buffering of printf,immediate flush to USART
    printf("A simple UDP client example using w5500\r\n");
    printf("---------------------------------------\r\n\r\n");
    EthernetInit();
    //The return value of socket() call is the socket number if success
    //we are using socket number 1 so it should return 1 for success
    //The 3rd argument is the local port, when a client is written
    //better give 0 here, that will use a random available port
    if(socket(1, Sn_MR_UDP, 0, 0)==1)
    {
        printf("\r\nUDP Socket Created Successfully\r\n");
    }
    else
    {
        printf("\r\nCannot create socket\r\n");
        while(1);
    }
    //net_ini();
    while (1)
    {
        char* message = "Hello from UDP! Client on STM32 was launched !!!\r\n";
        result=sendto(1, (uint8_t *)message, strlen(message), server_ip,SERVER_PORT);
        if(result<0)
        {
            //error
            printf("Data sending failed!\r\n");
        }
        else
        {
            //success
            printf("Data sent successfully!\r\n");
            printf("Written %ld bytes to socket\r\n", result);//on success it returns number of bytes written to socket
        }

        HAL_Delay(1000);
        //net_poll();
        //HAL_Delay(500);
    }
}