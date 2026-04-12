---
--- Created by Jimmy.
--- DateTime: 2017/11/2 0002 12:17
---
require "Match"

LogicListener = {}
LogicListener.hitCount = {}

function LogicListener:init()
    EntityHitEvent.registerCallBack(self.onEntityHit)
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)
end

function LogicListener.onEntityHit(entityId, vec3)
    if entityId == 332 then
        local pos = VectorUtil.newVector3i(vec3.x, vec3.y, vec3.z)

        local hashKey = VectorUtil.hashVector3(vec3)
        local hp = LogicListener.hitCount[hashKey]
        if hp then
            if hp > 0 then
                LogicListener.hitCount[hashKey] = hp - 1
                if LogicListener.hitCount[hashKey] <= 0 then
                    EngineWorld:setBlockToAir(pos)
                end
            end
            return
        end

        local blockId = EngineWorld:getBlockId(pos)
        local ifConfig = false
        for k, v in pairs(BlockConfig.block) do
            if blockId == v.id then
                LogicListener.hitCount[hashKey] = v.hp - 1
                if LogicListener.hitCount[hashKey] <= 0 then
                    EngineWorld:setBlockToAir(pos)
                end
                ifConfig = true
                break
            end
        end
        if ifConfig == false then
            EngineWorld:setBlockToAir(pos)
        end
    end
end

function LogicListener.onEntityItemSpawn(itemId, itemMeta, behavior)
    if behavior == "break.block" then
        return false
    end

    return true
end

return LogicListener
--endregion
