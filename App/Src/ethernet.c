/*
 * ethernet.c
 *
 *  Created on: Feb 21, 2025
 *      Author: gmail
 */
#include <stdbool.h>
#include <stdio.h>
#include <w5500_spi.h>
#include <wizchip_conf.h>

wiz_NetInfo gWIZNETINFO = {
		.mac = { 0x80, 0x34, 0x28, 0x74, 0xA5, 0xCB },//MSB - LSB
		.ip ={ 192, 168, 0, 197 },
		.sn = { 255, 255, 255, 0 },
		.gw ={ 192, 168, 0, 197 },
		.dns = { 8, 8, 8, 8 },
		.dhcp = NETINFO_STATIC };

void PHYStatusCheck(void);
void PrintPHYConf(void);

/*
 * This functions handles the complete initialization of
 * Ethernet peripheral (w5500). It performs all low level
 * initialization required.
 */
void EthernetInit()
{
	W5500Init();

	ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);

	//Configure PHY by software
	wiz_PhyConf phyconf;

	phyconf.by=PHY_CONFBY_SW;
	phyconf.duplex=PHY_DUPLEX_FULL;
	phyconf.speed=PHY_SPEED_10;
	phyconf.mode=PHY_MODE_AUTONEGO;//best to go with auto-negotiation

	ctlwizchip(CW_SET_PHYCONF, (void*) &phyconf);
	//*** End Phy Configuration

	PHYStatusCheck();//Wait for cable plug-in event
	PrintPHYConf();
}

void PHYStatusCheck(void)
{
	uint8_t tmp;

	do
	{
		printf("\r\nChecking Ethernet Cable Presence ...");
		ctlwizchip(CW_GET_PHYLINK, (void*) &tmp);

		if(tmp == PHY_LINK_OFF)
		{
			printf("NO Cable Connected!");
			HAL_Delay(1500);
		}
	}while(tmp == PHY_LINK_OFF);

	printf("Good! Cable got connected!");

}

void PrintPHYConf(void)
{
	wiz_PhyConf phyconf;

	ctlwizchip(CW_GET_PHYCONF, (void*) &phyconf);

	if(phyconf.by==PHY_CONFBY_HW)
	{
		printf("\r\nPHY Configured by Hardware Pins");
	}
	else
	{
		printf("\r\nPHY Configured by Registers");
	}

	if(phyconf.mode==PHY_MODE_AUTONEGO)
	{
		printf("\r\nAutonegotiation Enabled");
	}
	else
	{
		printf("\r\nAutonegotiation NOT Enabled");
	}

	if(phyconf.duplex==PHY_DUPLEX_FULL)
	{
		printf("\r\nDuplex Mode: Full");
	}
	else
	{
		printf("\r\nDuplex Mode: Half");
	}

	if(phyconf.speed==PHY_SPEED_10)
	{
		printf("\r\nSpeed: 10Mbps\r\n");
	}
	else
	{
		printf("\r\nSpeed: 100Mbps\r\n");
	}
}


