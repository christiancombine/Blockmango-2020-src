#pragma once

#ifndef _UI_REGISTER_H__
#define _UI_REGISTER_H__

#include "UI/GUIAction.h"
#include "UI/GUIActionKeyFrame.h"
#include "UI/GUIAnimate.h"
#include "UI/GUIAnimateController.h"
#include "UI/GUIAnimateWindow.h"
#include "UI/GUIAnimatMgr.h"
#include "UI/GUIBoundSlot.h"
#include "UI/GUIBubbleContain.h"
#include "UI/GUIButton.h"
#include "UI/GUICell.h"
#include "UI/GUICheckBox.h"
#include "UI/GUICoordConverter.h"
#include "UI/GUIEditBox.h"
#include "UI/GUIEditBoxImpl.h"
#include "UI/GUIEventArgs.h"
#include "UI/GUIFlyAnimateController.h"
#include "UI/GUIFont.h"
#include "UI/GUIFontGlyph.h"
#include "UI/GUIFontManager.h"
#include "UI/GUIFreeFunctionSlot.h"
#include "UI/GUIFunctorCopySlot.h"
#include "UI/GUIFunctorPointerSlot.h"
#include "UI/GUIFunctorReferenceBinder.h"
#include "UI/GUIFunctorReferenceSlot.h"
#include "UI/GUIGridView.h"
#include "UI/GUIHyperlinkSystem.h"
#include "UI/GUIImage.h"
#include "UI/GUIImageset.h"
#include "UI/GUIImagesetManager.h"
#include "UI/GUIInputBox.h"
#include "UI/GUIIteratorBase.h"
#include "UI/GUILayout.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUIMemberFunctionSlot.h"
#include "UI/GUIMultiLineEdit.h"
#include "UI/GUIMutualNonoverlappingGroup.h"
#include "UI/GUIPageView.h"
#include "UI/GUIPixmapFont.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIRefCounted.h"
#include "UI/GUIRenderManager.h"
#include "UI/GUIRenderTargetManager.h"
#include "UI/GUIRenderText.h"
#include "UI/GUISceneMap.h"
#include "UI/GUIScriptModule.h"
#include "UI/GUIScroll.h"
#include "UI/GUIScrollablePane.h"
#include "UI/GUIScrollCard.h"
#include "UI/GUIScrolledContainer.h"
#include "UI/GUISlider.h"
#include "UI/GUISlotFunctorBase.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIString.h"
#include "UI/GUITableView.h"
#include "UI/GUIWindow.h"
#include "UI/GUIWindowFactory.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUISystem.h"
#include "GUI/GuiActorWindow.h"
#include "GUI/GuiUrlImage.h"
#include "GUI/GuiEffectViewOfObtainment.h"
#include "GUI/GuiItemStack.h"
#include "GUI/GuiItemSlot.h"
#include "GUI/GuiSlotTable.h"
#include "GUI/GuiScrollableInventory.h"
#include "GUI/RootGuiLayout.h"
#include "LuaRegister/Template/LuaRegister.h"
#include "GUI/gui_gameSetting.h"
#include "GUI/gui_mainControl.h"
#include "GUI/gui_chat.h"

using namespace BLOCKMAN;
using namespace LUA_REGISTER;

_BEGIN_REGISTER_CLASS(UDim)
_CLASSREGISTER_AddCtor(float, float)
_CLASSREGISTER_AddVar(scale, UDim::d_scale)
_CLASSREGISTER_AddVar(offset, UDim::d_offset)
_CLASSREGISTER_AddMember(asAbsolute, UDim::asAbsolute)
_CLASSREGISTER_AddMember(asRelative, UDim::asRelative)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(UVector2)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddVar(x, UVector2::d_x)
_CLASSREGISTER_AddVar(y, UVector2::d_y)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(URect)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddVar(min, URect::d_min)
_CLASSREGISTER_AddVar(max, URect::d_max)
_END_REGISTER_CLASS()

// Register Class GUIAction
_BEGIN_REGISTER_CLASS(GUIAction)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(addKeyFrame, GUIAction::addKeyFrame)
_CLASSREGISTER_AddMember(clear, GUIAction::clear)
_CLASSREGISTER_AddMember(getCurrentFrameData, GUIAction::getCurrentFrameData)
_CLASSREGISTER_AddMember(update, GUIAction::update)
_END_REGISTER_CLASS()


// Register Class GUIActionKeyFrame
_BEGIN_REGISTER_CLASS(GUIActionKeyFrame)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor()
_END_REGISTER_CLASS()


// Register Class GUIAnimate
_BEGIN_REGISTER_CLASS(GUIAnimate)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(clearImage, GUIAnimate::clearImage)
_CLASSREGISTER_AddMember(doPlay, GUIAnimate::doPlay)
//_CLASSREGISTER_AddMember(doPlayEx, GUIAnimate::doPlayEx)
_CLASSREGISTER_AddMember(getCurrentImageVect2, GUIAnimate::getCurrentImageVect2)
_END_REGISTER_CLASS()



// Register Class GUIAnimateController
_BEGIN_REGISTER_CLASS(GUIAnimateController)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(DoAnimate, GUIAnimateController::DoAnimate)
_CLASSREGISTER_AddCtor(GUIWindow *)
_CLASSREGISTER_AddMember(GetAnimateType, GUIAnimateController::GetAnimateType)
_CLASSREGISTER_AddMember(GetNeedRT, GUIAnimateController::GetNeedRT)
_CLASSREGISTER_AddMember(GetWholeTime, GUIAnimateController::GetWholeTime)
_CLASSREGISTER_AddMember(IsStarted, GUIAnimateController::IsStarted)
_CLASSREGISTER_AddMember(SetAnimateType, GUIAnimateController::SetAnimateType)
_CLASSREGISTER_AddMember(SetNeedRT, GUIAnimateController::SetNeedRT)
_CLASSREGISTER_AddMember(SetRotateAngle, GUIAnimateController::SetRotateAngle)
_CLASSREGISTER_AddMember(SetTargetRotateY, GUIAnimateController::SetTargetRotateY)
_CLASSREGISTER_AddMember(SetWhloeTime, GUIAnimateController::SetWhloeTime)
_CLASSREGISTER_AddMember(SetWholeTime, GUIAnimateController::SetWholeTime)
_CLASSREGISTER_AddMember(Start, GUIAnimateController::Start)
_CLASSREGISTER_AddMember(Update, GUIAnimateController::Update)
_END_REGISTER_CLASS()


// Register Class GUIAnimateWindow
_BEGIN_REGISTER_CLASS(GUIAnimateWindow)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(DrawSelf, GUIAnimateWindow::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetAnimateID, GUIAnimateWindow::GetAnimateID)
_CLASSREGISTER_AddMember(GetProperty, GUIAnimateWindow::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIAnimateWindow::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIAnimateWindow::GetPropertyType)
_CLASSREGISTER_AddMember(SetAnimateID, GUIAnimateWindow::SetAnimateID)
_CLASSREGISTER_AddMember(SetProperty, GUIAnimateWindow::SetProperty)
_CLASSREGISTER_AddMember(pause, GUIAnimateWindow::pause)
_CLASSREGISTER_AddMember(play, GUIAnimateWindow::play)
_END_REGISTER_CLASS()


// Register Class GUIAnimatMgr
_BEGIN_REGISTER_CLASS(GUIAnimatMgr)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(GetAnimatById, GUIAnimatMgr::GetAnimatById)
_CLASSREGISTER_AddStaticMember(Instance, GUIAnimatMgr::Instance)
_CLASSREGISTER_AddMember(lordConfig, GUIAnimatMgr::lordConfig)
_END_REGISTER_CLASS()


// Register Class BoundSlot
_BEGIN_REGISTER_CLASS(BoundSlot)
//_CLASSREGISTER_AddCtor(BoundSlot::Group , const SubscriberSlot & , GUIEvent & )
_CLASSREGISTER_AddCtor(const BoundSlot &)
_CLASSREGISTER_AddMember(connected, BoundSlot::connected)
_CLASSREGISTER_AddMember(disconnect, BoundSlot::disconnect)
_END_REGISTER_CLASS()


// Register Class ElemInfo
_BEGIN_REGISTER_CLASS(ElemInfo)
_CLASSREGISTER_AddCtor(GUIWindow *, ElemStatus, i32, float, float)
_END_REGISTER_CLASS()


// Register Class GUIBubbleContain
_BEGIN_REGISTER_CLASS(GUIBubbleContain)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(AddChild_impl, GUIBubbleContain::AddChild_impl)
_CLASSREGISTER_AddMember(DrawSelf, GUIBubbleContain::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProperty, GUIBubbleContain::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIBubbleContain::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIBubbleContain::GetPropertyType)
_CLASSREGISTER_AddMember(SetProperty, GUIBubbleContain::SetProperty)
_CLASSREGISTER_AddMember(UpdateSelf, GUIBubbleContain::UpdateSelf)
_CLASSREGISTER_AddMember(addItem, GUIBubbleContain::addItem)
_CLASSREGISTER_AddMember(setItemBackImageName, GUIBubbleContain::setItemBackImageName)
_CLASSREGISTER_AddMember(setOverlap, GUIBubbleContain::setOverlap)
_CLASSREGISTER_AddMember(setSpeedAndStayTime, GUIBubbleContain::setSpeedAndStayTime)
_END_REGISTER_CLASS()


// Register Class GUIBubbleContainEx
_BEGIN_REGISTER_CLASS(GUIBubbleContainEx)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(AddChild_impl, GUIBubbleContainEx::AddChild_impl)
_CLASSREGISTER_AddMember(DrawSelf, GUIBubbleContainEx::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProperty, GUIBubbleContainEx::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIBubbleContainEx::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIBubbleContainEx::GetPropertyType)
_CLASSREGISTER_AddMember(SetProperty, GUIBubbleContainEx::SetProperty)
_CLASSREGISTER_AddMember(UpdateSelf, GUIBubbleContainEx::UpdateSelf)
_CLASSREGISTER_AddMember(addItem, GUIBubbleContainEx::addItem)
_CLASSREGISTER_AddMember(setItemBackImageName, GUIBubbleContainEx::setItemBackImageName)
_CLASSREGISTER_AddMember(setOverlap, GUIBubbleContainEx::setOverlap)
_CLASSREGISTER_AddMember(setSpeedAndStayTime, GUIBubbleContainEx::setSpeedAndStayTime)
_END_REGISTER_CLASS()


// Register Class GUIButton
_BEGIN_REGISTER_CLASS(GUIButton)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(DrawSelf, GUIButton::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProperty, GUIButton::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIButton::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIButton::GetPropertyType)
_CLASSREGISTER_AddMember(GetTextColor, GUIButton::GetTextColor)
_CLASSREGISTER_AddMember(MotionRelease, GUIButton::MotionRelease)
_CLASSREGISTER_AddMember(OnFontChanged, GUIButton::OnFontChanged)
_CLASSREGISTER_AddMember(OnSized, GUIButton::OnSized)
_CLASSREGISTER_AddMember(OnTextChanged, GUIButton::OnTextChanged)
_CLASSREGISTER_AddMember(SetHexagonImage, GUIButton::SetHexagonImage)
_CLASSREGISTER_AddMember(SetHexagonRenderHeight, GUIButton::SetHexagonRenderHeight)
_CLASSREGISTER_AddMember(SetNormalImage, GUIButton::SetNormalImage)
_CLASSREGISTER_AddMember(SetProperty, GUIButton::SetProperty)
_CLASSREGISTER_AddMember(SetPushedImage, GUIButton::SetPushedImage)
_CLASSREGISTER_AddMember(SetTextColor, GUIButton::SetTextColor)
_CLASSREGISTER_AddMember(SetTextScale, GUIButton::SetTextScale)
_CLASSREGISTER_AddMember(TouchDown, GUIButton::TouchDown)
_CLASSREGISTER_AddMember(TouchUp, GUIButton::TouchUp)
_CLASSREGISTER_AddMember(UpdateSelf, GUIButton::UpdateSelf)
_CLASSREGISTER_AddMember(SetRenderOffset, GUIButton::SetRenderOffset)
_CLASSREGISTER_AddMember(SetEnabled, GUIButton::SetEnabled)
_CLASSREGISTER_AddMember(GetRenderOffset, GUIButton::GetRenderOffset)
_CLASSREGISTER_AddMember(setGray, GUIButton::setGray)
_END_REGISTER_CLASS()



