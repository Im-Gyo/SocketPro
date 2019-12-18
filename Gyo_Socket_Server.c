#include <stdio.h>
#include <stdlib.h>		//문자열변환, 동적메모리관리
#include <string.h>
#include <winsock2.h>	//윈도우용 소켓 프로그래밍
#include <conio.h>

#define BUFSIZE 1024
//#define _WINSOCK_DEPRECATED_NO_WARNINGS.
//#define _CRT_SECURE_NO_WARNINGS

void ErrorHandling(char* message);

int main(int argc, char** argv) {

	WSADATA wsaData;	//WSADATA에 대한 구조체 정의(startup()사용하기위해사용))
	SOCKET servSock, clntSock;	//SOCKET 구조체 정의
	SOCKADDR_IN servAddr, clntAddr;	//SOCKET_IN 구조체 정의( 주소 체계 정의, IP와 포트주소에 대한 정보)

	char message[BUFSIZE];
	int strLen;
	int fromLen, nRcv;

	if (argc != 2) {
		printf("포트 넘버를 넣어주세요.\n");
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WinSock 2.2 DLL 로드 에러");

	servSock = socket(PF_INET, SOCK_STREAM, 0);	// 새로운 소켓 정의(인자도메인(주소패밀리), 소켓타입 설정(TCP/UDP), 프로토콜(자동선정 = 0)
	if (servSock == INVALID_SOCKET)
		ErrorHandling("소켓에러");

	memset(&servAddr, 0, sizeof(SOCKADDR_IN));	// 소켓의 인자로 사용할 값 정의(&servAddr 시작주소부터 0대입)
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(atoi(argv[1]));	
		//argv[1]인자를 문자열에서 정수로 변환해서 리턴된 값을 byte order로 변경해 sinport에 저장
		//네트워크 바이트 순서 변경(socket_in구조체에 값을 대입하기 위해 사용)
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	
		//INADDR_ANY를 통해 네트워크 byte order로 변경해 저장(INADDR_ANY는 서버의 IP주소를 자동으로 찾아 대입)
		//PC간 통신시 CPU가 다르면 처리하는 데이터 방식이 다르기 떄문
	
	if (bind(servSock, (void*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind 에러");
		//클라이언트가 서버에 접속하기 위한 함수	
		//bind(소켓을 이루는 정보(SOCKET 구조체의 값), SOCKADDR_IN의 포인터 변수(IP,Port,Address), 주소 정보 구조체의 길이)
	if (listen(servSock, 2) == SOCKET_ERROR)	
		// listen(socket함수를 통한 반환값(서버에서 생성한 소켓을 listen상태로 변경), 최대 연결 갯수(최대 n개의 연결까지 기다리는 것)
		// 서버가 클라이언트 요청을 받음
		ErrorHandling("listen 에러");

	fromLen = sizeof(clntAddr);

	clntSock = accept(servSock, (void*)& clntAddr, &fromLen);	//listen함수에 의해 연결시도 감지시 accept 함수로 넘어옴
		//accept(수신 대기할 소켓(소켓정보), 클라이언트의 소켓정보가 accept함수의 반환값으로 전달)
	if (clntSock == INVALID_SOCKET) {
		ErrorHandling("accept 에러");
	}
	else {
		printf("%s 연결 성공\n", inet_ntoa(clntAddr.sin_addr));
		printf("시작...\n");
	}

	closesocket(servSock);

	while (1) {
		printf("메세지 수신 ...\n");
		nRcv = recv(clntSock, message, sizeof(message) - 1, 0);
			//accept함수로 클라이언트의 소켓정보를 획득하면 send/recv통신 시도
			//recv() 소켓으로부터 데이터를 수신
			//recv(소켓디스크립터, 수신할 버퍼 포인터 데이터, 버퍼의 바이트 단위 크기, 옵션)
			//소켓정보를 정의, message변수로 데이터 수신, message크기만큼 데이터를 수신한 데이터를 nRCV에 정의			
		if (nRcv == SOCKET_ERROR) {
			printf("수신 에러..\n");
			break;
		}
		message[nRcv] = '\0';

		if (strcmp(message, "exit") == 0) {
			printf("클라이언트 연결 종료\n");
			break;
		}
		
		printf("수신 메세지 : %s", message);
		printf("\n발신 메세지 : ");
		gets_s(message,BUFSIZE);
		if (strcmp(message, "exit") == 0){
			send(clntSock, message, (int)strlen(message), 0);
				break;
		}
		send(clntSock, message, (int)strlen(message),0);
			// 클라이언트 소켓으로 데이터 전송
	}

	closesocket(clntSock);
	WSACleanup();

	printf("연결 종료\n");
	_getch();

	return 0;
}
void ErrorHandling(char* message) {  //message라는 문자열을 인자로 받음
	WSACleanup();	
		//Winsock을 사용하기 위해 ws2_32.dll 초기화(WSAStartUP())
		//closesocket() 실행 시 큐에 걸려있는 데이터 삭제(삭제가 안될경우 재연결시 남아있는 데이터 전송)
	fputs(message, stderr);		//사용자 정의 문자열 message를 표준에러출력 stderr에 씀
	fputc('\n', stderr);	//문자1개
	_getch();	//입력이 없으면 그대로 프로그램 진행, 버퍼를 사용하지 않고 바로 실행
	exit(1);
}