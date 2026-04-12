#pragma once

#ifndef __HEAD_TEXT_RENDERER_HEADER__
#define __HEAD_TEXT_RENDERER_HEADER__

#include "Core.h"
#include "UI/GUIString.h"

using namespace LORD;
namespace BLOCKMAN
{
	struct NineGridOffset
	{
		float fLeft = 0.0f;
		float fRight = 1.0f;
		float fTop = 0.0f;
		float fBottom = 1.0f;
		float fLeftScaleMax = 1.0f;
		float fLeftScaleMin = 0.0f;
		float fRightScaleMax = 1.0f;
		float fRightScaleMin = 0.0f;
		float fTopScaleMax = 1.0f;
		float fTopScaleMin = 0.0f;
		float fBottomScaleMax = 1.0f;
		float fBottomScaleMin = 0.0f;
		int iSpecialAreaType = 0;// 0:none, 1:left, 2:top, 3:right, 4:bottom
		float fSpecialOffset = 0.0f;
		float fSpecialOffsetCross = 0.0f;
		float fSpecialOffsetScaleMax = 1.0f;
		float fSpecialOffsetScaleMin = 0.0f;
		float fSpecialOffsetCrossScaleMax = 1.0f;
		float fSpecialOffsetCrossScaleMin = 0.0f;
		float fSpecialAreaCenterPercent = 0.5f;// from left to right, or top to bottom

		NineGridOffset(float l, float t, float r, float b, float lsMax = 1.0f, float lsMin = 0.0f, float tsMax = 1.0f, float tsMin = 0.0f, float rsMax = 1.0f, float rsMin = 0.0f, float bsMax = 1.0f, 
			float bsMin = 0.0f, int spcAreaType = 0, float spcOffset = 0.0f, float spcOffsetCross = 0.0f, float spcOffsetScaleMax = 1.0f, float spcOffsetScaleMin = 0.0f, 
			float spcOffsetCrossScaleMax = 1.0f, float spcOffsetCrossScaleMin = 0.0f, float spcAreaCenterPercent = 0.5f)
			: fLeft(l)
			, fRight(r)
			, fTop(t)
			, fBottom(b)
			, fLeftScaleMax(lsMax)
			, fLeftScaleMin(lsMin)
			, fRightScaleMax(rsMax)
			, fRightScaleMin(rsMin)
			, fTopScaleMax(tsMax)
			, fTopScaleMin(tsMin)
			, fBottomScaleMax(bsMax)
			, fBottomScaleMin(bsMin)
			, iSpecialAreaType(spcAreaType)
			, fSpecialOffset(spcOffset)
			, fSpecialOffsetCross(spcOffsetCross)
			, fSpecialOffsetScaleMax(spcOffsetScaleMax)
			, fSpecialOffsetScaleMin(spcOffsetScaleMin)
			, fSpecialOffsetCrossScaleMax(spcOffsetCrossScaleMax)
			, fSpecialOffsetCrossScaleMin(spcOffsetCrossScaleMin)
			, fSpecialAreaCenterPercent(spcAreaCenterPercent)
		{}
	};

	struct RenderAreaInfo
	{
		Vector3 lt;
		Vector3 rb;
		Vector3 lb;
		Vector3 rt;
		ERect	uv;

		RenderAreaInfo()
			: lt(Vector3::ZERO)
			, rb(Vector3::ZERO)
			, lb(Vector3::ZERO)
			, rt(Vector3::ZERO)
		{}

	};

	class HeadTextRenderer
	{
	public:
		struct HeadTextInfo
		{
			LORD::GUIString text;
			LORD::Color	  color;
		};
		struct CharacterInfo
		{
			LORD::Vector2  pos;
			LORD::Vector2  size;
			LORD::Vector2  offset;
			LORD::uint     color;
			LORD::ERect	 uv;
			LORD::Texture* tex;
		};

		enum NineGridAreaType
		{
			NGAT_LEFT_TOP,
			NGAT_MIDDLE_TOP,
			NGAT_RIGHT_TOP,
			NGAT_LEFT_MIDDLE,
			NGAT_MIDDLE_MIDDLE,
			NGAT_RIGHT_MIDDLE,
			NGAT_LEFT_BOTTOM,
			NGAT_MIDDLE_BOTTOM,
			NGAT_RIGHT_BOTTOM,
			NGAT_MAX_TYPE,
		};

	public:
		static void renderHeadText(const LORD::String& txt, LORD::Vector3 pos, bool canBeOccluded,
			Texture* backTexture = NULL, Color color1 = Color::INVALID, bool enableAlpha = false);

		static void renderHeadText(const LORD::String& txt, LORD::Vector3 pos, bool canBeOccluded, float scale,
			Texture* backTexture = NULL, Color color1 = Color::INVALID, bool enableAlpha = false);

		static void renderWordWrap(const LORD::String& txt, LORD::Vector3 pos, bool canBeOccluded, float scale, Texture* chatBubble, Color color, Matrix4 matBall);

	private: 
		static void renderText(LORD::vector<CharacterInfo>::type buffer, LORD::Vector3 pos, bool canBeOccluded);
		static bool getNineGridAreaList(vector<RenderAreaInfo>::type& vertices, const ERect& rtArea, const ERect& rtUV, float textureWidth,
			float textureHeight, const NineGridOffset& offset, const Matrix4& mat);
	};
}

#endif // !__HEAD_TEXT_RENDERER_HEADER__