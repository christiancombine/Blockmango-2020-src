--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "Match"

BlockListener = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
    PlayerPlaceBlockBoxJudgeEvent.registerCallBack(self.onPlayerPlaceBlockBoxJudge)
    BlockStationaryNotStationaryEvent.registerCallBack(self.onBlockStationaryNotStationary)
end

function BlockListener.onBlockStationaryNotStationary(blockId)
    return false
end

function BlockListener.onPlayerPlaceBlockBoxJudge(rakssid, blockId)
    return false
end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    return true
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
	local player = PlayerManager:getPlayerByRakssid(rakssid)

    if player ~= nil then
		HostApi.log("BlockListener.onBlockPlace, player:getYaw():" .. tostring(player:getYaw()))
	else 
		HostApi.log("BlockListener.onBlockPlace, player nil, rakssid:" .. tostring(rakssid))
	end
    
    return true
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

return BlockListener
--endregion
