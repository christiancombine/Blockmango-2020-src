GamePlayer = class("GamePlayer", BasePlayer)

PlayerManager:registerClientPlayerClass(GamePlayer)
PlayerManager:registerOtherPlayerClass(GamePlayer)

function GamePlayer:Instance()
    return PlayerManager:getClientPlayer()
end

function GamePlayer:init()
    self.level = 1
    self.cur_exp = 0
    self.Wallet = GameWallet.new(self.Player:getWallet())
    self.Wallet:setMoneyCount(Define.Money.HallGold, 0)
    self.hallGoldNum = 0
    self.towerList = {}
    self.skinList = {}
    self.equipSkinId = 0
    self.ExchangeRate = 1
    self.poleControlState = 1
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

function GamePlayer:onGetNewTower(towerInfo)
    if self.towerList == nil then
        self.towerList = {}
    end
    self.towerList[towerInfo.towerId] = towerInfo
    GetNewTowerEvent:invoke()
end

function GamePlayer:setSkinInfo(data)
    self.skinList = data.skinList
end

function GamePlayer:EquipSkin(data)
    self.equipSkinId = data.equipSkinId
    local config = SkinStoreConfig:getConfigById(tonumber(data.equipSkinId))
    if config then
        ClientHelper.putFloatPrefs("ThirdPersonDistance", config.thirdPersonDistance)
        ClientHelper.putFloatPrefs("ThirdPersonCameraYOffset", config.thirdPersonYOffset)
    end
end

function GamePlayer:setExchangeRate(data)
    self.ExchangeRate = data:getNumberParam("ExchangeRate")
end

function GamePlayer:setCountdownEffect(data)
    local time = data:getNumberParam("time")
    local areaId = data:getNumberParam("areaId")
    local area = AreaManager:getTeamAreaById(areaId)
    if area then
        area:initCountDown(time)
    end
end

function GamePlayer:setTeamAreaPlayerNum(data)
    local playerNum = data:getNumberParam("playerNum")
    local areaId = data:getNumberParam("areaId")
    local area = AreaManager:getTeamAreaById(areaId)
    area.isLocal = false
    if area then
        area:showPlayerNum(area.config.pos2, area.config.scale2, area.config.pYaw, playerNum, area.teamConfig.countLimit, "countLimit" .. area.config.Id, area.teamConfig.levelLimit)
    end
end
