#include "my_net.h"
//-----------------------------------------------
extern UART_HandleTypeDef huart2;
//-----------------------------------------------
uint8_t ipaddr[4]=IP_ADDR;
uint8_t ipgate[4]=IP_GATE;
uint8_t ipmask[4]=IP_MASK;
uint16_t local_port = LOCAL_PORT;
char str1[60]={0};
//-----------------------------------------------
void net_ini(void)
{
  w5500_ini();
}
//-----------------------------------------------
void packet_receive(void)
{

}
//-----------------------------------------------
void net_poll(void)
{
  packet_receive();
}
//-----------------------------------------------
