#include "HeadTextRenderer.h"

#include "Render/ActiveRenderInfo.h"
#include "UI/GUIFontManager.h"
#include "UI/GUIImageset.h"
#include "UI/GUIFontGlyph.h"
#include "UI/GUIFont.h"
#include "cTileEntity/TileEntityRenderMgr.h"
#include "Tessolator/FontRenderable.h"
#include "Blockman/cWorld/Blockman.h"
#include "Blockman/cEntity/EntityPlayerSPMP.h"
#include "World/GameSettings.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Util/LanguageManager.h"
#include "Util/PlayerPrefs.h"
#include "Entity/EntityFlyingMount.h"
#include "Render/RenderEntity.h"

using namespace LORD;

namespace BLOCKMAN
{
	void HeadTextRenderer::renderHeadText(const LORD::String & txt, LORD::Vector3 pos, bool canBeOccluded,
		Texture* backTexture, Color color1, bool enableAlpha)
	{
		renderHeadText(txt, pos, canBeOccluded, 1.0f, backTexture, color1, enableAlpha);
	}

	void HeadTextRenderer::renderHeadText(const LORD::String & txt, LORD::Vector3 pos, bool canBeOccluded, float scale,
		Texture* backTexture, Color color1, bool enableAlpha)
	{
		vector<HeadTextInfo>::type txt_lines;
		
		Color alphaColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
		if (enableAlpha)
		{
			float alpha = PlayerPrefs::Instance()->getFloatPrefs("RenderTextAlpha");
			alphaColor.a = alpha;
		}	

		Color defaultColor = Color::WHITE;
		StringArray txts = StringUtil::Split(txt, "\n");
		for (auto& line : txts)
		{
			int m = line.find("##", 0);
			if (m == std::string::npos || m + 2 >= line.length())
			{
				continue;
			}
			int n = line.find("##", m + 2);
			if (n == std::string::npos)
			{
				continue;
			}
			String txtstr = line.substr(m + 2, n - m - 2);
			txtstr = LanguageManager::Instance()->getString(txtstr);
			line.replace(m, n - m + 2, txtstr);

		}
		for (auto& line : txts)
		{
			if (line.find("[#") == 0)
			{
				String colorstr = line.substr(2, 6);
				String txtstr = line.substr(9, line.length() - 9);
				unsigned int nColor = 0;
				sscanf(colorstr.c_str(), "%X", &nColor);
				Color color = nColor;
				color.a = 1.f;
				txt_lines.push_back({ GUIString(txtstr.c_str()), color });
			}
			else
			{
				txt_lines.push_back({ GUIString(line.c_str()), defaultColor });
			}
		}

		float rx = ActiveRenderInfo::rotationX;
		float rz = ActiveRenderInfo::rotationZ;
		float ryz = ActiveRenderInfo::rotationYZ;
		float rxy = ActiveRenderInfo::rotationXY;
		float rxz = ActiveRenderInfo::rotationXZ;

		float line_height = 0.3f * scale;
		float line_space = 0.12f * scale;
		float word_space = 0.02f * scale;
		int line_count = txt_lines.size();
		int line_num = 0;
		GUIFont* pFont = GUIFontManager::Instance()->GetFont("HT20");
		float heightScale = line_height / pFont->GetFontHeight();
		vector<CharacterInfo>::type buffer;

		for (auto& txt_line : txt_lines)
		{
			utf16 cCode;
			float totalWidth = 0.f;
			vector<CharacterInfo>::type buffer_line;
			auto color = txt_line.color;
			for (size_t c = 0; c < txt_line.text.length(); ++c)
			{
				cCode = txt_line.text[c];
				switch (cCode)
				{
				case 0x25A2:
				{
					auto colorstr = txt_line.text.substr(c + 1, 8);
					c += 8;
					unsigned int nColor = 0;
					sscanf(colorstr.c_str(), "%X", &nColor);
					color = nColor;
					continue;
				}
				case 0x25A5:
					color = txt_line.color;
					continue;
				default:
					break;
				}

				color.a = alphaColor.a;

				const GUIFontGlyph* glyph = pFont->GetGlyphData(cCode);
				if (!glyph)
					continue;

				float charWidth = glyph->GetWidth() * heightScale;
				float charHeight = glyph->GetHeight() * heightScale;

				CharacterInfo info;
				const GUIImage* const img = glyph->GetImage();
				info.tex = img->GetImageset()->GetTexture();
				info.uv = glyph->GetImage()->GetUV();
				Vector2 offset = img->GetOffset() * heightScale;
				info.pos.x = totalWidth + offset.x + word_space;
				info.pos.y = (line_height + 0.02f) * (line_count - line_num - 1) - offset.y - charHeight;
				info.size.x = charWidth;
				info.size.y = charHeight;
				info.color = color.getABGR();
				buffer_line.push_back(info);

				totalWidth += charWidth + word_space;
			}
			// adjust all the xposition of all the characters.
			for (auto& it : buffer_line)
				it.pos.x -= totalWidth / 2.f;
			buffer.insert(buffer.end(), buffer_line.begin(), buffer_line.end());

			if (backTexture)
			{
				float width = totalWidth;
				float height = line_height - 0.05f;

				float offsetH = (line_height + 0.02f) * (line_count - line_num - 1) - 0.04f;

				RenderEntity* pRenderEntity = Blockman::Instance()->m_entityRender;
				if (!pRenderEntity)
					return;
				
				float pitch = -pRenderEntity->m_cameraController.getCameraPitch() * Math::DEG2RAD;
				float yaw = -pRenderEntity->m_cameraController.getCameraYaw() * Math::DEG2RAD + Math::PI;
				if (Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_SECOND)
				{
					pitch = -pitch;
					yaw -= Math::PI;
				}

				Matrix4 matText, matTemp;
				matText.identity(); 
				matText.translate(pos);
				matTemp.identity(); 
				matTemp.rotateY(yaw);
				matText.joint(matTemp);
				matTemp.identity();
				matTemp.rotateX(pitch);
				matText.joint(matTemp);

				float bgminu = 0.f;
				float bgmaxu = 1.f;
				float bgminv = 0.f;
				float bgmaxv = 1.f;

				vector<RenderAreaInfo>::type vertices;
				if (getNineGridAreaList(vertices, ERect(-width / 2, 0.f, width / 2, height), ERect(bgminu, bgmaxv, bgmaxu, bgminv),
					(float)backTexture->getWidth(), (float)backTexture->getHeight(),
					NineGridOffset(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f), matText))
				{
					FontRenderable* renderable = NULL;
					FontVertexFmt* pVertex = NULL;
					TileEntityRenderMgr* mgr = TileEntityRenderMgr::Instance();
					renderable = mgr->getFontRenderable(nullptr, backTexture, canBeOccluded);
					if (renderable && renderable->getLockPtr(vertices.size(), pVertex))
					{
						float z = 0.072f;
						int index = 0;
						Dword colorValue = color1.getABGR();
						for (int i = 0; i < vertices.size(); ++i)
						{
							pVertex[index++].set(Vector3(vertices[i].lt.x, vertices[i].lt.y + offsetH, z) * matText, colorValue, vertices[i].uv.left, vertices[i].uv.top);
							pVertex[index++].set(Vector3(vertices[i].rt.x, vertices[i].rt.y + offsetH, z) * matText, colorValue, vertices[i].uv.right, vertices[i].uv.top);
							pVertex[index++].set(Vector3(vertices[i].rb.x, vertices[i].rb.y + offsetH, z) * matText, colorValue, vertices[i].uv.right, vertices[i].uv.bottom);
							pVertex[index++].set(Vector3(vertices[i].lb.x, vertices[i].lb.y + offsetH, z) * matText, colorValue, vertices[i].uv.left, vertices[i].uv.bottom);
						}
					}
				}

			}

			line_num++;
		}

		renderText(buffer, pos, canBeOccluded);
	}

