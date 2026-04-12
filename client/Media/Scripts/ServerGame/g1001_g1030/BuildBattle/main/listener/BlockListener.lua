--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

require "Match"

BlockListener = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    BlockBreakWithMetaEvent.registerCallBack(self.onBlockBreakWithMeta)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    return true
end

function BlockListener.onBlockBreakWithMeta(rakssid, blockId, blockMeta, vec3)
    local player = GameMatch:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

	if GameMatch.curStatus ~= GameMatch.Status.WaitingPlayer
		and GameMatch.curStatus ~= GameMatch.Status.Buiding then
    	return false
    end

    if not EngineWorld:isBlockChange(vec3) then
    	return false
    end

    return true
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
	local player = GameMatch:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

	return GameMatch.curStatus == GameMatch.Status.WaitingPlayer
		or GameMatch.curStatus == GameMatch.Status.Buiding
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

return BlockListener
--endregion
