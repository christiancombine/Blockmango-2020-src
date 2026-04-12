/********************************************************************
filename: 	Serialize
file path:	H:\sandboxol\client\blockmango-client\dev\common\Src\network\packet\Serialize.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/
#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__
#include <map>
#include <memory>
#include "Network/protocol/DataPacket.h"
#include "raknet/include/BitStream.h"
#include "TypeDef.h"
#include "Core.h"

#include <array>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <Std/type_traits.h>
#include <memory>
#include "TypeTraits/RemoveSmartPointer.h"
#include "TypeTraits/IsSmartPointer.h"
#include "TypeTraits/IsTemplateOf.h"

using namespace LORD;

template<typename T>
struct is_encodableClass
{
private:
	template<typename U>
	static uint8_t test(decltype(&U::netWrite, &U::netRead)) { return 0; }
	template<typename U>
	static uint16_t test(...) { return 0; }
public:
	static constexpr bool value = sizeof(test<T>(0)) == sizeof(uint8_t);
};

template<typename T>
struct is_support7bitEncodeType
{
public:
	static constexpr bool value = (std::is_integral<T>::value 
		|| std::is_enum<T>::value);
};

template<typename T>
struct is_arrary_container
{
public:
	static constexpr bool value = false;
};

template<typename T, typename A>
struct is_arrary_container< std::vector<T, A> >
{
	static constexpr bool value = true;
};

template<typename T, typename A>
struct is_arrary_container< std::list<T, A> >
{
	static constexpr bool value = true;
};

template<typename T>
struct is_map_container
{
	static constexpr bool value = false;
};

template<typename T1, typename T2, typename T3, typename T4>
struct is_map_container< std::map<T1, T2, T3, T4> >
{
	static constexpr bool value = true;
};

template<typename T>
struct is_std_array
{
	static constexpr bool value = false;
};

template<typename T, size_t size>
struct is_std_array<std::array<T, size>>
{
	static constexpr bool value = true;
};

class Serializer
{
public:
	Serializer(){};
	~Serializer(){};

	template<typename T>
	static void write(const T &t, RakNet::BitStream* stream)
	{
		innerWrite<T>(t, stream);
	}

	template<typename T>
	static void read(T &t, RakNet::BitStream* stream)
	{
		innerRead(t, stream);
	}

	template<typename ... Args>
	static void writeMultiple(RakNet::BitStream* stream, Args&& ... args)
	{
		using swallow_t = int[];
		(void)swallow_t {0, (write(std::forward<Args>(args), stream), 0) ...};
	}

	template<typename ... Args>
	static void readMultiple(RakNet::BitStream* stream, Args&& ... args)
	{
		using swallow_t = int[];
		(void)swallow_t {0, (read(std::forward<Args>(args), stream), 0) ...};
	}

	template<typename T>
	static typename  std::enable_if<is_encodableClass<T>::value, void>::type innerWrite(const T &t, RakNet::BitStream* stream)
	{
		t.netWrite(stream);
	}

	template<typename T>
	static typename  std::enable_if<is_encodableClass<T>::value, void>::type innerRead(T &t, RakNet::BitStream* stream)
	{
		t.netRead(stream);
	}

	template<typename T>
	static typename  std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, void>::type innerWrite(const T &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, void>::type innerRead(T &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<std::is_integral<T>::value && !std::is_unsigned<T>::value, void>::type innerWrite(const T& t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<std::is_integral<T>::value && !std::is_unsigned<T>::value, void>::type innerRead(T& t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<std::is_enum<T>::value, void>::type innerWrite(const T &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<std::is_enum<T>::value, void>::type innerRead(T &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<is_arrary_container<T>::value, void>::type innerWrite(const T &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<is_arrary_container<T>::value, void>::type innerRead(T &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<is_std_array<T>::value, void>::type innerWrite(const T &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<is_std_array<T>::value, void>::type innerRead(T &t, RakNet::BitStream* stream);

	template<typename T, std::size_t size>
	static void writeFromArray(const std::array<T, size> &t, RakNet::BitStream* stream);
	
	template<typename T, std::size_t size>
	static void readToArray(std::array<T, size> &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<is_map_container<T>::value, void>::type innerWrite(const T &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<is_map_container<T>::value, void>::type innerRead(T &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<std::is_pointer<T>::value, void>::type innerWrite(const T &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<std::is_pointer<T>::value, void>::type innerRead(T &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<BLOCKMAN::is_smart_pointer<T>::value, void>::type innerWrite(const T &t, RakNet::BitStream* stream);

	template<typename T>
	static typename  std::enable_if<BLOCKMAN::is_smart_pointer<T>::value, void>::type innerRead(T &t, RakNet::BitStream* stream);

private:
};



template<typename T>
inline
typename  std::enable_if<std::is_enum<T>::value, void>::type Serializer::innerWrite(const T &t, RakNet::BitStream* stream)
{
	auto wt = static_cast<std::underlying_type_t<T>>(t);
	innerWrite(wt, stream);
}

template<typename T>
inline
typename  std::enable_if<std::is_enum<T>::value, void>::type Serializer::innerRead(T &t, RakNet::BitStream* stream)
{
	std::underlying_type_t<T> rt;
	innerRead(rt, stream);
	t = static_cast<T>(rt);
}


template<typename T>
inline 
typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, void>::type Serializer::innerWrite(const T &t, RakNet::BitStream* stream)
{
	auto temp = t;
	while ((temp >> 7) != 0)
	{
		ui8 wt = (ui8)(temp | 0x80);
		stream->Write(wt);
		temp = temp >> 7;
	}
	stream->Write((ui8)(temp & 0x7f));
}

template<typename T>
inline
typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, void>::type Serializer::innerRead(T &t, RakNet::BitStream* stream)
{
	ui32 bitShift = 0;
	T temp = 0;
	T ret = 0;
	do
	{
		if (bitShift >= sizeof(T) * 8)
		{
			throw std::logic_error("Failed to read Integer: integer overflow");
		}
		ui8 readByte;
		if (!stream->Read(readByte))
		{
			throw std::logic_error("Failed to read Integer: reached end of stream");
		}
		temp = readByte;
		ret |= ((temp & 0x7f) << bitShift);
		bitShift += 7;
	} while ((temp & 0x80) != 0);

	t = ret;
}


template<typename T>
inline
typename std::enable_if<std::is_integral<T>::value && !std::is_unsigned<T>::value, void>::type Serializer::innerWrite(const T& t, RakNet::BitStream* stream)
{
	auto temp = static_cast<std::make_unsigned_t<T>>((t << 1) ^ (t >> (sizeof(T) * 8 - 1)));
	innerWrite(temp, stream);
}

template<typename T>
inline
typename std::enable_if<std::is_integral<T>::value && !std::is_unsigned<T>::value, void>::type Serializer::innerRead(T& t, RakNet::BitStream* stream)
{
	std::make_unsigned_t<T> temp;
	innerRead(temp, stream);
	t = static_cast<T>(temp);
	t = static_cast<T>(temp >> 1) ^ (-(t & 1));
}


template<>
 inline void Serializer::write(const std::string & t, RakNet::BitStream* stream)
{
	ui32 len = t.size();
	Serializer::write(len, stream);
	stream->Write(t.c_str(), t.size());
}

template<>
 inline void Serializer::read(std::string & t, RakNet::BitStream* stream)
{
	ui32 stringLen;
	Serializer::read(stringLen,stream);
	if (stringLen == 0)
	{
		t.clear();
		return;
	}
	auto buf = std::make_unique<char[]>(stringLen);
	if (!stream->Read(buf.get(), stringLen))
	{
		throw std::logic_error("Failed to read string: reached end of stream");
	}
	t = std::string(buf.get(), stringLen);
}

 template<>
 inline void Serializer::write(const String & t, RakNet::BitStream* stream)
 {
	 ui32 len = t.size();
	 Serializer::write(len, stream);
	 stream->Write(t.c_str(), t.size());
 }

 template<>
 inline void Serializer::read(String & t, RakNet::BitStream* stream)
 {
	 ui32 stringLen;
	 Serializer::read(stringLen, stream);
	 if (stringLen == 0)
	 {
		 t.clear();
		 return;
	 }
	 auto buf = std::make_unique<char[]>(stringLen);
	 if (!stream->Read(buf.get(), stringLen))
	 {
		 throw std::logic_error("Failed to read String: reached end of stream");
	 }
	 t = String(buf.get(), stringLen);
 }

 template<>
 inline void Serializer::write(const bool & t, RakNet::BitStream* stream)
 {
	 stream->Write(t);
 }

 template<>
 inline void Serializer::read(bool & t, RakNet::BitStream* stream)
 {
	 bool rt;
	 if (!stream->Read(rt))
	 {
		 throw std::logic_error("Failed to read bool: reached end of stream");
	 }
	 t = rt;
 }

 template<>
 inline void Serializer::write(const Real & t, RakNet::BitStream* stream)
 {
	 Real wt = t;
	 stream->Write((char *)&wt, sizeof(Real));
 }

 template<>
 inline void Serializer::read(Real & t, RakNet::BitStream* stream)
 {
	 Real rt;
	 if (!stream->Read((char*)&rt, sizeof(rt)))
	 {
		 throw std::logic_error("Failed to read Real: reached end of stream");
	 }
	 t = rt;
 }

 template<>
 inline void Serializer::write(const Vector3i & t, RakNet::BitStream* stream)
 {
	 Serializer::write(t.x, stream);
	 Serializer::write(t.y, stream);
	 Serializer::write(t.z, stream);
 }

 template<>
 inline void Serializer::read(Vector3i & t, RakNet::BitStream* stream)
 {
	 Serializer::read(t.x, stream);
	 Serializer::read(t.y, stream);
	 Serializer::read(t.z, stream);
 }

 template<>
 inline void Serializer::write(const Vector3 & t, RakNet::BitStream* stream)
 {
	 Serializer::write(t.x, stream);
	 Serializer::write(t.y, stream);
	 Serializer::write(t.z, stream);
 }

 template<>
 inline void Serializer::read(Vector3 & t, RakNet::BitStream* stream)
 {
	 Serializer::read(t.x, stream);
	 Serializer::read(t.y, stream);
	 Serializer::read(t.z, stream);
 }


 template<typename T>
 inline
	 typename  std::enable_if<is_arrary_container<T>::value, void>::type Serializer::innerWrite(const T &t, RakNet::BitStream* stream)
 {
	 Serializer::write(t.size(), stream);
	 for (typename T::value_type ele : t)
	 {
		 Serializer::write(ele, stream);
	 }
 }

 template<typename T>
 inline 
	 typename  std::enable_if<is_arrary_container<T>::value, void>::type Serializer::innerRead(T &t, RakNet::BitStream* stream)
 {
	 size_t size = 0;
	 Serializer::read(size, stream);
	 t.clear();
	 while(size != 0)
	 {
		 size--;
		 typename T::value_type node = {};
		 Serializer::read(node, stream);
		 t.push_back(node);
	 }
 }

 template<typename T>
 inline typename  std::enable_if<is_std_array<T>::value, void>::type Serializer::innerWrite(const T &t, RakNet::BitStream* stream)
 {
	 Serializer::writeFromArray(t, stream);
 }

 template<typename T>
 inline typename  std::enable_if<is_std_array<T>::value, void>::type Serializer::innerRead(T &t, RakNet::BitStream* stream)
 {
	 Serializer::readToArray(t, stream);
 }

 template<typename T, std::size_t size>
 inline void Serializer::writeFromArray(const std::array<T, size>& t, RakNet::BitStream* stream)
 {
	 for (const auto& element : t)
	 {
		 Serializer::write(element, stream);
	 }
 }

 template<typename T, std::size_t size>
 inline void Serializer::readToArray(std::array<T, size>& t, RakNet::BitStream* stream)
 {
	 for (auto& element : t)
	 {
		 Serializer::read(element, stream);
	 }
 }

 template<typename T>
 inline
	 typename  std::enable_if<std::is_pointer<T>::value, void>::type Serializer::innerWrite(const T &t, RakNet::BitStream* stream)
 {
	 //static_assert(!std::is_polymorphic<std::remove_pointer_t<T>>::value, "polymorphic type is not supported");
	 Serializer::write(*t, stream);
 }

 template<typename T>
 inline
	 typename  std::enable_if<std::is_pointer<T>::value, void>::type Serializer::innerRead(T &t, RakNet::BitStream* stream)
 {
	 //static_assert(!std::is_polymorphic<std::remove_pointer_t<T>>::value, "polymorphic type is not supported");
	 t = LordNew(std::remove_pointer_t<T>);
	 Serializer::read(*t, stream);
 }

 template<typename T>
 inline
	 typename  std::enable_if<BLOCKMAN::is_smart_pointer<T>::value, void>::type Serializer::innerWrite(const T &t, RakNet::BitStream* stream)
 {
	// static_assert(!std::is_polymorphic<BLOCKMAN::remove_smart_pointer_t<T>>::value, "polymorphic type is not supported");
	 Serializer::write(*t, stream);
 }

 template<typename T>
 inline
	 typename  std::enable_if<BLOCKMAN::is_smart_pointer<T>::value, void>::type Serializer::innerRead(T &t, RakNet::BitStream* stream)
 {
	 //static_assert(!std::is_polymorphic<BLOCKMAN::remove_smart_pointer_t<T>>::value, "polymorphic type is not supported");
	 t.reset(LordNew(BLOCKMAN::remove_smart_pointer_t<T>));
	 Serializer::read(*t, stream);
 }

#endif


