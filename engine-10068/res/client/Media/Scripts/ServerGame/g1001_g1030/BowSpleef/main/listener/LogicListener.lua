---
--- Created by Jimmy.
--- DateTime: 2017/11/2 0002 12:17
---
require "Match"

LogicListener = {}

function LogicListener:init()
    EntityHitEvent.registerCallBack(self.onEntityHit)
end

function LogicListener.onEntityHit(entityId, vec3)
    if GameMatch:isGameStart() == false then
        return
    end
    if entityId == 262 then
        EngineWorld:setBlockToAir(VectorUtil.newVector3i(vec3.x, vec3.y, vec3.z))
    end
end

return LogicListener
--endregion
