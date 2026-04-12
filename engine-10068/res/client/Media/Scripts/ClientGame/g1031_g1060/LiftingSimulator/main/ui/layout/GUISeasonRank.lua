local UISeasonRank = class("UISeasonRank", IGUILayout)

function UISeasonRank:onLoad()
	self:onInitData()
	self:onLoadInitUI()
	self:onLoadEvent()
end

function UISeasonRank:onInitData()
	self.rankData = {}
	self.myRankData = {}
	self.getRankCd = 10
	self.nextGetRankTime = os.time()
	self.goToCd = 10
	self.nextGotoTime = os.time()
	-- LogUtil.log("UISeasonRank:onInitData")
end

function UISeasonRank:onLoadInitUI()
	self.closeBtn = self:getChildWindow("LiftingSimulatorSeasonRank-BtnClose", GUIType.Button)
	self.questionBtn = self:getChildWindow("LiftingSimulatorSeasonRank-WenHaoBtn", GUIType.Button)
	self.btGo = self:getChildWindow("LiftingSimulatorSeasonRank-GoBtn", GUIType.Button)
	self.btGo:SetText(Lang:getString("seasonRankBtnGo"))

	self.llRankList = self:getChildWindow("LiftingSimulatorSeasonRank-Content-ItemLayout", GUIType.Layout)
	self.ltRankList = IGUIListView.new("LiftingSimulator-Rank-View-List", self.llRankList)
	self.ltRankList:setItemSpace(2)

	self.stTitleRank = self:getChildWindow("LiftingSimulatorSeasonRank-TitleRank", GUIType.StaticText)
	self.stTitleRank:SetText(Lang:getString("LiftingSimulatorSeasonRank.season.title.rank"))

	self.stTitleID = self:getChildWindow("LiftingSimulatorSeasonRank-TitleID", GUIType.StaticText)
	self.stTitleID:SetText(Lang:getString("LiftingSimulatorSeasonRank.season.title.id"))

	self.stTitleKill = self:getChildWindow("LiftingSimulatorSeasonRank-TitleKill", GUIType.StaticText)
	self.stTitleKill:SetText(Lang:getString("LiftingSimulatorSeasonRank.season.title.kill"))

	self.stTitleScore = self:getChildWindow("LiftingSimulatorSeasonRank-TitleScore", GUIType.StaticText)
	self.stTitleScore:SetText(Lang:getString("LiftingSimulatorSeasonRank.season.title.score"))

	self.stTitleLv = self:getChildWindow("LiftingSimulatorSeasonRank-TitleLv", GUIType.StaticText)
	self.stTitleLv:SetText(Lang:getString("LiftingSimulatorSeasonRank.season.title.lv"))

	self.stMyRank = self:getChildWindow("LiftingSimulatorSeasonRank-MyDataRank", GUIType.StaticText)
	self.stMyId = self:getChildWindow("LiftingSimulatorSeasonRank-MyDataID", GUIType.StaticText)
	self.stMyKill = self:getChildWindow("LiftingSimulatorSeasonRank-MyDataKill", GUIType.StaticText)
	self.stMyScore = self:getChildWindow("LiftingSimulatorSeasonRank-MyDataScore", GUIType.StaticText)
	self.stMyLv = self:getChildWindow("LiftingSimulatorSeasonRank-MyDataLv", GUIType.StaticText)
	self.lvDes = self:getChildWindow("LiftingSimulatorSeasonRank-LvDes", GUIType.StaticText)
	self.siTitle = self:getChildWindow("LiftingSimulatorSeasonRank-TitleBg", GUIType.StaticImage) 
	self.siBg = self:getChildWindow("LiftingSimulatorSeasonRank-Bg", GUIType.StaticImage) 
	self.siMyRankBg = self:getChildWindow("LiftingSimulatorSeasonRank-MyDataBg", GUIType.StaticImage)

	self.tokenShopBtn = self:getChildWindow("LiftingSimulatorSeasonRank-ShopBtn", GUIType.Button)
	self.tokenShopBtn:SetText(Lang:getString("seasonRankBtnShop"))
