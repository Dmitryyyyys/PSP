#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include <thread>

#pragma comment(lib, "WS2_32.lib")
#pragma warning(disable: 4996)

std::string  GetErrorMsgText(int code)
{
	std::string msgText;
	switch (code)
	{
	case WSAEINTR:				msgText = "Работа функции прервана"; break;
	case WSAEACCES:				msgText = "Разрешение отвергнуто"; break;
	case WSAEFAULT:				msgText = "Ошибочный адрес"; break;
	case WSAEINVAL:				msgText = "Ошибка в аргументе"; break;
	case WSAEMFILE:				msgText = "Слишком много файлов открыто"; break;
	case WSAEWOULDBLOCK:		msgText = "Ресурс временно недоступен"; break;
	case WSAEINPROGRESS:		msgText = "Операция в процессе развития"; break;
	case WSAEALREADY:			msgText = "Операция уже выполняется"; break;
	case WSAENOTSOCK:			msgText = "Сокет задан неправильно"; break;
	case WSAEDESTADDRREQ:		msgText = "Требуется адрес расположения"; break;
	case WSAEMSGSIZE:			msgText = "Сообщение слишком длинное"; break;
	case WSAEPROTOTYPE:			msgText = "Неправильный тип протокола для сокета"; break;
	case WSAENOPROTOOPT:		msgText = "Ошибка в опции протокола"; break;
	case WSAEPROTONOSUPPORT:	msgText = "Протокол не поддерживается"; break;
	case WSAESOCKTNOSUPPORT:	msgText = "Тип сокета не поддерживается"; break;
	case WSAEOPNOTSUPP:			msgText = "Операция не поддерживается"; break;
	case WSAEPFNOSUPPORT:		msgText = "Тип протоколов не поддерживается"; break;
	case WSAEAFNOSUPPORT:		msgText = "Тип адресов не поддерживается протоколом"; break;
	case WSAEADDRINUSE:			msgText = "Адрес уже используется"; break;
	case WSAEADDRNOTAVAIL:		msgText = "Запрошенный адрес не может быть использован"; break;
	case WSAENETDOWN:			msgText = "Сеть отключена"; break;
	case WSAENETUNREACH:		msgText = "Сеть не достижима"; break;
	case WSAENETRESET:			msgText = "Сеть разорвала соединение"; break;
	case WSAECONNABORTED:		msgText = "Программный отказ связи"; break;
	case WSAECONNRESET:			msgText = "Связь восстановлена"; break;
	case WSAENOBUFS:			msgText = "Не хватает памяти для буферов"; break;
	case WSAEISCONN:			msgText = "Сокет уже подключен"; break;
	case WSAENOTCONN:			msgText = "Сокет не подключен"; break;
	case WSAESHUTDOWN:			msgText = "Нельзя выполнить send: сокет завершил работу"; break;
	case WSAETIMEDOUT:			msgText = "Закончился отведенный интервал  времени"; break;
	case WSAECONNREFUSED:		msgText = "Соединение отклонено"; break;
	case WSAEHOSTDOWN:			msgText = "Хост в неработоспособном состоянии"; break;
	case WSAEHOSTUNREACH:		msgText = "Нет маршрута для хоста"; break;
	case WSAEPROCLIM:			msgText = "Слишком много процессов"; break;
	case WSASYSNOTREADY:		msgText = "Сеть не доступна"; break;
	case WSAVERNOTSUPPORTED:	msgText = "Данная версия недоступна"; break;
	case WSANOTINITIALISED:		msgText = "Не выполнена инициализация WS2_32.DLL"; break;
	case WSAEDISCON:			msgText = "Выполняется отключение"; break;
	case WSATYPE_NOT_FOUND:		msgText = "Класс не найден"; break;
	case WSAHOST_NOT_FOUND:		msgText = "Хост не найден"; break;
	case WSATRY_AGAIN:			msgText = "Неавторизированный хост не найден"; break;
	case WSANO_RECOVERY:		msgText = "Неопределенная ошибка"; break;
	case WSANO_DATA:			msgText = "Нет записи запрошенного типа"; break;
	case WSA_INVALID_HANDLE:	msgText = "Указанный дескриптор события  с ошибкой"; break;
	case WSA_INVALID_PARAMETER: msgText = "Один или более параметров с ошибкой"; break;
	case WSA_IO_INCOMPLETE:		msgText = "Объект ввода-вывода не в сигнальном состоянии"; break;
	case WSA_IO_PENDING:		msgText = "Операция завершится позже"; break;
	case WSA_NOT_ENOUGH_MEMORY: msgText = "Не достаточно памяти"; break;
	case WSA_OPERATION_ABORTED: msgText = "Операция отвергнута"; break;
		//case WSAINVALIDPROCTABLE:	msgText = "Ошибочный сервис"; break;
		//case WSAINVALIDPROVIDER:	msgText = "Ошибка в версии сервиса"; break;
		//case WSAPROVIDERFAILEDINIT: msgText = "Невозможно инициализировать сервис"; break;
	case WSASYSCALLFAILURE:		msgText = "Аварийное завершение системного вызова"; break;
	default:                msgText = "***ERROR***";      break;
	};
	return msgText;
};
std::string  SetErrorMsgText(std::string msgText, int code)
{
	return  msgText + GetErrorMsgText(code);
};

// -- обработать запрос клиента 
// Назначение: функция предназначена для обработки запроса 
//             клиентской программы   

