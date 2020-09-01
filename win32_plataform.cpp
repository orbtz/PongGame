#include "utils.cpp";
#include <windows.h>

global_variable bool running = true;

struct Render_State {
	int height, width;
	void* memory;

	BITMAPINFO bitmap_info;
};
global_variable Render_State render_state;
#include "renderer.cpp";

LRESULT CALLBACK windows_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	
	switch (uMsg) {
		case WM_CLOSE:
		case WM_DESTROY: {
			running = false;
		} break;

		case WM_SIZE: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;

			int size = render_state.width * render_state.height * sizeof(unsigned int);

			if (render_state.memory) {
				VirtualFree(render_state.memory, 0, MEM_RELEASE);
			}

			render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
			render_state.bitmap_info.bmiHeader.biPlanes = 1;
			render_state.bitmap_info.bmiHeader.biBitCount = 32;
			render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
		}

		default: {
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	return result;
}

int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,	int nShowCmd) {

	// Create Window Class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = L"Game Window Class";
	window_class.lpfnWndProc = windows_callback;

	// Register Class
	RegisterClass(&window_class);

	// Render the Window
	HWND window = CreateWindow(window_class.lpszClassName, L"First Game", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 880, 420, 0, 0, hInstance, 0);
	HDC hdc = GetDC(window);

	while (running) {
		// Input
		MSG message;
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		// Simulate
		clear_screen( 0x71ABAB );
		//render_background();
		draw_rect(0, 0, 20, 20, 0xff748c);
		draw_rect(-70, 0, 7, 10, 0xe5b77d);
		draw_rect(60, 40, 5, 25, 0x772271);
		draw_rect(25, 15, 5, 5, 0x93b793);

		// Render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}

}