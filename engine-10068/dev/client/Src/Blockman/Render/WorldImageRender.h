#pragma once
#ifndef __WORLD_IMAGE_RENDER_HEADER__
#define __WORLD_IMAGE_RENDER_HEADER__

#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class AtlasSprite;

	class FloatNumber
	{
	public:
		list<AtlasSprite*>::type m_spriteList;
		Vector3 m_position;
		int m_duration{ 500 };
		float m_scale{ 1.0f };
	};

	class StaticNumber
	{
	public:
		String m_name{ "" };
		list<AtlasSprite*>::type m_spriteList;
		Vector3 m_position;
		int m_duration{ 500 };
		float m_scale{ 1.0f };
		float m_yaw{ 0.0f };
	};

	class StaticImage
	{
	public:
		String m_name{ "" };
		AtlasSprite* m_sprite;
		Vector3 m_position;
		float m_scale{ 1.0f };
		bool m_isCamera = { false };
		float m_yaw{ 0.0f };
	};

	class WorldImageRender : public Singleton<WorldImageRender>, public ObjectAlloc
	{
	private:
		map<String, AtlasSprite*>::type m_numberSpriteMap;
		list<FloatNumber>::type m_floatNumberList;
		list<StaticNumber>::type m_staticNumberList;
		list<StaticImage>::type m_staticImageList;

	public:
		WorldImageRender();
		~WorldImageRender();

		void addImageRender(const String& atlas, int color, const String& numberText, const String& image);

		void render(int frameTime);

		void addFloatNumber(const String& numberText, Vector3 position, float scale, int color);

		void addStaticNumber(const String& name, const String& numberText, Vector3 position, int duration, float scale, int color, float yaw);
		void removeStaticNumber(const String& name);

		void addStaticImage(const String& name, const String& atlas, const String& sprite, Vector3 position, float scale, bool isCamera, float yaw);
		void removeStaticImage(const String& name);

		void removeAllFloatNumber();

	private:
		void renderFloatNumber(int frameTime);
		void renderStaticNumber(int frameTime);
		void renderStaticImage(int frameTime);

		void removeExpiredFloatNumber();
		void removeExpiredStaticNumber();
	};
}

#endif