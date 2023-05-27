#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27015

int *get_post(int END, int Co_ord_Cntrl, int x_close, int y_start, int p_state, char* IP);