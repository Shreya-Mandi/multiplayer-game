#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

int END = 0;
int CLOSE = 0;
int START = 1;
int CLOSE_NEW = 0;
int START_NEW = 1;
int x_pos_1 = 370;
int y_pos_1 = 285;
int x_pos_2 = 500;
int y_pos_2 = 285;

int main(int argc, char *argv[]) {
    WSADATA wsadata;
    int iResult;

    printf("Initialising Win_sock .......!\n");

    iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (iResult != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    printf("Initialised.\n");

    struct addrinfo *result = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("get address info failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    printf("address info received? \n");

    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    printf("Listening Socket created\n");

    iResult = bind(ListenSocket, result->ai_addr, (int) result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);

    printf("Socket Bind success? \n");

    START:

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("Listening? \n");

    SOCKET ClientSocket;
    ClientSocket = INVALID_SOCKET;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("Client Accepted? \n");

    char recvbuf[DEFAULT_BUFLEN];
    char *sendbuf = calloc(0, sizeof(char));
    int iSendResult;
    int recvbuflen = DEFAULT_BUFLEN;

    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes recv: %d {%s}\n", iResult, recvbuf);
            if (recvbuf[0] == '1') {
                END = 1;
                break;
            }
            if (recvbuf[1] == '0') {
                // Controls
                CLOSE_NEW = recvbuf[3] - '0';
                START_NEW = recvbuf[4] - '0';
                int state = recvbuf[5] - '0';

                if(!state && !START_NEW)
                    START = 0;
                if(state && START_NEW)
                    START = 1;
                if(CLOSE_NEW)
                    CLOSE = 1;

                char str[] = "00";
                if (CLOSE)
                    str[0] = '1';
                if (START)
                    str[1] = '1';
                free(sendbuf);
                sendbuf = calloc(3, sizeof(char));
                strcpy(sendbuf, str);
            } else if (recvbuf[1] == '1') {
                // Coordinates
                if (recvbuf[3] == '1') {
                    // player_1
                    int size1 = recvbuf[5] - '0';
                    char num1[size1];
                    // + or - : recvbuf[6]
                    int index1 = 7;
                    int k1 = 0;
                    while (recvbuf[index1] != '-') {
                        num1[k1++] = recvbuf[index1++];
                    }
                    x_pos_1 = atoi(num1);
                    if (recvbuf[6] == '-') {
                        x_pos_1 *= -1;
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
                    y_pos_1 = atoi(num2);
                    if (recvbuf[index1 + 1] == '-') {
                        y_pos_1 *= -1;
                    }
                    printf("1: %d %d \n", x_pos_1, y_pos_1);

                    int n1 = x_pos_2;
                    int count1 = 0;
                    do {
                        n1 /= 10;
                        ++count1;
                    } while (n1 != 0);
                    char str1_buf[DEFAULT_BUFLEN];

                    int n2 = y_pos_2;
                    int count2 = 0;
                    do {
                        n2 /= 10;
                        ++count2;
                    } while (n2 != 0);
                    char str2_buf[DEFAULT_BUFLEN];

                    itoa(x_pos_2, str1_buf, 10);
                    itoa(y_pos_2, str2_buf, 10);

                    char str1[DEFAULT_BUFLEN] = "+";
                    if (x_pos_2 > -1) {
                        strcat(str1, str1_buf);
                    } else {
                        strcpy(str1, str1_buf);
                    }
                    char str2[DEFAULT_BUFLEN] = "+";
                    if (y_pos_2 > -1) {
                        strcat(str2, str2_buf);
                    } else {
                        strcpy(str2, str2_buf);
                    }

                    char final[DEFAULT_BUFLEN] = "";
                    itoa(count1, final, 10);
                    strcat(final, str1);
                    strcat(final, "-");
                    itoa(count2, str1, 10);
                    strcat(final, str1);
                    strcat(final, str2);
                    strcat(final, "-");
                    printf("2: %d %d\n", x_pos_2, y_pos_2);

                    free(sendbuf);
                    sendbuf = calloc(count1 + count2 + 7, sizeof(char));
                    strcpy(sendbuf, final);

                } else if (recvbuf[3] == '2') {
                    // player_2
                    int size1 = recvbuf[5] - '0';
                    char num1[size1];
                    // + or - : recvbuf[6]
                    int index1 = 7;
                    int k1 = 0;
                    while (recvbuf[index1] != '-') {
                        num1[k1++] = recvbuf[index1++];
                    }
                    x_pos_2 = atoi(num1);
                    if (recvbuf[6] == '-') {
                        x_pos_2 *= -1;
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
                    y_pos_2 = atoi(num2);
                    if (recvbuf[index1 + 1] == '-') {
                        y_pos_2 *= -1;
                    }
                    printf("2: %d %d \n", x_pos_2, y_pos_2);

                    int n1 = x_pos_1;
                    int count1 = 0;
                    do {
                        n1 /= 10;
                        ++count1;
                    } while (n1 != 0);
                    char str1_buf[DEFAULT_BUFLEN];

                    int n2 = y_pos_1;
                    int count2 = 0;
                    do {
                        n2 /= 10;
                        ++count2;
                    } while (n2 != 0);
                    char str2_buf[DEFAULT_BUFLEN];

                    itoa(x_pos_1, str1_buf, 10);
                    itoa(y_pos_1, str2_buf, 10);

                    char str1[DEFAULT_BUFLEN] = "+";
                    if (x_pos_1 > -1) {
                        strcat(str1, str1_buf);
                    } else {
                        strcpy(str1, str1_buf);
                    }
                    char str2[DEFAULT_BUFLEN] = "+";
                    if (y_pos_1 > -1) {
                        strcat(str2, str2_buf);
                    } else {
                        strcpy(str2, str2_buf);
                    }

                    char final[DEFAULT_BUFLEN] = "";
                    itoa(count1, final, 10);
                    strcat(final, str1);
                    strcat(final, "-");
                    itoa(count2, str1, 10);
                    strcat(final, str1);
                    strcat(final, str2);
                    strcat(final, "-");
                    printf("2: %d %d\n", x_pos_1, y_pos_1);

                    free(sendbuf);
                    sendbuf = calloc(count1 + count2 + 7, sizeof(char));
                    strcpy(sendbuf, final);
                }

            }
            iSendResult = send(ClientSocket, sendbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d {%s}\n", iSendResult, sendbuf);
        } else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    printf("done? \n");

    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    if (!END)
        goto START;

    closesocket(ListenSocket);
    WSACleanup();

    printf("Shutdown? \n");

    return 0;
}