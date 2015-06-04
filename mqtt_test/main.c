/*Include: Board configuration*/
#include "IoTEVB.h"

/*Include: MCU peripheral Library*/
#include "stm32f10x_rcc.h"
#include "stm32f10x.h"

/*Include: W5500 iolibrary*/
#include "w5500.h"
#include "wizchip_conf.h"

#include "mqtt_interface.h"
#include "MQTTClient.h"

/*Include: MCU Specific W5500 driver*/
#include "W5500HardwareDriver.h"

/*Include: Standard IO Library*/
#include <stdio.h>

/*Socket number defines*/
#define TCP_SOCKET	0
#define UDP_SOCKET	1

extern uint8_t  DNS_SOCKET;    // SOCKET number for DNS

/*Port number defines*/
#define TCP_PORT 	60000
#define UDP_PORT 	60001

/*Receive Buffer Size define*/
#define BUFFER_SIZE	2048

/*Global variables*/
unsigned char targetIP[4] = {107,22,157,223};
unsigned int targetPort = 15210;

unsigned char TargetName[] = "m11.cloudmqtt.com";

uint8_t mac_address[6] = {};
wiz_NetInfo gWIZNETINFO = { .mac = {},
							.ip = {222, 98, 173, 249},
							.sn = {255, 255, 255, 192},
							.gw = {222, 98, 173, 254},
							.dns = {168, 126, 63, 1},
							.dhcp = NETINFO_STATIC};

struct opts_struct
{
	char* clientid;
	int nodelimiter;
	char* delimiter;
	enum QoS qos;
	char* username;
	char* password;
	char* host;
	int port;
	int showtopics;
} opts =
{
	(char*)"stdout-subscriber", 0, (char*)"\n", QOS0, "xfprwrvc", "0ks6T_RP33kF", (char*)"localhost", 1883, 0
};

void messageArrived(MessageData* md)
{
	unsigned char testbuffer[100];
	MQTTMessage* message = md->message;

	if (opts.showtopics)
	{
		memcpy(testbuffer,(char*)message->payload,(int)message->payloadlen);
		*(testbuffer + (int)message->payloadlen + 1) = "\n";
		printf("%s\r\n",testbuffer);
	}
	//	printf("%12s\r\n", md->topicName->lenstring.len, md->topicName->lenstring.data);
	if (opts.nodelimiter)
		printf("%.*s", (int)message->payloadlen, (char*)message->payload);
	else
		printf("%.*s%s", (int)message->payloadlen, (char*)message->payload, opts.delimiter);
	//fflush(stdout);
}


unsigned char tempBuffer[BUFFER_SIZE] = {};

int main(void)
{
	led_ctrl led1,led2;
	int i;
	int rc = 0;
	unsigned char buf[100];
	/*Usart initialization for Debug.*/
	USART1Initialze();
		printf("USART initialized.\n\r");

	I2C1Initialize();
		printf("I2C initialized.\n\r");

	MACEEP_Read(mac_address,0xfa,6);
	printf("Mac address\n\r");
	for(i = 0 ; i < 6 ; i++)
	{
		printf("%02x ",mac_address[i]);
	}
	printf("\n\r");

	/*LED initialization.*/
	led_initialize();
	led1 = led2 = ON;

	led2Ctrl(led2);
	led1Ctrl(led1);

	/*W5500 initialization.*/
	W5500HardwareInitilize();
		printf("W5500 hardware interface initialized.\n\r");

	W5500Initialze();
		printf("W5500 IC initialized.\n\r");

	/*Set network informations*/
	wizchip_setnetinfo(&gWIZNETINFO);

	setSHAR(mac_address);

	print_network_information();

	Network n;
	Client c;

	n.my_socket = 0;
	DNS_init(1,tempBuffer);
	while(DNS_run(gWIZNETINFO.dns,TargetName,targetIP) == 0){}
	NewNetwork(&n);
	ConnectNetwork(&n, targetIP, targetPort);
	MQTTClient(&c,&n,1000,buf,100,tempBuffer,2048);

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.willFlag = 0;
	data.MQTTVersion = 3;
	data.clientID.cstring = opts.clientid;
	data.username.cstring = opts.username;
	data.password.cstring = opts.password;

	data.keepAliveInterval = 60;
	data.cleansession = 1;

	rc = MQTTConnect(&c, &data);
	printf("Connected %d\r\n", rc);
	opts.showtopics = 1;

	printf("Subscribing to %s\r\n", "hello/wiznet");
	rc = MQTTSubscribe(&c, "hello/wiznet", opts.qos, messageArrived);
	printf("Subscribed %d\r\n", rc);

	int size;

    while(1)
    {
    	MQTTYield(&c, 1000);
    }
}
