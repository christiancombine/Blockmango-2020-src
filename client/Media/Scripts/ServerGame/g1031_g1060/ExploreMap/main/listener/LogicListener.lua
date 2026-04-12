---
--- Created by Yang.
--- DateTime: 2018/7/5 15:30
---
LogicListener = {}

function LogicListener:init()
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)
    PlacingConsumeBlockEvent.registerCallBack(self.onPlacingConsumeBlock)
end

function LogicListener.onEntityItemSpawn(itemId, itemMeta, behavior)
    return false
end

function LogicListener.onPlacingConsumeBlock(itemId, itemMeta)
    return false
end

return LogicListener