	void HeadTextRenderer::renderWordWrap(const LORD::String & txt, LORD::Vector3 pos, bool canBeOccluded, float scale, 
		Texture* chatBubble, Color color1, Matrix4 matBall)
	{
		HeadTextInfo txtInfo = { GUIString(txt.c_str()), Color::WHITE };

		float line_height = 0.15f * scale;
		float word_space = 0.01f * scale;
		GUIFont* pFont = GUIFontManager::Instance()->GetFont("HT18");
		float heightScale = line_height / pFont->GetFontHeight();
		vector<CharacterInfo>::type buffer;

		utf16 cCode{};
		float lineWidth = 0.f;
		float totalWith = 0.f;
		auto color = txtInfo.color;
		for (size_t c = 0; c < txtInfo.text.length(); ++c)
		{
			cCode = txtInfo.text[c];
			switch (cCode)
			{
			case 0x25A2:
			{
				auto colorstr = txtInfo.text.substr(c + 1, 8);
				c += 8;
				unsigned int nColor = 0;
				sscanf(colorstr.c_str(), "%X", &nColor);
				color = nColor;
				continue;
			}
			case 0x25A5:
				color = txtInfo.color;
				continue;
			default:
				break;
			}

			const GUIFontGlyph* glyph = pFont->GetGlyphData(cCode);
			if (!glyph)
				continue;

			float charWidth = glyph->GetWidth() * heightScale;
			float charHeight = glyph->GetHeight() * heightScale;

			CharacterInfo info;
			const GUIImage* const img = glyph->GetImage();
			info.tex = img->GetImageset()->GetTexture();
			info.uv = glyph->GetImage()->GetUV();
			Vector2 offset = img->GetOffset() * heightScale;
			info.offset = offset;
			info.size.x = charWidth;
			info.size.y = charHeight;
			info.color = color.getABGR();
			buffer.push_back(info);
			totalWith += charWidth + word_space;
		}

		float miniWith = 80 * heightScale;
		float maxWith = 1400 * heightScale;

		float with = 0.f;
		if (totalWith >= maxWith)
		{
			with = maxWith;
		}
		else
		{
			with = (totalWith <= miniWith ? miniWith : totalWith);
		}

		float height = ceil(totalWith / with) * line_height + 0.3f * scale;
		
		int line_num = 1;
		for (auto& info : buffer)
		{
			info.pos.x = lineWidth + info.offset.x + word_space;
			info.pos.y = height - line_height * line_num - info.offset.y - info.size.y;
			lineWidth += info.size.x + word_space;
			if (lineWidth > with)
			{
				lineWidth = 0.f;
				line_num++;
			}
		}

		for (auto& it : buffer)
		{
			if (line_num > 1)
			{
				it.pos.x -= with / 2.f;
			}
			else
			{
				it.pos.x -= totalWith / 2.f;
			}
			it.pos.y += (line_height * line_num - height) / 2.f + 0.05f;
		}


		float bgminu = 0.f; //chatBubble->getMinU();
		float bgmaxu = 1.f;//chatBubble->getMaxU();
		float bgminv = 0.f;//chatBubble->getMinV();
		float bgmaxv = 1.f;//chatBubble->getMaxV();

		vector<RenderAreaInfo>::type vertices;
		if (getNineGridAreaList(vertices, ERect(-with / 2 - 0.15f, 0.f, with / 2 + 0.15f, height), ERect(bgminu, bgmaxv, bgmaxu, bgminv), 
			(float)chatBubble->getWidth(), (float)chatBubble->getHeight(), NineGridOffset(9.0f, 9.0f, 9.0f, 9.0f, 0.1f, 0.0f, 0.2f, 0.0f, 0.1f, 0.0f, 0.2f, 0.0f, 2, 13, 26, 0.1f, 0.0f, 0.2f, 0.0f, 0.5f), matBall))
		{
			FontRenderable* renderable = NULL;
			FontVertexFmt* pVertex = NULL;
			TileEntityRenderMgr* mgr = TileEntityRenderMgr::Instance();
			renderable = mgr->getFontRenderable(nullptr, chatBubble, canBeOccluded);
			if (renderable && renderable->getLockPtr(vertices.size(), pVertex))
			{
				float z = 0.072f;
				int index = 0;
				for (int i = 0; i < vertices.size(); ++i)
				{
					pVertex[index++].set(Vector3(vertices[i].lt.x, vertices[i].lt.y, z) * matBall, color1.getBGRA(), vertices[i].uv.left, vertices[i].uv.top);
					pVertex[index++].set(Vector3(vertices[i].rt.x, vertices[i].rt.y, z) * matBall, color1.getBGRA(), vertices[i].uv.right, vertices[i].uv.top);
					pVertex[index++].set(Vector3(vertices[i].rb.x, vertices[i].rb.y, z) * matBall, color1.getBGRA(), vertices[i].uv.right, vertices[i].uv.bottom);
					pVertex[index++].set(Vector3(vertices[i].lb.x, vertices[i].lb.y, z) * matBall, color1.getBGRA(), vertices[i].uv.left, vertices[i].uv.bottom);
				}
			}
		}
			
		// adjust all the xposition of all the characters.
		renderText(buffer, pos, canBeOccluded);
	}

