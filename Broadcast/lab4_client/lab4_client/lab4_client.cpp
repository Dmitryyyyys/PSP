﻿#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>

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

// -- послать запрос серверу и получить ответ   
// Назначение: функция предназначена широковещательной 
//             пересылки позывного серверу и приема от 
//             сервера ответа.   

bool  GetServer(
	char* call, //[in] позывной сервера  
	short            port, //[in] номер порта сервера    
	struct sockaddr* from, //[out] указатель на SOCKADDR_IN
	int* flen  //[out] указатель на размер from 
) {
	
		SOCKET client_socket;
	if ((client_socket = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		throw SetErrorMsgText("socket:", WSAGetLastError());

	bool trueflag = true;
	if (setsockopt(client_socket, SOL_SOCKET, SO_BROADCAST,
		(char*)&trueflag, sizeof(trueflag)) < 0)
		throw  SetErrorMsgText("setsockopt:", WSAGetLastError());

	SOCKADDR_IN serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = inet_addr("192.168.43.255");

	if (sendto(client_socket, call, strlen(call) + 1, NULL,
		(sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR)
		throw  SetErrorMsgText("sendto:", WSAGetLastError());

	char ibuf[50];
	memset(ibuf, '\0', sizeof(ibuf));

	while (strcmp(call, ibuf) != 0) {
		if (recvfrom(client_socket, ibuf, sizeof(ibuf), NULL, from, flen) == SOCKET_ERROR)
			throw  SetErrorMsgText("GetServer:", WSAGetLastError());
	}

	if (closesocket(client_socket) == SOCKET_ERROR)
		throw SetErrorMsgText("closesocket:", WSAGetLastError());

	return false;
}

// -- послать запрос серверу, заданному символическим именем     
// Назначение: функция предназначена пересылки позывного  
//             серверу, адрес которого задан в виде  
//             символического имени компьютера.   

bool  GetServerByName(
	char* name, //[in] имя компьютера в сети  
	char* call, //[in] позывной   
	struct sockaddr* from, //[in,out] указатель на SOCKADDR_IN
	int* flen  //[in,out] указатель на размер from 
) {
	hostent* host = gethostbyname(name);
	if (host == nullptr)
		return true;

	std::cout << "exist host \"" << name << "\"" << std::endl;
	SOCKET client_socket;
	if ((client_socket = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		throw SetErrorMsgText("socket:", WSAGetLastError());

	bool trueflag = true;
	if (setsockopt(client_socket, SOL_SOCKET, SO_BROADCAST,
		(char*)&trueflag, sizeof(trueflag)) < 0)
		throw  SetErrorMsgText("setsockopt:", WSAGetLastError());

	SOCKADDR_IN serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(2000);
	serv.sin_addr.s_addr = *(long*)host->h_addr_list[0];

	if (sendto(client_socket, call, strlen(call) + 1, NULL,
		(sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR)
		throw  SetErrorMsgText("sendto:", WSAGetLastError());

	char ibuf[50];
	memset(ibuf, '\0', sizeof(ibuf));

	while (strcmp(call, ibuf) != 0) {
		if (recvfrom(client_socket, ibuf, sizeof(ibuf), NULL, from, flen) == SOCKET_ERROR)
			throw  SetErrorMsgText("GetServer:", WSAGetLastError());
	}

	if (closesocket(client_socket) == SOCKET_ERROR)
		throw SetErrorMsgText("closesocket:", WSAGetLastError());

	return false;
}

int main(int argc, char* argv[]) {
	WSADATA wsaData;
	try {
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup:", WSAGetLastError());

		//........................................
		SOCKADDR_IN serv;
		memset(&serv, 0, sizeof(serv));
		int lc = sizeof(serv);

		char* server_call = const_cast<char*>("Hello");
		char* server_name = const_cast<char*>("oracle2");
		if (GetServerByName(server_name, server_call, (struct sockaddr*)&serv, &lc)) {
			std::cout << "not exist host \"" << server_name << "\"" << std::endl;
		}

		std::cout << "Connect from " << inet_ntoa(serv.sin_addr) << ":" << ntohs(serv.sin_port) << " " << server_name << std::endl;
		//........................................

		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (std::string errorMsgText) {
		std::cout << std::endl << errorMsgText;
	}

	system("pause");
	return 0;
}
