--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "Match"
require "manager.BlockManager"

BlockListener = {}
BlockListener.BlockLifeCache = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
    BlockBreakWithGunEvent.registerCallBack(self.onBlockBreakWithGun)
    SchemeticPlaceBlockBeginEvent.registerCallBack(self.onSchemeticPlaceBlockBegin)
    SchemeticPlaceBlockEvent.registerCallBack(self.onSchemeticPlaceBlock)
    SchemeticPlaceBlockFinishEvent.registerCallBack(self.onSchemeticPlaceBlockFinish)
    SchemeticPlaceCoverEvent.registerCallBack(self.onSchemeticPlaceCover)
end

function BlockListener.onBlockBreak(rakssid, blockId, blockPos)
    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("BlockListener.onBlockPlace player == nil")
        return false
    end
    if GameMatch.Process:isRunning(player.siteId) == false then
        return false
    end
    if BlockConfig:inSafeArea(toPlacePos) then
        return false
    end
    BlockManager:onPlayerPlaceBlock(player, itemId, toPlacePos)
    return true
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

function BlockListener.onBlockBreakWithGun(rakssid, blockId, blockPos, gunId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if GameMatch.Process:isRunning(player.siteId) == false then
        return false
    end
    if BlockConfig:inSafeArea(blockPos) then
        return false
    end
    return BlockManager:onPlayerShootBlock(player, blockPos, blockId, gunId)
end

function BlockListener.onSchemeticPlaceBlockBegin(rakssid, blockPos)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if GameMatch.Process:isRunning(player.siteId) == false then
        return false
    end
    if BlockConfig:inSafeArea(blockPos) then
        return false
    end
    return true
end

function BlockListener.onSchemeticPlaceBlock(rakssid, vec3, blockId, blockMeta, placeBlockParam)
    return BlockListener.onBlockPlace(rakssid, blockId, blockMeta, vec3)
end

function BlockListener.onSchemeticPlaceBlockFinish(rakssid, blockId, _, costNum)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    local itemId = 0;
    for _, block in pairs(player.equip_blocks) do
        if block.ItemId == blockId then
            itemId = block.ItemId
        end
    end
    return player:removeItem(itemId, costNum)
end

function BlockListener.onSchemeticPlaceCover(_, blockPos)
    if EngineWorld:isBlockChange(blockPos) then
        --强制覆盖
        return false
    end
    return true
end

return BlockListener
--endregion
