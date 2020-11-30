// Source ref: https://stackoverflow.com/questions/20595340/loading-a-tga-bmp-file-in-c-opengl

#ifndef _BMP_HEADERS_
#define _BMP_HEADERS_

#include <vector>
#include <fstream>

class BMP
{
private:
	std::uint32_t width, height;
	std::uint16_t BitsPerPixel;
	std::vector<std::uint8_t> Pixels;

public:
	BMP(const char* FilePath);
	std::vector<std::uint8_t> GetPixels() const { return this->Pixels; }
	std::uint32_t GetWidth() const { return this->width; }
	std::uint32_t GetHeight() const { return this->height; }
	bool HasAlphaChannel() { return BitsPerPixel == 32; }
};



#endif