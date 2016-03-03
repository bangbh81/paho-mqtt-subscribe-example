#ifndef __MQTT_INTERFACE_H_
#define __MQTT_INTERFACE_H_

typedef struct Timer Timer;

struct Timer {
	unsigned long systick_period;
	unsigned long end_time;
};

typedef struct Network Network;

struct Network
{
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
};

void InitTimer(Timer*);

char expired(Timer*);
void countdown_ms(Timer*, unsigned int);
void countdown(Timer*, unsigned int);
int left_ms(Timer*);

int w5500_read(Network*, unsigned char*, int, int);
int w5500_write(Network*, unsigned char*, int, int);
void w5500_disconnect(Network*);
void NewNetwork(Network*);

int ConnectNetwork(Network*, char*, int);

#endif
