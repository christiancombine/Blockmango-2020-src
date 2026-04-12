--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

require "Match"

BlockListener = {}

function BlockListener:init()
    BlockBreakWithMetaEvent.registerCallBack(self.onBlockBreakWithMeta)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
    BlockBreakWithGunEvent.registerCallBack(self.onBlockBreakWithGun)
    BlockSwitchEvent.registerCallBack(self.onBlockSwitch)
    BlockSwitchPlusEvent.registerCallBack(self.onBlockSwitchPlus)
    DamageOnFireByLavaTimeEvent.registerCallBack(self.onDamageOnFireByLavaTime)
end

function BlockListener.onBlockBreakWithMeta(rakssid, blockId, blockMeta, vec3, itemID)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if blockId == BlockID.BEDROCK  or blockId == BlockID.CHEST then
        return false
    end
    local isCanBreak = GameConfig:isInArea(vec3, GameConfig.canBreakArea, false)
    if  not isCanBreak then
		return false
	end
    BlockSettingConfig:BreakBlockDropItem(player, blockId, blockMeta, vec3)
    return true
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    return false
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

function BlockListener.onBlockBreakWithGun(rakssid, blockId, blockPos, gunId)
    return false
end

function BlockListener.onBlockSwitch(entityId, downOrUp, blockId, vec3i)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil or player.entityPlayerMP == nil then
        return true
    end

    if downOrUp and blockId == BlockID.PRESSURE_PLATE_STONE then

        if FlipFrontConfig.font then
            for i = 1, #FlipFrontConfig.font do
                local pos = VectorUtil.newVector3(tonumber(FlipFrontConfig.font[i].pos.x), tonumber(FlipFrontConfig.font[i].pos.y), tonumber(FlipFrontConfig.font[i].pos.z))
                if pos.x == vec3i.x and pos.y == vec3i.y and pos.z == vec3i.z then
                    player.entityPlayerMP:specialJump(0.5, 1, 1)
                    return false
                end
            end
        end
        if FlipBackConfig.back then
            for i = 1, #FlipBackConfig.back do
                local pos = VectorUtil.newVector3(tonumber(FlipBackConfig.back[i].pos.x), tonumber(FlipBackConfig.back[i].pos.y), tonumber(FlipBackConfig.back[i].pos.z))
                if pos.x == vec3i.x and pos.y == vec3i.y and pos.z == vec3i.z then
                    player.entityPlayerMP:specialJump(0.5, -1.7, -1.7)
                    return false
                end
            end
        end
    end

    return true
end

function BlockListener.onBlockSwitchPlus(vec3i)
    return false
end

function BlockListener.onDamageOnFireByLavaTime(entityId, num)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player ~= nil then
        num.value = GameConfig.lavaFireTime
    end
    return true
end

return BlockListener
--endregion
