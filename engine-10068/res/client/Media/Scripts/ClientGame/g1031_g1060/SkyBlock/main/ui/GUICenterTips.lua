local UICenterTips = class("UICenterTips", IGUILayout)

function UICenterTips:onLoad()
	self.tvtipsText = self:getChildWindow("SkyBlockCenterTips-Tip-Text")
	self.llClosePanel = self:getChildWindow("SkyBlockCenterTips-Close-Panel", GUIType.Layout) 
	self.llClosePanel:registerEvent(GUIEvent.TouchDown, function()
        self:hidePanel()
    end)

	self.showTime = 0
	self.textMsg = ""
	LuaTimer:schedule(function(userId)
		self:upTimeTextTick()
	end, 1000, 1000)
end

function UICenterTips:upTimeTextTick()
	if self.showTime > 0 then
		self.showTime = self.showTime - 1
		if self.showTime <= 0 then
			self:hidePanel()
		end
	end
end

function UICenterTips:setTextandTime(msg, time)
	self.showTime = time or 2
	self.textMsg = msg or ""
	self.tvtipsText:SetText(self.textMsg)
	self:show()
end

function UICenterTips:hidePanel()
	self:hide()
end

GUICenterTips = UICenterTips.new("SkyBlockCenterTips.json")
GUICenterTips:hide()