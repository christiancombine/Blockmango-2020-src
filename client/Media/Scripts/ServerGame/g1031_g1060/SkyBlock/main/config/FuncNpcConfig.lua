-- @Author: tanjianyong
-- @Date:   2019-08-23 11:24:07
-- @Last Modified by:   tanjianyong
-- @Last Modified time: 2019-10-24 15:48:11

require "data.GameFuncNpc"

FuncNpcConfig = {}
FuncNpcConfig.FuncNpcMap = {}

function FuncNpcConfig:initConfig(npcs)
    for _, config in pairs(npcs) do
        local npc = GameFuncNpc.new(config)
        self.FuncNpcMap[tostring(npc.entityId)] = npc
    end
end

function FuncNpcConfig:onPlayerMove(player, x, y, z)
    for _, npc in pairs(self.FuncNpcMap) do
        npc:onPlayerMove(player, x, y, z)
    end
end

return FuncNpcConfig