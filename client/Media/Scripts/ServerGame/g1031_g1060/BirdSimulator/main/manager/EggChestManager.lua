require "config.EggChestConfig"
require "data.GameEggChest"
EggChestManager = {}

function EggChestManager:Init()
    self.eggChestDatas = {}
    local eggChestConfigs = EggChestConfig:GetEggChestConfig()
    for _, config in pairs(eggChestConfigs) do
        local eggChest = GameEggChest.new(config)
        local npcId = eggChest:OnCreate()
        self.eggChestDatas[npcId] = eggChest
    end
end

function EggChestManager:SyncEggChestInfo(player)
    for _, eggChest in pairs(self.eggChestDatas) do
        if type(eggChest) == "table" then
            eggChest:SyncEggChestInfo(player)
        end
    end
end

function EggChestManager:OnPlayerReceive(player, npcId)
    local eggChest = self.eggChestDatas[npcId]
    if type(eggChest) ~= "table" then
        return false
    end

    local countdown = player.eggChestCountdowns[eggChest.eggChestId]
    if not countdown then
        return false
    end

    if os.time() - countdown >= 0 then
        eggChest:OnPlayerClick(player)
    end
end

function EggChestManager:onPlayerLogout(player)
    for _, eggChest in pairs(self.eggChestDatas) do
        eggChest:onPlayerExit(player)
    end
end
