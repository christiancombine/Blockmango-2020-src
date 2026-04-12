require "config.TigerLotteryConfig"
require "data.GameTigerLottery"
TigerLotteryManager = {}
TigerLotteryManager.tigerLotterys = {}

function TigerLotteryManager:Init()
    local tigerLotteryConfigs = TigerLotteryConfig:GetTigerLotteryConfig()
    for _, config in pairs(tigerLotteryConfigs) do
        local tigerLottery = GameTigerLottery.new(config)
        local npcId = tigerLottery:OnCreate()
        self.tigerLotterys[npcId] = tigerLottery
    end
end

function TigerLotteryManager:SyncTigerLotteryInfo(player)
    for _, tigerLottery in pairs(self.tigerLotterys) do
        if type(tigerLottery) == "table" then
            tigerLottery:SyncTigerLotteryInfo(player)
        end
    end
end

function TigerLotteryManager:OnPlayerReceive(player, npcId)
    local tigerLottery = self.tigerLotterys[npcId]
    if type(tigerLottery) ~= "table" then
        return false
    end
    tigerLottery:OnPlayerClick(player)
end

function TigerLotteryManager:onPlayerLogout(player)
    -- for _, eggChest in pairs(self.tigerLotterys) do
    --     eggChest:onPlayerExit(player)
    --end
end