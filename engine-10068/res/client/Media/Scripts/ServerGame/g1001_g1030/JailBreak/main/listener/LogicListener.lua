---
--- Created by Jimmy.
--- DateTime: 2018/3/9 0009 11:29
---

LogicListener = {}

function LogicListener:init()
    PotionConvertGlassBottleEvent.registerCallBack(self.onPotionConvertGlassBottle)
end

function LogicListener.onPotionConvertGlassBottle(entityId, itemId)
   return false
end

return LogicListener