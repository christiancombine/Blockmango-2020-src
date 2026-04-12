#ifndef GZIP_H
#define GZIP_H

#include "zlib.h"
#include "Memory/LordMemory.h"

namespace LORD
{

static void* gzAlloc(void* opaque, unsigned int count, unsigned int size)
{
	return LordMalloc(size * count);
}

static void gzFree(void* opaque, void* ptr)
{
	LordFree(ptr);
}

int gzCompress(Bytef *input, uLong length, Bytef *output, uLong& size)
{
	if (!input || length <= 0)
		return Z_DATA_ERROR;

	z_stream c_stream;
	c_stream.zalloc = gzAlloc;
	c_stream.zfree = gzFree;
	c_stream.opaque = NULL;

	//只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
	int err;
	if ((err = deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
		MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY)) != Z_OK)
		return Z_DATA_ERROR;

	c_stream.next_in = input;
	c_stream.avail_in = length;
	c_stream.next_out = output;
	c_stream.avail_out = size;
	while (c_stream.avail_in != 0 && c_stream.total_out < size)
	{
		if ((err = deflate(&c_stream, Z_NO_FLUSH)) != Z_OK)
			return err;
	}
	if (c_stream.avail_in != 0)
		return c_stream.avail_in;

	for (;;)
	{
		if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END)
			break;
		if (err != Z_OK)
			return err;
	}
	if ((err = deflateEnd(&c_stream)) != Z_OK)
		return err;
	size = c_stream.total_out;
	return Z_OK;
}

int gzUncompress(Byte *input, uLong length, Byte *output, uLong& size)
{
	static char dummy_head[2] = {
		0x8 + 0x7 * 0x10,
		(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	};
	z_stream d_stream = { 0 };
	d_stream.zalloc = gzAlloc;
	d_stream.zfree = gzFree;
	d_stream.opaque = NULL;
	d_stream.next_in = input;
	d_stream.avail_in = 0;
	d_stream.next_out = output;

	//只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
	int err;
	if ((err = inflateInit2(&d_stream, MAX_WBITS + 16)) != Z_OK)
		return err;
	//if ((err = inflateInit2(&d_stream, 47)) != Z_OK)
	//	return err;

	while (d_stream.total_out < size && d_stream.total_in < length)
	{
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END)
			break;
		if (err == Z_OK)
			continue;;
		if (err != Z_DATA_ERROR)
			return err;
		d_stream.next_in = (Bytef*)dummy_head;
		d_stream.avail_in = sizeof(dummy_head);
		if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK)
			return err;
	}
	if ((err = inflateEnd(&d_stream)) != Z_OK)
		return err;
	size = d_stream.total_out;
	return Z_OK;
}

}	// end of namespace

#endif // GZIP_H