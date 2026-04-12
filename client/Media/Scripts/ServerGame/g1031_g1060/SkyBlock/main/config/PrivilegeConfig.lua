PrivilegeConfig = {}
PrivilegeConfig.privilege = {}
PrivilegeConfig.item = {}

PrivilegeConfig.priType = {
    block_use_money = 1,	    --魔方方块可用金币买
    extend_area_half = 2,	    --解锁岛屿半价
    pri_of_block = 3,	        --解锁专属方块
    gift_iron_throne = 4,	    --赠送铁王座
    unlock_fly = 5,	            --解锁飞行
    buy_block_half = 6,	        --解锁所有方块打5折
    pri_of_npc = 7,	            --解锁赠送专属矿机
    gift_money = 8,	            --赠送游戏币
    gift_equip_tool = 9,	    --赠送全套紫金装备和工具
    unlock_common_npc = 10,	    --赠送所有普通挖掘机
    up_npc_half = 11,           --所有挖掘机半价升级
    npc_eff_double = 12,        --所有挖掘机产出效率加倍
    un_lock_sky = 13,           --解锁天空
    forever_all_pri = 14, 	    --永久解锁所有旧特权
    forever_new_all_pri = 15    --永久解锁所有新特权
}

function PrivilegeConfig:initPrivilege(config)
    for i, v in pairs(config) do
        local item = {}
        item.Id = tonumber(v.Id) or 0
        item.Privilege = StringUtil.split(v.Privilege, "#") or ""
        PrivilegeConfig.privilege[item.Id] = item or 0
    end
end

function PrivilegeConfig:initItem(config)
    for i, v in pairs(config) do
        local item = {}
        item.Id = tonumber(v.Id) or 0
        item.PriType = tonumber(v.PriType) or 0
        item.Detail = tostring(v.Detail) or ""
        item.Num = tonumber(v.Num) or 0
        PrivilegeConfig.item[item.Id] = item
    end
end

function PrivilegeConfig:getPrivilege(id)
    for k, v in pairs(PrivilegeConfig.privilege) do
        if v.Id == id then
            return v
        end
    end
end

function PrivilegeConfig:getPrivilegeItem(id)
    for k, v in pairs(PrivilegeConfig.item) do
        if v.Id == id then
            return v
        end
    end
end

function PrivilegeConfig:getPrivilegeItemByType(curType)
    for k, v in pairs(PrivilegeConfig.item) do
        if v.PriType == curType then
            return v
        end
    end
end

function PrivilegeConfig:setplayerPrivilege(player)
    for _, pri in pairs(player.privilege) do
        if pri.recive == nil or pri.forever == true then
            pri.recive = true
        end
        self:setplayerPrivilegeItem(pri, player)
    end
    self:canReciveNewPri(player)
end

function PrivilegeConfig:checkPlayerShowPri(player)
    if not player.privilege then
        return false
    end
    for _, pri in pairs(player.privilege) do
        local cur_pri = PrivilegeConfig:getPrivilege(pri.itemId)
        if cur_pri then
            for k, v in pairs(cur_pri.Privilege) do
                local priItem = PrivilegeConfig:getPrivilegeItem(tonumber(v))
                if priItem and (priItem.PriType == PrivilegeConfig.priType.forever_all_pri 
                            or priItem.PriType == PrivilegeConfig.priType.forever_new_all_pri) then
                    return true
                end
            end
        end
    end
    return false
end

function PrivilegeConfig:canReciveNewPri(player)
    local isHaveAllForever = false
    local NeedRecivePriId = {}

    for _, pri in pairs(player.privilege) do
        local cur_pri = PrivilegeConfig:getPrivilege(pri.itemId)
        if cur_pri then
            for k, v in pairs(cur_pri.Privilege) do
                local priItem = PrivilegeConfig:getPrivilegeItem(tonumber(v))
                if priItem and priItem.PriType == PrivilegeConfig.priType.forever_all_pri then
                    isHaveAllForever = true
                    break
                end
            end
        end
    end


    if isHaveAllForever then
        local forever_all_pri = self:getPrivilegeItemByType(PrivilegeConfig.priType.forever_all_pri)
        if forever_all_pri then
            local itmeList = StringUtil.split(forever_all_pri.Detail, "#")
            for _, pri_item in pairs(itmeList) do
                local is_recive_pri = false
                for _, pri in pairs(player.privilege) do
                    if tonumber(pri_item) == tonumber(pri.id) and pri.recive then
                        is_recive_pri = true
                        break
                    end
                end

                if is_recive_pri == false then
                    local curConfig = AppShopConfig:getItemCfgById(tonumber(pri_item))
                    if curConfig then
                        table.insert(NeedRecivePriId, tonumber(pri_item)) 
                    end
                end
            end
        end
        
        if #NeedRecivePriId > 0 then
            GamePacketSender:sendCanReciveNewPri(player.rakssid, true, NeedRecivePriId)
            player.recive_new_prive = false
            return
        end
    end
    PrivilegeConfig:canReciveNewPriGift(player)
