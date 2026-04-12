-- @Author: tanjianyong
-- @Date:   2019-12-13 15:55:33
-- @Last Modified by:   tanjianyong
-- @Last Modified time: 2019-12-13 15:57:13

local UIChristmastree = class("UIChristmastree", IGUILayout)

function UIChristmastree:onLoad()
	self.EntityId = 0

	self.btnTreeCancel = self:getChildWindow("SkyBlockChristmastree-Cancel-Btn", GUIType.Button)
    self.btnTreeCancel:registerEvent(GUIEvent.ButtonClick, function()
    	SoundUtil.clickUI()
    	self:onUIHide()
    end)

	self.btnTreeRecycle = self:getChildWindow("SkyBlockChristmastree-Recycle-Btn", GUIType.Button)
    self.btnTreeRecycle:registerEvent(GUIEvent.ButtonClick, function()
    	SoundUtil.clickUI()
    	self:onRecycle()
    	self:onUIHide()
    end)

	self.stTreeRecycle = self:getChildWindow("SkyBlockChristmastree-Recycle-Text", GUIType.StaticText)
	self.stTreeRecycle:SetText(Lang:getString("gui_skyblock_Christmastree_recycle"))

    self:registerDataCallBack("showChristmastreeUI", function(data)
    	self:onUIShowChristmastree(data)
    end, JsonBuilderProcessor)
end

function UIChristmastree:onRecycle()
	if self.EntityId ~= 0 then
	    local data = {
	        EntityId = self.EntityId
	    }
		GamePacketSender:sendRecycleChristmastree(data)
	end
end

function UIChristmastree:onUIShow(data)
    if data and data.EntityId then
        self.EntityId = data.EntityId
        self:show()
    end
end

function UIChristmastree:onUIHide()
	self:hide()
end

function UIChristmastree:onUIShowChristmastree(data)
    self:onUIShow(data)
end

GUIChristmastree = UIChristmastree.new("SkyBlockChristmastree.json")
GUIChristmastree:hide()