bool GetRequestFromClient(
	char* name, //[in] позывной сервера  
	short            port, //[in] номер просушиваемого порта 
	struct sockaddr* from, //[out] указатель на SOCKADDR_IN
	int* flen  //[out] указатель на размер from 
) {
	SOCKET server_socket;
	if ((server_socket = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		throw  SetErrorMsgText("socket:", WSAGetLastError());

	SOCKADDR_IN serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_socket, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
		throw  SetErrorMsgText("bind:", WSAGetLastError());

	char ibuf[50];
	memset(ibuf, '\0', sizeof(ibuf));

	while (strcmp(name, ibuf) != 0) {
		if (recvfrom(server_socket, ibuf, sizeof(ibuf), NULL, from, flen) == SOCKET_ERROR)
			throw  SetErrorMsgText("GetRequestFromClient:", WSAGetLastError());
	}

	if (closesocket(server_socket) == SOCKET_ERROR)
		throw  SetErrorMsgText("closesocket:", WSAGetLastError());

	return true;
}

void t1(const char* name, SOCKET server_socket) {
	int len = 1;
	while (len > 0) {
		SOCKADDR_IN clnt;
		memset(&clnt, 0, sizeof(clnt));
		int lc = sizeof(clnt);

		char ibuf[50];
		memset(ibuf, '\0', sizeof(ibuf));

		if ((len = recvfrom(server_socket, ibuf, sizeof(ibuf), NULL, (struct sockaddr*)&clnt, &lc)) == SOCKET_ERROR)
			throw  SetErrorMsgText("GetRequestFromClient:", WSAGetLastError());

		std::cout << "server_name = \"" << name << "\"" << " ip: " << inet_ntoa(clnt.sin_addr) << ":" << ntohs(clnt.sin_port) << std::endl;
	}
}

void GetServerCounts(const char* name, short port) {
	SOCKET server_socket;
	if ((server_socket = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		throw  SetErrorMsgText("socket:", WSAGetLastError());

	bool trueflag = true;
	if (setsockopt(server_socket, SOL_SOCKET, SO_BROADCAST,
		(char*)&trueflag, sizeof(trueflag)) < 0)
		throw  SetErrorMsgText("setsockopt:", WSAGetLastError());

	SOCKADDR_IN serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = inet_addr("192.168.43.255");

	if (sendto(server_socket, name, strlen(name) + 1, NULL,
		(sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR)
		throw  SetErrorMsgText("sendto:", WSAGetLastError());

	clock_t start_time = clock();
	int len = 1;
	while (len > 0) {
		SOCKADDR_IN clnt;
		memset(&clnt, 0, sizeof(clnt));
		int lc = sizeof(clnt);

		char ibuf[50];
		memset(ibuf, '\0', sizeof(ibuf));

		if ((len = recvfrom(server_socket, ibuf, sizeof(ibuf), NULL, (struct sockaddr*)&clnt, &lc)) == SOCKET_ERROR)
			throw  SetErrorMsgText("GetRequestFromClient:", WSAGetLastError());

		if ((clock() - start_time) / CLOCKS_PER_SEC < 1)
			std::cout << "server_name = \"" << name << "\"" << " ip: " << inet_ntoa(clnt.sin_addr) << ":" << ntohs(clnt.sin_port) << std::endl;
		else
			break;
	}

	if (closesocket(server_socket) == SOCKET_ERROR)
		throw  SetErrorMsgText("closesocket:", WSAGetLastError());
}

// -- ответить на запрос клиента 
// Назначение: функция предназначена пересылки позывного 
//             сервера программе клиента   

bool PutAnswerToClient(
	char* name, //[in] позывной сервера  
	struct sockaddr* to,   //[in] указатель на SOCKADDR_IN
	int* lto   //[in] указатель на размер from 
) {
	SOCKET server_socket;
	if ((server_socket = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		throw  SetErrorMsgText("socket:", WSAGetLastError());

	SOCKADDR_IN serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(2000);
	serv.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_socket, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
		throw  SetErrorMsgText("bind:", WSAGetLastError());

	char ibuf[50];
	memset(ibuf, '\0', sizeof(ibuf));

	if (sendto(server_socket, name, strlen(name) + 1, NULL, to, *lto) == SOCKET_ERROR)
		throw  SetErrorMsgText("PutAnswerToClient", WSAGetLastError());

	if (closesocket(server_socket) == SOCKET_ERROR)
		throw  SetErrorMsgText("closesocket:", WSAGetLastError());

	return true;
}

int main() {
	WSADATA wsaData;
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());

		//..........................
		char* server_name = const_cast<char*>("Hello");

		char ibuf[50];
		gethostname(ibuf, sizeof(ibuf));
		std::cout << "Server name: " << ibuf << std::endl;

		std::cout << "Other servers:" << std::endl;
		std::thread tr(&GetServerCounts, server_name, 2000);
		tr.detach();
		Sleep(2000);
		std::cout << std::endl;

		std::cout << "Connections:" << std::endl;
		while (true) {
			SOCKADDR_IN clnt;
			memset(&clnt, 0, sizeof(clnt));
			int lc = sizeof(clnt);

			GetRequestFromClient(server_name, 2000, (struct sockaddr*)&clnt, &lc);
			PutAnswerToClient(server_name, (struct sockaddr*)&clnt, &lc);

			const char* client_ip = inet_ntoa(clnt.sin_addr);
			std::cout << "Connect from " << gethostbyaddr(const_cast<const char*>((char*)&clnt.sin_addr), sizeof(in_addr), AF_INET)->h_name << ":" << ntohs(clnt.sin_port) << " " << server_name << std::endl;
		}
		//..........................

		if (WSACleanup() == SOCKET_ERROR)
			throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (std::string errorMsgText)
	{
		std::cout << std::endl << errorMsgText;
	}

	system("pause");
	return 0;
}