end

function PrivilegeConfig:canReciveNewPriGift(player)
    local isHaveAllForever = false
    local NeedRecivePriId = {}

    for _, pri in pairs(player.privilege) do
        local cur_pri = PrivilegeConfig:getPrivilege(pri.itemId)
        if cur_pri then
            for k, v in pairs(cur_pri.Privilege) do
                local priItem = PrivilegeConfig:getPrivilegeItem(tonumber(v))
                if priItem and priItem.PriType == PrivilegeConfig.priType.forever_new_all_pri then
                    isHaveAllForever = true
                    break
                end
            end
        end
    end


    if isHaveAllForever then
        local forever_new_all_pri = self:getPrivilegeItemByType(PrivilegeConfig.priType.forever_new_all_pri)
        if forever_new_all_pri then
            local itmeList = StringUtil.split(forever_new_all_pri.Detail, "#")
            for _, pri_item in pairs(itmeList) do
                local is_recive_pri = false
                for _, pri in pairs(player.privilege) do
                    if tonumber(pri_item) == tonumber(pri.id) and pri.recive then
                        is_recive_pri = true
                        break
                    end
                end

                if is_recive_pri == false then
                    local curConfig = AppShopConfig:getItemCfgById(tonumber(pri_item))
                    if curConfig then
                        table.insert(NeedRecivePriId, tonumber(pri_item)) 
                    end
                end
            end
        end
        
        if #NeedRecivePriId > 0 then
            GamePacketSender:sendCanReciveNewPri(player.rakssid, true, NeedRecivePriId)
            player.recive_new_prive = true
            return
        end
    end
end

function PrivilegeConfig:onReciveNewPri(player)
    local isHaveAllForever = false
    local curNeedReciveItem = ""
    local needRecivePriId = {}
    for _, pri in pairs(player.privilege) do
        local cur_pri = PrivilegeConfig:getPrivilege(pri.itemId)
        if cur_pri then
            for k, v in pairs(cur_pri.Privilege) do
                local priItem = PrivilegeConfig:getPrivilegeItem(tonumber(v))
                if priItem and priItem.PriType == PrivilegeConfig.priType.forever_all_pri then
                    isHaveAllForever = true
                    break
                end
            end
        end
    end


    if isHaveAllForever then
        local forever_all_pri = self:getPrivilegeItemByType(PrivilegeConfig.priType.forever_all_pri)
        if forever_all_pri then
            local itmeList = StringUtil.split(forever_all_pri.Detail, "#")
            for _, pri_item in pairs(itmeList) do
                local is_recive_pri = false
                for _, pri in pairs(player.privilege) do
                    if tonumber(pri_item) == tonumber(pri.id) and pri.recive then
                        is_recive_pri = true
                        break
                    end
                end

                if is_recive_pri == false then
                    local curConfig = AppShopConfig:getItemCfgById(tonumber(pri_item))
                    if curConfig then 
                        table.insert(needRecivePriId, tonumber(pri_item))
                        if curConfig.HaveItem ~= "#" then
                            if curNeedReciveItem == "" then
                                curNeedReciveItem = curConfig.HaveItem
                            else
                                curNeedReciveItem = curNeedReciveItem.."#"..curConfig.HaveItem
                            end
                        end
                    end
                end
            end
        end

        if self:isCanBuyPrivilegeByItems(player, curNeedReciveItem, 1) then
            for k, v in pairs(needRecivePriId) do
                AppShopConfig:buyPrivilege(player, v, 1, true)
            end
            GamePacketSender:sendCanReciveNewPri(player.rakssid, false)
            player:sendSkyBlockShopInfo()
            player:checkShowPriSign()
        end
    end

    PrivilegeConfig:canReciveNewPriGift(player)
