require "config.BonusConfig"
require "data.DrawLuckyNpc"
DrawLuckyManager = {}

function DrawLuckyManager:Init()
    self.NpcList = {}
    local npcs = BonusConfig.npc
    self.drawLuckyNpcs = {}
    for _, npcInfo in pairs(npcs) do
        if npcInfo.type == 1 then
            local npc = DrawLuckyNpc.new(npcInfo)
            local id = npc:OnCreate()
            self.NpcList[id] = npc
        end
    end
end

function DrawLuckyManager:OnPlayerDrawLucky(player, npcId)
    local npc = self.NpcList[npcId]
    if npc == nil then
        return false
    end
    if npc:OnPlayerClick(player) then
        return true
    end
    npc:SyncDrawLuckyInfo(player)
    return false
end

function DrawLuckyManager:SyncDrawLucky(player)
    for _, npc in pairs(self.NpcList) do
        npc:SyncDrawLuckyInfo(player)
    end
end
