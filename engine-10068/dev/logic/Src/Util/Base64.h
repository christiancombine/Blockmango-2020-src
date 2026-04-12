#ifndef BASE64_H
#define BASE64_H
#include <string>

/* Type define */
typedef unsigned char byte;
typedef unsigned int uint32;

/* Base64 declaration. */
class Base64 {
public:
	Base64() = default;
	~Base64() = default;
	Base64(const Base64&) = delete;
	Base64& operator=(const Base64&) = delete;

	static uint32 encodeBound(uint32 length) { return (length + 2) / 3 * 4; }
	static uint32 decodeBound(uint32 length) { return length / 4 * 3; }

	static std::string encode(const std::string& input);
	static std::string encode(const uint8_t input[], size_t size);

	static bool decode(const std::string& input, std::string& output);

private:
	static const char base64chars[];
	static unsigned char base64map[256];
};

#endif /*BASE64_H*/