// Register Class GUICell
_BEGIN_REGISTER_CLASS(GUICell)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(DrawSelf, GUICell::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(SetVertexData, GUICell::SetVertexData)
_END_REGISTER_CLASS()


// Register Class GUICheckBox
_BEGIN_REGISTER_CLASS(GUICheckBox)
_CLASSREGISTER_AddBaseClass(GUIButton)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetChecked, GUICheckBox::GetChecked)
_CLASSREGISTER_AddMember(MotionRelease, GUICheckBox::MotionRelease)
_CLASSREGISTER_AddMember(SetChecked, GUICheckBox::SetChecked)
_CLASSREGISTER_AddMember(SetCheckedNoEvent, GUICheckBox::SetCheckedNoEvent)
_CLASSREGISTER_AddMember(TouchDown, GUICheckBox::TouchDown)
_CLASSREGISTER_AddMember(TouchUp, GUICheckBox::TouchUp)
_END_REGISTER_CLASS()


// Register Class CoordConverter
_BEGIN_REGISTER_CLASS(CoordConverter)
_CLASSREGISTER_AddStaticMember_Override(screenToWindow, CoordConverter::screenToWindow, Vector2, const GUIWindow &, const UVector2 &)
_CLASSREGISTER_AddStaticMember_Override(screenToWindow1, CoordConverter::screenToWindow, Vector2, const GUIWindow &, const Vector2 &)
_CLASSREGISTER_AddStaticMember_Override(screenToWindow2, CoordConverter::screenToWindow, ERect, const GUIWindow &, const URect &)
_CLASSREGISTER_AddStaticMember_Override(screenToWindow3, CoordConverter::screenToWindow, ERect, const GUIWindow &, const ERect &)
_CLASSREGISTER_AddStaticMember_Override(screenToWindowX, CoordConverter::screenToWindowX, float, const GUIWindow &, const UDim &)
_CLASSREGISTER_AddStaticMember_Override(screenToWindowX1, CoordConverter::screenToWindowX, float, const GUIWindow &, const float)
_CLASSREGISTER_AddStaticMember_Override(screenToWindowY, CoordConverter::screenToWindowY, float, const GUIWindow &, const UDim &)
_CLASSREGISTER_AddStaticMember_Override(screenToWindowY1, CoordConverter::screenToWindowY, float, const GUIWindow &, const float)
_CLASSREGISTER_AddStaticMember_Override(windowToScreen, CoordConverter::windowToScreen, Vector2, const GUIWindow &, const UVector2 &)
_CLASSREGISTER_AddStaticMember_Override(windowToScreen1, CoordConverter::windowToScreen, Vector2, const GUIWindow &, const Vector2 &)
_CLASSREGISTER_AddStaticMember_Override(windowToScreen2, CoordConverter::windowToScreen, ERect, const GUIWindow &, const URect &)
_CLASSREGISTER_AddStaticMember_Override(windowToScreen3, CoordConverter::windowToScreen, ERect, const GUIWindow &, const ERect &)
_CLASSREGISTER_AddStaticMember_Override(windowToScreenX, CoordConverter::windowToScreenX, float, const GUIWindow &, const UDim &)
_CLASSREGISTER_AddStaticMember_Override(windowToScreenX1, CoordConverter::windowToScreenX, float, const GUIWindow &, const float)
_CLASSREGISTER_AddStaticMember_Override(windowToScreenY, CoordConverter::windowToScreenY, float, const GUIWindow &, const UDim &)
_CLASSREGISTER_AddStaticMember_Override(windowToScreenY1, CoordConverter::windowToScreenY, float, const GUIWindow &, const float)
_END_REGISTER_CLASS()


// Register Class GUIEditBox
_BEGIN_REGISTER_CLASS(GUIEditBox)
_CLASSREGISTER_AddBaseClass(GUIStaticText)
_CLASSREGISTER_AddMember(CloseKeyboard, GUIEditBox::CloseKeyboard)
_CLASSREGISTER_AddMember(DrawSelf, GUIEditBox::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetMaxLength, GUIEditBox::GetMaxLength)
_CLASSREGISTER_AddMember(GetProperty, GUIEditBox::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIEditBox::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIEditBox::GetPropertyType)
_CLASSREGISTER_AddMember(OnInputText, GUIEditBox::OnInputText)
_CLASSREGISTER_AddMember(OnTextChanged, GUIEditBox::OnTextChanged)
_CLASSREGISTER_AddMember(OpenKeyboard, GUIEditBox::OpenKeyboard)
_CLASSREGISTER_AddMember(SetBackImage, GUIEditBox::SetBackImage)
_CLASSREGISTER_AddMember(SetBlinkshow, GUIEditBox::SetBlinkshow)
_CLASSREGISTER_AddMember(SetCaratImage, GUIEditBox::SetCaratImage)
_CLASSREGISTER_AddMember(SetCaratOffset, GUIEditBox::SetCaratOffset)
_CLASSREGISTER_AddMember(SetMaxLength, GUIEditBox::SetMaxLength)
_CLASSREGISTER_AddMember(SetProperty, GUIEditBox::SetProperty)
_CLASSREGISTER_AddMember(TouchUp, GUIEditBox::TouchUp)
_CLASSREGISTER_AddMember(UpdateSelf, GUIEditBox::UpdateSelf)
_CLASSREGISTER_AddMember(getEditBoxImpl, GUIEditBox::getEditBoxImpl)
//_CLASSREGISTER_AddMember(EnableSmallKeyboard, GUIEditBox::EnableSmallKeyboard)
_END_REGISTER_CLASS()


_REGISTER_FUNC(__createSystemEditBox)

_REGISTER_FUNC(__destroySystemEditBox)

// Register Class EventArgs
_BEGIN_REGISTER_CLASS(EventArgs)
_CLASSREGISTER_AddCtor()
_END_REGISTER_CLASS()


// Register Class MouseEventArgs
_BEGIN_REGISTER_CLASS(MouseEventArgs)
_CLASSREGISTER_AddBaseClass(WindowEventArgs)
_CLASSREGISTER_AddCtor(GUIWindow *)
_CLASSREGISTER_AddVar(position, MouseEventArgs::position)
_END_REGISTER_CLASS()


// Register Class TouchEventArgs
_BEGIN_REGISTER_CLASS(TouchEventArgs)
_CLASSREGISTER_AddBaseClass(WindowEventArgs)
_CLASSREGISTER_AddCtor(GUIWindow *)
_END_REGISTER_CLASS()


// Register Class WindowEventArgs
_BEGIN_REGISTER_CLASS(WindowEventArgs)
_CLASSREGISTER_AddBaseClass(EventArgs)
_CLASSREGISTER_AddCtor(GUIWindow *)
_END_REGISTER_CLASS()


// Register Class ScrollEventArgs
_BEGIN_REGISTER_CLASS(ScrollEventArgs)
_CLASSREGISTER_AddBaseClass(EventArgs)
_CLASSREGISTER_AddVar(offset, ScrollEventArgs::offset)
_END_REGISTER_CLASS()


// Register Class GUIFlyAnimateController
_BEGIN_REGISTER_CLASS(GUIFlyAnimateController)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(DoAnimate, GUIFlyAnimateController::DoAnimate)
_CLASSREGISTER_AddCtor(GUIWindow *)
_CLASSREGISTER_AddMember(GetAnimateType, GUIFlyAnimateController::GetAnimateType)
_CLASSREGISTER_AddMember(GetBeginArea, GUIFlyAnimateController::GetBeginArea)
_CLASSREGISTER_AddMember(GetEndArea, GUIFlyAnimateController::GetEndArea)
_CLASSREGISTER_AddMember(GetFlyAnimatInfo, GUIFlyAnimateController::GetFlyAnimatInfo)
//_CLASSREGISTER_AddMember(GetFlyTime, GUIFlyAnimateController::GetFlyTime)
_CLASSREGISTER_AddMember(GetNeedRT, GUIFlyAnimateController::GetNeedRT)
_CLASSREGISTER_AddMember(SetAnimateType, GUIFlyAnimateController::SetAnimateType)
_CLASSREGISTER_AddMember_Override(SetBeginArea, GUIFlyAnimateController::SetBeginArea, void)
_CLASSREGISTER_AddMember_Override(SetBeginArea1, GUIFlyAnimateController::SetBeginArea, void, const URect &)
_CLASSREGISTER_AddMember_Override(SetEndArea, GUIFlyAnimateController::SetEndArea, void)
_CLASSREGISTER_AddMember_Override(SetEndArea1, GUIFlyAnimateController::SetEndArea, void, const URect &)
_CLASSREGISTER_AddMember(SetFlyTime, GUIFlyAnimateController::SetFlyTime)
_CLASSREGISTER_AddMember(SetNeedRT, GUIFlyAnimateController::SetNeedRT)
_CLASSREGISTER_AddMember(Start, GUIFlyAnimateController::Start)
_CLASSREGISTER_AddMember(Update, GUIFlyAnimateController::Update)
_CLASSREGISTER_AddMember(onChangeAnimateInfo, GUIFlyAnimateController::onChangeAnimateInfo)
_END_REGISTER_CLASS()


// Register Class GUIFont
_BEGIN_REGISTER_CLASS(GUIFont)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(GetBaseline, GUIFont::GetBaseline)
_CLASSREGISTER_AddMember(GetFontHeight, GUIFont::GetFontHeight)
_CLASSREGISTER_AddMember(GetFontType, GUIFont::GetFontType)
_CLASSREGISTER_AddMember(GetLineExtraSpace, GUIFont::GetLineExtraSpace)
_CLASSREGISTER_AddMember(GetLineSpacing, GUIFont::GetLineSpacing)
_CLASSREGISTER_AddMember(GetName, GUIFont::GetName)
_CLASSREGISTER_AddMember(GetTextExtent, GUIFont::GetTextExtent)
_CLASSREGISTER_AddMember(GetTextHigh, GUIFont::GetTextHigh)
_CLASSREGISTER_AddMember(IsCodepointAvailable, GUIFont::IsCodepointAvailable)
//_CLASSREGISTER_AddMember_Override(RenderText, GUIFont::RenderText,void ,const GUIString & ,const Vector2 & ,const ERect & ,const Color & ,bool ,const Color & ,bool ,const Color & ,const Vector2 & ,GUIWindow * ,FontBufferListType * )
//_CLASSREGISTER_AddMember_Override(RenderText1, GUIFont::RenderText,void ,const GUIString & ,const Vector2 & ,const ERect & ,const ColorRect & ,bool ,const Color & ,bool ,const Color & ,const Vector2 & ,GUIWindow * ,FontBufferListType * )
_CLASSREGISTER_AddMember(SetLineExtraSpace, GUIFont::SetLineExtraSpace)
_CLASSREGISTER_AddMember(SplitStringToMultiLine, GUIFont::SplitStringToMultiLine)
_END_REGISTER_CLASS()

// Register Class GUIFontGlyph
_BEGIN_REGISTER_CLASS(GUIFontGlyph)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(float, const GUIImage *)
_CLASSREGISTER_AddMember(GetAdvance, GUIFontGlyph::GetAdvance)
_CLASSREGISTER_AddMember(GetHeight, GUIFontGlyph::GetHeight)
_CLASSREGISTER_AddMember(GetImage, GUIFontGlyph::GetImage)
_CLASSREGISTER_AddMember(GetImageset, GUIFontGlyph::GetImageset)
_CLASSREGISTER_AddMember(GetRenderedAdvance, GUIFontGlyph::GetRenderedAdvance)
_CLASSREGISTER_AddMember(GetSize, GUIFontGlyph::GetSize)
_CLASSREGISTER_AddMember(GetWidth, GUIFontGlyph::GetWidth)
_CLASSREGISTER_AddMember(SetAdvance, GUIFontGlyph::SetAdvance)
_CLASSREGISTER_AddMember(SetImage, GUIFontGlyph::SetImage)
_END_REGISTER_CLASS()

