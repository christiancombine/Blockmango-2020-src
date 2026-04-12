---
--- Created by Yaoqiang.
--- DateTime: 2018/6/22 0025 17:50
---
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

GamePlayer.Achievement = {}
GamePlayer.Achievement.FindChanceRoom = "1"
GamePlayer.Achievement.FindDangerRoom = "2"
GamePlayer.Achievement.FindWealthRoom = "3"
GamePlayer.Achievement.FindCageRoom = "4"
GamePlayer.Achievement.FindBox = "5"
GamePlayer.Achievement.FinishTask = "6"

function GamePlayer:init()

    self:initStaticAttr(0)

    self.config = {}

    self.money = 0

    self:setCurrency(self.money)
    self.scorePoints = {}

    self.score = 0
    self.find_room = 0

    self.achievements = {}

    self.tempInventory = {}

    self.ranch_goods = {}

    -- self:reset()

end

function GamePlayer:onChestOpen(vec3i)
    self.openChestPos = vec3i
    self.openChestTick = 2
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
end

function GamePlayer:teleInitPos()
    HostApi.sendPlaySound(self.rakssid, 141)
    self:teleportPosWithYaw(GameConfig.initPos, 0)
end

function GamePlayer:reward()

end

function GamePlayer:initDataFromDB(data)
    if not data.__first then
        self.money = data.money or 0
    end
    self:initPlayer()
end

function GamePlayer:initPlayer()
    self:setCurrency(self.money)
    self:reset()
end

function GamePlayer:addMoney(money)
    money = NumberUtil.getIntPart(money)
    self:addCurrency(money)
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:subMoney(money)
    self:subCurrency(money)
end

function GamePlayer:overGame()
    HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
end

function GamePlayer:reset()
    self:initScorePoint()
    self:changeMaxHealth(200)
    self:teleInitPos()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:addItem(ItemID.PICKAXE_STONE, 1, 0, false, ToolConfig:getDurableByToolId(ItemID.PICKAXE_STONE))
        self.entityPlayerMP:addItem(BlockID.TORCH_WOOD, 5, 0, false, 0)
        self.entityPlayerMP:changeCurrentItem(0)
    end
end

function GamePlayer:onDie()
    -- self:reset()
    self:initScorePoint()
    self:changeMaxHealth(200)
    self:teleInitPos()
    if self.entityPlayerMP ~= nil then
        self.entityPlayerMP:changeCurrentItem(0)
    end
end

function GamePlayer:onAddAchievement(enum_type, num)
    HostApi.log("onAddAchievement" .. tostring(self.rakssid) .. " " .. enum_type .. " " .. num)
    if self.achievements[enum_type] == nil then
        self.achievements[enum_type] = num
    else
        self.achievements[enum_type] = self.achievements[enum_type] + num
    end
end

function GamePlayer:sendRanchExItem()
    local data = {}
    data.item_list = {}
    for k, v in pairs(self.ranch_goods) do
        local item = {}
        item.itemId = v.itemId
        item.itemNum = v.itemNum
        item.itemImg = RanchItemConfig:getRanchItemImgById(v.itemId)
        table.insert(data.item_list, item)
    end
    HostApi.log("sendRanchExItem")
    HostApi.showRanchExItem(self.rakssid, true, json.encode(data))
end

function GamePlayer:onAddRanchGoods(itemId, itemNum)
    HostApi.log("onAddRanchGoods " .. itemId .. " " .. itemNum)
    for k, v in pairs(self.ranch_goods) do
        if v.itemId == itemId then
            v.itemNum = v.itemNum + itemNum
            self:sendRanchExItem()
            return
        end
    end
    local item = {}
    item.itemId = itemId
    item.itemNum = itemNum
    table.insert(self.ranch_goods, item)

    self:sendRanchExItem()
end

return GamePlayer