end

function PrivilegeConfig:onReciveNewPriGift(player)
    local isHaveAllForever = false
    local curNeedReciveItem = ""
    local needRecivePriId = {}
    for _, pri in pairs(player.privilege) do
        local cur_pri = PrivilegeConfig:getPrivilege(pri.itemId)
        if cur_pri then
            for k, v in pairs(cur_pri.Privilege) do
                local priItem = PrivilegeConfig:getPrivilegeItem(tonumber(v))
                if priItem and priItem.PriType == PrivilegeConfig.priType.forever_new_all_pri then
                    isHaveAllForever = true
                    break
                end
            end
        end
    end


    if isHaveAllForever then
        local forever_new_all_pri = self:getPrivilegeItemByType(PrivilegeConfig.priType.forever_new_all_pri)
        if forever_new_all_pri then
            local itmeList = StringUtil.split(forever_new_all_pri.Detail, "#")
            for _, pri_item in pairs(itmeList) do
                local is_recive_pri = false
                for _, pri in pairs(player.privilege) do
                    if tonumber(pri_item) == tonumber(pri.id) and pri.recive then
                        is_recive_pri = true
                        break
                    end
                end

                if is_recive_pri == false then
                    local curConfig = AppShopConfig:getItemCfgById(tonumber(pri_item))
                    if curConfig then 
                        table.insert(needRecivePriId, tonumber(pri_item))
                        if curConfig.HaveItem ~= "#" then
                            if curNeedReciveItem == "" then
                                curNeedReciveItem = curConfig.HaveItem
                            else
                                curNeedReciveItem = curNeedReciveItem.."#"..curConfig.HaveItem
                            end
                        end
                    end
                end
            end
        end

        if self:isCanBuyPrivilegeByItems(player, curNeedReciveItem, 1) then
            for k, v in pairs(needRecivePriId) do
                AppShopConfig:buyPrivilege(player, v, 1, true)
            end
            GamePacketSender:sendCanReciveNewPri(player.rakssid, false)
            player:sendSkyBlockShopInfo()
            player:checkShowPriSign()
            return
        end
    end 
end

function PrivilegeConfig:isCanBuyPrivilegeByItems(player, needReciveItem, num)
    local inv = player:getInventory()
    if inv then
        if inv:SkyBlockisCanBuyPrivilege(needReciveItem, num) == false then
            HostApi.sendCommonTip(player.rakssid, "gui_inventory_not_enough")
            return false
        end
    end
    return true
end

function PrivilegeConfig:setplayerPrivilegeItem(pri, player)

    local priConfig = self:getPrivilege(pri.itemId)
    if priConfig == nil then
        return
    end

    for k, v in pairs(priConfig.Privilege) do
        local ishave = false
        local priConfigItem = self:getPrivilegeItem(tonumber(v))
        if priConfigItem then
            for _, have_pri in pairs(player.cur_pri_item) do
                if have_pri.Id == priConfigItem.Id then
                    ishave = true
                    if (pri.forever) then
                        have_pri.Forever = pri.forever
                    end

                    if have_pri.Forever == false then
                        have_pri.Time = have_pri.Time >= pri.time and have_pri.Time or pri.time
                    end
                    break
                end
            end

            if ishave == false then
                local data = {}
                data.Id = priConfigItem.Id
                data.PriType = priConfigItem.PriType
                data.Detail = priConfigItem.Detail
                data.Forever = pri.forever
                data.Time = pri.time
                table.insert(player.cur_pri_item, data)
            end
        end
    end
    self:CheckisCanUnLockOtherPri(player)
    self:checkSpePri(player)
end

function PrivilegeConfig:CheckisCanUnLockOtherPri(player)
    local priData = AppShopConfig:getAllNewPriData()
    for _, goods in pairs(priData) do
        if tonumber(goods.ItemId) ~= Define.ALL_PRI and tonumber(goods.ItemId) ~= Define.NEW_ALL_PRI then
            if player:checkIsCanUnlockByOther(tonumber(goods.ItemId)) then
                local priItemList = PrivilegeConfig:getPrivilege(tonumber(goods.ItemId))
                if priItemList then
                    if self:thePriCanUnLockByOtherPri(player, priItemList) then
                        AppShopConfig:UnLockPrivilegeByOtherPri(player, goods.ItemId, 1, false)
                        return
                    end
                end
            end
        end
    end
