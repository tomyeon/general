#include <stdio.h> 
#include <string.h> 
#include <unistd.h>
#include <fcntl.h> 
#include <errno.h> 
#include <termios.h> 
#include <time.h>  

int open_port(char *dev)
{
	int fd; // file description for the serial port

	printf("Device = %s\n", dev); 
	
	fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
	
	if(fd == -1)	{ // if open is unsucessful
		printf("open_port: Unable to open /dev/ttyS0. \n");
		} 
	else		{
		fcntl(fd, F_SETFL, 0);
		printf("port is open.\n");
		}		
	
	return(fd);

} //open_port

int configure_port(int fd)      // configure the port
{
	struct termios port_settings;      // structure to store the port settings in

	cfsetispeed(&port_settings, B9600);    // set baud rates
	cfsetospeed(&port_settings, B9600);

	port_settings.c_cflag &= ~PARENB;    // set no parity, stop bits, data bits
	port_settings.c_cflag &= ~CSTOPB;
	port_settings.c_cflag &= ~CSIZE;
	port_settings.c_cflag |= CS8;
	
	tcsetattr(fd, TCSANOW, &port_settings);    // apply the settings to the port
	return(fd);

} //configure_port

int query_modem(int fd, unsigned char send_bytes[])   // query modem with an AT command
{
		
	write(fd, send_bytes, 8);  //Send data
	
	return 0;
	
} //query_modem

int main(int argc, char **argv) { 

	int fd = open_port(argv[1]);
	configure_port(fd);
	int count = 0; 
	int timer = atoi(argv[2]); 

	unsigned char off[] = { 0x55, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x5B };
	unsigned char on[] = { 0x55, 0x01, 0x01, 0x02, 0x01, 0x01, 0x01, 0x5C };

	printf ("Delay = %d\n", timer); 

	while (1) { 

		count++;

		if ( (count%2)  == 0 ) {
			query_modem(fd, on);
			count  = 0; 
			sleep (timer); 
		} else {
			query_modem(fd, off);
			sleep (3);
			}			
		
		}
	return(0);
	
} //main