// Register Class GUIFontManager
_BEGIN_REGISTER_CLASS(GUIFontManager)
_CLASSREGISTER_AddMember(CreateFreeTypeFont, GUIFontManager::CreateFreeTypeFont)
_CLASSREGISTER_AddMember(CreatePixmapFont, GUIFontManager::CreatePixmapFont)
_CLASSREGISTER_AddMember(DestroyAllFont, GUIFontManager::DestroyAllFont)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(GetDefaultFont, GUIFontManager::GetDefaultFont)
_CLASSREGISTER_AddMember(GetFont, GUIFontManager::GetFont)
_CLASSREGISTER_AddStaticMember(Instance, GUIFontManager::Instance)
_END_REGISTER_CLASS()

/*
// Register Class GUIFreeTypeFont
_BEGIN_REGISTER_CLASS(GUIFreeTypeFont)
_CLASSREGISTER_AddBaseClass(GUIFont)
_CLASSREGISTER_AddCtor(const GUIString & , const GUIString & , const float )
_CLASSREGISTER_AddMember(GetCurrentImageset, GUIFreeTypeFont::GetCurrentImageset)
_CLASSREGISTER_AddMember(GetGlyphData, GUIFreeTypeFont::GetGlyphData)
_END_REGISTER_CLASS()
*/

// Register Class GUIGridView
_BEGIN_REGISTER_CLASS(GUIGridView)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(AddItem, GUIGridView::AddItem)
//_CLASSREGISTER_AddMember(AddItemByIndex, GUIGridView::AddItemByIndex)
_CLASSREGISTER_AddMember(GetItem, GUIGridView::GetItem)
_CLASSREGISTER_AddMember(GetItemCount, GUIGridView::GetItemCount)
_CLASSREGISTER_AddMember(GetProperty, GUIGridView::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIGridView::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIGridView::GetPropertyType)
_CLASSREGISTER_AddMember(HasItemHidden, GUIGridView::HasItemHidden)
_CLASSREGISTER_AddMember(InitConfig, GUIGridView::InitConfig)
_CLASSREGISTER_AddMember(MotionRelease, GUIGridView::MotionRelease)
_CLASSREGISTER_AddMember(OnEndDraw, GUIGridView::OnEndDraw)
_CLASSREGISTER_AddMember(RemoveAllItems, GUIGridView::RemoveAllItems)
_CLASSREGISTER_AddMember(RemoveItem, GUIGridView::RemoveItem)
_CLASSREGISTER_AddMember(ResetPos, GUIGridView::ResetPos)
_CLASSREGISTER_AddMember(SetItemAlignment, GUIGridView::SetItemAlignment)
_CLASSREGISTER_AddMember(SetMoveAble, GUIGridView::SetMoveAble)
_CLASSREGISTER_AddMember(SetProperty, GUIGridView::SetProperty)
_CLASSREGISTER_AddMember(TouchDown, GUIGridView::TouchDown)
_CLASSREGISTER_AddMember(TouchMove, GUIGridView::TouchMove)
_CLASSREGISTER_AddMember(TouchUp, GUIGridView::TouchUp)
_CLASSREGISTER_AddMember(UpdateSelf, GUIGridView::UpdateSelf)
_CLASSREGISTER_AddMember(SetScrollOffset, GUIGridView::SetScrollOffset)
_CLASSREGISTER_AddMember(GetScrollOffset, GUIGridView::GetScrollOffset)
_CLASSREGISTER_AddMember(ResetPos1, GUIGridView::ResetPos)
_CLASSREGISTER_AddMember(GetHorizontalInterval, GUIGridView::GetHorizontalInterval)
_CLASSREGISTER_AddMember(GetVerticalInterval, GUIGridView::GetVerticalInterval)
_CLASSREGISTER_AddMember(GetRowSize, GUIGridView::GetRowSize)
_CLASSREGISTER_AddMember(SetVirtualListOffset, GUIGridView::SetVirtualListOffset)
_CLASSREGISTER_AddMember(GetVirtualListOffset, GUIGridView::GetVirtualListOffset)
_CLASSREGISTER_AddMember(EnableVirtualList, GUIGridView::EnableVirtualList)
_END_REGISTER_CLASS()


// Register Class GUIHyperlinkManager
_BEGIN_REGISTER_CLASS(GUIHyperlinkManager)
_CLASSREGISTER_AddMember(AddHyperLink, GUIHyperlinkManager::AddHyperLink)
_CLASSREGISTER_AddMember(ClearAllHyperLink, GUIHyperlinkManager::ClearAllHyperLink)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(GetClickArea, GUIHyperlinkManager::GetClickArea)
_CLASSREGISTER_AddStaticMember(Instance, GUIHyperlinkManager::Instance)
_CLASSREGISTER_AddMember(IsClickedLink, GUIHyperlinkManager::IsClickedLink)
_CLASSREGISTER_AddMember(OnClickHyperLink, GUIHyperlinkManager::OnClickHyperLink)
_CLASSREGISTER_AddMember(SetClickLink, GUIHyperlinkManager::SetClickLink)
_END_REGISTER_CLASS()


// Register Class FontBuffer
_BEGIN_REGISTER_CLASS(FontBuffer)
_CLASSREGISTER_AddCtor()
_END_REGISTER_CLASS()

// Register Class GUIImage
_BEGIN_REGISTER_CLASS(GUIImage)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddStaticMember(CaculateRotate, GUIImage::CaculateRotate)
_CLASSREGISTER_AddMember(DrawFontImage, GUIImage::DrawFontImage)
_CLASSREGISTER_AddMember(DrawHexagon, GUIImage::DrawHexagon)
_CLASSREGISTER_AddMember(DrawImage, GUIImage::DrawImage)
_CLASSREGISTER_AddMember(DrawStretchImage, GUIImage::DrawStretchImage)
_CLASSREGISTER_AddMember(DrawStretchImageLR, GUIImage::DrawStretchImageLR)
_CLASSREGISTER_AddMember(DrawStretchImageTB, GUIImage::DrawStretchImageTB)
_CLASSREGISTER_AddCtor(GUIImageset *, const GUIString &, const ERect &, const ERect &, const Vector2 &)
_CLASSREGISTER_AddCtor(const GUIImage &)
_CLASSREGISTER_AddMember(GetHeight, GUIImage::GetHeight)
_CLASSREGISTER_AddMember(GetImageset, GUIImage::GetImageset)
_CLASSREGISTER_AddMember(GetName, GUIImage::GetName)
_CLASSREGISTER_AddMember(GetOffset, GUIImage::GetOffset)
_CLASSREGISTER_AddMember(GetUV, GUIImage::GetUV)
_CLASSREGISTER_AddMember(GetWidth, GUIImage::GetWidth)
_CLASSREGISTER_AddMember(SetUV, GUIImage::SetUV)
_CLASSREGISTER_AddMember(UpdateImageData, GUIImage::UpdateImageData)
_CLASSREGISTER_AddMember(UpdateStretchImageData, GUIImage::UpdateStretchImageData)
_CLASSREGISTER_AddMember(UpdateStretchImageLRData, GUIImage::UpdateStretchImageLRData)
_CLASSREGISTER_AddMember(UpdateStretchImageTBData, GUIImage::UpdateStretchImageTBData)
_END_REGISTER_CLASS()


// Register Class ImageBuffer
_BEGIN_REGISTER_CLASS(ImageBuffer)
_CLASSREGISTER_AddCtor()
_END_REGISTER_CLASS()


// Register Class GUIImageset
_BEGIN_REGISTER_CLASS(GUIImageset)
_CLASSREGISTER_AddBaseClass(Resource)
_CLASSREGISTER_AddMember(DefineImage, GUIImageset::DefineImage)
_CLASSREGISTER_AddCtor(const String &, ResourceType)
_CLASSREGISTER_AddMember(GetImage, GUIImageset::GetImage)
_CLASSREGISTER_AddMember(GetImages, GUIImageset::GetImages)
_CLASSREGISTER_AddMember(GetName, GUIImageset::GetName)
_CLASSREGISTER_AddMember(GetTexture, GUIImageset::GetTexture)
_CLASSREGISTER_AddMember(IsFreeTypeFontBuffer, GUIImageset::IsFreeTypeFontBuffer)
_CLASSREGISTER_AddMember(IsImageDefined, GUIImageset::IsImageDefined)
_CLASSREGISTER_AddMember(ReplaceTexture, GUIImageset::ReplaceTexture)
_CLASSREGISTER_AddMember(SetFontBuffer, GUIImageset::SetFontBuffer)
_CLASSREGISTER_AddMember(UndefineAllImage, GUIImageset::UndefineAllImage)
_CLASSREGISTER_AddMember(UndefineImage, GUIImageset::UndefineImage)
_CLASSREGISTER_AddMember(isPreparing, GUIImageset::isPreparing)
_CLASSREGISTER_AddMember(loadOutSide, GUIImageset::loadOutSide)
_CLASSREGISTER_AddMember(setPreparing, GUIImageset::setPreparing)
_END_REGISTER_CLASS()



// Register Class GUIImagesetManager
_BEGIN_REGISTER_CLASS(GUIImagesetManager)
_CLASSREGISTER_AddBaseClass(ResourceManager)

_CLASSREGISTER_AddMember(CreateFromFile, GUIImagesetManager::CreateFromFile)
_CLASSREGISTER_AddMember(CreateManual, GUIImagesetManager::CreateManual)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddStaticMember(Instance, GUIImagesetManager::Instance)
_CLASSREGISTER_AddMember(ParseImageString, GUIImagesetManager::ParseImageString)
_CLASSREGISTER_AddMember(createImageset, GUIImagesetManager::createImageset)
_CLASSREGISTER_AddMember(createOrRetrieveImageset, GUIImagesetManager::createOrRetrieveImageset)
_CLASSREGISTER_AddMember(findImageSet, GUIImagesetManager::findImageSet)
_CLASSREGISTER_AddMember(findImageSetByName, GUIImagesetManager::findImageSetByName)
_CLASSREGISTER_AddMember(handleImagesetRelease, GUIImagesetManager::handleImagesetRelease)
_CLASSREGISTER_AddMember(pushbackReleaseImageset, GUIImagesetManager::pushbackReleaseImageset)
_CLASSREGISTER_AddMember(registerGUIImageset, GUIImagesetManager::registerGUIImageset)
_END_REGISTER_CLASS()


// Register Class GUIInputBox
_BEGIN_REGISTER_CLASS(GUIInputBox)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(ADDText, GUIInputBox::ADDText)
_CLASSREGISTER_AddMember(ClearList, GUIInputBox::ClearList)
_CLASSREGISTER_AddMember(DrawSelf, GUIInputBox::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetHeight, GUIInputBox::GetHeight)
_CLASSREGISTER_AddMember(GetProperty, GUIInputBox::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIInputBox::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIInputBox::GetPropertyType)
_CLASSREGISTER_AddMember(GetWidth, GUIInputBox::GetWidth)
_CLASSREGISTER_AddMember(Init, GUIInputBox::Init)
_CLASSREGISTER_AddMember(SetMoveAble, GUIInputBox::SetMoveAble)
_CLASSREGISTER_AddMember(SetNormalImage, GUIInputBox::SetNormalImage)
_CLASSREGISTER_AddMember(SetProperty, GUIInputBox::SetProperty)
_CLASSREGISTER_AddMember(TouchDown, GUIInputBox::TouchDown)
_CLASSREGISTER_AddMember(TouchMove, GUIInputBox::TouchMove)
_CLASSREGISTER_AddMember(TouchUp, GUIInputBox::TouchUp)
_END_REGISTER_CLASS()




