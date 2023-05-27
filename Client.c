#include "Client.h"
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int *get_post(int END, int Co_ord_Cntrl, int x_close, int y_start, int p_state, char* IP) {
    WSADATA wsadata;
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (iResult != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return NULL;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %d\n", WSAGetLastError());
        WSACleanup();
        return NULL;
    }

    struct sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(IP);
    clientService.sin_port = htons(DEFAULT_PORT);

    iResult = connect(ConnectSocket, (SOCKADDR *) &clientService, sizeof(clientService));
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return NULL;
    }

    int recvbuflen = DEFAULT_BUFLEN;
    char *sendbuf = NULL;
    char recvbuf[DEFAULT_BUFLEN];

    if (END) {
        sendbuf = calloc(2, sizeof(char));
        sendbuf[0] = '1';
    } else {
        if (!Co_ord_Cntrl) {
            // controls
            char temp[DEFAULT_BUFLEN] = "";
            char final[DEFAULT_BUFLEN] = "";
            strcat(final, "00-");
            itoa(x_close, temp, 10);
            strcat(final, temp);
            itoa(y_start, temp, 10);
            strcat(final, temp);
            itoa(p_state, temp, 10);
            strcat(final, temp);

            sendbuf = calloc(7, sizeof(char));
            strcpy(sendbuf, final);
        } else {
            // co-ords
            char temp[DEFAULT_BUFLEN] = "";
            char final[DEFAULT_BUFLEN] = "";
            strcat(final, "01-");
            itoa(p_state, temp, 10);
            strcat(final, temp);
            strcat(final, "-");

            int n1 = x_close;
            int count1 = 0;
            do {
                n1 /= 10;
                ++count1;
            } while (n1 != 0);
            char str1_buf[DEFAULT_BUFLEN];

            int n2 = y_start;
            int count2 = 0;
            do {
                n2 /= 10;
                ++count2;
            } while (n2 != 0);
            char str2_buf[DEFAULT_BUFLEN];

            itoa(x_close, str1_buf, 10);
            itoa(y_start, str2_buf, 10);

            char str1[DEFAULT_BUFLEN] = "+";
            if (x_close > -1) {
                strcat(str1, str1_buf);
            } else {
                strcpy(str1, str1_buf);
            }
            char str2[DEFAULT_BUFLEN] = "+";
            if (y_start > -1) {
                strcat(str2, str2_buf);
            } else {
                strcpy(str2, str2_buf);
            }

            itoa(count1, temp, 10);
            strcat(final, temp);
            strcat(final, str1);
            strcat(final, "-");
            itoa(count2, temp, 10);
            strcat(final, temp);
            strcat(final, str2);
            strcat(final, "-");

            sendbuf = calloc(12 + count1 + count2, sizeof(char));
            strcpy(sendbuf, final);
        }
    }

    iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return NULL;
    }

    printf("Bytes Sent: %d {%s} \n", iResult, sendbuf);
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return NULL;
    }

    int *Ret_Val = calloc(2, sizeof(int));

    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes recv: %d {%s} \n", iResult, recvbuf);

            if (!END) {
                if (!Co_ord_Cntrl) {
                    Ret_Val[0] = recvbuf[0] - '0';
                    Ret_Val[1] = recvbuf[1] - '0';
                } else {
                    int x_pos, y_pos;
                    int size1 = recvbuf[0] - '0';
                    char num1[size1];
                    // + or - : recvbuf[1]
                    int index1 = 2;
                    int k1 = 0;
                    while (recvbuf[index1] != '-') {
                        num1[k1++] = recvbuf[index1++];
                    }
                    x_pos = atoi(num1);
                    if (recvbuf[1] == '-') {
                        x_pos *= -1;
                    }
                    index1++;
                    int size2 = recvbuf[index1] - '0';
                    char num2[size2];
                    // + or - : recvbuf[index1+1]
                    int index2 = index1 + 2;
                    int k2 = 0;
                    while (recvbuf[index2] != '-') {
                        num2[k2++] = recvbuf[index2++];
                    }
                    y_pos = atoi(num2);
                    if (recvbuf[index1 + 1] == '-') {
                        y_pos *= -1;
                    }
                    Ret_Val[0] = x_pos;
                    Ret_Val[1] = y_pos;
                }
            }
        } else if (iResult < 0)
            printf("recv failed: %d\n", WSAGetLastError());
    } while (iResult > 0);

    closesocket(ConnectSocket);
    WSACleanup();

    return Ret_Val;
}