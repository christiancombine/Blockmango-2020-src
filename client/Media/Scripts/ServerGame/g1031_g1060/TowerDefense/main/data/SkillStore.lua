SkillStore = {}

function SkillStore:init()
    --TODO
end

function SkillStore:onBuyPayItem(player, skillId)
    if not player then
        return
    end

    local config = SkillStoreConfig:getConfigById(skillId)
    if not config then
        return
    end

    if not player.paySkillOffList[skillId] then
        return
    end

    if player.paySkillOffList[skillId].onUse == true then
        return
    end

    local process = ProcessManager:getProcessByProcessId(player.processId)
    if not process then
        return
    end

    if process.mapManager:getMapHp() <= 0 then
        return
    end

    if player.paySkillOffList[skillId] and player.paySkillOffList[skillId].off and #player.paySkillOffList[skillId].off > 0 then
        if player.paySkillOffList[skillId].off[1] == 0 then
            player.paySkillOffList[skillId].onUse = true
            self:onBuyPaySkillSuccess(player, skillId, 0)
        elseif player.paySkillOffList[skillId].off[1] > 0 then
            PayHelper.payDiamondsSync(player.userId, config.id, player.paySkillOffList[skillId].off[1], function(player)
                player.paySkillOffList[skillId].onUse = true
                self:onBuyPaySkillSuccess(player, skillId, player.paySkillOffList[skillId].off[1])
            end, nil)
        end
    else
        PayHelper.payDiamondsSync(player.userId, config.id, config.price, function(player)
            player.paySkillOffList[skillId].onUse = true
            self:onBuyPaySkillSuccess(player, skillId, config.price)
        end, nil)
    end
end

function SkillStore:onBuyPaySkillSuccess(player, skillId, price)
    local config = SkillStoreConfig:getConfigById(skillId)
    local process = ProcessManager:getProcessByProcessId(player.processId)
    local propConfig = PropsConfig:getConfigById(config.prop)
    ReportUtil.reportPlayerBuySkill(player, skillId, price)

    if not process then
        LogUtil.log("[PlayerUserPropsEvent] can't find progress userId = " .. tostring(player.userId))
        return
    end
    process:useProps(tonumber(config.prop), player.userId)

    player:onBuyPaySkill(skillId, propConfig.cd)

    local skillInfo = {
        skillId = skillId,
        isFree = false,
        isOff = false,
        price = player.paySkillOffList[skillId].price,
        off = player.paySkillOffList[skillId].off[1] or player.paySkillOffList[skillId].price,
        propId = config.prop
    }

    if skillInfo.off == 0 then
        skillInfo.isFree = true
    elseif skillInfo.price > skillInfo.off then
        skillInfo.isOff = true
    end

    GamePacketSender:sendBuySkillSuccess(player.rakssid, skillInfo)
    --通知玩家购买成功
end

return SkillStore