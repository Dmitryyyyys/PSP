#include <iostream>
#include <Windows.h>
#include "ErrorHandling.h"
#include <tchar.h>

#define BUFSIZE 1024

using namespace std;

int main()
{
	LPCWSTR lpvMessage = TEXT("Hello from the Client! This is a test message.");

	DWORD  cbRead, cbToWrite, cbWritten;
	TCHAR  chBuf[BUFSIZE];
	TCHAR checkBuf[BUFSIZE] = TEXT("");
	BOOL   fSuccess = FALSE;

	HANDLE hPipe; 

	bool multi = true; //////////////////////////////////////

	try
	{
		do
		{
			if ((hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Tube"),
				PIPE_ACCESS_DUPLEX, //дуплексный канал
				PIPE_TYPE_MESSAGE | PIPE_WAIT, // сообщения|синхронный
				3, NULL, NULL, 
				INFINITE, NULL)) == INVALID_HANDLE_VALUE)
				throw SetPipeError("create:", GetLastError());

			cout << "Waiting.." << endl;

			if (!ConnectNamedPipe(hPipe, NULL)) // ожидать клиента
				throw SetPipeError("connect:", GetLastError());

			cout << "Connected!" << endl;

			//..................................................................
			do
			{
				do
				{
					fSuccess = ReadFile(
						hPipe, 
						chBuf, 
						BUFSIZE * sizeof(TCHAR), 
						&cbRead, 
						NULL); 
				} while (!fSuccess);

				if (!fSuccess)
				{
					throw SetPipeError("readfile:", GetLastError());
				}

				if (!wcscmp(chBuf, checkBuf)) {
					break;
				}

				_tprintf(TEXT("%s\n"), chBuf);

				//..................................................................

				cbToWrite = (lstrlen(chBuf) + 1) * sizeof(TCHAR);

				fSuccess = WriteFile(
					hPipe, 
					chBuf, 
					cbToWrite, 
					&cbWritten, 
					NULL); 

				if (!fSuccess)
				{
					throw SetPipeError("writefile:", GetLastError());
				}

			} while (wcscmp(chBuf, checkBuf) && multi);

			cout << "Disconnected" << endl;

			DisconnectNamedPipe(hPipe);
			CloseHandle(hPipe);
		} while (true);
	}
	catch (string ErrorPipeText)
	{
		cout << endl << ErrorPipeText;

		
	}

}