// Register Class GUILayout
_BEGIN_REGISTER_CLASS(GUILayout)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(DrawSelf, GUILayout::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProperty, GUILayout::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUILayout::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUILayout::GetPropertyType)
_CLASSREGISTER_AddMember(LayoutChild, GUILayout::LayoutChild)
_CLASSREGISTER_AddMember(SetBackImage, GUILayout::SetBackImage)
_CLASSREGISTER_AddMember(SetProperty, GUILayout::SetProperty)
_CLASSREGISTER_AddMember(SetEnabled, GUILayout::SetEnabled)
_END_REGISTER_CLASS()


// Register Class GUIListBox
_BEGIN_REGISTER_CLASS(GUIListBox)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember_Override(AddItem, GUIListBox::AddItem, void, GUIWindow*, bool)
_CLASSREGISTER_AddMember(ClearAllItem, GUIListBox::ClearAllItem)
_CLASSREGISTER_AddMember(DeleteItem, GUIListBox::DeleteItem)
_CLASSREGISTER_AddMember(RemoveItem, GUIListBox::RemoveItem)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetItemHeight, GUIListBox::GetItemHeight)
_CLASSREGISTER_AddMember(GetProperty, GUIListBox::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIListBox::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIListBox::GetPropertyType)
_CLASSREGISTER_AddMember(IsBlinkSelect, GUIListBox::IsBlinkSelect)
_CLASSREGISTER_AddMember(MotionRelease, GUIListBox::MotionRelease)
_CLASSREGISTER_AddMember(OnEndDraw, GUIListBox::OnEndDraw)
_CLASSREGISTER_AddMember(ResetScroll, GUIListBox::ResetScroll)
_CLASSREGISTER_AddMember(SetAllowScroll, GUIListBox::SetAllowScroll)
_CLASSREGISTER_AddMember(SetBlinkSelect, GUIListBox::SetBlinkSelect)
_CLASSREGISTER_AddMember(SetInterval, GUIListBox::SetInterval)
_CLASSREGISTER_AddMember(SetItemHeight, GUIListBox::SetItemHeight)
_CLASSREGISTER_AddMember(SetItemNormalImage, GUIListBox::SetItemNormalImage)
_CLASSREGISTER_AddMember(SetItemSelectImage, GUIListBox::SetItemSelectImage)
_CLASSREGISTER_AddMember(SetMoveAble, GUIListBox::SetMoveAble)
_CLASSREGISTER_AddMember(SetProperty, GUIListBox::SetProperty)
_CLASSREGISTER_AddMember(SetScrollOffset, GUIListBox::SetScrollOffset)
_CLASSREGISTER_AddMember(SetSelectItem, GUIListBox::SetSelectItem)
_CLASSREGISTER_AddMember(TouchDown, GUIListBox::TouchDown)
_CLASSREGISTER_AddMember(TouchMove, GUIListBox::TouchMove)
_CLASSREGISTER_AddMember(TouchUp, GUIListBox::TouchUp)
_CLASSREGISTER_AddMember(UnSelectAll, GUIListBox::UnSelectAll)
_CLASSREGISTER_AddMember(UpdateSelf, GUIListBox::UpdateSelf)
_CLASSREGISTER_AddMember(getContainerWindow, GUIListBox::getContainerWindow)
_CLASSREGISTER_AddMember(setContainerWindow, GUIListBox::setContainerWindow)
_CLASSREGISTER_AddMember(GetScrollOffset, GUIListBox::GetScrollOffset)
_CLASSREGISTER_AddMember(GetInterval, GUIListBox::GetInterval)
_CLASSREGISTER_AddMember(SetVirtualListOffset, GUIListBox::SetVirtualListOffset)
_CLASSREGISTER_AddMember(GetVirtualListOffset, GUIListBox::GetVirtualListOffset)
_CLASSREGISTER_AddMember(EnableVirtualList, GUIListBox::EnableVirtualList)
_END_REGISTER_CLASS()



// Register Class GUIMultiLineEdit
_BEGIN_REGISTER_CLASS(GUIMultiLineEdit)
_CLASSREGISTER_AddBaseClass(GUIEditBox)
_CLASSREGISTER_AddMember(DrawSelf, GUIMultiLineEdit::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(SetMaxLength, GUIMultiLineEdit::SetMaxLength)
_CLASSREGISTER_AddMember(GetProperty, GUIMultiLineEdit::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIMultiLineEdit::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIMultiLineEdit::GetPropertyType)
_CLASSREGISTER_AddMember(OnFontChanged, GUIMultiLineEdit::OnFontChanged)
_CLASSREGISTER_AddMember(OnSized, GUIMultiLineEdit::OnSized)
_CLASSREGISTER_AddMember(OnTextChanged, GUIMultiLineEdit::OnTextChanged)
_CLASSREGISTER_AddMember(SetProperty, GUIMultiLineEdit::SetProperty)
_END_REGISTER_CLASS()



// Register Class GUIMutualNonoverlappingGroup
_BEGIN_REGISTER_CLASS(GUIMutualNonoverlappingGroup)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(Render, GUIMutualNonoverlappingGroup::Render)
_END_REGISTER_CLASS()


// Register Class GUIPageView
_BEGIN_REGISTER_CLASS(GUIPageView)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(AddNewPage, GUIPageView::AddNewPage)
_CLASSREGISTER_AddMember(AddPages, GUIPageView::AddPages)
_CLASSREGISTER_AddMember(DeleteAllPages, GUIPageView::DeleteAllPages)
//_CLASSREGISTER_AddMember(Destroy, GUIPageView::Destroy)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetPageByIndex, GUIPageView::GetPageByIndex)
_CLASSREGISTER_AddMember(GetPagesCount, GUIPageView::GetPagesCount)
_CLASSREGISTER_AddMember(GetProperty, GUIPageView::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIPageView::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIPageView::GetPropertyType)
_CLASSREGISTER_AddMember(JumpToPage, GUIPageView::JumpToPage)
_CLASSREGISTER_AddMember(MotionRelease, GUIPageView::MotionRelease)
_CLASSREGISTER_AddMember(NextPage, GUIPageView::NextPage)
_CLASSREGISTER_AddMember(PreviousPage, GUIPageView::PreviousPage)
_CLASSREGISTER_AddMember(SetConfigInfo, GUIPageView::SetConfigInfo)
_CLASSREGISTER_AddMember(SetProperty, GUIPageView::SetProperty)
_CLASSREGISTER_AddMember(TouchDown, GUIPageView::TouchDown)
_CLASSREGISTER_AddMember(TouchMove, GUIPageView::TouchMove)
_CLASSREGISTER_AddMember(TouchUp, GUIPageView::TouchUp)
_CLASSREGISTER_AddMember(UpdateSelf, GUIPageView::UpdateSelf)
_END_REGISTER_CLASS()


// Register Class GUIPixmapFont
_BEGIN_REGISTER_CLASS(GUIPixmapFont)
_CLASSREGISTER_AddBaseClass(GUIFont)
_CLASSREGISTER_AddMember(DefineMapping, GUIPixmapFont::DefineMapping)
_CLASSREGISTER_AddCtor(const GUIString &, const GUIString &)
_CLASSREGISTER_AddMember(GetGlyphData, GUIPixmapFont::GetGlyphData)
_END_REGISTER_CLASS()


// Register Class GUIProgressBar
_BEGIN_REGISTER_CLASS(GUIProgressBar)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(DrawSelf, GUIProgressBar::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProgress, GUIProgressBar::GetProgress)
_CLASSREGISTER_AddMember(GetProperty, GUIProgressBar::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIProgressBar::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIProgressBar::GetPropertyType)
_CLASSREGISTER_AddMember(SetBackImage, GUIProgressBar::SetBackImage)
_CLASSREGISTER_AddMember(SetHeaderImage, GUIProgressBar::SetHeaderImage)
_CLASSREGISTER_AddMember(SetProgress, GUIProgressBar::SetProgress)
_CLASSREGISTER_AddMember(SetProgressImage, GUIProgressBar::SetProgressImage)
_CLASSREGISTER_AddMember(SetProperty, GUIProgressBar::SetProperty)
_END_REGISTER_CLASS()


// Register Class GUIRadioButton
_BEGIN_REGISTER_CLASS(GUIRadioButton)
_CLASSREGISTER_AddBaseClass(GUIButton)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetGroupID, GUIRadioButton::GetGroupID)
_CLASSREGISTER_AddMember(GetProperty, GUIRadioButton::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIRadioButton::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIRadioButton::GetPropertyType)
_CLASSREGISTER_AddMember(IsSelected, GUIRadioButton::IsSelected)
_CLASSREGISTER_AddMember(MotionRelease, GUIRadioButton::MotionRelease)
_CLASSREGISTER_AddMember(SetGroupID, GUIRadioButton::SetGroupID)
_CLASSREGISTER_AddMember(SetProperty, GUIRadioButton::SetProperty)
_CLASSREGISTER_AddMember(SetSelected, GUIRadioButton::SetSelected)
_CLASSREGISTER_AddMember(TouchDown, GUIRadioButton::TouchDown)
_CLASSREGISTER_AddMember(TouchUp, GUIRadioButton::TouchUp)
_END_REGISTER_CLASS()


// Register Class GUIRenderManager
_BEGIN_REGISTER_CLASS(GUIRenderManager)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)

_CLASSREGISTER_AddMember(AddQuadToBuffer, GUIRenderManager::AddQuadToBuffer)
_CLASSREGISTER_AddMember(BeginGUIViewport, GUIRenderManager::BeginGUIViewport)
_CLASSREGISTER_AddMember(BeginRender, GUIRenderManager::BeginRender)
_CLASSREGISTER_AddMember(BeginScissorRect, GUIRenderManager::BeginScissorRect)
_CLASSREGISTER_AddMember(CreateRenderable, GUIRenderManager::CreateRenderable)
_CLASSREGISTER_AddMember(Destroy, GUIRenderManager::Destroy)
_CLASSREGISTER_AddMember(DestroyRenderable, GUIRenderManager::DestroyRenderable)
_CLASSREGISTER_AddMember(DrawBuffer, GUIRenderManager::DrawBuffer)
_CLASSREGISTER_AddMember(EndGUIViewport, GUIRenderManager::EndGUIViewport)
_CLASSREGISTER_AddMember(EndRender, GUIRenderManager::EndRender)
_CLASSREGISTER_AddMember(EndScissorRect, GUIRenderManager::EndScissorRect)
_CLASSREGISTER_AddCtor(uint, uint)
_CLASSREGISTER_AddMember(GetCurrentScissor, GUIRenderManager::GetCurrentScissor)
_CLASSREGISTER_AddMember(GetCurrentTexture1, GUIRenderManager::GetCurrentTexture1)
_CLASSREGISTER_AddMember(GetCurrentTexture2, GUIRenderManager::GetCurrentTexture2)
_CLASSREGISTER_AddMember(GetDepth, GUIRenderManager::GetDepth)
_CLASSREGISTER_AddMember(GetDisplaySize, GUIRenderManager::GetDisplaySize)
_CLASSREGISTER_AddMember(GetHeight, GUIRenderManager::GetHeight)
_CLASSREGISTER_AddMember(GetMaterial, GUIRenderManager::GetMaterial)
_CLASSREGISTER_AddMember(GetWidth, GUIRenderManager::GetWidth)
_CLASSREGISTER_AddMember(Initialize, GUIRenderManager::Initialize)
_CLASSREGISTER_AddStaticMember(Instance, GUIRenderManager::Instance)
_CLASSREGISTER_AddMember(IsInScissor, GUIRenderManager::IsInScissor)
_CLASSREGISTER_AddMember(OnSized, GUIRenderManager::OnSized)
_CLASSREGISTER_AddMember(RenderEvents, GUIRenderManager::RenderEvents)
_CLASSREGISTER_AddMember(SetDepth, GUIRenderManager::SetDepth)
_CLASSREGISTER_AddMember(clearQuadNumber, GUIRenderManager::clearQuadNumber)
_CLASSREGISTER_AddMember(getQuadNumber, GUIRenderManager::getQuadNumber)
_END_REGISTER_CLASS()


