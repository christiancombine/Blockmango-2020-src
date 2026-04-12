---
--- Created by Jimmy.
--- DateTime: 2018/3/9 0009 11:29
---
require "config.AirplaneConfig"

LogicListener = {}

function LogicListener:init()
    PotionConvertGlassBottleEvent.registerCallBack(self.onPotionConvertGlassBottle)
    AircraftMoveEvent.registerCallBack(self.onAircraftMove)
end

function LogicListener.onPotionConvertGlassBottle(entityId, itemId)
   return false
end

function LogicListener.onAircraftMove(entityId, pos)
    if GameMatch.isAllLanding == true then
        return
    end

    if GameMatch.isCanLanding == false and AirplaneConfig:inLandingArea(pos) then
        GameMatch.isCanLanding = true
        HostApi.sendChangeAircraftUI(0, true)
    end

    if GameMatch.isCanLanding == true and AirplaneConfig:inLandingArea(pos) == false then
        EngineWorld:getWorld():tryAllPlayerOffAircraft()
        GameMatch.isAllLanding = true
    end
end

return LogicListener