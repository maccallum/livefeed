#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 10000
#define BUFSIZE 2048

#define ROUTER 1
#define SERVER 2

struct binding
{
	char *oscaddress;
	char *ipaddress;
};

struct params
{
	int32_t brightness, contrast, saturation, hue, gamma, gain,
		white_balance_temperature, sharpness, backlight_compensation,
		exposure_absolute, white_balance_temperature_auto;
};

struct config
{
	int role;
	int haserrors;
	struct binding bindings[16];
	int nbindings;
};

struct config _processargs(int ac, char **av, struct config c)
{
	if(ac == 0)
    {
		return c;
	}
	if(!strncmp(av[0], "-r", 2))
    {
		c.role = ROUTER;
		return _processargs(ac - 1, av + 1, c);
	}
    else if(!strncmp(av[0], "-s", 2))
    {
		c.role = SERVER;
		return _processargs(ac - 1, av + 1, c);
	}
    else if(!strncmp(av[0], "-b", 2))
    {
		c.bindings[c.nbindings++] = (struct binding){strdup(av[1]), strdup(av[2])};
		return _processargs(ac - 3, av + 3, c);
	}
	return c;
}

struct config processargs(int ac, char **av)
{
	struct config c;
	memset(&c, 0, sizeof(struct config));
	return _processargs(ac - 1, av + 1, c);
}

int route(int buflen, char *buf, struct config c)
{
	if(strncmp(buf, "#bundle", 8))
    {
		return -1;
	}
	// expecting address in the form of /jn/0, /jn/1, etc.
	char *address = buf + 20;
	char *ipaddress = NULL;
	/* switch(address[4]){ */
	/* case '0': */
	/*     ipaddress = "192.168.2.2"; */
	/*     break; */
	/* case '1': */
	/*     ipaddress = "192.168.2.3"; */
	/*     break; */
	/* case '2': */
	/*     ipaddress = "192.168.2.4"; */
	/*     break; */
	/* } */
	for(int i = 0; i < c.nbindings; i++)
    {
		if(!strncmp(c.bindings[i].oscaddress,
                    address,
                    strlen(c.bindings[i].oscaddress)))
        {
			ipaddress = c.bindings[i].ipaddress;
			break;
		}
	}
	if(!ipaddress)
    {
		return -1;
	}
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_addr.s_addr = inet_addr(ipaddress);
	servaddr.sin_port = htons(PORT);
	servaddr.sin_family = AF_INET;
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
		printf("couldn't connect to %s\n", ipaddress);
		return -1;
	}
	sendto(sock, buf, buflen, 0, (struct sockaddr *)NULL, sizeof(servaddr));
	close(sock);
	return 0;
}

int serve(int buflen, char *buf, struct config c)
{
	if(strncmp(buf, "#bundle", 8))
    {
		return -1;
	}
	// expecting address in the form of /jn/0/0, /jn/0/1, etc.
	// where the first int is the jn instance, and the second is
	// the video input
	int32_t ao = 20;
	const char *dev = NULL;
	switch(buf[ao + 6])
    {
	case '0':
		dev = "/dev/video0";
		break;
	case '1':
		dev = "/dev/video1";
		break;
	}
	int32_t tto = ao + strlen(buf + ao);
	tto++;
	while(tto % 4)
    {
		tto++;
	}
	int32_t plo = tto + strlen(buf + tto);
	plo++;
	while(plo % 4)
    {
		plo++;
	}
	struct params *p = (struct params *)(buf + plo);
	/* printf("brightness = %d, contrast = %d\n", ntohl(p->brightness), */
	/*        ntohl(p->contrast)); */
	char cmd[8192];
	/* snprintf(cmd, 8192, "v4l2-ctl -d %s -c white_balance_temperature_auto=%d", */
	/*          dev, ntohl(p->white_balance_temperature_auto)); */
	/* printf("%s\n", cmd); */
	/* system(cmd); */
	snprintf(cmd, 8192, "v4l2-ctl -d %s "
             "-c brightness=%d "
             "-c contrast=%d "
             "-c saturation=%d "
             "-c hue=%d "
             "-c white_balance_temperature_auto=0 "
             "-c gamma=%d "
             "-c gain=%d "
             "-c white_balance_temperature=%d "
             "-c white_balance_temperature_auto=%d "
             "-c sharpness=%d "
             "-c backlight_compensation=%d ",
             //"-c exposure_absolute=%d ",
             dev,
             ntohl(p->brightness),
             ntohl(p->contrast),
             ntohl(p->saturation),
             ntohl(p->hue),
             ntohl(p->gamma),
             ntohl(p->gain),
             ntohl(p->white_balance_temperature),
             ntohl(p->white_balance_temperature_auto),
             ntohl(p->sharpness),
             ntohl(p->backlight_compensation));
	printf("%s\n", cmd);
	system(cmd);

	return 0;
}

int main(int ac, char **av)
{
	struct config c = processargs(ac, av);

	int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;

    bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));

    struct sockaddr_in cliaddr;
    unsigned int cliaddrlen = sizeof(cliaddr);

    while(1)
    {
		char buf[BUFSIZE];
		int n = recvfrom(sock, buf, BUFSIZE, 0,
                         (struct sockaddr *)&cliaddr,
                         &cliaddrlen);
		if(c.role == ROUTER)
        {
			route(n, buf, c);
		}
        else if(c.role == SERVER)
        {
			serve(n, buf, c);
		}
    }
	return 0;
}