// Register Class GUIRenderTargetManager
_BEGIN_REGISTER_CLASS(GUIRenderTargetManager)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)

_CLASSREGISTER_AddMember(BeginRenderTarget, GUIRenderTargetManager::BeginRenderTarget)
_CLASSREGISTER_AddMember(CreateRenderTarget, GUIRenderTargetManager::CreateRenderTarget)
_CLASSREGISTER_AddMember(DestroyRenderTarget, GUIRenderTargetManager::DestroyRenderTarget)
_CLASSREGISTER_AddMember(EndRenderTarget, GUIRenderTargetManager::EndRenderTarget)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddStaticMember(Instance, GUIRenderTargetManager::Instance)
_END_REGISTER_CLASS()


// Register Class GUIRenderText
_BEGIN_REGISTER_CLASS(GUIRenderText)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(DrawGUIText, GUIRenderText::DrawGUIText)
_CLASSREGISTER_AddStaticMember(DrawTextBuffer, GUIRenderText::DrawTextBuffer)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(GetBorderColor, GUIRenderText::GetBorderColor)
_CLASSREGISTER_AddMember(GetFont, GUIRenderText::GetFont)
_CLASSREGISTER_AddMember(GetLineExtraSpace, GUIRenderText::GetLineExtraSpace)
_CLASSREGISTER_AddMember(GetShadowColor, GUIRenderText::GetShadowColor)
_CLASSREGISTER_AddMember(GetTextColor, GUIRenderText::GetTextColor)
_CLASSREGISTER_AddMember(GetTextExtent, GUIRenderText::GetTextExtent)
_CLASSREGISTER_AddMember(GetTextHorzAlign, GUIRenderText::GetTextHorzAlign)
_CLASSREGISTER_AddMember(GetTextStringHigh, GUIRenderText::GetTextStringHigh)
_CLASSREGISTER_AddMember(GetTextStringList, GUIRenderText::GetTextStringList)
_CLASSREGISTER_AddMember(GetTextVertAlign, GUIRenderText::GetTextVertAlign)
_CLASSREGISTER_AddMember(IsBordered, GUIRenderText::IsBordered)
_CLASSREGISTER_AddMember(IsShadowed, GUIRenderText::IsShadowed)
_CLASSREGISTER_AddMember(IsWordWrap, GUIRenderText::IsWordWrap)
_CLASSREGISTER_AddMember(LayoutWordWrapText, GUIRenderText::LayoutWordWrapText)
_CLASSREGISTER_AddMember(SetBorderColor, GUIRenderText::SetBorderColor)
_CLASSREGISTER_AddMember(SetBordered, GUIRenderText::SetBordered)
_CLASSREGISTER_AddMember(SetFont, GUIRenderText::SetFont)
_CLASSREGISTER_AddMember(SetLineExtraSpace, GUIRenderText::SetLineExtraSpace)
_CLASSREGISTER_AddMember(SetShadowColor, GUIRenderText::SetShadowColor)
_CLASSREGISTER_AddMember(SetShadowed, GUIRenderText::SetShadowed)
_CLASSREGISTER_AddMember(SetText, GUIRenderText::SetText)
_CLASSREGISTER_AddMember(SetTextColor, GUIRenderText::SetTextColor)
_CLASSREGISTER_AddMember(SetTextHorzAlign, GUIRenderText::SetTextHorzAlign)
_CLASSREGISTER_AddMember(SetTextScale, GUIRenderText::SetTextScale)
_CLASSREGISTER_AddMember(SetTextVertAlign, GUIRenderText::SetTextVertAlign)
_CLASSREGISTER_AddMember(SetWidth, GUIRenderText::SetWidth)
_CLASSREGISTER_AddMember(SetWordWrap, GUIRenderText::SetWordWrap)
_CLASSREGISTER_AddMember(getRenderOffset, GUIRenderText::getRenderOffset)
_CLASSREGISTER_AddMember(setRenderOffset, GUIRenderText::setRenderOffset)
_END_REGISTER_CLASS()




// Register Class GUISceneMap
_BEGIN_REGISTER_CLASS(GUISceneMap)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(ClearAllIcon, GUISceneMap::ClearAllIcon)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProperty, GUISceneMap::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUISceneMap::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUISceneMap::GetPropertyType)
_CLASSREGISTER_AddMember(GetTargetPosition, GUISceneMap::GetTargetPosition)
_CLASSREGISTER_AddMember(InitSceneMap, GUISceneMap::InitSceneMap)
_CLASSREGISTER_AddMember(ResetAllWindow, GUISceneMap::ResetAllWindow)
_CLASSREGISTER_AddMember(SetCenterPostion, GUISceneMap::SetCenterPostion)
_CLASSREGISTER_AddMember(SetIcon, GUISceneMap::SetIcon)
_CLASSREGISTER_AddMember(SetProperty, GUISceneMap::SetProperty)
_CLASSREGISTER_AddMember(SetScale, GUISceneMap::SetScale)
_CLASSREGISTER_AddMember(TouchDown, GUISceneMap::TouchDown)
_CLASSREGISTER_AddMember(TouchMove, GUISceneMap::TouchMove)
_CLASSREGISTER_AddMember(TouchUp, GUISceneMap::TouchUp)
_CLASSREGISTER_AddMember(UpdateSelf, GUISceneMap::UpdateSelf)
_END_REGISTER_CLASS()




// Register Class GUIScrollablePane
_BEGIN_REGISTER_CLASS(GUIScrollablePane)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(AddItem, GUIScrollablePane::AddItem)
_CLASSREGISTER_AddMember(ClearAllItem, GUIScrollablePane::ClearAllItem)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetContainer, GUIScrollablePane::GetContainer)
_CLASSREGISTER_AddMember(GetHoriScrollOffset, GUIScrollablePane::GetHoriScrollOffset)
_CLASSREGISTER_AddMember(GetProperty, GUIScrollablePane::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIScrollablePane::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIScrollablePane::GetPropertyType)
_CLASSREGISTER_AddMember(GetVertScrollOffset, GUIScrollablePane::GetVertScrollOffset)
_CLASSREGISTER_AddMember(InitializeContainer, GUIScrollablePane::InitializeContainer)
_CLASSREGISTER_AddMember(InitializePos, GUIScrollablePane::InitializePos)
_CLASSREGISTER_AddMember(MotionRelease, GUIScrollablePane::MotionRelease)
_CLASSREGISTER_AddMember(OnEndDraw, GUIScrollablePane::OnEndDraw)
_CLASSREGISTER_AddMember(SetHoriScrollEnable, GUIScrollablePane::SetHoriScrollEnable)
_CLASSREGISTER_AddMember(SetHoriScrollOffset, GUIScrollablePane::SetHoriScrollOffset)
_CLASSREGISTER_AddMember(SetProperty, GUIScrollablePane::SetProperty)
_CLASSREGISTER_AddMember(SetVertScrollEnable, GUIScrollablePane::SetVertScrollEnable)
_CLASSREGISTER_AddMember(SetVertScrollOffset, GUIScrollablePane::SetVertScrollOffset)
_CLASSREGISTER_AddMember(TouchDown, GUIScrollablePane::TouchDown)
_CLASSREGISTER_AddMember(TouchMove, GUIScrollablePane::TouchMove)
_CLASSREGISTER_AddMember(TouchUp, GUIScrollablePane::TouchUp)
_CLASSREGISTER_AddMember(UpdateSelf, GUIScrollablePane::UpdateSelf)
_END_REGISTER_CLASS()




// Register Class GUIScrollCard
_BEGIN_REGISTER_CLASS(GUIScrollCard)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(AddCard, GUIScrollCard::AddCard)
_CLASSREGISTER_AddMember(ClearAllCard, GUIScrollCard::ClearAllCard)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProperty, GUIScrollCard::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIScrollCard::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIScrollCard::GetPropertyType)
_CLASSREGISTER_AddMember(GetSelectCardIndex, GUIScrollCard::GetSelectCardIndex)
_CLASSREGISTER_AddMember(GetTargetCardIndex, GUIScrollCard::GetTargetCardIndex)
_CLASSREGISTER_AddMember(MotionRelease, GUIScrollCard::MotionRelease)
_CLASSREGISTER_AddMember(ResetCardPos, GUIScrollCard::ResetCardPos)
_CLASSREGISTER_AddMember(ScrollToCard, GUIScrollCard::ScrollToCard)
_CLASSREGISTER_AddMember(SetProperty, GUIScrollCard::SetProperty)
_CLASSREGISTER_AddMember(TouchDown, GUIScrollCard::TouchDown)
_CLASSREGISTER_AddMember(TouchMove, GUIScrollCard::TouchMove)
_CLASSREGISTER_AddMember(TouchUp, GUIScrollCard::TouchUp)
_CLASSREGISTER_AddMember(UpdateSelf, GUIScrollCard::UpdateSelf)
_CLASSREGISTER_AddMember(handleCardClicked, GUIScrollCard::handleCardClicked)
_END_REGISTER_CLASS()




// Register Class GUIScrolledContainer
_BEGIN_REGISTER_CLASS(GUIScrolledContainer)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProperty, GUIScrolledContainer::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIScrolledContainer::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIScrolledContainer::GetPropertyType)
_CLASSREGISTER_AddMember(OnChildAdd, GUIScrolledContainer::OnChildAdd)
_CLASSREGISTER_AddMember(OnChildRemove, GUIScrolledContainer::OnChildRemove)
_CLASSREGISTER_AddMember(OnResizeContainer, GUIScrolledContainer::OnResizeContainer)
_CLASSREGISTER_AddMember(SetProperty, GUIScrolledContainer::SetProperty)
_END_REGISTER_CLASS()




// Register Class GUISlider
_BEGIN_REGISTER_CLASS(GUISlider)
_CLASSREGISTER_AddBaseClass(GUIProgressBar)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProperty, GUISlider::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUISlider::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUISlider::GetPropertyType)
_CLASSREGISTER_AddMember(MotionRelease, GUISlider::MotionRelease)
_CLASSREGISTER_AddMember(SetProperty, GUISlider::SetProperty)
_CLASSREGISTER_AddMember(TouchDown, GUISlider::TouchDown)
_CLASSREGISTER_AddMember(TouchMove, GUISlider::TouchMove)
_CLASSREGISTER_AddMember(TouchUp, GUISlider::TouchUp)
_END_REGISTER_CLASS()






// Register Class GUIStaticImage
_BEGIN_REGISTER_CLASS(GUIStaticImage)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(DrawSelf, GUIStaticImage::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProperty, GUIStaticImage::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIStaticImage::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIStaticImage::GetPropertyType)
_CLASSREGISTER_AddMember(SetHexagonImage, GUIStaticImage::SetHexagonImage)
_CLASSREGISTER_AddMember(SetHexagonRenderHeight, GUIStaticImage::SetHexagonRenderHeight)
_CLASSREGISTER_AddMember(SetHexagonSideColor, GUIStaticImage::SetHexagonSideColor)
_CLASSREGISTER_AddMember(SetHexagonTopColor, GUIStaticImage::SetHexagonTopColor)
_CLASSREGISTER_AddMember(SetImage, GUIStaticImage::SetImage)
_CLASSREGISTER_AddMember(SetOverlayColor, GUIStaticImage::SetOverlayColor)
_CLASSREGISTER_AddMember(SetOverlayImage, GUIStaticImage::SetOverlayImage)
_CLASSREGISTER_AddMember(SetProperty, GUIStaticImage::SetProperty)
_CLASSREGISTER_AddMember(SetRenderAsBlock, GUIStaticImage::SetRenderAsBlock)
_CLASSREGISTER_AddMember(SetVertexData, GUIStaticImage::SetVertexData)
_CLASSREGISTER_AddMember(SetEnabled, GUIStaticImage::SetEnabled)
_CLASSREGISTER_AddMember(SetEffectName, GUIStaticImage::SetEffectName)
//_CLASSREGISTER_AddMember(setCooldownAnim, GUIStaticImage::setCooldownAnim)
_END_REGISTER_CLASS()