end

function UISeasonRank:onLoadEvent()
	self:registerEvent(ToolBarSeasonRankClickEvent, function ()
		self:onToolBarOpenSeasonRank()
	end)

	self.closeBtn:registerEvent(GUIEvent.ButtonClick, function()
		self:onClickCloseButton()
	end)

	self.questionBtn:registerEvent(GUIEvent.ButtonClick, function()
		self:onClickQuestionButton()
	end)

	self.btGo:registerEvent(GUIEvent.ButtonClick, function()
		self:onClickGoToChampionship()
	end)


	self.tokenShopBtn:registerEvent(GUIEvent.ButtonClick, function()
		self:onClicktokenShopButton()
	end)
end

function UISeasonRank:onToolBarOpenSeasonRank()
	RootGuiLayout.Instance():showMainControl()
	self:show()
end

function UISeasonRank:onClickCloseButton()
	self:hide()
end

function UISeasonRank:onClickQuestionButton()
	SeasonRankTipOpenEvent:invoke()
end

function UISeasonRank:onClickGoToChampionship()
	local player = PlayerManager:getClientPlayer()
	if player.property.isAdvancing 
		or player.property.advancedLevel < GameConfig.btnChampionShipEnableLevel
		or player.property.muscleMass < GameConfig.btnChampionShipEnableMuscle then
		return
	end

	if os.time() > self.nextGotoTime then
		local data = {}
		local builder = DataBuilder.new():fromTable(data):getData()
		PacketSender:sendLuaCommonData("GoToChampionshipClick", builder)
		self.nextGotoTime = os.time() + self.goToCd
		LogUtil.log("onClickGoToChampionship ")
	end
end

function UISeasonRank:onClicktokenShopButton()
	GUITokenShop:showShop()
end

function UISeasonRank:isMainLayout()
    return false
end

function UISeasonRank:onShow()
	self:checkNeedUpdateDataFromWeb()
	self:updateGridView()
	self:updateMyRankData()
	self:updateGoBtn()
end

function UISeasonRank:onTick(frameTime)
	self:checkNeedUpdateDataFromWeb()
end

