#include "Base64.h"
#include <stdexcept>
#include <new>
#include <cstring>
#include <sstream>

const char Base64::base64chars[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=" };
unsigned char Base64::base64map[256] = { 0 };

static bool adjust(std::string& str, uint32 size)
{
	if (str.size() == size)
		return true;
	try
	{
		str.resize(size);
	}
	catch (const std::length_error&) {
		return false;
	}
	catch (const std::bad_alloc&) {
		return false;
	}
	return true;
}

std::string Base64::encode(const std::string& input)
{
	return encode(reinterpret_cast<const uint8_t*>(input.data()), input.size());
}

std::string Base64::encode(const uint8_t input[], size_t size)
{
	std::ostringstream os;
	for (uint32 i = 0; i < size / 3; i++, input += 3)
	{
		os << base64chars[input[0] >> 2];
		os << base64chars[((input[0] & 3) << 4) + (input[1] >> 4)];
		os << base64chars[((input[1] & 0xf) << 2) + (input[2] >> 6)];
		os << base64chars[input[2] & 0x3f];
	}
	int rest = size % 3;
	if (rest)
	{
		os << base64chars[input[0] >> 2];
		if (rest > 1)
		{
			os << base64chars[((input[0] & 3) << 4) + (input[1] >> 4)];
			os << base64chars[(input[1] & 0xf) << 2];
			os << "=";
		}
		else
		{
			os << base64chars[(input[0] & 3) << 4];
			os << "==";
		}
	}
	return os.str();
}

bool Base64::decode(const std::string& input, std::string& output)
{
	if (!base64map[0])
	{
		std::memset(base64map, 255, sizeof(base64map));
		for (int i = 0; i <= 64; i++)
			base64map[base64chars[i]] = i;
	}
	uint32 bound = decodeBound(input.length());
	if (!adjust(output, bound))
		return false;

	char *p = &output[0];
	unsigned int r = 3, j = 0, n = 0;
	for (uint32 i = 0; i < input.length(); i++)
	{
		unsigned int c = base64map[input[i]];
		if (c == 255)
			continue;
		if (c == 64)
		{
			c = 0;
			r--;
		}

		n = (n << 6) | c;
		if (++j != 4)
			continue;
		*p++ = static_cast<char>((n >> 16) & 255);
		if (r > 1)
			*p++ = static_cast<char>((n >> 8) & 255);
		if (r > 2)
			*p++ = static_cast<char>(n & 255);
		if (r < 3)
			break;
		n = j = 0;
	}
	return true;
}

