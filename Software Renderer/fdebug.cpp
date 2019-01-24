#include "fdebug.h"

FDebug * FDebug::GetInstance()
{
	static FDebug kInstance;
	return &kInstance;
}

FDebug::FDebug()
{
	AllocConsole();
	freopen_s(&console_fp, "CONOUT$", "w", stdout);
}


FDebug::~FDebug()
{
	fclose(console_fp);
}

void FDebug::AssertHresult(HRESULT hr) const
{
	if (FAILED(hr))
	{
		_com_error err(hr);

		LPCTSTR error_message = err.ErrorMessage();
		std::wcout << error_message << "\n";
		MessageBox(NULL
			, error_message
			, (LPCWSTR)L"Assert Details"
			, MB_ICONSTOP | MB_OK | MB_DEFBUTTON1
		);

		// UNDONE MiniDumpWriteDump 
		exit(hr);
	}
}

void FDebug::Assert(const char *message) const
{
	std::cout << message << "\n";

	WCHAR message_wchar[256];
	memset(message_wchar, 0, sizeof(message_wchar));
	MultiByteToWideChar(CP_ACP
		, 0
		, message
		, strlen(message) + 1
		, message_wchar
		, sizeof(message_wchar) / sizeof(message_wchar[0]));

	MessageBox(NULL
		, message_wchar
		, (LPCWSTR)L"Assert Details"
		, MB_ICONSTOP | MB_OK | MB_DEFBUTTON1
	);

	exit(-1);
}