// Register Class GUIStaticText
_BEGIN_REGISTER_CLASS(GUIStaticText)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(AllShowOneLine, GUIStaticText::AllShowOneLine)
_CLASSREGISTER_AddMember(DrawSelf, GUIStaticText::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProperty, GUIStaticText::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIStaticText::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIStaticText::GetPropertyType)
_CLASSREGISTER_AddMember(GetTextColor, GUIStaticText::GetTextColor)
_CLASSREGISTER_AddMember(OnFontChanged, GUIStaticText::OnFontChanged)
_CLASSREGISTER_AddMember(OnSized, GUIStaticText::OnSized)
_CLASSREGISTER_AddMember(OnTextChanged, GUIStaticText::OnTextChanged)
_CLASSREGISTER_AddMember(SetProperty, GUIStaticText::SetProperty)
_CLASSREGISTER_AddMember(SetSelfAdaptionArea, GUIStaticText::SetSelfAdaptionArea)
_CLASSREGISTER_AddMember(SetTextBoader, GUIStaticText::SetTextBoader)
_CLASSREGISTER_AddMember(SetTextColor, GUIStaticText::SetTextColor)
_CLASSREGISTER_AddMember_Override(SetTextHorzAlign, GUIStaticText::SetTextHorzAlign, void, HorizontalAlignment)
_CLASSREGISTER_AddMember_Override(SetTextVertAlign, GUIStaticText::SetTextVertAlign, void, VerticalAlignment)
_CLASSREGISTER_AddMember(SetTextLineExtraSpace, GUIStaticText::SetTextLineExtraSpace)
_CLASSREGISTER_AddMember(SetTextScale, GUIStaticText::SetTextScale)
_CLASSREGISTER_AddMember(SetWordWrap, GUIStaticText::SetWordWrap)
//_CLASSREGISTER_AddMember(SetScrollEffect, GUIStaticText::SetScrollEffect)
//_CLASSREGISTER_AddMember(setNeedCastLuaString, GUIStaticText::setNeedCastLuaString)
_CLASSREGISTER_AddMember(GetTextStringHigh, GUIStaticText::GetTextStringHigh)
_CLASSREGISTER_AddMember(SetBordered, GUIStaticText::SetTextBoader)
_END_REGISTER_CLASS()

// Register Class GUITableView
_BEGIN_REGISTER_CLASS(GUITableView)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(AddCell, GUITableView::AddCell)
_CLASSREGISTER_AddMember(AddHeader, GUITableView::AddHeader)
_CLASSREGISTER_AddMember(ClearAllSections, GUITableView::ClearAllSections)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProperty, GUITableView::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUITableView::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUITableView::GetPropertyType)
_CLASSREGISTER_AddMember(GetSectionCount, GUITableView::GetSectionCount)
_CLASSREGISTER_AddMember(LayoutSections, GUITableView::LayoutSections)
_CLASSREGISTER_AddMember(MotionRelease, GUITableView::MotionRelease)
_CLASSREGISTER_AddMember(OnEndDraw, GUITableView::OnEndDraw)
_CLASSREGISTER_AddMember(SetIsRadio, GUITableView::SetIsRadio)
_CLASSREGISTER_AddMember(SetProperty, GUITableView::SetProperty)
_CLASSREGISTER_AddMember(SetSectionOpen, GUITableView::SetSectionOpen)
_CLASSREGISTER_AddMember(TouchDown, GUITableView::TouchDown)
_CLASSREGISTER_AddMember(TouchMove, GUITableView::TouchMove)
_CLASSREGISTER_AddMember(TouchUp, GUITableView::TouchUp)
_CLASSREGISTER_AddMember(UpdateSelf, GUITableView::UpdateSelf)
_END_REGISTER_CLASS()

// Register Class GuiActorWindow
_BEGIN_REGISTER_CLASS(GuiActorWindow)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(DrawSelf, GuiActorWindow::DrawSelf)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetProperty, GuiActorWindow::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GuiActorWindow::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GuiActorWindow::GetPropertyType)
_CLASSREGISTER_AddMember(SetProperty, GuiActorWindow::SetProperty)
_CLASSREGISTER_AddMember(UpdateSelf, GuiActorWindow::UpdateSelf)
_CLASSREGISTER_AddMember(UseBodyPart, GuiActorWindow::UseBodyPart)
_CLASSREGISTER_AddMember(UnloadBodyPart, GuiActorWindow::UnloadBodyPart)
_CLASSREGISTER_AddMember(TouchDown, GuiActorWindow::TouchDown)
_CLASSREGISTER_AddMember(TouchMove, GuiActorWindow::TouchMove)
_CLASSREGISTER_AddMember(SetActorScale, GuiActorWindow::SetActorScale)
_CLASSREGISTER_AddMember(PlaySkill, GuiActorWindow::PlaySkill)
_CLASSREGISTER_AddMember(SetRotateX, GuiActorWindow::SetRotateX)
_CLASSREGISTER_AddMember(SetRotateY, GuiActorWindow::SetRotateY)
_CLASSREGISTER_AddMember(SetRotateZ, GuiActorWindow::SetRotateZ)
_CLASSREGISTER_AddMember(GetRotateX, GuiActorWindow::GetRotateX)
_CLASSREGISTER_AddMember(GetRotateY, GuiActorWindow::GetRotateY)
_CLASSREGISTER_AddMember(GetRotateZ, GuiActorWindow::GetRotateZ)
_CLASSREGISTER_AddMember(GetActor, GuiActorWindow::GetActor)
_CLASSREGISTER_AddMember(reflreshArmor, GuiActorWindow::reflreshArmor)
_CLASSREGISTER_AddMember(RemoveActor, GuiActorWindow::RemoveActor)
_CLASSREGISTER_AddMember(SetCustomColor, GuiActorWindow::SetCustomColor)
_CLASSREGISTER_AddMember_Override(SetActor, GuiActorWindow::SetActor, void, ActorObject*)
_CLASSREGISTER_AddMember_Override(SetActor1, GuiActorWindow::SetActor, void, const GUIString &, const GUIString&, float)
_END_REGISTER_CLASS()

// Register Class GuiUrlImage
_BEGIN_REGISTER_CLASS(GuiUrlImage)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(setDefaultImage, GuiUrlImage::setDefaultImage)
_CLASSREGISTER_AddMember(setUrl, GuiUrlImage::setUrl)
_CLASSREGISTER_AddMember(setShowHttpIcon, GuiUrlImage::setShowHttpIcon)
_CLASSREGISTER_AddMember(setChildMaterial, GuiUrlImage::setChildMaterial)
_CLASSREGISTER_AddMember(setBorder, GuiUrlImage::setBorder)
_CLASSREGISTER_AddMember(setMaskTextureName, GuiUrlImage::setMaskTextureName)

_END_REGISTER_CLASS()

// Register Class GuiUrlImage
_BEGIN_REGISTER_CLASS(GuiEffectViewOfObtainment)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember_Override(SetImage, GuiEffectViewOfObtainment::SetImage, void, const GUIString &)
_CLASSREGISTER_AddMember_Override(SetImage1, GuiEffectViewOfObtainment::SetImage, void, const GUIString &, const GUIString &)
_CLASSREGISTER_AddMember_Override(SetActor, GuiEffectViewOfObtainment::SetActor, void, const GUIString &)
_CLASSREGISTER_AddMember_Override(SetActor1, GuiEffectViewOfObtainment::SetActor, void, const GUIString &, const GUIString &)
_CLASSREGISTER_AddMember_Override(SetActor2, GuiEffectViewOfObtainment::SetActor, void, const GUIString &, const GUIString &, float)
_CLASSREGISTER_AddMember_Override(SetActor3, GuiEffectViewOfObtainment::SetActor, void, const GUIString &, const GUIString &, float, const GUIString &)
_CLASSREGISTER_AddMember(SetSkillName, GuiEffectViewOfObtainment::SetSkillName)
_CLASSREGISTER_AddMember(UseBodyPart, GuiEffectViewOfObtainment::UseBodyPart)
_CLASSREGISTER_AddMember(SetActorPos, GuiEffectViewOfObtainment::SetActorPos)
_END_REGISTER_CLASS()

// Register Class GUIListHorizontalBox
_BEGIN_REGISTER_CLASS(GUIListHorizontalBox)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember_Override(AddItem, GUIListHorizontalBox::AddItem, void, GUIWindow*, bool)
_CLASSREGISTER_AddMember(ClearAllItem, GUIListHorizontalBox::ClearAllItem)
_CLASSREGISTER_AddMember(DeleteItem, GUIListHorizontalBox::DeleteItem)
_CLASSREGISTER_AddMember(RemoveItem, GUIListHorizontalBox::RemoveItem)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetItemWidth, GUIListHorizontalBox::GetItemWidth)
_CLASSREGISTER_AddMember(GetProperty, GUIListHorizontalBox::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIListHorizontalBox::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIListHorizontalBox::GetPropertyType)
_CLASSREGISTER_AddMember(IsBlinkSelect, GUIListHorizontalBox::IsBlinkSelect)
_CLASSREGISTER_AddMember(MotionRelease, GUIListHorizontalBox::MotionRelease)
_CLASSREGISTER_AddMember(OnEndDraw, GUIListHorizontalBox::OnEndDraw)
_CLASSREGISTER_AddMember(ResetScroll, GUIListHorizontalBox::ResetScroll)
_CLASSREGISTER_AddMember(SetAllowScroll, GUIListHorizontalBox::SetAllowScroll)
_CLASSREGISTER_AddMember(SetBlinkSelect, GUIListHorizontalBox::SetBlinkSelect)
_CLASSREGISTER_AddMember(SetInterval, GUIListHorizontalBox::SetInterval)
_CLASSREGISTER_AddMember(GetInterval, GUIListHorizontalBox::GetInterval)
_CLASSREGISTER_AddMember(SetItemWidth, GUIListHorizontalBox::SetItemWidth)
_CLASSREGISTER_AddMember(SetItemNormalImage, GUIListHorizontalBox::SetItemNormalImage)
_CLASSREGISTER_AddMember(SetItemSelectImage, GUIListHorizontalBox::SetItemSelectImage)
_CLASSREGISTER_AddMember(SetMoveAble, GUIListHorizontalBox::SetMoveAble)
_CLASSREGISTER_AddMember(SetProperty, GUIListHorizontalBox::SetProperty)
_CLASSREGISTER_AddMember(SetScrollOffset, GUIListHorizontalBox::SetScrollOffset)
_CLASSREGISTER_AddMember(SetSelectItem, GUIListHorizontalBox::SetSelectItem)
_CLASSREGISTER_AddMember(TouchDown, GUIListHorizontalBox::TouchDown)
_CLASSREGISTER_AddMember(TouchMove, GUIListHorizontalBox::TouchMove)
_CLASSREGISTER_AddMember(TouchUp, GUIListHorizontalBox::TouchUp)
_CLASSREGISTER_AddMember(UnSelectAll, GUIListHorizontalBox::UnSelectAll)
_CLASSREGISTER_AddMember(UpdateSelf, GUIListHorizontalBox::UpdateSelf)
_CLASSREGISTER_AddMember(getContainerWindow, GUIListHorizontalBox::getContainerWindow)
_CLASSREGISTER_AddMember(setContainerWindow, GUIListHorizontalBox::setContainerWindow)
_CLASSREGISTER_AddMember(GetScrollOffset, GUIListHorizontalBox::GetScrollOffset)
_CLASSREGISTER_AddMember(SetVirtualListOffset, GUIListHorizontalBox::SetVirtualListOffset)
_CLASSREGISTER_AddMember(GetVirtualListOffset, GUIListHorizontalBox::GetVirtualListOffset)
_CLASSREGISTER_AddMember(EnableVirtualList, GUIListHorizontalBox::EnableVirtualList)
_END_REGISTER_CLASS()

