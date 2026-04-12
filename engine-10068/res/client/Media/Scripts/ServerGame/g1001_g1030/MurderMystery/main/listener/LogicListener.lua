---
--- Created by Jimmy.
--- DateTime: 2018/3/9 0009 11:29
---
LogicListener = {}

function LogicListener:init()
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)
end

function LogicListener.onEntityItemSpawn(itemId, itemMeta, behavior)
    return false
end

return LogicListener