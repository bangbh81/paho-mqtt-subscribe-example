#include "mqtt_interface.h"
#include "wizchip_conf.h"
#include "misc.h"

unsigned long MilliTimer;

void SysTick_Handler(void) {
	MilliTimer++;
}

char expired(Timer* timer) {
	long left = timer->end_time - MilliTimer;
	return (left < 0);
}


void countdown_ms(Timer* timer, unsigned int timeout) {
	timer->end_time = MilliTimer + timeout;
}


void countdown(Timer* timer, unsigned int timeout) {
	timer->end_time = MilliTimer + (timeout * 1000);
}


int left_ms(Timer* timer) {
	long left = timer->end_time - MilliTimer;
	return (left < 0) ? 0 : left;
}


void InitTimer(Timer* timer) {
	timer->end_time = 0;
}


void NewNetwork(Network* n) {
	n->my_socket = 0;
	n->mqttread = w5500_read;
	n->mqttwrite = w5500_write;
	n->disconnect = w5500_disconnect;
}

int w5500_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{

	if((getSn_SR(n->my_socket) == SOCK_ESTABLISHED) && (getSn_RX_RSR(n->my_socket)>0))
		return recv(n->my_socket, buffer, len);
}

int w5500_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	if(getSn_SR(n->my_socket) == SOCK_ESTABLISHED)
		return send(n->my_socket, buffer, len);
}

void w5500_disconnect(Network* n)
{
	disconnect(n->my_socket);
}

int ConnectNetwork(Network* n, char* ip, int port)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	socket(n->my_socket,Sn_MR_TCP,12345,0);
	connect(n->my_socket,ip,port);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(72000);
	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // Highest priority
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