// Register Class GuiItemSlot
_BEGIN_REGISTER_CLASS(GuiItemSlot)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(SetProperty, GuiItemSlot::SetProperty)
_CLASSREGISTER_AddMember_Override(setStyle, GuiItemSlot::setStyle, void, int)
_CLASSREGISTER_AddMember(getWindow, GuiItemSlot::getWindow)
_END_REGISTER_CLASS()

// Register Class GuiSlotTable
_BEGIN_REGISTER_CLASS(GuiSlotTable)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(SetProperty, GuiSlotTable::SetProperty)
_CLASSREGISTER_AddMember(GetItemSlot, GuiSlotTable::GetItemSlot)
_END_REGISTER_CLASS()

// Register Class GuiScrollableInventory
_BEGIN_REGISTER_CLASS(GuiScrollableInventory)
_CLASSREGISTER_AddBaseClass(GUIWindow)
_CLASSREGISTER_AddMember(SetProperty, GuiScrollableInventory::SetProperty)
_CLASSREGISTER_AddMember(getInventory, GuiScrollableInventory::getInventory)
_CLASSREGISTER_AddMember(getItemStack, GuiScrollableInventory::getItemStack)
_CLASSREGISTER_AddMember(setStyle, GuiScrollableInventory::setStyle)
_CLASSREGISTER_AddMember(setShowCount, GuiScrollableInventory::setShowCount)
_CLASSREGISTER_AddMember(setRowSize, GuiScrollableInventory::setRowSize)
_CLASSREGISTER_AddMember(setAutoColumnCount, GuiScrollableInventory::setAutoColumnCount)
_CLASSREGISTER_AddMember(getGridView, GuiScrollableInventory::getGridView)
_END_REGISTER_CLASS()

