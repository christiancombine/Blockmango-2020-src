#pragma once
struct FloatProxy
{
	float value{};
};

struct BoolProxy
{
	bool value{};
};

struct IntProxy
{
	int value{};
};

struct LongProxy
{
	long long value{};
};

struct UI64Proxy
{
	LORD::ui64 value{};
};

struct StringProxy
{
	std::string value{};
};

struct Vector3Proxy
{
	float x{};
	float y{};
	float z{};
};

struct Vector3iProxy
{
	int x{};
	int y{};
	int z{};
};

struct ShoppingAttr
{
	bool isConsumeCoin{};
	bool isAddGoods{};
};

struct CommodityInfo
{
	int goodsId{};
	int goodsNum{};
	int goodsMeta{};
	int coinId{};
	int coinNum{};
};