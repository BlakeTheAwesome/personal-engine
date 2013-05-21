#include <windows.h>

#include "BlakesEngine\Core\beTypeTests.h"

//int main()
//{
//	beTypeTests::RunTypeTests();
//}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
	beTypeTests::RunTypeTests();
}