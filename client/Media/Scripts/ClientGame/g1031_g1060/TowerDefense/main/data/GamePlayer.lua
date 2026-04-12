------
--Client
------
GamePlayer = class("GamePlayer", BaseGamePlayer)

PlayerManager:registerClientPlayerClass(GamePlayer)
PlayerManager:registerOtherPlayerClass(GamePlayer)

function GamePlayer:Instance()
    return PlayerManager:getClientPlayer()
end

function GamePlayer:init()
    self.super:init()
    self.Wallet = GameWallet.new(self.Player:getWallet())
    self.Wallet:setMoneyCount(Define.Money.HallGold, 0)
    self.Wallet:setMoneyCount(Define.Money.GameGold, 0)
    self.userId = self.Player:getPlatformUserId()
    self.towerList = {}
    self.formulaList = {}
    self.ExchangeRate = 1
    self.poleControlState = 1
    self.exchangeLimit = 0
    self.gameMoneyRate = 1

    self.towerItemTip = {
        towerItem_1 = false,
        towerItem_2 = false,
        towerItem_3 = false,
        shortcutBar_1 = false,
        shortcutBar_2 = false,
    }
end

function GamePlayer:setProcessId(data)
    local builder = DataBuilder.new():fromData(data)
    self.processId = builder:getNumberParam("processId")
end

function GamePlayer:setCanPlaceBlockId(data)
    local builder = DataBuilder.new():fromData(data)
    self.canPlaceBlockId = builder:getNumberParam("blockId")
end

function GamePlayer:tryPlaceTower(pos)
    local current_tower_id = GUIMain:getCurrentTowerId()
    if current_tower_id ~= 0 then
        local Config = TowerConfig:getConfigById(current_tower_id)
        if Config ~= nil and self:canPlaceTower(pos, Config.CoinId, Config.BuildPrice, self.processId) then
            local builder = DataBuilder.new():addVector3Param("pos", VectorUtil.toVector3(pos))
            builder:addParam("Id", current_tower_id)
            GamePacketSender:sendPlaceTower(builder:getData())
            LuaTimer:schedule(function()
                self.Player:setInsertUpperAction(1000, 1000)
            end, 100)
        end
    end
end

function GamePlayer:getOwnedTowerList()
    return self.towerList
end

function GamePlayer:setOwnedTowerList(data)
    if data ~= nil then
        for _, tower in pairs(data) do
            self.towerList[tower.towerId] = tower
        end
    end
end

function GamePlayer:onGetNewTowerBook(towerInfo)
    if self.towerList == nil then
        self.towerList = {}
    end
    self.towerList[towerInfo.towerId].isHaveBook = true
    GetNewTowerBookEvent:invoke()
end

function GamePlayer:onGetNewTower(towerInfo)
    if self.towerList == nil then
        self.towerList = {}
    end
    self.towerList[towerInfo.towerId] = towerInfo
    GetNewTowerEvent:invoke()
    self:checkAllTowerItem()
end

