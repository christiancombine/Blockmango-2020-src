require "Match"
ChestListener = {}

function ChestListener:init()
    ChestOpenEvent.registerCallBack(function(vec3, inv, rakssid)
        self:onChestOpen(vec3, inv, rakssid)
    end)
    FurnaceOpenEvent.registerCallBack(function(vec3, inv, rakssid)
        return self:onFurnaceOpen(vec3, inv, rakssid)
    end)
    ChestOpenSpHandleEvent.registerCallBack(self.onChestOpenSpHandle)
    FurnaceOpenSpHandleEvent.registerCallBack(self.onFurnaceOpenSpHandle)
end

function ChestListener:onChestOpen(vec3, inv, rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if not GameMatch:isMaster(player.userId) then
        HostApi.sendCommonTip(player.rakssid, "plase_not_steal")
        return
    end

    local targetManorInfo = GameMatch:getUserManorInfo(player.userId)
    if targetManorInfo ~= nil then
        if not targetManorInfo:isIn(vec3) then
            HostApi.log("onChestOpen Weak net back home bug. userId " .. tostring(player.userId) .. " " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
            return
        end
        HostApi.log("onChestOpen userId " .. tostring(player.userId) .. " " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        targetManorInfo:fillOpenChest(vec3, inv)
    end
end

function ChestListener:onFurnaceOpen(vec3, inv, rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    local master = PlayerManager:getPlayerByUserId(player.target_user_id)
    if master == nil then
        return false
    end

    local masterManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
    if masterManorInfo == nil then
        return false
    end

    if not masterManorInfo:checkPower(player.userId, vec3, "onFurnaceOpen") then
        return false
    end

    if not DbUtil.CanSavePlayerData(master, DbUtil.TAG_FURNACE) then
        return false
    end

    if masterManorInfo:isLoadingBlock() then
        return false
    end

    if not player:checkChunkLoad() then
        return false
    end

    if not masterManorInfo:isIn(vec3) then
        HostApi.log("onFurnaceOpen Weak net back home bug. userId: " .. tostring(player.userId) .. " targetId: " .. tostring(player.target_user_id)
                .. " " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        return false
    end

    HostApi.log("onFurnaceOpen userId: " .. tostring(player.userId) .. " targetId: " .. tostring(player.target_user_id)
            .. " " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)

    masterManorInfo:fillFurnace(vec3, inv)
    return true
end

function ChestListener.onChestOpenSpHandle(vec3, entityId)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil then
        return false
    end

    -- in the main hall
    if player.isGenisland then
        return false
    end

    if not GameMatch:isMaster(player.userId) then
        HostApi.sendCommonTip(player.rakssid, "plase_not_steal")
        return false
    end

    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_CHEST) then
        return false
    end

    local selfManor = GameMatch:getUserManorInfo(player.userId)
    if selfManor == nil then
        return false
    end

    if selfManor:isLoadingBlock() then
        return false
    end

    if not player:checkChunkLoad() then
        return false
    end

    if not selfManor:isIn(vec3) then
        HostApi.log("onChestOpenSpHandle Weak net back home bug. userId: " .. tostring(player.userId) .. " " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        return false
    end

    HostApi.log("onChestOpenSpHandle userId: " .. tostring(player.userId) .. " " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
    return true
end

function ChestListener.onFurnaceOpenSpHandle(vec3, entityId)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil then
        return false
    end

    -- in the main hall
    if player.isGenisland then
        return false
    end

    local master = PlayerManager:getPlayerByUserId(player.target_user_id)
    if master == nil then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 1, Messages:msgCannotOperatorVistor())
        return false
    end

    local masterManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
    if masterManorInfo == nil then
        return false
    end

    if not masterManorInfo:checkPower(player.userId, vec3, "onFurnaceOpenSpHandle") then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 1, Messages:msgCannotOperatorVistor())
        return false
    end

    if not DbUtil.CanSavePlayerData(master, DbUtil.TAG_FURNACE) then
        return false
    end

    if masterManorInfo:isLoadingBlock() then
        return false
    end

    if not player:checkChunkLoad() then
        return false
    end

    if not masterManorInfo:isIn(vec3) then
        HostApi.log("onFurnaceOpenSpHandle Weak net back home bug. userId: " .. tostring(player.userId) .. " targetId: " .. tostring(player.target_user_id)
                .. " " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        return false
    end

    HostApi.log("onFurnaceOpenSpHandle userId: " .. tostring(player.userId) .. " targetId: " .. tostring(player.target_user_id)
            .. " " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
    player:logPlayerBuildGroup("onFurnaceOpenSpHandle")

    return true
end

return ChestListener