// Register Class GUIWindow
_BEGIN_REGISTER_CLASS(GUIWindow)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddBaseClass(EventSet)
_CLASSREGISTER_AddMember_Override(AddChildWindow, GUIWindow::AddChildWindow, void, GUIWindow*)
_CLASSREGISTER_AddMember(AddWindowToDrawList, GUIWindow::AddWindowToDrawList)
_CLASSREGISTER_AddMember(ChangeAllChildToAbsolute, GUIWindow::ChangeAllChildToAbsolute)
_CLASSREGISTER_AddMember(ChangeAllChildToReletive, GUIWindow::ChangeAllChildToReletive)
_CLASSREGISTER_AddMember(CleanupChildren, GUIWindow::CleanupChildren)
_CLASSREGISTER_AddMember(Clone, GUIWindow::Clone)
//_CLASSREGISTER_AddMember(Destroy, GUIWindow::Destroy)
_CLASSREGISTER_AddMember(DrawBackground, GUIWindow::DrawBackground)
_CLASSREGISTER_AddMember(DrawSelf, GUIWindow::DrawSelf)
_CLASSREGISTER_AddMember(ExportJSONData, GUIWindow::ExportJSONData)
//_CLASSREGISTER_AddMember(ExportXMLData, GUIWindow::ExportXMLData)
_CLASSREGISTER_AddCtor(GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(GetAlpha, GUIWindow::GetAlpha)
_CLASSREGISTER_AddMember(GetArea, GUIWindow::GetArea)
_CLASSREGISTER_AddMember(GetBackgroundColor, GUIWindow::GetBackgroundColor)
_CLASSREGISTER_AddMember(GetChildAtPosition, GUIWindow::GetChildAtPosition)
_CLASSREGISTER_AddMember(GetChildByIndex, GUIWindow::GetChildByIndex)
_CLASSREGISTER_AddMember(GetChildByIndexInDrawlist, GUIWindow::GetChildByIndexInDrawlist)
_CLASSREGISTER_AddMember(GetChildCount, GUIWindow::GetChildCount)
_CLASSREGISTER_AddMember(GetClippedOuterRect, GUIWindow::GetClippedOuterRect)
_CLASSREGISTER_AddMember(GetDrawColor, GUIWindow::GetDrawColor)
_CLASSREGISTER_AddMember(GetDrawlistCount, GUIWindow::GetDrawlistCount)
_CLASSREGISTER_AddMember(GetFont, GUIWindow::GetFont)
_CLASSREGISTER_AddMember(GetHeight, GUIWindow::GetHeight)
_CLASSREGISTER_AddMember(GetHitTestRect, GUIWindow::GetHitTestRect)
_CLASSREGISTER_AddMember(GetHorizontalAlignment, GUIWindow::GetHorizontalAlignment)
_CLASSREGISTER_AddMember(GetInScroll, GUIWindow::GetInScroll)
_CLASSREGISTER_AddMember(GetLevel, GUIWindow::GetLevel)
_CLASSREGISTER_AddMember(GetLuaEventList, GUIWindow::GetLuaEventList)
_CLASSREGISTER_AddMember(GetName, GUIWindow::GetName)
_CLASSREGISTER_AddMember(GetOwnerDialogName, GUIWindow::GetOwnerDialogName)
_CLASSREGISTER_AddMember(GetParent, GUIWindow::GetParent)
_CLASSREGISTER_AddMember(GetPixelSize, GUIWindow::GetPixelSize)
_CLASSREGISTER_AddMember(GetPosition, GUIWindow::GetPosition)
_CLASSREGISTER_AddMember(GetProperty, GUIWindow::GetProperty)
_CLASSREGISTER_AddMember(GetPropertyList, GUIWindow::GetPropertyList)
_CLASSREGISTER_AddMember(GetPropertyType, GUIWindow::GetPropertyType)
_CLASSREGISTER_AddMember(GetRenderArea, GUIWindow::GetRenderArea)
_CLASSREGISTER_AddMember(GetRotate, GUIWindow::GetRotate)
_CLASSREGISTER_AddMember(GetRotateX, GUIWindow::GetRotateX)
_CLASSREGISTER_AddMember(GetRotateY, GUIWindow::GetRotateY)
_CLASSREGISTER_AddMember(GetScreenRenderArea, GUIWindow::GetScreenRenderArea)
_CLASSREGISTER_AddMember(GetTargetChildAtPosition, GUIWindow::GetTargetChildAtPosition)
_CLASSREGISTER_AddMember(GetText, GUIWindow::GetText)
_CLASSREGISTER_AddMember(GetTouchableParent, GUIWindow::GetTouchableParent)
_CLASSREGISTER_AddMember(GetType, GUIWindow::GetType)
_CLASSREGISTER_AddMember(GetUnclippedOuterRect, GUIWindow::GetUnclippedOuterRect)
_CLASSREGISTER_AddMember(GetUserData, GUIWindow::GetUserData)
_CLASSREGISTER_AddMember(GetUserData2, GUIWindow::GetUserData2)
_CLASSREGISTER_AddMember(GetVerticalAlignment, GUIWindow::GetVerticalAlignment)
_CLASSREGISTER_AddMember(GetWidth, GUIWindow::GetWidth)
_CLASSREGISTER_AddMember(GetXPosition, GUIWindow::GetXPosition)
_CLASSREGISTER_AddMember(GetYPosition, GUIWindow::GetYPosition)
_CLASSREGISTER_AddMember(ImportJSONData, GUIWindow::ImportJSONData)
_CLASSREGISTER_AddMember(ImportTemplateData, GUIWindow::ImportTemplateData)
//_CLASSREGISTER_AddMember(ImportXMLData, GUIWindow::ImportXMLData)
//_CLASSREGISTER_AddMember_Override(Invalidate, GUIWindow::Invalidate,void ,void)
_CLASSREGISTER_AddMember_Override(Invalidate1, GUIWindow::Invalidate, void, const bool)
_CLASSREGISTER_AddMember(IsActive, GUIWindow::IsActive)
_CLASSREGISTER_AddMember(IsAlwaysOnTop, GUIWindow::IsAlwaysOnTop)
_CLASSREGISTER_AddMember(IsAncestor, GUIWindow::IsAncestor)
_CLASSREGISTER_AddMember(IsAutoSize, GUIWindow::IsAutoSize)
_CLASSREGISTER_AddMember(IsClipChild, GUIWindow::IsClipChild)
_CLASSREGISTER_AddMember(IsEnabled, GUIWindow::IsEnabled)
_CLASSREGISTER_AddMember(IsHit, GUIWindow::IsHit)
_CLASSREGISTER_AddMember(IsMultiClicked, GUIWindow::IsMultiClicked)
_CLASSREGISTER_AddMember(IsOutputLuaVariables, GUIWindow::IsOutputLuaVariables)
_CLASSREGISTER_AddMember(IsTouchSlide, GUIWindow::IsTouchSlide)
_CLASSREGISTER_AddMember(IsTouchable, GUIWindow::IsTouchable)
_CLASSREGISTER_AddMember(IsVisible, GUIWindow::IsVisible)
_CLASSREGISTER_AddMember(MotionRelease, GUIWindow::MotionRelease)
_CLASSREGISTER_AddMember(MoveBack, GUIWindow::MoveBack)
_CLASSREGISTER_AddMember(MoveFront, GUIWindow::MoveFront)
_CLASSREGISTER_AddMember(NotifyScreenAreaChanged, GUIWindow::NotifyScreenAreaChanged)
_CLASSREGISTER_AddMember(OnBeforeDraw, GUIWindow::OnBeforeDraw)
_CLASSREGISTER_AddMember(OnChildAdd, GUIWindow::OnChildAdd)
_CLASSREGISTER_AddMember(OnChildRemove, GUIWindow::OnChildRemove)
_CLASSREGISTER_AddMember(OnEndDraw, GUIWindow::OnEndDraw)
_CLASSREGISTER_AddMember(OnFontChanged, GUIWindow::OnFontChanged)
_CLASSREGISTER_AddMember(OnMoved, GUIWindow::OnMoved)
_CLASSREGISTER_AddMember(OnParentSized, GUIWindow::OnParentSized)
_CLASSREGISTER_AddMember(OnSized, GUIWindow::OnSized)
_CLASSREGISTER_AddMember(OnTextChanged, GUIWindow::OnTextChanged)
_CLASSREGISTER_AddMember_Override(PlayEffect, GUIWindow::PlayEffect, float)
_CLASSREGISTER_AddMember_Override(PlayEffect1, GUIWindow::PlayEffect, float, const GUIString &)
_CLASSREGISTER_AddMember(PlaySoundForWndClose, GUIWindow::PlaySoundForWndClose)
_CLASSREGISTER_AddMember(PlaySoundForWndOpen, GUIWindow::PlaySoundForWndOpen)
_CLASSREGISTER_AddMember(PrepareEffect, GUIWindow::PrepareEffect)
_CLASSREGISTER_AddMember_Override(RemoveChildWindow, GUIWindow::RemoveChildWindow, void, const GUIString &)
_CLASSREGISTER_AddMember_Override(RemoveChildWindow1, GUIWindow::RemoveChildWindow, void, GUIWindow *)
_CLASSREGISTER_AddMember(RemoveWindowFromDrawList, GUIWindow::RemoveWindowFromDrawList)
_CLASSREGISTER_AddMember(Render, GUIWindow::Render)
_CLASSREGISTER_AddMember(SetActionMatrixScale, GUIWindow::SetActionMatrixScale)
_CLASSREGISTER_AddMember(setMaterial, GUIWindow::setMaterial)
_CLASSREGISTER_AddMember(SetActive, GUIWindow::SetActive)
_CLASSREGISTER_AddMember(SetAlpha, GUIWindow::SetAlpha)
_CLASSREGISTER_AddMember(SetAlwaysOnTop, GUIWindow::SetAlwaysOnTop)
_CLASSREGISTER_AddMember_Override(SetArea, GUIWindow::SetArea, void, const UDim &, const UDim &, const UDim &, const UDim &)
_CLASSREGISTER_AddMember_Override(SetArea1, GUIWindow::SetArea, void, const UVector2 &, const UVector2 &)
_CLASSREGISTER_AddMember_Override(SetArea2, GUIWindow::SetArea, void, const URect &)
_CLASSREGISTER_AddMember(SetAutoSize, GUIWindow::SetAutoSize)
_CLASSREGISTER_AddMember(SetBackgroundColor, GUIWindow::SetBackgroundColor)
_CLASSREGISTER_AddMember(SetClipChild, GUIWindow::SetClipChild)
_CLASSREGISTER_AddMember(SetDrawColor, GUIWindow::SetDrawColor)
_CLASSREGISTER_AddMember(SetEffectName, GUIWindow::SetEffectName)
_CLASSREGISTER_AddMember(SetEnabled, GUIWindow::SetEnabled)
_CLASSREGISTER_AddMember(SetFont, GUIWindow::SetFont)
_CLASSREGISTER_AddMember(SetHeight, GUIWindow::SetHeight)
_CLASSREGISTER_AddMember_Override(SetHorizontalAlignment, GUIWindow::SetHorizontalAlignment, void, HorizontalAlignment)
_CLASSREGISTER_AddMember(SetLevel, GUIWindow::SetLevel)
_CLASSREGISTER_AddMember(SetMultiClicked, GUIWindow::SetMultiClicked)
_CLASSREGISTER_AddMember(SetParent, GUIWindow::SetParent)
_CLASSREGISTER_AddMember(SetPosition, GUIWindow::SetPosition)
_CLASSREGISTER_AddMember(SetProperty, GUIWindow::SetProperty)
_CLASSREGISTER_AddMember(SetRotate, GUIWindow::SetRotate)
_CLASSREGISTER_AddMember(SetRotateX, GUIWindow::SetRotateX)
_CLASSREGISTER_AddMember(SetRotateY, GUIWindow::SetRotateY)
_CLASSREGISTER_AddMember(SetScale, GUIWindow::SetScale)
_CLASSREGISTER_AddMember(SetSize, GUIWindow::SetSize)
_CLASSREGISTER_AddMember(SetText, GUIWindow::SetText)
_CLASSREGISTER_AddMember(SetThisWindowDialog, GUIWindow::SetThisWindowDialog)
_CLASSREGISTER_AddMember(SetTouchSlide, GUIWindow::SetTouchSlide)
_CLASSREGISTER_AddMember(SetTouchable, GUIWindow::SetTouchable)
_CLASSREGISTER_AddMember(SetUserData, GUIWindow::SetUserData)
_CLASSREGISTER_AddMember(SetUserData2, GUIWindow::SetUserData2)
_CLASSREGISTER_AddMember_Override(SetVerticalAlignment, GUIWindow::SetVerticalAlignment, void, VerticalAlignment)
_CLASSREGISTER_AddMember(SetVisible, GUIWindow::SetVisible)
_CLASSREGISTER_AddMember(SetWidth, GUIWindow::SetWidth)
_CLASSREGISTER_AddMember(SetXPosition, GUIWindow::SetXPosition)
_CLASSREGISTER_AddMember(SetYPosition, GUIWindow::SetYPosition)
_CLASSREGISTER_AddMember(ShowOnTop, GUIWindow::ShowOnTop)
_CLASSREGISTER_AddMember(ToAbsolute, GUIWindow::ToAbsolute)
_CLASSREGISTER_AddMember(ToReletive, GUIWindow::ToReletive)
_CLASSREGISTER_AddMember(TouchDown, GUIWindow::TouchDown)
_CLASSREGISTER_AddMember(TouchMove, GUIWindow::TouchMove)
_CLASSREGISTER_AddMember(TouchUp, GUIWindow::TouchUp)
_CLASSREGISTER_AddMember(UnprepareEffect, GUIWindow::UnprepareEffect)
_CLASSREGISTER_AddMember(Update, GUIWindow::Update)
_CLASSREGISTER_AddMember(UpdateSelf, GUIWindow::UpdateSelf)
_CLASSREGISTER_AddMember(endAction, GUIWindow::endAction)
_CLASSREGISTER_AddMember(getAction, GUIWindow::getAction)
_CLASSREGISTER_AddMember(getActionMatrix, GUIWindow::getActionMatrix)
_CLASSREGISTER_AddMember(getMaskTexture, GUIWindow::getMaskTexture)
_CLASSREGISTER_AddMember(getMaskTextureName, GUIWindow::getMaskTextureName)
_CLASSREGISTER_AddMember(getMaterial, GUIWindow::getMaterial)
_CLASSREGISTER_AddMember(getStringUserData, GUIWindow::getStringUserData)
_CLASSREGISTER_AddMember(isDragging, GUIWindow::isDragging)
_CLASSREGISTER_AddMember(isEnableDrag, GUIWindow::isEnableDrag)
_CLASSREGISTER_AddMember(isEnableLongTouch, GUIWindow::isEnableLongTouch)
_CLASSREGISTER_AddMember(isPlayingAction, GUIWindow::isPlayingAction)
_CLASSREGISTER_AddMember(onDragEnd, GUIWindow::onDragEnd)
_CLASSREGISTER_AddMember(onDragging, GUIWindow::onDragging)
_CLASSREGISTER_AddMember(onLongTouchEnd, GUIWindow::onLongTouchEnd)
_CLASSREGISTER_AddMember(onLongTouchStart, GUIWindow::onLongTouchStart)
_CLASSREGISTER_AddMember(playAction, GUIWindow::playAction)
_CLASSREGISTER_AddMember(setChildPlayAction, GUIWindow::setChildPlayAction)
_CLASSREGISTER_AddMember(setDragging, GUIWindow::setDragging)
_CLASSREGISTER_AddMember(setEnableDrag, GUIWindow::setEnableDrag)
_CLASSREGISTER_AddMember(setEnableLongTouch, GUIWindow::setEnableLongTouch)
_CLASSREGISTER_AddMember(setMaskTextureName, GUIWindow::setMaskTextureName)
_CLASSREGISTER_AddMember(setStringUserData, GUIWindow::setStringUserData)
_CLASSREGISTER_AddMember(Rename, GUIWindow::Rename)
//_CLASSREGISTER_AddMember(registerEvent, GUIWindow::registerEvent)
//_CLASSREGISTER_AddMember(registerMouseEvent, GUIWindow::registerMouseEvent)
//_CLASSREGISTER_AddMember(registerScrollEvent, GUIWindow::registerScrollEvent)
_CLASSREGISTER_AddMember(removeAllEvents, GUIWindow::removeAllEvents)
_CLASSREGISTER_AddMember(removeEvent, GUIWindow::removeEvent)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(gui_layout)
_CLASSREGISTER_AddMember(getRootWindow, gui_layout::getRootWindow)
_END_REGISTER_CLASS()


_BEGIN_REGISTER_CLASS(RootGuiLayout)
_CLASSREGISTER_AddBaseClass(gui_layout)
_CLASSREGISTER_AddStaticMember(Instance, RootGuiLayout::Instance)
_CLASSREGISTER_AddMember(getLayout, RootGuiLayout::getLayout)
_CLASSREGISTER_AddMember(isLayoutShow, RootGuiLayout::isLayoutShow)
_CLASSREGISTER_AddMember(showMainControl, RootGuiLayout::showMainControl)
_CLASSREGISTER_AddMember(hideAllChildren, RootGuiLayout::hideAllChildren)
_CLASSREGISTER_AddMember(showWindowByName, RootGuiLayout::showWindowByName)
_CLASSREGISTER_AddMember(hideWindowByName, RootGuiLayout::hideWindowByName)
_CLASSREGISTER_AddMember(showAppShop, RootGuiLayout::showAppShop)
_CLASSREGISTER_AddMember(showMenu, RootGuiLayout::showMenu)
_CLASSREGISTER_AddMember(showSkyBlocReciveNewPri, RootGuiLayout::showSkyBlocReciveNewPri)
_CLASSREGISTER_AddMember(showSkyBlockSignIn, RootGuiLayout::showSkyBlockSignIn)
_CLASSREGISTER_AddMember(armorPanelSellItem, RootGuiLayout::armorPanelSellItem)
_CLASSREGISTER_AddMember(againDropItem, RootGuiLayout::againDropItem)
_CLASSREGISTER_AddMember(getViewNum, RootGuiLayout::getViewNum)
_CLASSREGISTER_AddMember(showSkyBlockAppShopPage, RootGuiLayout::showSkyBlockAppShopPage)
_END_REGISTER_CLASS()


// Register Class GUIWindowFactory
_BEGIN_REGISTER_CLASS(GUIWindowFactory)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddStaticMember(Instance, GUIWindowFactory::Instance)
_CLASSREGISTER_AddMember(create, GUIWindowFactory::create)
_CLASSREGISTER_AddMember(registerWindowType, GUIWindowFactory::registerWindowType)
_CLASSREGISTER_AddMember(registry, GUIWindowFactory::registry)
_END_REGISTER_CLASS()


// Register Class GUIWindowManager
_BEGIN_REGISTER_CLASS(GUIWindowManager)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(CleanDeadPool, GUIWindowManager::CleanDeadPool)
_CLASSREGISTER_AddMember(CloneWindow, GUIWindowManager::CloneWindow)
_CLASSREGISTER_AddMember_Override(CreateGUIWindow, GUIWindowManager::CreateGUIWindow, GUIWindow *, const GUIString &, const GUIString &)
_CLASSREGISTER_AddMember_Override(CreateGUIWindow1, GUIWindowManager::CreateGUIWindow, GUIWindow *, GUIWindowType, const GUIString &)
_CLASSREGISTER_AddMember(CreateWindowFromTemplate, GUIWindowManager::CreateWindowFromTemplate)
_CLASSREGISTER_AddMember(GetGUIWindow, GUIWindowManager::GetGUIWindow)
_CLASSREGISTER_AddMember(DestroyAllGUIWindows, GUIWindowManager::DestroyAllGUIWindows)
_CLASSREGISTER_AddMember_Override(DestroyGUIWindow, GUIWindowManager::DestroyGUIWindow, void, GUIWindow*)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddStaticMember(Instance, GUIWindowManager::Instance)
_CLASSREGISTER_AddMember(LoadWindowFromJSON, GUIWindowManager::LoadWindowFromJSON)
_CLASSREGISTER_AddMember(WriteJSON, GUIWindowManager::WriteJSON)
_END_REGISTER_CLASS()

// Register Class GUISystem
_BEGIN_REGISTER_CLASS(GUISystem)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddStaticMember(Instance, GUISystem::Instance)
_CLASSREGISTER_AddMember(GetRootWindow, GUISystem::GetRootWindow)
_CLASSREGISTER_AddMember(GetScreenWidth, GUISystem::GetScreenWidth)
_CLASSREGISTER_AddMember(GetScreenHeight, GUISystem::GetScreenHeight)
_END_REGISTER_CLASS()

// Register Class GuiItemStack
_BEGIN_REGISTER_CLASS(GuiItemStack)
_CLASSREGISTER_AddBaseClass(CustomGuiWindow)
_CLASSREGISTER_AddStaticMember(showItemIcon, GuiItemStack::showItemIcon)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(gui_gameSetting)
_CLASSREGISTER_AddBaseClass(gui_layout)
_CLASSREGISTER_AddMember(refreshPoleControlState, gui_gameSetting::refreshPoleControlState)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(gui_menu)
_CLASSREGISTER_AddBaseClass(gui_layout)
_CLASSREGISTER_AddMember(getRadioPanel, gui_menu::getRadioPanel)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(gui_mainControl)
_CLASSREGISTER_AddBaseClass(gui_layout)
_CLASSREGISTER_AddMember(updateCameraDistance, gui_mainControl::updateCameraDistance)
_CLASSREGISTER_AddMember(onCancelAim, gui_mainControl::onCancelAim)
_CLASSREGISTER_AddMember(addChatMessage, gui_mainControl::addChatMessage)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(gui_chat)
_CLASSREGISTER_AddBaseClass(gui_layout)
_CLASSREGISTER_AddMember(addChatMessage, gui_chat::addChatMessage)
_END_REGISTER_CLASS()

#endif