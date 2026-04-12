local UIPlayerOperation = class("UIPlayerOperation", IGUILayout)

UIPlayerOperation.ClickType = 
{
	ADD_FRIEND = 1,
	VISIT_FRIEND = 2,
	BUILD_FRIEND = 3,
	EXPEL = 4,
	CLOSE = 5
}

function UIPlayerOperation:onLoad()
	print("UIPlayerOperation:onLoad()")
	self.stAddFriend = self:getChildWindow("SkyBlockPlayerOperation-BlockFort-AddFriend-Name")
	self.stAddFriend:SetText(Lang:getString("gui_player_operation_add"))
	self.btnAddFriend = self:getChildWindow("SkyBlockPlayerOperation-BlockFort-AddFriend", GUIType.Button)
    self.btnAddFriend:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickClose(UIPlayerOperation.ClickType.ADD_FRIEND)
	end)

	self.stVisitFriend = self:getChildWindow("SkyBlockPlayerOperation-BlockFort-Visit-Name")
	self.stVisitFriend:SetText(Lang:getString("gui_player_operation_visit"))
	self.btnVisitFriend = self:getChildWindow("SkyBlockPlayerOperation-BlockFort-Visit", GUIType.Button)
    self.btnVisitFriend:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickClose(UIPlayerOperation.ClickType.VISIT_FRIEND)
	end)

	self.stBuildFriend = self:getChildWindow("SkyBlockPlayerOperation-BlockFort-Invite-Name")
	self.stBuildFriend:SetText(Lang:getString("gui_player_operation_add_build"))
	self.btnBuildFriend = self:getChildWindow("SkyBlockPlayerOperation-BlockFort-Invite", GUIType.Button)
    self.btnBuildFriend:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickClose(UIPlayerOperation.ClickType.BUILD_FRIEND)
	end)

	self.stExpel = self:getChildWindow("SkyBlockPlayerOperation-BlockFort-MoveOut-Manor-Name")
	self.stExpel:SetText(Lang:getString("gui_player_operation_expel"))
	self.btnExpel = self:getChildWindow("SkyBlockPlayerOperation-BlockFort-MoveOut-Manor", GUIType.Button)
    self.btnExpel:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickClose(UIPlayerOperation.ClickType.EXPEL)
	end)
    self.btnExpel:SetVisible(false)

	self.llClose = self:getChildWindow("SkyBlockPlayerOperation-Close-Btn", GUIType.Layout)
    self.llClose:registerEvent(GUIEvent.TouchUp, function()
    	self:onClickClose(UIPlayerOperation.ClickType.CLOSE)
	end)

	self:registerDataCallBack("SkyBlockFriendOperation", function(data)
        self:onFriendOperation(data)
    end, JsonBuilderProcessor)

	self.targetUserId = 0
	self.name = ""
	self.isShowExpel = false
end

function UIPlayerOperation:onFriendOperation(data)
	if data then
		GUIPlayerOperation:show()
		self.targetUserId = data.targetUserId or 0
		self.name = data.name or ""
		self.isShowExpel = data.isShowExpel or false
    	self.btnExpel:SetVisible(self.isShowExpel)
	end
end

function UIPlayerOperation:onClickClose(Clicktype)
	if Clicktype == UIPlayerOperation.ClickType.ADD_FRIEND then
		MultiBuildData:postAddFriend(self.targetUserId)
	end	

	if Clicktype == UIPlayerOperation.ClickType.VISIT_FRIEND then
    	PacketSender:sendCallOnManor(self.targetUserId, 3, true, 0)
	end	

	if Clicktype == UIPlayerOperation.ClickType.BUILD_FRIEND then
		if not MultiBuildData:checkCanAddBuildFriend(self.targetUserId) then
	        GUICenterTips:setTextandTime(Lang:getString("is_build_frient_already"))
		else
			MultiBuildData:postInvitationOther(GameData.userId, self.targetUserId)
    	end
	end	

	if Clicktype == UIPlayerOperation.ClickType.EXPEL then
		local data = {}
		if MultiBuildData:checkIsBuildMember(self.targetUserId) then
		    local tips = string.format(Lang:getString("are_you_sure_expel"), tostring(self.name)) 
		    GUITipDialog:onShowTipDialog(tips, GUITipDialog.TipType.COMMON, "", function(isSure)
		            if isSure then
		                MultiBuildData:postKickOut(GameData.userId, self.targetUserId , SkyBlockWeb.KickOutType.MASTER_KICK_OUT, true)
		            end
		        end)
		else
		    local msg = string.format(Lang:getString("other_expel_you"), GameData.userName)
			GamePacketSender:kickOutPlayer(self.targetUserId, msg)
		end
	end	
	self:hide()
end

GUIPlayerOperation = UIPlayerOperation.new("SkyBlockPlayerOperation.json")
GUIPlayerOperation:hide()