require "config.ChestConfig"
require "data.GameChest"
ChestManager = {}

function ChestManager:Init()
    self.chestDatas = {}
    local chestConfigs = ChestConfig:GetChestConfig()
    for _, config in pairs(chestConfigs) do
        local chest = GameChest.new(config)
        local npcId = chest:OnCreate()
        self.chestDatas[npcId] = chest
    end
end

function ChestManager:SyncChestInfo(player)
    for _, chest in pairs(self.chestDatas) do
        if type(chest) == "table" then
            chest:SyncChestInfo(player)
        end
    end
end

function ChestManager:OnPlayerReceive(player, npcId)
    local chest = self.chestDatas[npcId]
    if type(chest) ~= "table" then
        return false
    end
   
    if chest:isVipChest() == true then
        if player:isHavePrivilegeId(chest.bindld) == true then
            chest:OnPlayerClick(player)
        else
            HostApi.sendCommonTipByType(player.rakssid, 14, chest.tips)
            return false
        end 
    else
        chest:OnPlayerClick(player)
    end
end

function ChestManager:onPlayerLogout(player)
    for _, chest in pairs(self.chestDatas) do
        chest:onPlayerExit(player)
    end
end
