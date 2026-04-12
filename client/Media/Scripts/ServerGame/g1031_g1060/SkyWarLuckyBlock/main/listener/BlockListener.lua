--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "Match"
require "manager.BlockManager"

BlockListener = {}
BlockListener.BlockLifeCache = {}

function BlockListener:init()
    BlockBreakWithMetaEvent.registerCallBack(self.onBlockBreakWithMeta)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
    PlayerPlaceBlockBoxJudgeEvent.registerCallBack(self.onPlayerPlaceBlockBoxJudge)
    BlockTNTExplodeBreakBlockEvent.registerCallBack(self.onBlockTNTExplodeBreakBlock)
    BlockStationaryNotStationaryEvent.registerCallBack(self.onBlockStationaryNotStationary)
    BlockTNTExplodeHurtEntityEvent.registerCallBack(self.onBlockTNTExplodeHurtEntity)
end

function BlockListener.onBlockBreakWithMeta(rakssid, blockId, blockMeta, vec3)
    if not GameMatch:isGameRunning() then
        return false
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if player:getHeldItemId() == Define.HotPotato then
        return false
    end

    if blockId == BlockID.CHEST
            or TableUtil.include(GameConfig.unbreakableBlock, tostring(blockId)) then
        return false
    end
    if blockId == BlockID.PACKED_ICE then
        EngineWorld:setBlockToAir(vec3)
        return false
    end
    player:getLuckyBlock(blockId, vec3)
    if not (TableUtil.include(BlockConfig.BlockId, blockId)) then
        local pos = VectorUtil.newVector3(vec3.x, vec3.y, vec3.z)
        EngineWorld:addEntityItem(blockId, 1, blockMeta, 600, pos)
    end
    ReportUtil:addBreakBlock(blockId)
    return true
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    if not GameMatch:isGameRunning() then
        return false
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if itemId - 1000 == BlockConfig.BlockId.HollowBlock then
        return false
    end

    local yaw = player:getYaw()
    if PropBlockConfig:isPropBlock(itemId) then
        local offsetX, offsetZ, rotationX, rotationZ = PropBlockConfig:getOffset(itemId, yaw)
        local schematic_file = PropBlockConfig:getSchmeticFile(itemId)
        if schematic_file ~= nil then
            toPlacePos.x = toPlacePos.x + offsetX
            toPlacePos.z = toPlacePos.z + offsetZ
            EngineWorld:createSchematic(schematic_file, toPlacePos, rotationX, rotationZ, false, 0, 0, 0, 1)
            player:removeCurItem()
        end
        return false
    end
    ReportUtil:addPlaceBlock(itemId)
    return true
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr, sourceItemId)
    if not GameMatch:isGameRunning() then
        return false
    end
    if sourceItemId == BlockID.TNT then
        attr.custom_explosionSize = GameConfig.tntExplosionSize
        attr.custom_explosionDamageFactor = GameConfig.tntExplosionDamageFactor
        attr.custom_explosionRepelDistanceFactor = GameConfig.tntExplosionRepelDistanceFactor
    else
        attr.isCanHurt = false
    end
    return false
end

function BlockListener.onPlayerPlaceBlockBoxJudge(rakssid, blockId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return true
    end
    return false
end

function BlockListener.onBlockTNTExplodeHurtEntity(sourceItemId, hurtEntityId, placeEntityId)
    local hurter = PlayerManager:getPlayerByEntityId(hurtEntityId)
    if hurter == nil then
        return false
    end

    local attacker = PlayerManager:getPlayerByEntityId(placeEntityId)
    if attacker ~= nil and attacker:getTeamId() == hurter:getTeamId() then
        return false
    end

    local attackerUserId = 0
    if attacker ~= nil then
        attackerUserId = attacker.userId
    end
    hurter:updateHurtInfo(ReportUtil.DeadType.Skill, sourceItemId, 0, attackerUserId)
    return true
end

function BlockListener.onBlockTNTExplodeBreakBlock(blockPos, blockId)
    if not GameMatch:isGameRunning() then
        return false
    end
    if blockId == BlockID.CHEST
            or TableUtil.include(GameConfig.unbreakableBlock, tostring(blockId))
            or (TableUtil.include(BlockConfig.BlockId, blockId)) then
        return false
    end
    return true
end

function BlockListener.onBlockStationaryNotStationary(blockId)
    return false
end

return BlockListener
--endregion