function GamePlayer:checkTowerFormulaByStarLevel()
    --检查某一星级的塔是否能合成玩家未拥有的更高级的塔
    local guideBuyTowerList = {}
    for StarLevel = 1, 4 do
        local higherStarTowerList = TowerConfig:getTowerListByStar(StarLevel + 1)
        if higherStarTowerList and self.formulaList["Start" .. StarLevel] ~= nil and #self.formulaList["Start" .. StarLevel] >= 3 then
            for _, targetTower in pairs(higherStarTowerList) do
                --如果玩家拥有这座塔，则跳过这个配方
                if self.towerList[targetTower.Id] == nil then
                    local formulaConfig = TowerFormulaConfig:getConfigByTowerId(targetTower.Id)
                    if formulaConfig ~= nil then
                        local formulaItem = {
                            itemA = 0,
                            itemB = 0,
                            itemC = 0,
                        }
                        for _, tower in pairs(self.formulaList["Start" .. StarLevel]) do
                            local towerClassId = tower.config.ClassId
                            if towerClassId == formulaConfig.ClassAId and formulaItem.itemA == 0 then
                                formulaItem.itemA = tower
                            elseif towerClassId == formulaConfig.ClassBId and formulaItem.itemB == 0 then
                                formulaItem.itemB = tower
                            elseif towerClassId == formulaConfig.ClassCId and formulaItem.itemC == 0 then
                                formulaItem.itemC = tower
                            end

                            if formulaItem.itemA ~= 0 and formulaItem.itemB ~= 0 and formulaItem.itemC ~= 0 then
                                --符合条件，弹出界面
                                table.insert(guideBuyTowerList, #guideBuyTowerList + 1, targetTower.Id)
                                break
                            end
                        end
                    end
                end
            end
        end
    end
    GUIMain:checkBuyGuideList(guideBuyTowerList)
end

function GamePlayer:checkAllTowerItem()
    for index = 1, 3 do
        local towerItem = GUIMain.towerList:getItem("index", index)
        if towerItem ~= nil then
            self.towerItemTip["towerItem_" .. tostring(index)] = false
            self.towerItemTip["towerItem_" .. tostring(index)] = self:checkTowerFormulaAfterCanPlace(towerItem)
        end
    end

    if GUIMain.shortCutBar1.Item ~= nil then
        self.towerItemTip["shortcutBar_1"] = false
        self.towerItemTip["shortcutBar_1"] = self:checkTowerFormulaAfterCanPlace(GUIMain.shortCutBar1.Item)
    end

    if GUIMain.shortCutBar2.Item ~= nil then
        self.towerItemTip["shortcutBar_2"] = false
        self.towerItemTip["shortcutBar_2"] = self:checkTowerFormulaAfterCanPlace(GUIMain.shortCutBar2.Item)
    end
end

function GamePlayer:checkTowerFormulaAfterCanPlace(towerItem)
    --检查玩家再放置一个塔之后是否能合成高级塔
    local towerConfig = TowerConfig:getConfigById(towerItem.towerId)
    if towerConfig == nil then
        return false
    end
    local higherStarTowerList = TowerConfig:getTowerListByStar(towerConfig.StarLevel + 1)
    if not higherStarTowerList then
        return false
    end
    if self.formulaList["Start" .. towerConfig.StarLevel] == nil then
        return false
    end
    if #self.formulaList["Start" .. towerConfig.StarLevel] < 2 then
        return false
    end
    for _, targetTower in pairs(higherStarTowerList) do
        if self.towerList[targetTower.Id] ~= nil then
            local formulaConfig = TowerFormulaConfig:getConfigByTowerId(targetTower.Id)
            if formulaConfig ~= nil then
                local formulaItem = {
                    itemA = 0,
                    itemB = 0,
                    itemC = 0,
                }
                if formulaConfig.ClassAId == towerConfig.ClassId then
                    formulaItem.itemA = towerConfig.ClassId
                elseif formulaConfig.ClassBId == towerConfig.ClassId then
                    formulaItem.itemB = towerConfig.ClassId
                elseif formulaConfig.ClassCId == towerConfig.ClassId then
                    formulaItem.itemC = towerConfig.ClassId
                end

                if formulaItem.itemA ~= 0 or formulaItem.itemB ~= 0 or formulaItem.itemC ~= 0 then
                    for _, tower in pairs(self.formulaList["Start" .. towerConfig.StarLevel]) do
                        local towerClassId = tower.config.ClassId
                        if towerClassId == formulaConfig.ClassAId and formulaItem.itemA == 0 then
                            formulaItem.itemA = tower.config.ClassId
                        elseif towerClassId == formulaConfig.ClassBId and formulaItem.itemB == 0 then
                            formulaItem.itemB = tower.config.ClassId
                        elseif towerClassId == formulaConfig.ClassCId and formulaItem.itemC == 0 then
                            formulaItem.itemC = tower.config.ClassId
                        end

                        if formulaItem.itemA ~= 0 and formulaItem.itemB ~= 0 and formulaItem.itemC ~= 0 then
                            --符合条件，显示特效
                            --towerItem.siStarsEffect:SetVisible(true)
                            --break
                            return true
                        end
                    end
                end
            end
        end
    end
    return false
end

function GamePlayer:setFormulaTip()
    local towerItem

    towerItem = GUIMain.towerList:getItem("index", 1)
    if not towerItem then
        return
    end

    if self.towerItemTip["towerItem_1"] == true then
        towerItem.siStarsEffect:SetVisible(true)
    else
        towerItem.siStarsEffect:SetVisible(false)
    end

    towerItem = GUIMain.towerList:getItem("index", 2)
    if towerItem and self.towerItemTip["towerItem_2"] == true then
        towerItem.siStarsEffect:SetVisible(true)
    else
        towerItem.siStarsEffect:SetVisible(false)
    end

    towerItem = GUIMain.towerList:getItem("index", 3)
    if towerItem and self.towerItemTip["towerItem_3"] == true then
        towerItem.siStarsEffect:SetVisible(true)
    else
        towerItem.siStarsEffect:SetVisible(false)
    end

    towerItem = GUIMain.shortCutBar1.Item
    if towerItem and self.towerItemTip["shortcutBar_1"] == true then
        towerItem.siStarsEffect:SetVisible(true)
    else
        towerItem.siStarsEffect:SetVisible(false)
    end

    towerItem = GUIMain.shortCutBar2.Item
    if towerItem and self.towerItemTip["shortcutBar_2"] == true then
        towerItem.siStarsEffect:SetVisible(true)
    else
        towerItem.siStarsEffect:SetVisible(false)
    end
end

function GamePlayer:setExchangeRate(data)
    self.ExchangeRate = data:getNumberParam("ExchangeRate")
end

function GamePlayer:setGameGoldExchangeRate(data)
    self.exchangeLimit = data:getNumberParam("exchangeLimit")
    self.gameMoneyRate = data:getNumberParam("GGoldExchangeRate")
end

function GamePlayer:setOwnedPayItemList(payItemList)
    if payItemList ~= nil then
        for _, payItemId in pairs(payItemList) do
            GUIMain:onGetNewPayItem(payItemId)
        end
    end
end

function GamePlayer:onGetNewPayItem(data)
    local payItemId = data:getNumberParam("itemId")
    local num = data:getNumberParam("num")
    local config = PayStoreConfig:getConfigById(payItemId)

    GUIMain:onGetNewPayItem(config.itemType, num)
end

function GamePlayer:EquipSkin(data)
    self.equipSkinId = data.equipSkinId
    local config = SkinStoreConfig:getConfigById(tonumber(data.equipSkinId))
    if config then
        ClientHelper.putFloatPrefs("ThirdPersonDistance", config.thirdPersonDistance)
        ClientHelper.putFloatPrefs("ThirdPersonCameraYOffset", config.thirdPersonYOffset)
    end
end
