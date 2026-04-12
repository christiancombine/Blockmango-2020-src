
require "ui.GUIRankItem"
local UIRank = class("UIRank", IGUILayout)

function UIRank:onLoad()
    self:onLoadInitUI()
end

function UIRank:onLoadInitUI()


    self.curTabType = RankData.Type.Week

    self.closeBtn = self:getChildWindow("SkyBlockRankClient-Close", GUIType.Button)
    self.closeBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickClose()
    end)

    self.questionBtn = self:getChildWindow("SkyBlockRankClient-Question", GUIType.Button)
    self.questionBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickQuestion()
    end)

    self.rankTxt = self:getChildWindow("SkyBlockRankClient-SelfRank")
    self.scoreTxt = self:getChildWindow("SkyBlockRankClient-SelfScore")
    self.head = self:getChildWindow("SkyBlockRankClient-SelfHead", GUIType.UrlImage)
    self.head:setDefaultImage("set:userItem.json image:icon")
    self.nameTxt = self:getChildWindow("SkyBlockRankClient-SelfName")
    self.time = self:getChildWindow("SkyBlockRankClient-Time")

    self.topName = {}
    self.topHead = {}
    self.topScore = {}
    self.topCallOnBtn = {}
    for i = 1, 3 do
        self.topName[i] = self:getChildWindow("SkyBlockRankClient-TopRank" .. tostring(i) .. "Name")
        self.topHead[i] = self:getChildWindow("SkyBlockRankClient-TopRank" .. tostring(i) .. "HeadInfo", GUIType.UrlImage)
        self.topHead[i]:setDefaultImage("set:userItem.json image:icon")
        self.topScore[i] = self:getChildWindow("SkyBlockRankClient-TopRank" .. tostring(i) .. "Score")
        self.topCallOnBtn[i] = self:getChildWindow("SkyBlockRankClient-TopRank" .. tostring(i) .. "CallonBtn", GUIType.Button)
        self.topCallOnBtn[i]:registerEvent(GUIEvent.ButtonClick, function()
            self:onClickTopCallOn(i)
        end)
    end

    self:registerDataCallBack("SkyBlockRankData", function(data)
        self:onUpdateRankData(data)
    end, JsonBuilderProcessor)

    self.radioBtn = {}
    self.radioBtnTxt = {}
    self.viewLayout = {}
    self.viewList = {}

    for i = 1, RankData.Type.Count do
        self.radioBtnTxt[i] = self:getChildWindow("SkyBlockRankClient-TableTxt_" .. tostring(i))
        self.radioBtnTxt[i]:SetText(Lang:getString("gui_rank_title_" .. tostring(i) .. "_normal"))
        self.radioBtn[i] = self:getChildWindow("SkyBlockRankClient-Table_"..tostring(i), GUIType.RadioButton)
        self.radioBtn[i]:registerEvent(GUIEvent.RadioStateChanged, function()
            self:onRadioStateChanged(self.radioBtn[i], i)
        end)

        self.viewLayout[i] = self:getChildWindow("SkyBlockRankClient-View" .. tostring(i), GUIType.Layout)
        self.viewList[i] = IGUIListView.new("SkyBlockRankClient-View-List" .. tostring(i), self.viewLayout[i])
    end
end

function UIRank:onShow()
    self.radioBtn[1]:SetSelected(true)

    -- all rank is not need
    self.radioBtn[2]:SetVisible(false)
end
--
function UIRank:onTick(frameTime)

    if self.curTabType == RankData.Type.Week then
        local now = os.time()
        local endTime = DateUtil.getWeekLastTime()
        local time = RankData:getFormatTimeWithDay(endTime - now)
        self.time:SetVisible(true)
        self.time:SetText(Lang:getString("gui_time_count_down") .. " : " .. tostring(time))
    else
        self.time:SetVisible(false)
    end
end

function UIRank:onRadioStateChanged(radioBtn, index)
    if radioBtn:IsSelected() then
        self.curTabType = index
        self:updateCurView(index)

        for i = 1, RankData.Type.Count do
            if index == i then
                self.radioBtnTxt[i]:SetText(Lang:getString("gui_rank_title_" .. tostring(i) .. "_select"))
            else
                self.radioBtnTxt[i]:SetText(Lang:getString("gui_rank_title_" .. tostring(i) .. "_normal"))
            end
        end
        HostApi.log("onRadioStateChanged" .. self.curTabType)
    end
end

function UIRank:updateCurView(index)
    self.rankTxt:SetText("-1")
    self.nameTxt:SetText("")
    self.scoreTxt:SetText("-1")
    self.head:setUrl("")

    for i = 1, RankData.Type.Count do
        self.viewLayout[i]:SetVisible(false)
        self.viewList[i].root:SetVisible(false)
    end

    if RankData.AllData[index].Update == true then
        self.viewList[index]:clearItems()
    end

    for i = 1, 3 do
        self.topName[i]:SetText("")
        self.topHead[i]:setUrl("")
        self.topScore[i]:SetText("")
    end

    self.viewList[index].root:SetScrollOffset(0)
    self.viewList[index].root:SetVisible(true)
    self.viewLayout[index]:SetVisible(true)

    local list = RankData.AllData[index].RankData

    HostApi.log("updateCurView AllData " .. index .. " " .. #list)
    if list then

        if RankData.AllData[index].Update == true then
            for j = 1, #list do
                if j > 3 then
                    local curItem = UIRankItem.new("SkyBlockRankClientItem.json")
                    curItem.root:SetArea({ 0, 0 }, { 0, 0 }, { 1, 0 }, { 0, 71 })
                    curItem:setItemData(list[j])
                    self.viewList[index]:addItem(curItem)
                end
            end
        end

        for i = 1, 3 do
            if list[i] then
                self.topName[i]:SetText(list[i].name or "")
                self.topHead[i]:setUrl(list[i].picUrl or "")
                self.topScore[i]:SetText(list[i].score or "")
            end
        end
    end

    local own = RankData.AllData[index].Own

    if own then
        self.rankTxt:SetText(own.rank or "")
        self.nameTxt:SetText(own.name or "")
        self.scoreTxt:SetText(own.score or "")
        self.head:setUrl(own.picUrl or "")
    end

    RankData.AllData[index].Update = false
end

function UIRank:isMainLayout()
    return false
end

function UIRank:onClickClose()
    self:hide()
end

function UIRank:onClickTopCallOn(index)
    if index >= 1 and index <= 3 then
        local data = RankData.AllData[self.curTabType].RankData
        if data then
            HostApi.log("onClickTopCallOn " .. data[index].userId)
            PacketSender:sendCallOnManor(data[index].userId, 3, true, 0)
        end
    end
end

function UIRank:onClickQuestion()
    GUIRankTip:onShowTipDialog(self.curTabType)
end

function UIRank:onUpdateRankData(data)

    RankData:UpdateData(data)
end

GUIRank = UIRank.new("SkyBlockRankClient.json")
GUIRank:hide()