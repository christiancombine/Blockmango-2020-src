local UISign = class("UISign", IGUILayout)

function UISign:onLoad()
	print("UISign:onLoad()")

	self.title = self:getChildWindow("SkyBlockSign-Title-Name")
	self.title:SetText(Lang:getString("gui_skyblocksign_title_name"))

	self.closeBtn = self:getChildWindow("SkyBlockSign-Close-Btn", GUIType.Button)
    self.closeBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickClose()
	end)

	self.applyBtn = self:getChildWindow("SkyBlockSign-Apply-Btn", GUIType.Button)
	self.applyBtn:SetText(Lang:getString("gui_skyblocksign_apply_btn"))
    self.applyBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickApply()
	end)

	self.editContent = self:getChildWindow("SkyBlockSign-Content-Text")
	self.editContent:SetText(Lang:getString("gui_skyblocksign_content_text"))
	self.editDefaultContent = self.editContent:GetText()
	self.edit = self:getChildWindow("SkyBlockSign-Content-Edit", GUIType.Edit)
    self.edit:registerEvent(GUIEvent.EditTextInput, function()
		self:onClickEdit()
	end)

    self:registerDataCallBack("EditSignPost", function(data)
        self:onShow(data)
    end, JsonBuilderProcessor)

end

function UISign:onClickClose()
	self:hide()
end

function UISign:onClickApply()
	self:onSendEdit()
	self:hide()
end

function UISign:onClickEdit()
	local text = self.edit:GetText()
	if text ~= "" then
		self.editContent:SetText(text)
	end
end

function UISign:onSendEdit()
	local editText = self.editContent:GetText()
	if editText == "" then
	 	HostApi.log("onSendEdit : input text is empty")
	 	return
	end
	if self.cur_pos then
		local data = {
			editText = editText,
			cur_pos = self.cur_pos
		}
		GamePacketSender:sendApplySignPost(json.encode(data))
	end
end

function UISign:onShow(data)
 	if data then
	    local tab = json.decode(data)
 		self.cur_pos = tostring(tab.hashVec3i)
 		local old_text = tostring(tab.oldText)
 		if old_text == "" then
			self.editContent:SetText(self.editDefaultContent)
			self.edit:SetText("")
		else
 			self.editContent:SetText(old_text)
 			self.edit:SetText(old_text)
 		end
 		self:show()
 	end
end

GUISign = UISign.new("SkyBlockSign.json")
GUISign:hide()
