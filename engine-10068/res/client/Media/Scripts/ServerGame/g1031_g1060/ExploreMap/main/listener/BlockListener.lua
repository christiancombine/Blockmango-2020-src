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
    BlockSwitchEvent.registerCallBack(self.onBlockSwitch)
    BlockPressurePlateWeightedEvent.registerCallBack(self.onBlockPressurePlateWeighted)
end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    return true
end

function BlockListener.onBlockBreakWithMeta(rakssid, blockId, blockMeta, vec3)
    if true then
        return true
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
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
    if true then
        return true
    end
	local player = PlayerManager:getPlayerByRakssid(rakssid)
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

function BlockListener.onBlockSwitch(entityId, downOrUp, blockId, vec3i)
    local player = GameMatch:getPlayerByEntityId(entityId)
    if player == nil then
        return false
    end

    if player.entityPlayerMP ~= nil then
        if downOrUp and blockId == 72 then
            player.entityPlayerMP:specialJump(0.5, 1.0, 1.0)
        end
    end

    return true
end

function BlockListener.onBlockPressurePlateWeighted(power, blockId, vec3i)
    if blockId == 217 then
        HostApi.log("onBlockPressurePlateWeighted " .. power)
    end
end

return BlockListener
--endregion