	void HeadTextRenderer::renderText(LORD::vector<CharacterInfo>::type buffer, LORD::Vector3 pos, bool canBeOccluded)
	{
		auto camera = SceneManager::Instance()->getMainCamera();

		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		if (!player)
			return;

		Matrix4 matText;
		if (player->isOnFlyingMount())
		{
			auto mount = player->getFlyingMount();
			matText.identity();
			matText.translate(pos);
			Matrix4 matTemp;
			matTemp.identity(); matTemp.fromQuan(mount->getCurrentViewOrientation(1));
			matText.joint(matTemp);
			matTemp.identity(); matTemp.rotateY(Math::PI);
			matText.joint(matTemp);
		}
		else
		{
			RenderEntity* pRenderEntity = Blockman::Instance()->m_entityRender;
            if (!pRenderEntity)
            	return;
            float pitch = -pRenderEntity->m_cameraController.getCameraPitch() * Math::DEG2RAD;
            float yaw = -pRenderEntity->m_cameraController.getCameraYaw() * Math::DEG2RAD + Math::PI;
            if (Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_SECOND)
            {
            	pitch = -pitch;
            	yaw -= Math::PI;
            }
			Matrix4 matTemp;
			matText.identity(); matText.translate(pos);
			matTemp.identity(); matTemp.rotateY(yaw);
			matText.joint(matTemp);
			matTemp.identity(); matTemp.rotateX(pitch);
			matText.joint(matTemp);
		}

		FontRenderable* renderable = NULL;
		FontVertexFmt* pVertex = NULL;
		TileEntityRenderMgr* mgr = TileEntityRenderMgr::Instance();
		for (auto& it : buffer)
		{
			renderable = mgr->getFontRenderable(it.tex, nullptr, canBeOccluded);
			if (!renderable || !renderable->getLockPtr(1, pVertex))
				continue;

			float z = 0.072f;
			Vector3 v1(it.pos.x, it.pos.y, z);
			Vector3 v2(it.pos.x, it.pos.y + it.size.y, z);
			Vector3 v3(it.pos.x + it.size.x, it.pos.y + it.size.y, z);
			Vector3 v4(it.pos.x + it.size.x, it.pos.y, z);
			pVertex[0].set(v1 * matText, it.color, it.uv.left, it.uv.bottom);
			pVertex[3].set(v2 * matText, it.color, it.uv.left, it.uv.top);
			pVertex[2].set(v3 * matText, it.color, it.uv.right, it.uv.top);
			pVertex[1].set(v4 * matText, it.color, it.uv.right, it.uv.bottom);
		}
	}

