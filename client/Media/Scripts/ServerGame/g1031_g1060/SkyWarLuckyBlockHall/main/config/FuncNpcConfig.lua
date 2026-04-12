require "data.GameFuncNpc"

FuncNpcConfig = {}
FuncNpcConfig.FuncNpcMap = {}

function FuncNpcConfig:initConfig(npcs)
    for _, config in pairs(npcs) do
        if BaseMain:isChina() then
            if config.id ~= "1" and config.id ~= "4" then
                local npc = GameFuncNpc.new(config)
                self.FuncNpcMap[tostring(npc.entityId)] = npc
            end
        else
            local npc = GameFuncNpc.new(config)
            self.FuncNpcMap[tostring(npc.entityId)] = npc
        end
    end
end

function FuncNpcConfig:onPlayerMove(player, x, y, z)
    for _, npc in pairs(self.FuncNpcMap) do
        npc:onPlayerMove(player, x, y, z)
    end
end

return FuncNpcConfig