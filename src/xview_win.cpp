#include "xview_win.h"

#ifndef WITH_X

XView::XView()
{
}

int XView::getFD() const
{
	return -1;
}

void XView::readHandler()
{
}

void XView::update(const std::vector<uint32_t> & /* spectrum */, const std::vector<uint32_t> & /* spectrogram */, const std::vector<uint32_t> & /* constellation */, const std::vector<uint32_t> & /* peakMeter */, const std::vector<int16_t> & /* audioBuffer */)
{
}

void XView::reopenWindows()
{
}

#else

#include <cstring>
#include "decoder_constants.h"
#include "throw.h"
#include "log.h"

static const unsigned OSCILLOSCOPE_INITIAL_WIDTH  = 640;
static const unsigned OSCILLOSCOPE_INITIAL_HEIGHT = 240;
static const unsigned PEAK_METER_HEIGHT           = 16;

XView::XView()
{
	reopenWindows();
}

int XView::getFD() const
{
	return 0;
}

void XView::readHandler()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void XView::update(const std::vector<uint32_t> &spectrum, const std::vector<uint32_t> &spectrogram, const std::vector<uint32_t> &constellation, const std::vector<uint32_t> &peakMeter, const std::vector<int16_t> &audioBuffer)
{
	if(m_res.oscilloscope) {
		createOscilloscope(audioBuffer);
		m_res.oscilloscope->redraw();
	}

	if(m_res.spectrum) {
		createSpectrum(spectrum, spectrogram);
		m_res.spectrum->redraw();
	}

	if(m_res.constellation) {
		createConstellation(constellation, peakMeter);
		m_res.constellation->redraw();
	}
	readHandler();
}

void XView::createOscilloscope(const std::vector<int16_t> &audioBuffer)
{
	m_res.oscilloscope->clear();

	const unsigned width(m_res.oscilloscope->getWidth());
	const unsigned height(m_res.oscilloscope->getHeight());
	unsigned oldx(width), oldy(height);

	for(size_t i(0); i < audioBuffer.size(); ++i) {
		const unsigned x((double) i * m_res.oscilloscope->getWidth() / audioBuffer.size());
		const uint16_t sample(65535 - (audioBuffer[i] + 32768));
		const unsigned y((double) sample * m_res.oscilloscope->getHeight() / 65535);

		/* Can it happen? */
		if(x >= m_res.oscilloscope->getWidth() || y >= m_res.oscilloscope->getHeight()) {
			continue;
		}

		if(oldx != width) {
			m_res.oscilloscope->drawLine(oldx, oldy, x, y, 0xff, 0xff, 0xff);
		}

		oldx = x;
		oldy = y;
	}
}

void XView::createSpectrum(const std::vector<uint32_t> &spectrum, const std::vector<uint32_t> &spectrogram)
{
	for(unsigned x(0); x < decoder_constants::spectrum_width; ++x) {
		bool drawingBlue(false);
		for(unsigned y(0); y < decoder_constants::spectrum_height; ++y) {
			if(spectrum[y * decoder_constants::spectrum_width + x]) {
				m_res.spectrum->setPixel(x, y, 0xff, 0xff, 0xff);
				drawingBlue = true;
			}
			else if(drawingBlue) {
				m_res.spectrum->setPixel(x, y, 0, 0, 0x30);
			}
			else {
				m_res.spectrum->setPixel(x, y, 0, 0, 0);
			}
		}
	}

	for(unsigned y(0); y < decoder_constants::spectrogram_height; ++y) {
		for(unsigned x(0); x < decoder_constants::spectrogram_width; ++x) {
			m_res.spectrum->setPixel(x, y + decoder_constants::spectrum_height, spectrogram[y * decoder_constants::spectrogram_width + x]);
		}
	}
}

void XView::createConstellation(const std::vector<uint32_t> &constellation, const std::vector<uint32_t> &peakMeter)
{
	for(unsigned y(0); y < decoder_constants::constellation_height; ++y) {
		for(unsigned x(0); x < decoder_constants::constellation_width; ++x) {
			const uint8_t rgb(constellation[y * decoder_constants::constellation_width + x] ? 0xff : 0);
			m_res.constellation->setPixel(x, y, rgb, rgb, rgb);
		}
	}

	for(unsigned y(0); y < PEAK_METER_HEIGHT; ++y) {
		for(unsigned x(0); x < decoder_constants::constellation_width; ++x) {
			m_res.constellation->setPixel(x, y + decoder_constants::constellation_height, peakMeter[x / (decoder_constants::constellation_width / decoder_constants::peak_meter_width)]);
		}
	}
}

void XView::reopenWindows()
{
	if(!m_res.oscilloscope) {
		m_res.oscilloscope.reset(new XWindow(m_res, "Oscilloscope", OSCILLOSCOPE_INITIAL_WIDTH, OSCILLOSCOPE_INITIAL_HEIGHT, true));
	}

	if(!m_res.spectrum) {
		m_res.spectrum.reset(new XWindow(m_res, "Spectrum", decoder_constants::spectrum_width, decoder_constants::spectrum_height + decoder_constants::spectrogram_height, false));
	}

	if(!m_res.constellation) {
		m_res.constellation.reset(new XWindow(m_res, "Constellation", decoder_constants::constellation_width, decoder_constants::constellation_height + PEAK_METER_HEIGHT, false));
	}
}

/*
std::unique_ptr<XWindow> *XView::getWindow(Window w)
{
	if(m_res.oscilloscope && m_res.oscilloscope->getWindow() == w) {
		return &m_res.oscilloscope;
	}

	if(m_res.spectrum && m_res.spectrum->getWindow() == w) {
		return &m_res.spectrum;
	}

	if(m_res.constellation && m_res.constellation->getWindow() == w) {
		return &m_res.constellation;
	}

	return nullptr;
}
*/
#endif
