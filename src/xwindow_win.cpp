#ifdef WITH_X

#include <cstring>
#include "xresources_win.h"
#include "xwindow_win.h"
#include <windows.h>
#include "throw.h"

XWindow::XWindow(XResources &res, const std::string &name, unsigned width, unsigned height, bool canResize)
    : m_width(width), m_height(height), m_canResize(canResize)
{
	pixels = (BYTE*)malloc(width * height * 3);

	bmpHeader.biSize = sizeof(bmpHeader);
	bmpHeader.biWidth = width;
	bmpHeader.biHeight = -height;
	bmpHeader.biPlanes = 1;
	bmpHeader.biBitCount = 24;
	bmpHeader.biCompression = BI_RGB;
	bmpHeader.biSizeImage = 0;
	bmpHeader.biXPelsPerMeter = 0;
	bmpHeader.biYPelsPerMeter = 0;
	bmpHeader.biClrUsed = 0;
	bmpHeader.biClrImportant = 0;

	WNDCLASSEX wndclass = {};
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = StaticWindowProc;
	wndclass.hInstance = GetModuleHandle(0);
	wndclass.hIcon = 0;
	wndclass.hCursor = 0;
	wndclass.lpszClassName = name.c_str();

	if(RegisterClassEx(&wndclass)) {
		RECT windowRect = {0, 0, (LONG)width, (LONG)height};
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		if (!m_canResize) dwStyle &= ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, 0);
		hWnd = CreateWindow(name.c_str(), name.c_str(), dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,  windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, GetModuleHandle(NULL), this);
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);
		recreateImage();
	}
}

XWindow::~XWindow() {
	delete pixels;
}

LRESULT CALLBACK XWindow::StaticWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	XWindow *targetWindow = nullptr;
    if (message == WM_NCCREATE)
    {
        CREATESTRUCT *cs = (CREATESTRUCT*) lParam; 
        targetWindow = (XWindow*) cs->lpCreateParams;

        SetLastError(0);
        if (SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) targetWindow) == 0)
        {
            if (GetLastError() != 0)
                return FALSE;
        }
    }
    else
    {
        targetWindow = (XWindow*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (targetWindow && (targetWindow->hWnd == hwnd)) {
			return targetWindow->WindowProc(message, wParam, lParam);
		} else {
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT XWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_PAINT:
			{
				HDC wndDC = BeginPaint(hWnd, &ps);
				SetDIBitsToDevice(wndDC, 0, 0, m_width, m_height, 0, 0, 0, m_height, pixels, reinterpret_cast<BITMAPINFO*>(&bmpHeader), DIB_RGB_COLORS);
				EndPaint(hWnd, &ps);
			}
            break;
		case WM_SIZE:
			if (m_canResize) resize(LOWORD(lParam), HIWORD(lParam));
			break;
        case WM_DESTROY:
            PostQuitMessage(0);       
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
	return 0;
}

void XWindow::recreateImage()
{
	InvalidateRect(hWnd, NULL, true);
}

unsigned XWindow::getWidth() const
{
	return m_width;
}

unsigned XWindow::getHeight() const
{
	return m_height;
}

void XWindow::setPixel(unsigned x, unsigned y, uint8_t red, uint8_t green, uint8_t blue)
{
	xassert(x < m_width && y < m_height, "Pixel coords out of range (pixel at %ux%u, size is %ux%u)", x, y, m_width, m_height);
	pixels[(y * m_width + x) * 3]     = blue;
	pixels[(y * m_width + x) * 3 + 1] = green;
	pixels[(y * m_width + x) * 3 + 2] = red;
}

void XWindow::setPixel(unsigned x, unsigned y, uint32_t rgb)
{
	xassert(x < m_width && y < m_height, "Pixel coords out of range (pixel at %ux%u, size is %ux%u)", x, y, m_width, m_height);
	pixels[(y * m_width + x) * 3]     = (rgb >> 16) & 0xff;
	pixels[(y * m_width + x) * 3 + 1] = (rgb >> 8) & 0xff;
	pixels[(y * m_width + x) * 3 + 2] = rgb >> 24;
}

void XWindow::drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, uint8_t red, uint8_t green, uint8_t blue)
{
	xassert(x1 < m_width && y1 < m_height, "Line start out of range (point at %ux%u, size is %ux%u)", x1, y1, m_width, m_height);
	xassert(x2 < m_width && y2 < m_height, "Line end out of range (point at %ux%u, size is %ux%u)", x2, y2, m_width, m_height);

	if(abs((int)x2 - (int)x1) > abs((int)y2 - (int)y1)) {
		if(x1 > x2) {
			unsigned tmp(x1);
			x1 = x2;
			x2 = tmp;

			tmp = y1;
			y1  = y2;
			y2  = tmp;
		}

		const unsigned dx(x2 - x1);
		int dy(y2 - y1);

		int yi(1);
		if(dy < 0) {
			yi = -1;
			dy = -dy;
		}

		int d(2 * dy - dx);
		unsigned y(y1);

		for(unsigned x(x1); x <= x2; ++x) {
			setPixel(x, y, red, green, blue);

			if(d > 0) {
				y += yi;
				d -= 2 * dx;
			}

			d += 2 * dy;
		}
	}
	else {
		if(y1 > y2) {
			unsigned tmp(x1);
			x1 = x2;
			x2 = tmp;

			tmp = y1;
			y1  = y2;
			y2  = tmp;
		}

		int dx(x2 - x1);
		const unsigned dy(y2 - y1);

		int xi(1);
		if(dx < 0) {
			xi = -1;
			dx = -dx;
		}

		int d(2 * dx - dy);
		unsigned x(x1);

		for(unsigned y(y1); y <= y2; ++y) {
			setPixel(x, y, red, green, blue);

			if(d > 0) {
				x += xi;
				d -= 2 * dy;
			}

			d += 2 * dx;
		}
	}
}

void XWindow::clear()
{
	memset(pixels, 0, m_width * m_height * 3);
}

void XWindow::resize(unsigned width, unsigned height)
{
	m_width  = width;
	m_height = height;
	bmpHeader.biWidth = width;
	bmpHeader.biHeight = -height;
	pixels = (BYTE*)realloc(pixels, width * height * 3);
	recreateImage();
}

void XWindow::redraw()
{
	recreateImage();
}

#endif