end

function PrivilegeConfig:thePriCanUnLockByOtherPri(player, priItemList)
    for _, priItem in pairs(priItemList.Privilege) do
        local isHave = false
        for _, havePri in pairs(player.cur_pri_item) do
            if tostring(priItem) == tostring(havePri.Id) then
                isHave = true
            end
        end

        if isHave == false then
            return false
        end
    end
    return true
end


function PrivilegeConfig:checkSpePri(player)
    if player:checkIsHAvePri(PrivilegeConfig.priType.unlock_fly) or player.isGenisland then
        -- 发送飞行剩余时间
        local remainingTime = 0
        local isForever = true
        for i, v in pairs(player.privilege) do
            if v.id == 3201 and v.forever == false then
                remainingTime = v.time - os.time()
                isForever = false
                break
            end
        end

        local data = {
            remainingTime = remainingTime,
            isForever = isForever
        }
        GamePacketSender:sendFlyRemainingTime(player.rakssid, data)

        player:setAllowFlying(true)
    else
        player:setAllowFlying(false)
    end

    if player:checkIsHAvePri(PrivilegeConfig.priType.un_lock_sky) then
        player.isSky = true
        HostApi.setCustomSceneSwitch(player.rakssid, 2, 2, true, true, true)
    else
        player.isSky = false
        HostApi.setCustomSceneSwitch(player.rakssid, 2, 2, true, false, true)
    end
end


function PrivilegeConfig:checkUnlockThreeNpc(player)
    local priId = Define.PriItemId.THREE_NPC
    if player:checkIsCanBuyPri(priId) == false then
        return
    end

    local cur_cfg = PrivilegeConfig:getPrivilege(priId)

    for _, pri in pairs(cur_cfg.Privilege) do
        local priItemCfg = PrivilegeConfig:getPrivilegeItem(tonumber(pri))
        if priItemCfg == nil then
            return
        end

        local ishave = false
        local  cur_list = StringUtil.split(priItemCfg.Detail, "#") or {}
        for _, npcId in pairs(cur_list) do
            for _, pro in pairs(player.products) do
                if tostring(pro.ShopId)  == tostring(npcId) then
                    ishave = true
                    break
                end
            end
        end

        if ishave == false then
            return
        end
    end

    AppShopConfig:buyPrivilege(player, priId, 1, false)    
    player:sendSkyBlockShopInfo()
    player:checkShowPriSign()
end


function PrivilegeConfig:setPrivilegePriImg(player)
    for _, pri in pairs(player.privilege) do
        self:setPriImgById(player, pri.itemId)
    end
end

function PrivilegeConfig:setInventoryPriImg(player)
    local inv = player:getInventory()
    for i = 1, InventoryUtil.MAIN_INVENTORY_COUNT + InventoryUtil.ARMOR_INVENTORY_COUNT do
        local info = inv:getItemStackInfo(i - 1)
        if info.id > 0 and info.id < 10000 then
            self:setPriImgById(player, info.id)
        end
    end
end

function PrivilegeConfig:setPriImgById(player, id)
    local cfg = AppShopConfig:getItemCfgByItemId(id)
    if cfg == nil then
        return
    end

    local isChange = false
    for _, imgId in pairs(cfg.ShowPriImg) do
        local isHave = false
        for _, haveId in pairs(player.havePriImgList) do
            if tonumber(imgId) == tonumber(haveId) then
                isHave = true 
                break
            end
        end

        if isHave == false then
            table.insert(player.havePriImgList, tonumber(imgId))
            isChange = true
        end
    end

    if isChange then
        player:sendSkyBlockPriImg()
    end
end

function PrivilegeConfig:checkCanBuyPri(player, id)
    local curConfig = AppShopConfig:getItemCfgById(tonumber(id))
    if curConfig then 
        if curConfig.HaveItem ~= "#" then
            if self:isCanBuyPrivilegeByItems(player, curConfig.HaveItem, 1) then
                return true
            end
        else
            return true
        end
    end
    return false
end


return PrivilegeConfig