#include "my_w5500.h"
//-----------------------------------------------
#define RST_W5500_Pin GPIO_PIN_3
#define RST_W5500_GPIO_Port GPIOA
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart2;
//-----------------------------------------------
extern char str1[60];
tcp_prop_ptr tcpprop;
//-----------------------------------------------
uint8_t macaddr[6]=MAC_ADDR;
extern uint8_t ipaddr[4];
extern uint8_t ipgate[4];
extern uint8_t ipmask[4];
extern uint16_t local_port;
//-----------------------------------------------
static void Error(void)
{
  HAL_UART_Transmit(&huart2,(uint8_t*)"Error!rn",8,0x1000);
  //CDC_Transmit_FS((uint8_t*)"Error!rn", 8,0x1000);
}
//-----------------------------------------------
void w5500_writeReg(uint8_t op, uint16_t addres, uint8_t data)
{
  uint8_t buf[] = {addres >> 8, addres, op|(RWB_WRITE<<2), data};
  SS_SELECT();
  HAL_SPI_Transmit(&hspi1, buf, 4, 0xFFFFFFFF);
  SS_DESELECT();
}
//-----------------------------------------------
uint8_t w5500_readReg(uint8_t op, uint16_t addres)
{
  uint8_t data;
  uint8_t wbuf[] = {addres >> 8, addres, op, 0x0};
  uint8_t rbuf[4];
  SS_SELECT();
  HAL_SPI_TransmitReceive(&hspi1, wbuf, rbuf, 4, 0xFFFFFFFF);
  SS_DESELECT();
  data = rbuf[3];
  return data;
}
//-----------------------------------------------
void OpenSocket(uint8_t sock_num, uint16_t mode)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, Sn_MR, mode);
  w5500_writeReg(opcode, Sn_CR, 0x01);
}
//-----------------------------------------------
void SocketInitWait(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_readReg(opcode, Sn_SR)==SOCK_INIT)
    {
      break;
    }
  }
}
//-----------------------------------------------
void ListenSocket(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, Sn_CR, 0x02); //LISTEN SOCKET
}
//-----------------------------------------------
void SocketListenWait(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_readReg(opcode, Sn_SR)==SOCK_LISTEN)
    {
      break;
    }
  }
}
//-----------------------------------------------
void w5500_ini(void)
{
  uint8_t dtt=0;
  uint8_t opcode=0;
  //Hard Reset
  HAL_GPIO_WritePin(RST_W5500_GPIO_Port, RST_W5500_Pin, GPIO_PIN_RESET);
  HAL_Delay(70);
  HAL_GPIO_WritePin(RST_W5500_GPIO_Port, RST_W5500_Pin, GPIO_PIN_SET);
  HAL_Delay(70);
  //Soft Reset
  opcode = (BSB_COMMON<<3)|OM_FDM1;
  w5500_writeReg(opcode, MR, 0x80);
  HAL_Delay(100);
  //Configute Net
  w5500_writeReg(opcode, SHAR0,macaddr[0]);
  w5500_writeReg(opcode, SHAR1,macaddr[1]);
  w5500_writeReg(opcode, SHAR2,macaddr[2]);
  w5500_writeReg(opcode, SHAR3,macaddr[3]);
  w5500_writeReg(opcode, SHAR4,macaddr[4]);
  w5500_writeReg(opcode, SHAR5,macaddr[5]);
  w5500_writeReg(opcode, GWR0,ipgate[0]);
  w5500_writeReg(opcode, GWR1,ipgate[1]);
  w5500_writeReg(opcode, GWR2,ipgate[2]);
  w5500_writeReg(opcode, GWR3,ipgate[3]);
  w5500_writeReg(opcode, SUBR0,ipmask[0]);
  w5500_writeReg(opcode, SUBR1,ipmask[1]);
  w5500_writeReg(opcode, SUBR2,ipmask[2]);
  w5500_writeReg(opcode, SUBR3,ipmask[3]);
  w5500_writeReg(opcode, SIPR0,ipaddr[0]);
  w5500_writeReg(opcode, SIPR1,ipaddr[1]);
  w5500_writeReg(opcode, SIPR2,ipaddr[2]);
  w5500_writeReg(opcode, SIPR3,ipaddr[3]);
  //Настраиваем сокет 0
  opcode = (BSB_S0<<3)|OM_FDM1;
  w5500_writeReg(opcode, Sn_PORT0,local_port>>8);
  w5500_writeReg(opcode, Sn_PORT1,local_port);
  //инициализируем активный сокет
  tcpprop.cur_sock = 0;
  //Открываем сокет 0
  OpenSocket(0,Mode_TCP);
  SocketInitWait(0);
  //Начинаем слушать сокет
  ListenSocket(0);
  SocketListenWait(0);
  HAL_Delay(500);
  //Посмотрим статусы
  opcode = (BSB_S0<<3)|OM_FDM1;
  dtt = w5500_readReg(opcode, Sn_SR);
  sprintf(str1,"First Status Sn0: 0x%02X\r\n",dtt);
  HAL_UART_Transmit(&huart2,(uint8_t*)str1,strlen(str1),0x1000);
  //CDC_Transmit_FS((uint8_t*)str1, strlen(str1));
}

