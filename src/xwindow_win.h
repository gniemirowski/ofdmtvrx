#pragma once

#ifdef WITH_X

#include <string>
#include <windows.h>

class XResources;

class XWindow {
public:
	XWindow(XResources &res, const std::string &name, unsigned width, unsigned height, bool canResize);
	~XWindow();

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	unsigned getWidth() const;
	unsigned getHeight() const;

	/* TODO: This approach might be slow. If it is, rethink. */
	void setPixel(unsigned x, unsigned y, uint8_t red, uint8_t green, uint8_t blue);
	void setPixel(unsigned x, unsigned y, uint32_t rgb);
	void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, uint8_t red, uint8_t green, uint8_t blue);
	void clear();
	void resize(unsigned width, unsigned height);
	void redraw();

private:
	HWND hWnd;
	BYTE * pixels;
	PAINTSTRUCT ps = {};
	BITMAPINFOHEADER bmpHeader = {};
	unsigned m_width{0};
	unsigned m_height{0};
	bool m_canResize{false};

	void recreateImage();
};

#endif
