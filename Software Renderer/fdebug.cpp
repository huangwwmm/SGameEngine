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

void FDebug::AssertHresult(HRESULT hr)
{
	if (FAILED(hr))
	{
		_com_error err(hr);

		LPCTSTR error_message = err.ErrorMessage();
		std::wcout << error_message << "\n";
		MessageBox(
			NULL
			, error_message
			, (LPCWSTR)L"Assert Details"
			, MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2
		);

		exit(hr);
	}
}