	bool HeadTextRenderer::getNineGridAreaList(vector<RenderAreaInfo>::type& vertices, const ERect& rtArea, const ERect& rtUV, 
		float textureWidth, float textureHeight, const NineGridOffset& offset, const Matrix4& mat)
	{
		if (Math::Abs(textureWidth) < 0.00001f || Math::Abs(textureHeight) < 0.00001f)
		{
			LordLogError("NineGridArea: texture is invalid");
			return false;
		}

		float areaWidth = Math::Abs(rtArea.right - rtArea.left);
		float areaHeight = Math::Abs(rtArea.bottom - rtArea.top);
		if (areaWidth < 0.00001f || areaHeight < 0.00001f)
		{
			LordLogError("NineGridArea: rtArea is invalid");
			return false;
		}

		float uvWidth = Math::Abs(rtUV.right - rtUV.left);
		float uvHeight = Math::Abs(rtUV.bottom - rtUV.top);
		if (uvWidth < 0.00001f || uvHeight < 0.00001f)
		{
			LordLogError("NineGridArea: rtUV is invalid");
			return false;
		}

		float inverseTextureWidth = 1.0f / textureWidth;
		float inverseTextureHeight = 1.0f / textureHeight;

		if (offset.iSpecialAreaType == 1 || offset.iSpecialAreaType == 3)
		{
			float temp = offset.fSpecialOffsetCross * inverseTextureHeight / Math::Abs(rtUV.bottom - rtUV.top) * 0.5f;
			if (temp > offset.fSpecialAreaCenterPercent || temp > 1.0f - offset.fSpecialAreaCenterPercent)
			{
				return false;
			}
		}
		else if (offset.iSpecialAreaType == 2 || offset.iSpecialAreaType == 4)
		{
			float temp = offset.fSpecialOffsetCross * inverseTextureWidth / Math::Abs(rtUV.right - rtUV.left) * 0.5f;
			if (temp > offset.fSpecialAreaCenterPercent || temp > 1.0f - offset.fSpecialAreaCenterPercent)
			{
				return false;
			}
		}

		if (rtUV.left > rtUV.right)
		{
			inverseTextureWidth = -inverseTextureWidth;
		}
		if (rtUV.top > rtUV.bottom)
		{
			inverseTextureHeight = -inverseTextureHeight;
		}

		Matrix4 projMatrix = SceneManager::Instance()->getMainCamera()->getViewProjMatrix();
		float viewportWidth = (float)Renderer::Instance()->getMainViewport()->getWidth();
		float viewportHeight = (float)Renderer::Instance()->getMainViewport()->getHeight();
		float zNear = SceneManager::Instance()->getMainCamera()->getNearClip();
		float zFar = SceneManager::Instance()->getMainCamera()->getFarClip();
		if (Math::Abs(zNear) < 0.00001f)	return false;
		float _zc1 = zFar / zNear;
		float _zc0 = 1.0f - _zc1;
		if (Math::Abs(zFar) < 0.00001f || Math::Abs(_zc0) < 0.00001f)	return false;

		// left top screen point
		Vector3 v = Vector3(rtArea.left, rtArea.top, 0.0f) * mat;
		float w = projMatrix.m03 * v.x + projMatrix.m13 * v.y + projMatrix.m23 * v.z + projMatrix.m33;
		if (w <= 0.0f)	return false;
		Vector4 ret = Vector4(v.x, v.y, v.z, 1.0f) * projMatrix;
		float invW = 1.0f / w;
		Vector3 ltTemp = Vector3(ret.x * invW, ret.y * invW, ret.z * invW);
		if (Math::Abs(ltTemp.z - 1.0f) < 0.00001f)	return false;
		ltTemp.x = (ltTemp.x + 1.0f) * 0.5f * viewportWidth;
		ltTemp.y = (ltTemp.y + 1.0f) * 0.5f * viewportHeight;
		ltTemp.z = (1.0f / (ltTemp.z - 1.0f) - _zc1 / zFar) * zFar / _zc0;

		// right top screen point
		v = Vector3(rtArea.right, rtArea.top, 0.0f) * mat;
		w = projMatrix.m03 * v.x + projMatrix.m13 * v.y + projMatrix.m23 * v.z + projMatrix.m33;
		if (w <= 0.0f)	return false;
		ret = Vector4(v.x, v.y, v.z, 1.0f) * projMatrix;
		invW = 1.0f / w;
		Vector3 rtTemp = Vector3(ret.x * invW, ret.y * invW, ret.z * invW);
		if (Math::Abs(rtTemp.z - 1.0f) < 0.00001f)	return false;
		rtTemp.x = (rtTemp.x + 1.0f) * 0.5f * viewportWidth;
		rtTemp.y = (rtTemp.y + 1.0f) * 0.5f * viewportHeight;
		rtTemp.z = (1.0f / (rtTemp.z - 1.0f) - _zc1 / zFar) * zFar / _zc0;

		// right bottom screen point
		v = Vector3(rtArea.right, rtArea.bottom, 0.0f) * mat;
		w = projMatrix.m03 * v.x + projMatrix.m13 * v.y + projMatrix.m23 * v.z + projMatrix.m33;
		if (w <= 0.0f)	return false;
		ret = Vector4(v.x, v.y, v.z, 1.0f) * projMatrix;
		invW = 1.0f / w;
		Vector3 rbTemp = Vector3(ret.x * invW, ret.y * invW, ret.z * invW);
		if (Math::Abs(rbTemp.z - 1.0f) < 0.00001f)	return false;
		rbTemp.x = (rbTemp.x + 1.0f) * 0.5f * viewportWidth;
		rbTemp.y = (rbTemp.y + 1.0f) * 0.5f * viewportHeight;
		rbTemp.z = (1.0f / (rbTemp.z - 1.0f) - _zc1 / zFar) * zFar / _zc0;

		float areaScreenWidth = ltTemp.distanceTo(rtTemp);
		float areaScreenHeight = rtTemp.distanceTo(rbTemp);
		if (Math::Abs(areaScreenWidth) < 0.00001f || Math::Abs(areaScreenHeight) < 0.00001f)
		{
			return false;
		}

		ERect rtAreaList[NGAT_MAX_TYPE];
		ERect rtUVList[NGAT_MAX_TYPE];

		for (size_t i = 0; i < NGAT_MAX_TYPE; ++i)
		{
			rtAreaList[i] = rtArea;
			rtUVList[i] = rtUV;
		}

		float scaleL = Math::Clamp(offset.fLeft / areaScreenWidth, offset.fLeftScaleMin, offset.fLeftScaleMax);
		float offsetL = (rtArea.right - rtArea.left) * scaleL;

		float scaleT = Math::Clamp(offset.fTop / areaScreenHeight, offset.fTopScaleMin, offset.fTopScaleMax);
		float offsetT = (rtArea.bottom - rtArea.top) * scaleT;

		float scaleR = Math::Clamp(offset.fRight / areaScreenWidth, offset.fRightScaleMin, offset.fRightScaleMax);
		float offsetR = (rtArea.right - rtArea.left) * scaleR;

		float scaleB = Math::Clamp(offset.fBottom / areaScreenHeight, offset.fBottomScaleMin, offset.fBottomScaleMax);
		float offsetB = (rtArea.bottom - rtArea.top) * scaleB;

		Vector2 areaHelperRect0;
		Vector2 areaHelperRect1;
		areaHelperRect0.x = rtArea.left + offsetL;
		areaHelperRect0.y = rtArea.top + offsetT;
		areaHelperRect1.x = rtArea.right - offsetR;
		areaHelperRect1.y = rtArea.bottom - offsetB;

		Vector2 uvHelperRect0;
		Vector2 uvHelperRect1;
		uvHelperRect0.x = rtUV.left + offset.fLeft * inverseTextureWidth;
		uvHelperRect0.y = rtUV.top + offset.fTop * inverseTextureHeight;
		uvHelperRect1.x = rtUV.right - offset.fRight * inverseTextureWidth;
		uvHelperRect1.y = rtUV.bottom - offset.fBottom * inverseTextureHeight;

		// lt
		rtAreaList[NGAT_LEFT_TOP].right = areaHelperRect0.x;
		rtAreaList[NGAT_LEFT_TOP].bottom = areaHelperRect0.y;
		rtUVList[NGAT_LEFT_TOP].right = uvHelperRect0.x;
		rtUVList[NGAT_LEFT_TOP].bottom = uvHelperRect0.y;

		// mt
		rtAreaList[NGAT_MIDDLE_TOP].left = areaHelperRect0.x;
		rtAreaList[NGAT_MIDDLE_TOP].right = areaHelperRect1.x;
		rtAreaList[NGAT_MIDDLE_TOP].bottom = areaHelperRect0.y;
		rtUVList[NGAT_MIDDLE_TOP].left = uvHelperRect0.x;
		rtUVList[NGAT_MIDDLE_TOP].right = uvHelperRect1.x;
		rtUVList[NGAT_MIDDLE_TOP].bottom = uvHelperRect0.y;

		// rt
		rtAreaList[NGAT_RIGHT_TOP].left = areaHelperRect1.x;
		rtAreaList[NGAT_RIGHT_TOP].bottom = areaHelperRect0.y;
		rtUVList[NGAT_RIGHT_TOP].left = uvHelperRect1.x;
		rtUVList[NGAT_RIGHT_TOP].bottom = uvHelperRect0.y;

		// lm
		rtAreaList[NGAT_LEFT_MIDDLE].top = areaHelperRect0.y;
		rtAreaList[NGAT_LEFT_MIDDLE].right = areaHelperRect0.x;
		rtAreaList[NGAT_LEFT_MIDDLE].bottom = areaHelperRect1.y;
		rtUVList[NGAT_LEFT_MIDDLE].top = uvHelperRect0.y;
		rtUVList[NGAT_LEFT_MIDDLE].right = uvHelperRect0.x;
		rtUVList[NGAT_LEFT_MIDDLE].bottom = uvHelperRect1.y;

		// mm
		rtAreaList[NGAT_MIDDLE_MIDDLE].left = areaHelperRect0.x;
		rtAreaList[NGAT_MIDDLE_MIDDLE].top = areaHelperRect0.y;
		rtAreaList[NGAT_MIDDLE_MIDDLE].right = areaHelperRect1.x;
		rtAreaList[NGAT_MIDDLE_MIDDLE].bottom = areaHelperRect1.y;
		rtUVList[NGAT_MIDDLE_MIDDLE].left = uvHelperRect0.x;
		rtUVList[NGAT_MIDDLE_MIDDLE].top = uvHelperRect0.y;
		rtUVList[NGAT_MIDDLE_MIDDLE].right = uvHelperRect1.x;
		rtUVList[NGAT_MIDDLE_MIDDLE].bottom = uvHelperRect1.y;

		// rm
		rtAreaList[NGAT_RIGHT_MIDDLE].left = areaHelperRect1.x;
		rtAreaList[NGAT_RIGHT_MIDDLE].top = areaHelperRect0.y;
		rtAreaList[NGAT_RIGHT_MIDDLE].bottom = areaHelperRect1.y;
		rtUVList[NGAT_RIGHT_MIDDLE].left = uvHelperRect1.x;
		rtUVList[NGAT_RIGHT_MIDDLE].top = uvHelperRect0.y;
		rtUVList[NGAT_RIGHT_MIDDLE].bottom = uvHelperRect1.y;

		// lb
		rtAreaList[NGAT_LEFT_BOTTOM].top = areaHelperRect1.y;
		rtAreaList[NGAT_LEFT_BOTTOM].right = areaHelperRect0.x;
		rtUVList[NGAT_LEFT_BOTTOM].top = uvHelperRect1.y;
		rtUVList[NGAT_LEFT_BOTTOM].right = uvHelperRect0.x;

		// mb
		rtAreaList[NGAT_MIDDLE_BOTTOM].left = areaHelperRect0.x;
		rtAreaList[NGAT_MIDDLE_BOTTOM].top = areaHelperRect1.y;
		rtAreaList[NGAT_MIDDLE_BOTTOM].right = areaHelperRect1.x;
		rtUVList[NGAT_MIDDLE_BOTTOM].left = uvHelperRect0.x;
		rtUVList[NGAT_MIDDLE_BOTTOM].top = uvHelperRect1.y;
		rtUVList[NGAT_MIDDLE_BOTTOM].right = uvHelperRect1.x;

		// rb
		rtAreaList[NGAT_RIGHT_BOTTOM].left = areaHelperRect1.x;
		rtAreaList[NGAT_RIGHT_BOTTOM].top = areaHelperRect1.y;
		rtUVList[NGAT_RIGHT_BOTTOM].left = uvHelperRect1.x;
		rtUVList[NGAT_RIGHT_BOTTOM].top = uvHelperRect1.y;

		ERect rtSpecialAreaList[3];
		ERect rtSpecialUVList[3];
		for (size_t i = 0; i < 3; ++i)
		{
			rtSpecialAreaList[i] = rtArea;
			rtSpecialUVList[i] = rtUV;
		}
		int specialAreaNum = 0;
		if (offset.iSpecialAreaType == 1)
		{
			specialAreaNum = 3;
			float scaleSpecial = Math::Clamp(offset.fSpecialOffset / areaScreenWidth, offset.fSpecialOffsetScaleMin, offset.fSpecialOffsetScaleMax);
			float offsetSpecial = (rtArea.right - rtArea.left) * scaleSpecial;

			float scaleSpecialCross = Math::Clamp(offset.fSpecialOffsetCross / areaScreenHeight, offset.fSpecialOffsetCrossScaleMin, offset.fSpecialOffsetCrossScaleMax);
			float scaleSpecialCrossTop = Math::Clamp((offset.fSpecialAreaCenterPercent - scaleSpecialCross * 0.5f), 0.0f, 1.0f);
			float scaleSpecialCrossBottom = Math::Clamp((offset.fSpecialAreaCenterPercent + scaleSpecialCross * 0.5f), 0.0f, 1.0f);
			float offsetSpecialCrossTop = (rtArea.bottom - rtArea.top) * scaleSpecialCrossTop;
			float offsetSpecialCrossBottom = (rtArea.bottom - rtArea.top) * scaleSpecialCrossBottom;

			Vector2 areaHelperRect2;
			Vector2 areaHelperRect3;
			areaHelperRect2.x = rtArea.left + offsetSpecial;
			areaHelperRect2.y = rtArea.top + offsetSpecialCrossTop;
			areaHelperRect3.x = rtArea.left + offsetSpecial;
			areaHelperRect3.y = rtArea.top + offsetSpecialCrossBottom;

			Vector2 uvHelperRect2;
			Vector2 uvHelperRect3;

			float texOffsetTop = (rtUV.bottom - rtUV.top) * offset.fSpecialAreaCenterPercent - offset.fSpecialOffsetCross * inverseTextureHeight * 0.5f;
			float texOffsetBottom = (rtUV.bottom - rtUV.top) * offset.fSpecialAreaCenterPercent + offset.fSpecialOffsetCross * inverseTextureHeight * 0.5f;

			uvHelperRect2.x = rtUV.left + offset.fSpecialOffset * inverseTextureWidth;
			uvHelperRect2.y = rtUV.top + texOffsetTop;
			uvHelperRect3.x = rtUV.left + offset.fSpecialOffset * inverseTextureWidth;
			uvHelperRect3.y = rtUV.top + texOffsetBottom;

			rtSpecialAreaList[0].right = areaHelperRect2.x;
			rtSpecialAreaList[0].bottom = areaHelperRect2.y;
			rtSpecialUVList[0].right = uvHelperRect2.x;
			rtSpecialUVList[0].bottom = uvHelperRect2.y;

			rtSpecialAreaList[1].top = areaHelperRect2.y;
			rtSpecialAreaList[1].bottom = areaHelperRect3.y;
			rtSpecialAreaList[1].right = areaHelperRect2.x;
			rtSpecialUVList[1].top = uvHelperRect2.y;
			rtSpecialUVList[1].bottom = uvHelperRect3.y;
			rtSpecialUVList[1].right = uvHelperRect2.x;

			rtSpecialAreaList[2].top = areaHelperRect3.y;
			rtSpecialAreaList[2].right = areaHelperRect3.x;
			rtSpecialUVList[2].top = uvHelperRect3.y;
			rtSpecialUVList[2].right = uvHelperRect3.x;

			float areaTemp = rtSpecialAreaList[0].right - rtSpecialAreaList[0].left;
			float uvTemp = rtSpecialUVList[0].right - rtSpecialUVList[0].left;

			rtAreaList[NGAT_LEFT_TOP].left += areaTemp;
			rtAreaList[NGAT_LEFT_TOP].right += areaTemp;
			rtUVList[NGAT_LEFT_TOP].left += uvTemp;
			rtUVList[NGAT_LEFT_TOP].right += uvTemp;

			rtAreaList[NGAT_LEFT_MIDDLE].left += areaTemp;
			rtAreaList[NGAT_LEFT_MIDDLE].right += areaTemp;
			rtUVList[NGAT_LEFT_MIDDLE].left += uvTemp;
			rtUVList[NGAT_LEFT_MIDDLE].right += uvTemp;

			rtAreaList[NGAT_LEFT_BOTTOM].left += areaTemp;
			rtAreaList[NGAT_LEFT_BOTTOM].right += areaTemp;
			rtUVList[NGAT_LEFT_BOTTOM].left += uvTemp;
			rtUVList[NGAT_LEFT_BOTTOM].right += uvTemp;

			rtAreaList[NGAT_MIDDLE_TOP].left += areaTemp;
			rtUVList[NGAT_MIDDLE_TOP].left += uvTemp;

			rtAreaList[NGAT_MIDDLE_MIDDLE].left += areaTemp;
			rtUVList[NGAT_MIDDLE_MIDDLE].left += uvTemp;

			rtAreaList[NGAT_MIDDLE_BOTTOM].left += areaTemp;
			rtUVList[NGAT_MIDDLE_BOTTOM].left += uvTemp;
		}
		else if (offset.iSpecialAreaType == 2)
		{
			specialAreaNum = 3;
			float scaleSpecial = Math::Clamp(offset.fSpecialOffset / areaScreenHeight, offset.fSpecialOffsetScaleMin, offset.fSpecialOffsetScaleMax);
			float offsetSpecial = (rtArea.bottom - rtArea.top) * scaleSpecial;

			float scaleSpecialCross = Math::Clamp(offset.fSpecialOffsetCross / areaScreenWidth, offset.fSpecialOffsetCrossScaleMin, offset.fSpecialOffsetCrossScaleMax);
			float scaleSpecialCrossLeft = Math::Clamp((offset.fSpecialAreaCenterPercent - scaleSpecialCross * 0.5f), 0.0f, 1.0f);
			float scaleSpecialCrossRight = Math::Clamp((offset.fSpecialAreaCenterPercent + scaleSpecialCross * 0.5f), 0.0f, 1.0f);
			float offsetSpecialCrossLeft = (rtArea.right - rtArea.left) * scaleSpecialCrossLeft;
			float offsetSpecialCrossRight = (rtArea.right - rtArea.left) * scaleSpecialCrossRight;

			Vector2 areaHelperRect2;
			Vector2 areaHelperRect3;
			areaHelperRect2.x = rtArea.left + offsetSpecialCrossLeft;
			areaHelperRect2.y = rtArea.top + offsetSpecial;
			areaHelperRect3.x = rtArea.left + offsetSpecialCrossRight;
			areaHelperRect3.y = rtArea.top + offsetSpecial;

			Vector2 uvHelperRect2;
			Vector2 uvHelperRect3;

			float texOffsetLeft = (rtUV.right - rtUV.left) * offset.fSpecialAreaCenterPercent - offset.fSpecialOffsetCross * inverseTextureWidth * 0.5f;
			float texOffsetRight = (rtUV.right - rtUV.left) * offset.fSpecialAreaCenterPercent + offset.fSpecialOffsetCross * inverseTextureWidth * 0.5f;

			uvHelperRect2.x = rtUV.left + texOffsetLeft;
			uvHelperRect2.y = rtUV.top + offset.fSpecialOffset * inverseTextureHeight;
			uvHelperRect3.x = rtUV.left + texOffsetRight;
			uvHelperRect3.y = rtUV.top + offset.fSpecialOffset * inverseTextureHeight;

			rtSpecialAreaList[0].right = areaHelperRect2.x;
			rtSpecialAreaList[0].bottom = areaHelperRect2.y;
			rtSpecialUVList[0].right = uvHelperRect2.x;
			rtSpecialUVList[0].bottom = uvHelperRect2.y;

			rtSpecialAreaList[1].left = areaHelperRect2.x;
			rtSpecialAreaList[1].right = areaHelperRect3.x;
			rtSpecialAreaList[1].bottom = areaHelperRect2.y;
			rtSpecialUVList[1].left = uvHelperRect2.x;
			rtSpecialUVList[1].right = uvHelperRect3.x;
			rtSpecialUVList[1].bottom = uvHelperRect2.y;

			rtSpecialAreaList[2].left = areaHelperRect3.x;
			rtSpecialAreaList[2].bottom = areaHelperRect3.y;
			rtSpecialUVList[2].left = uvHelperRect3.x;
			rtSpecialUVList[2].bottom = uvHelperRect3.y;

			float areaTemp = rtSpecialAreaList[0].bottom - rtSpecialAreaList[0].top;
			float uvTemp = rtSpecialUVList[0].bottom - rtSpecialUVList[0].top;

			rtAreaList[NGAT_LEFT_TOP].top += areaTemp;
			rtAreaList[NGAT_LEFT_TOP].bottom += areaTemp;
			rtUVList[NGAT_LEFT_TOP].top += uvTemp;
			rtUVList[NGAT_LEFT_TOP].bottom += uvTemp;

			rtAreaList[NGAT_MIDDLE_TOP].top += areaTemp;
			rtAreaList[NGAT_MIDDLE_TOP].bottom += areaTemp;
			rtUVList[NGAT_MIDDLE_TOP].top += uvTemp;
			rtUVList[NGAT_MIDDLE_TOP].bottom += uvTemp;

			rtAreaList[NGAT_RIGHT_TOP].top += areaTemp;
			rtAreaList[NGAT_RIGHT_TOP].bottom += areaTemp;
			rtUVList[NGAT_RIGHT_TOP].top += uvTemp;
			rtUVList[NGAT_RIGHT_TOP].bottom += uvTemp;

			rtAreaList[NGAT_LEFT_MIDDLE].top += areaTemp;
			rtUVList[NGAT_LEFT_MIDDLE].top += uvTemp;

			rtAreaList[NGAT_MIDDLE_MIDDLE].top += areaTemp;
			rtUVList[NGAT_MIDDLE_MIDDLE].top += uvTemp;

			rtAreaList[NGAT_RIGHT_MIDDLE].top += areaTemp;
			rtUVList[NGAT_RIGHT_MIDDLE].top += uvTemp;
		}
		else if (offset.iSpecialAreaType == 3)
		{
			specialAreaNum = 3;
			float scaleSpecial = Math::Clamp(offset.fSpecialOffset / areaScreenWidth, offset.fSpecialOffsetScaleMin, offset.fSpecialOffsetScaleMax);
			float offsetSpecial = (rtArea.right - rtArea.left) * scaleSpecial;

			float scaleSpecialCross = Math::Clamp(offset.fSpecialOffsetCross / areaScreenHeight, offset.fSpecialOffsetCrossScaleMin, offset.fSpecialOffsetCrossScaleMax);
			float scaleSpecialCrossTop = Math::Clamp((offset.fSpecialAreaCenterPercent - scaleSpecialCross * 0.5f), 0.0f, 1.0f);
			float scaleSpecialCrossBottom = Math::Clamp((offset.fSpecialAreaCenterPercent + scaleSpecialCross * 0.5f), 0.0f, 1.0f);
			float offsetSpecialCrossTop = (rtArea.bottom - rtArea.top) * scaleSpecialCrossTop;
			float offsetSpecialCrossBottom = (rtArea.bottom - rtArea.top) * scaleSpecialCrossBottom;

			Vector2 areaHelperRect2;
			Vector2 areaHelperRect3;
			areaHelperRect2.x = rtArea.right - offsetSpecial;
			areaHelperRect2.y = rtArea.top + offsetSpecialCrossTop;
			areaHelperRect3.x = rtArea.right - offsetSpecial;
			areaHelperRect3.y = rtArea.top + offsetSpecialCrossBottom;

			Vector2 uvHelperRect2;
			Vector2 uvHelperRect3;

			float texOffsetTop = (rtUV.bottom - rtUV.top) * offset.fSpecialAreaCenterPercent - offset.fSpecialOffsetCross * inverseTextureHeight * 0.5f;
			float texOffsetBottom = (rtUV.bottom - rtUV.top) * offset.fSpecialAreaCenterPercent + offset.fSpecialOffsetCross * inverseTextureHeight * 0.5f;

			uvHelperRect2.x = rtUV.right - offset.fSpecialOffset * inverseTextureWidth;
			uvHelperRect2.y = rtUV.top + texOffsetTop;
			uvHelperRect3.x = rtUV.right - offset.fSpecialOffset * inverseTextureWidth;
			uvHelperRect3.y = rtUV.top + texOffsetBottom;

			rtSpecialAreaList[0].left = areaHelperRect2.x;
			rtSpecialAreaList[0].bottom = areaHelperRect2.y;
			rtSpecialUVList[0].left = uvHelperRect2.x;
			rtSpecialUVList[0].bottom = uvHelperRect2.y;

			rtSpecialAreaList[1].top = areaHelperRect2.y;
			rtSpecialAreaList[1].bottom = areaHelperRect3.y;
			rtSpecialAreaList[1].left = areaHelperRect2.x;
			rtSpecialUVList[1].top = uvHelperRect2.y;
			rtSpecialUVList[1].bottom = uvHelperRect3.y;
			rtSpecialUVList[1].left = uvHelperRect2.x;

			rtSpecialAreaList[2].top = areaHelperRect3.y;
			rtSpecialAreaList[2].left = areaHelperRect3.x;
			rtSpecialUVList[2].top = uvHelperRect3.y;
			rtSpecialUVList[2].left = uvHelperRect3.x;

			float areaTemp = rtSpecialAreaList[0].right - rtSpecialAreaList[0].left;
			float uvTemp = rtSpecialUVList[0].right - rtSpecialUVList[0].left;

			rtAreaList[NGAT_RIGHT_TOP].left -= areaTemp;
			rtAreaList[NGAT_RIGHT_TOP].right -= areaTemp;
			rtUVList[NGAT_RIGHT_TOP].left -= uvTemp;
			rtUVList[NGAT_RIGHT_TOP].right -= uvTemp;

			rtAreaList[NGAT_RIGHT_MIDDLE].left -= areaTemp;
			rtAreaList[NGAT_RIGHT_MIDDLE].right -= areaTemp;
			rtUVList[NGAT_RIGHT_MIDDLE].left -= uvTemp;
			rtUVList[NGAT_RIGHT_MIDDLE].right -= uvTemp;

			rtAreaList[NGAT_RIGHT_BOTTOM].left -= areaTemp;
			rtAreaList[NGAT_RIGHT_BOTTOM].right -= areaTemp;
			rtUVList[NGAT_RIGHT_BOTTOM].left -= uvTemp;
			rtUVList[NGAT_RIGHT_BOTTOM].right -= uvTemp;

			rtAreaList[NGAT_MIDDLE_TOP].right -= areaTemp;
			rtUVList[NGAT_MIDDLE_TOP].right -= uvTemp;

			rtAreaList[NGAT_MIDDLE_MIDDLE].right -= areaTemp;
			rtUVList[NGAT_MIDDLE_MIDDLE].right -= uvTemp;

			rtAreaList[NGAT_MIDDLE_BOTTOM].right -= areaTemp;
			rtUVList[NGAT_MIDDLE_BOTTOM].right -= uvTemp;
		}
		else if (offset.iSpecialAreaType == 4)
		{
			specialAreaNum = 3;
			float scaleSpecial = Math::Clamp(offset.fSpecialOffset / areaScreenHeight, offset.fSpecialOffsetScaleMin, offset.fSpecialOffsetScaleMax);
			float offsetSpecial = (rtArea.bottom - rtArea.top) * scaleSpecial;

			float scaleSpecialCross = Math::Clamp(offset.fSpecialOffsetCross / areaScreenWidth, offset.fSpecialOffsetCrossScaleMin, offset.fSpecialOffsetCrossScaleMax);
			float scaleSpecialCrossLeft = Math::Clamp((offset.fSpecialAreaCenterPercent - scaleSpecialCross * 0.5f), 0.0f, 1.0f);
			float scaleSpecialCrossRight = Math::Clamp((offset.fSpecialAreaCenterPercent + scaleSpecialCross * 0.5f), 0.0f, 1.0f);
			float offsetSpecialCrossLeft = (rtArea.right - rtArea.left) * scaleSpecialCrossLeft;
			float offsetSpecialCrossRight = (rtArea.right - rtArea.left) * scaleSpecialCrossRight;

			Vector2 areaHelperRect2;
			Vector2 areaHelperRect3;
			areaHelperRect2.x = rtArea.left + offsetSpecialCrossLeft;
			areaHelperRect2.y = rtArea.bottom - offsetSpecial;
			areaHelperRect3.x = rtArea.left + offsetSpecialCrossRight;
			areaHelperRect3.y = rtArea.bottom - offsetSpecial;

			Vector2 uvHelperRect2;
			Vector2 uvHelperRect3;

			float texOffsetLeft = (rtUV.right - rtUV.left) * offset.fSpecialAreaCenterPercent - offset.fSpecialOffsetCross * inverseTextureWidth * 0.5f;
			float texOffsetRight = (rtUV.right - rtUV.left) * offset.fSpecialAreaCenterPercent + offset.fSpecialOffsetCross * inverseTextureWidth * 0.5f;

			uvHelperRect2.x = rtUV.left + texOffsetLeft;
			uvHelperRect2.y = rtUV.bottom - offset.fSpecialOffset * inverseTextureHeight;
			uvHelperRect3.x = rtUV.left + texOffsetRight;
			uvHelperRect3.y = rtUV.bottom - offset.fSpecialOffset * inverseTextureHeight;

			rtSpecialAreaList[0].right = areaHelperRect2.x;
			rtSpecialAreaList[0].top = areaHelperRect2.y;
			rtSpecialUVList[0].right = uvHelperRect2.x;
			rtSpecialUVList[0].top = uvHelperRect2.y;

			rtSpecialAreaList[1].left = areaHelperRect2.x;
			rtSpecialAreaList[1].right = areaHelperRect3.x;
			rtSpecialAreaList[1].top = areaHelperRect2.y;
			rtSpecialUVList[1].left = uvHelperRect2.x;
			rtSpecialUVList[1].right = uvHelperRect3.x;
			rtSpecialUVList[1].top = uvHelperRect2.y;

			rtSpecialAreaList[2].left = areaHelperRect3.x;
			rtSpecialAreaList[2].top = areaHelperRect3.y;
			rtSpecialUVList[2].left = uvHelperRect3.x;
			rtSpecialUVList[2].top = uvHelperRect3.y;

			float areaTemp = rtSpecialAreaList[0].bottom - rtSpecialAreaList[0].top;
			float uvTemp = rtSpecialUVList[0].bottom - rtSpecialUVList[0].top;

			rtAreaList[NGAT_LEFT_BOTTOM].top -= areaTemp;
			rtAreaList[NGAT_LEFT_BOTTOM].bottom -= areaTemp;
			rtUVList[NGAT_LEFT_BOTTOM].top -= uvTemp;
			rtUVList[NGAT_LEFT_BOTTOM].bottom -= uvTemp;

			rtAreaList[NGAT_MIDDLE_BOTTOM].top -= areaTemp;
			rtAreaList[NGAT_MIDDLE_BOTTOM].bottom -= areaTemp;
			rtUVList[NGAT_MIDDLE_BOTTOM].top -= uvTemp;
			rtUVList[NGAT_MIDDLE_BOTTOM].bottom -= uvTemp;

			rtAreaList[NGAT_RIGHT_BOTTOM].top -= areaTemp;
			rtAreaList[NGAT_RIGHT_BOTTOM].bottom -= areaTemp;
			rtUVList[NGAT_RIGHT_BOTTOM].top -= uvTemp;
			rtUVList[NGAT_RIGHT_BOTTOM].bottom -= uvTemp;

			rtAreaList[NGAT_LEFT_MIDDLE].bottom -= areaTemp;
			rtUVList[NGAT_LEFT_MIDDLE].bottom -= uvTemp;

			rtAreaList[NGAT_MIDDLE_MIDDLE].bottom -= areaTemp;
			rtUVList[NGAT_MIDDLE_MIDDLE].bottom -= uvTemp;

			rtAreaList[NGAT_RIGHT_MIDDLE].bottom -= areaTemp;
			rtUVList[NGAT_RIGHT_MIDDLE].bottom -= uvTemp;
		}
		

		for (int i = 0; i < NGAT_MAX_TYPE; ++i)
		{
			Vector3 lt, rb, lb, rt;
			lt = Vector3(rtAreaList[i].left, rtAreaList[i].top, 0.0f);
			rb = Vector3(rtAreaList[i].right, rtAreaList[i].bottom, 0.0f);
			lb = Vector3(rtAreaList[i].left, rtAreaList[i].bottom, 0.0f);
			rt = Vector3(rtAreaList[i].right, rtAreaList[i].top, 0.0f);

			RenderAreaInfo temp;
			temp.lt = lt;
			temp.rb = rb;
			temp.lb = lb;
			temp.rt = rt;
			temp.uv = rtUVList[i];
			vertices.push_back(temp);
		}
		for (int i = 0; i < specialAreaNum; ++i)
		{
			Vector3 lt, rb, lb, rt;
			lt = Vector3(rtSpecialAreaList[i].left, rtSpecialAreaList[i].top, 0.0f);
			rb = Vector3(rtSpecialAreaList[i].right, rtSpecialAreaList[i].bottom, 0.0f);
			lb = Vector3(rtSpecialAreaList[i].left, rtSpecialAreaList[i].bottom, 0.0f);
			rt = Vector3(rtSpecialAreaList[i].right, rtSpecialAreaList[i].top, 0.0f);

			RenderAreaInfo temp;
			temp.lt = lt;
			temp.rb = rb;
			temp.lb = lb;
			temp.rt = rt;
			temp.uv = rtSpecialUVList[i];
			vertices.push_back(temp);
		}

		return true;
	}

}