function UISeasonRank:updateGridView()
	if self.rankData == nil
			or self.rankData.list == nil then
		return
	end

	if not self.rankData.update then
		return
	end

	self.ltRankList.root:SetScrollOffset(0)
	self.ltRankList:clearItems()

	LogUtil.log("UISeasonRank:updateGridView ", #self.rankData)

	for _, data in pairs(self.rankData.list) do
		LogUtil.log("UISeasonRankItem " .. data.rank .. " " .. data.name .. " " .. data.kill .. " " .. data.score .. " " .. data.lv)
		local curItem = GUISeasonRankItem.new("LiftingSimulatorSeasonRankItem.json")
		curItem.root:SetArea({ 0, 0 }, { 0, 0 }, { 1, 0 }, { 0, 29 })
		curItem.root:SetHorizontalAlignment(HorizontalAlignment.Center)
		curItem:setItemData(data)
		self.ltRankList:addItem(curItem)
	end
	self.rankData.update = false
end

function UISeasonRank:updateMyRankData()
	if self.myRankData == nil or self.myRankData.data == nil then
		return
	end
	if not self.myRankData.update then
		return
	end

	LogUtil.log("UISeasonRank:updateMyRankData id : " .. tostring(self.myRankData.data.id) .. " rank: "
		.. self.myRankData.data.rank .. " kill: " .. self.myRankData.data.kill .. " score: " .. self.myRankData.data.score
		.. " lv: " .. self.myRankData.data.lv)

	self.stMyRank:SetText(self.myRankData.data.rank)
	self.stMyId:SetText(self.myRankData.data.id)
	self.stMyKill:SetText(self.myRankData.data.kill)
	self.stMyScore:SetText(self.myRankData.data.score)
	self.stMyLv:SetText(self.myRankData.data.lv)

	local lvConfig = self:getLvConfigByLv(self.myRankData.data.lv)
	if lvConfig then
		self.lvDes:SetText(Lang:getString(tostring(lvConfig.seasonRankDes)))
	end

	local duan = self:getDuanConfigByLv(self.myRankData.data.lv)
	if duan > 0 then
		self.siTitle:SetImage("set:LiftingSeasonRank.json image:duan_title" .. duan)
		self.siBg:SetImage("set:LiftingSeasonRank.json image:duan_bg" .. duan)
		self.siMyRankBg:SetImage("set:LiftingSeasonRank.json image:duan_mydatabg" .. duan)
	end

	self.myRankData.update = false
end

function UISeasonRank:updateGoBtn()
	local player = PlayerManager:getClientPlayer()
	if player.property.muscleMass < GameConfig.btnChampionShipEnableMuscle then
		self.btGo:SetText(Lang:getString("seasonRankBtnGoLimit"))
		self.btGo:setGray(true)
	else
		self.btGo:SetText(Lang:getString("seasonRankBtnGo"))
		self.btGo:setGray(false)
	end
end

function UISeasonRank:getLvConfigByLv(lv)
    local lvConfig = GameConfig.championshipsLvConfig
    for _, info in pairs(lvConfig) do
        if tonumber(lv) >= tonumber(info.lvDown) and tonumber(lv) <= tonumber(info.lvUp) then
            return info
        end
    end
    return nil
end

function UISeasonRank:getDuanConfigByLv(lv)
    local lvConfig = GameConfig.championshipsLvConfig

    for i = 1, #lvConfig do
        if tonumber(lv) >= tonumber(lvConfig[i].lvDown) and tonumber(lv) <= tonumber(lvConfig[i].lvUp) then
            return i
        end
    end
    return 0
end

function UISeasonRank:checkNeedUpdateDataFromWeb()
	if os.time() >= self.nextGetRankTime then
		self:getDataFromWeb()
		self.nextGetRankTime = os.time() + self.getRankCd
	end
end

function UISeasonRank:getDataFromWeb()
	local player = PlayerManager:getClientPlayer()
	if player ~= nil then
		local userId = player.userId

		-- 100 players rank data
		LiftingSimulatorWeb:getRankData(userId, 0, 100, function (info, code)
			if code == 1 then
				LogUtil.log("web UISeasonRank getRankData suc userId: " .. tostring(userId) .. " " .. inspect.inspect(info))
				if info then
					self.rankData.list = {}
					for _, v in pairs(info) do
						local item = {}
						item.rank = v.rank
						item.name = v.userName or ""
						item.kill = v.killNum
						item.score = v.integral
						item.lv = v.stage
						table.insert(self.rankData.list, item)
					end
					self.rankData.update = true
					self:updateGridView()
				end
			else
				LogUtil.log("web UISeasonRank getRankData error userId: " .. tostring(userId) .. " info: " .. tostring(info))
			end
		end)

		-- my rank data
		LiftingSimulatorWeb:getMyRankData(userId, function (info, code)
			if code == 1 then
				LogUtil.log("web UISeasonRank getMyRankData suc userId: " .. tostring(userId) .. " " .. inspect.inspect(info))
				if info then
					local player = PlayerManager:getClientPlayer()
			        local name = player:getName()
			        local rank = info.rank
					self.myRankData.data = {}
					if rank < 1 then
						rank = "#"
					end
					self.myRankData.data.rank = tostring(rank)
					self.myRankData.data.id = tostring(name)
					self.myRankData.data.kill = info.killNum
					self.myRankData.data.score = info.integral
					self.myRankData.data.lv = info.stage
					self.myRankData.update = true
					self:updateMyRankData()
				end
			else
				LogUtil.log("web UISeasonRank getMyRankData error userId: " .. tostring(userId) .. " info: " .. tostring(info))
			end
		end)
	end
end

return UISeasonRank