#include <stdio.h>
#include <stdlib.h>		//���ڿ���ȯ, �����޸𸮰���
#include <string.h>
#include <winsock2.h>	//������� ���� ���α׷���
#include <conio.h>

#define BUFSIZE 1024
//#define _WINSOCK_DEPRECATED_NO_WARNINGS.
//#define _CRT_SECURE_NO_WARNINGS

void ErrorHandling(char* message);

int main(int argc, char** argv) {

	WSADATA wsaData;	//WSADATA�� ���� ����ü ����(startup()����ϱ����ػ��))
	SOCKET servSock, clntSock;	//SOCKET ����ü ����
	SOCKADDR_IN servAddr, clntAddr;	//SOCKET_IN ����ü ����( �ּ� ü�� ����, IP�� ��Ʈ�ּҿ� ���� ����)

	char message[BUFSIZE];
	int strLen;
	int fromLen, nRcv;

	if (argc != 2) {
		printf("��Ʈ �ѹ��� �־��ּ���.\n");
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WinSock 2.2 DLL �ε� ����");

	servSock = socket(PF_INET, SOCK_STREAM, 0);	// ���ο� ���� ����(���ڵ�����(�ּ��йи�), ����Ÿ�� ����(TCP/UDP), ��������(�ڵ����� = 0)
	if (servSock == INVALID_SOCKET)
		ErrorHandling("���Ͽ���");

	memset(&servAddr, 0, sizeof(SOCKADDR_IN));	// ������ ���ڷ� ����� �� ����(&servAddr �����ּҺ��� 0����)
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(atoi(argv[1]));	
		//argv[1]���ڸ� ���ڿ����� ������ ��ȯ�ؼ� ���ϵ� ���� byte order�� ������ sinport�� ����
		//��Ʈ��ũ ����Ʈ ���� ����(socket_in����ü�� ���� �����ϱ� ���� ���)
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	
		//INADDR_ANY�� ���� ��Ʈ��ũ byte order�� ������ ����(INADDR_ANY�� ������ IP�ּҸ� �ڵ����� ã�� ����)
		//PC�� ��Ž� CPU�� �ٸ��� ó���ϴ� ������ ����� �ٸ��� ����
	
	if (bind(servSock, (void*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind ����");
		//Ŭ���̾�Ʈ�� ������ �����ϱ� ���� �Լ�	
		//bind(������ �̷�� ����(SOCKET ����ü�� ��), SOCKADDR_IN�� ������ ����(IP,Port,Address), �ּ� ���� ����ü�� ����)
	if (listen(servSock, 2) == SOCKET_ERROR)	
		// listen(socket�Լ��� ���� ��ȯ��(�������� ������ ������ listen���·� ����), �ִ� ���� ����(�ִ� n���� ������� ��ٸ��� ��)
		// ������ Ŭ���̾�Ʈ ��û�� ����
		ErrorHandling("listen ����");

	fromLen = sizeof(clntAddr);

	clntSock = accept(servSock, (void*)& clntAddr, &fromLen);	//listen�Լ��� ���� ����õ� ������ accept �Լ��� �Ѿ��
		//accept(���� ����� ����(��������), Ŭ���̾�Ʈ�� ���������� accept�Լ��� ��ȯ������ ����)
	if (clntSock == INVALID_SOCKET) {
		ErrorHandling("accept ����");
	}
	else {
		printf("%s ���� ����\n", inet_ntoa(clntAddr.sin_addr));
		printf("����...\n");
	}

	closesocket(servSock);

	while (1) {
		printf("�޼��� ���� ...\n");
		nRcv = recv(clntSock, message, sizeof(message) - 1, 0);
			//accept�Լ��� Ŭ���̾�Ʈ�� ���������� ȹ���ϸ� send/recv��� �õ�
			//recv() �������κ��� �����͸� ����
			//recv(���ϵ�ũ����, ������ ���� ������ ������, ������ ����Ʈ ���� ũ��, �ɼ�)
			//���������� ����, message������ ������ ����, messageũ�⸸ŭ �����͸� ������ �����͸� nRCV�� ����			
		if (nRcv == SOCKET_ERROR) {
			printf("���� ����..\n");
			break;
		}
		message[nRcv] = '\0';

		if (strcmp(message, "exit") == 0) {
			printf("Ŭ���̾�Ʈ ���� ����\n");
			break;
		}
		
		printf("���� �޼��� : %s", message);
		printf("\n�߽� �޼��� : ");
		gets_s(message,BUFSIZE);
		if (strcmp(message, "exit") == 0){
			send(clntSock, message, (int)strlen(message), 0);
				break;
		}
		send(clntSock, message, (int)strlen(message),0);
			// Ŭ���̾�Ʈ �������� ������ ����
	}

	closesocket(clntSock);
	WSACleanup();

	printf("���� ����\n");
	_getch();

	return 0;
}
void ErrorHandling(char* message) {  //message��� ���ڿ��� ���ڷ� ����
	WSACleanup();	
		//Winsock�� ����ϱ� ���� ws2_32.dll �ʱ�ȭ(WSAStartUP())
		//closesocket() ���� �� ť�� �ɷ��ִ� ������ ����(������ �ȵɰ�� �翬��� �����ִ� ������ ����)
	fputs(message, stderr);		//����� ���� ���ڿ� message�� ǥ�ؿ������ stderr�� ��
	fputc('\n', stderr);	//����1��
	_getch();	//�Է��� ������ �״�� ���α׷� ����, ���۸� ������� �ʰ� �ٷ� ����
	exit(1);
}