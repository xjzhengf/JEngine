#include "stdafx.h"
#include "GameInstance.h"

#ifdef _WIN32
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(2907);
#endif
	try
	{
		std::unique_ptr<GameInstance> mGameInstance = std::make_unique<GameInstance>();
		mGameInstance->Init(hInstance);
		mGameInstance->UpDate();
		mGameInstance->Destroy();
		return 0;
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}
#else

int main() {
	std::cout << "其他平台" << std::endl;
	return 0;
}
#endif