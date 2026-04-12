
AppShopConfig = {}
AppShopConfig.shopList = {}
AppShopConfig.sortList = {}

AppShopConfig.GoodType = {
    item = 1,
    privilege = 2,
    npc = 3,
    money = 4,
    block = 5,
}

AppShopConfig.TabType = {
    new_block = 3,
    old_privilege = 5,
}


AppShopConfig.OperateType = {
    BUY_GOOD = 0,
    UPDATE_DATA = 1,
}

AppShopConfig.ClickType = {
    CLICK_ITEM = 1,
    CLICK_PRI = 2,
    CLICK_LOOK_AD = 3
}

function AppShopConfig:initConfig(config)
    for _, config in pairs(config) do
        local item = {}
        item.Id = tonumber(config.Id) or 0
        item.TabId = tonumber(config.TabId) or 0
        item.Icon = config.Icon == "#" and "" or config.Icon
        item.ItemId = tonumber(config.ItemId) or 0
        item.HaveItem = tostring(config.HaveItem) or ""
        item.Meta = tonumber(config.Meta) or 0
        item.ItemNum = tonumber(config.ItemNum) or 1
        item.GoodType = tonumber(config.GoodType) or 0
        item.CoinId = tonumber(config.CoinId) or 0
        item.CoinNum1 = tonumber(config.CoinNum1) or 0
        item.CoinNum2 = tonumber(config.CoinNum2) or 0
        item.Time = tonumber(config.Time) or 0
        item.Level = tonumber(config.Level) or 1
        item.ShowPriImg = StringUtil.split(tostring(config.ShowPriImg), "#") or ""
        local ever = tonumber(config.Forever) or 0
        item.Forever = ever == 1 and true or false
        
        local priBlock = tonumber(config.PriBlock) or 0
        item.PriBlock = priBlock == 1 and true or false
        
        local priNpc = tonumber(config.PriNpc) or 0
        item.PriNpc = priNpc == 1 and true or false

        AppShopConfig.shopList[item.Id] = item
    end
end

function AppShopConfig:initSortConfig(config)
    for _, config in pairs(config) do
        local item = {}
        item.Id = tonumber(config.Id) or 0
        item.MinLevel = tonumber(config.MinLevel) or 1
        item.MaxLevel = tonumber(config.MaxLevel) or 1
        item.SortType = tonumber(config.SortType) or 1
        AppShopConfig.sortList[item.Id] = item
    end
end

function AppShopConfig:getAllNewPriData()
    local data = {}
    for k, goods in pairs(self.shopList) do
        if goods.GoodType == AppShopConfig.GoodType.privilege and goods.TabId ~= AppShopConfig.TabType.old_privilege then
            table.insert(data, goods)
        end
    end
    return data
end

function AppShopConfig:getItemCfgById(id)
    local data = AppShopConfig.shopList[id]
    if data then
        return data
    end
end

function AppShopConfig:getItemCfgByItemId(id)
    for i, v in pairs(AppShopConfig.shopList) do
        if v.ItemId == tonumber(id) then
            return  v
        end
    end
    return nil
end

function AppShopConfig:getSortTypeByScore(level)
    local sortType = 1
    for k, v in pairs(AppShopConfig.sortList) do
        if level >= v.MinLevel and level <= v.MaxLevel then
            sortType = v.SortType
            break
        end
    end
    return sortType
end

function AppShopConfig:getPriNpcIdList()
    local data = {}
    for k, v in pairs(AppShopConfig.shopList) do
        if v.PriNpc then
            table.insert(data, v.Id)
        end
    end
    return data
end

function AppShopConfig:BuyGoods(rakssid, indexId, num)
    
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if not player:CanSavePlayerShareData() then
        return
    end

    local shopItem = self.shopList[indexId]
    if shopItem == nil then
        return 
    end

    if self:canBuyCurItemByInv(player, indexId, num) == false then
        HostApi.sendCommonTip(player.rakssid, "gui_inventory_not_enough")
        return
    end
    
    if player.level < shopItem.Level then
        return
    end

    local diamond = player:getDiamond()
    if shopItem.GoodType == AppShopConfig.GoodType.privilege then
        if player.enableIndie then
            GameAnalytics:design(player.userId, 0, { "g1048buy_privilege_indie", shopItem.Id, diamond, num})
            GameAnalytics:design(player.userId, 0, { "g1048buy_privilege_indie_new", shopItem.Id})
        else
            GameAnalytics:design(player.userId, 0, { "g1048buy_privilege", shopItem.Id, diamond, num})
            GameAnalytics:design(player.userId, 0, { "g1048buy_privilege_new", shopItem.Id})
        end
    else
        if player.enableIndie then
            GameAnalytics:design(player.userId, 0, { "g1048buy_item_indie", shopItem.Id, diamond, num})
            GameAnalytics:design(player.userId, 0, { "g1048buy_item_indie_new", shopItem.Id})
        else
            GameAnalytics:design(player.userId, 0, { "g1048buy_item", shopItem.Id, diamond, num})
            GameAnalytics:design(player.userId, 0, { "g1048buy_item_new", shopItem.Id})
        end
    end

    local price_id = shopItem.CoinId
    local price_num = shopItem.CoinNum1

    if AppShopConfig.GoodType.block == shopItem.GoodType then
        if player:checkIsHAvePri(PrivilegeConfig.priType.block_use_money) then
            HostApi.log("block_use_money userId == "..tostring(player.userId))
            price_id = Define.Money.MONEY
            price_num = shopItem.CoinNum2
        end

        if shopItem.PriBlock then
            if not player:checkIsHAvePri(PrivilegeConfig.priType.pri_of_block) then
                return
            end
        end

        if player:checkIsHAvePri(PrivilegeConfig.priType.buy_block_half) then
            price_num = math.floor(price_num / 2)
            if price_num < 1 then
                price_num = 1
            end
            HostApi.log("buy_block_half"..price_num .." userId == "..tostring(player.userId))
        end
    end

    if AppShopConfig.GoodType.npc == shopItem.GoodType then        
        if shopItem.PriNpc then
            if not player:checkIsHAvePri(PrivilegeConfig.priType.pri_of_npc) then
                return
            end
        end

        for k, v in pairs(player.products) do
            if shopItem.ItemId == v.Id and v.Time == -1 then
                return
            end
        end
    end

    if AppShopConfig.GoodType.privilege == shopItem.GoodType then        
        if not player:checkIsCanBuyPri(shopItem.ItemId) then
            return 
        end
    end

    if (price_num * num) <= 0 then
        return
    end 

    if price_id == Define.Money.DIAMOND then
        if player.canSendCostDiamond == false then
            return
        end
        
        if player:checkMoney(Define.Money.DIAMOND, price_num * num) then
            player.canSendCostDiamond = false
            player:consumeDiamonds(1000 + indexId, price_num * num, Define.buyGood .. "#" .. indexId .."@"..num)
        end
        return
    end
    
    if price_id == Define.Money.MONEY then
        if player:checkMoney(Define.Money.MONEY, price_num * num) then
            -- player.money = player.money - price_num * num
            player:subCurrency(price_num * num)
            GameAnalytics:design(player.userId, 0, { "g1048subMoney", tostring(player.userId), price_num * num})
            self:BuyGoodSuccess(player, indexId, num)
        end
        return
    end
end

function AppShopConfig:BuyGoodSuccess(player, index, num)
    local shopItem = self.shopList[index]
    if shopItem == nil then
        return 
    end
    HostApi.log(tostring(player.userId).."  BuyGoodSuccess  index == "..index.."  num == "..num)

    HostApi.notifyGetGoodsByItemId(player.rakssid, shopItem.ItemId, shopItem.Meta, shopItem.ItemNum * num,  shopItem.Icon)
    if shopItem.GoodType == AppShopConfig.GoodType.item or shopItem.GoodType == AppShopConfig.GoodType.block then
        player.taskController:packingProgressData(player, Define.TaskType.collect, GameMatch.gameType, shopItem.ItemId, num, shopItem.Meta)
        player:addItem(shopItem.ItemId, num, shopItem.Meta)  
    end

    if shopItem.GoodType == AppShopConfig.GoodType.npc then
        self:unLockNpc(player, index, num)
    end

    if shopItem.GoodType == AppShopConfig.GoodType.privilege then
        self:buyPrivilege(player, index, num, false)
        player:sendSkyBlockShopInfo()
        player:checkShowPriSign()
    end

    if shopItem.GoodType == AppShopConfig.GoodType.money then
        player:addCurrency(shopItem.ItemNum * num)
    end

    PrivilegeConfig:setPriImgById(player, shopItem.ItemId)
end

function AppShopConfig:buyPrivilege(player, index, num, recive)
    if not player:CanSavePlayerShareData() then
        return
    end
    local shopItem = self.shopList[index]
    if shopItem == nil then
        return 
    end


    local cur_pri = PrivilegeConfig:getPrivilege(shopItem.ItemId)

    if cur_pri == nil then
        return 
    end

    local cur_recive = recive 
    if shopItem.Forever then
        cur_recive = true
    end

    if not player:checkIsCanBuyPri(shopItem.ItemId) then
        return 
    end

    HostApi.log(tostring(player.userId).."  buyPrivilege  index == "..index.."num == "..num)

    for k, v in pairs(cur_pri.Privilege) do
        local priItem = PrivilegeConfig:getPrivilegeItem(tonumber(v))
        if priItem then
            if (priItem.PriType == PrivilegeConfig.priType.gift_iron_throne) then
                HostApi.log("gift_iron_throne userId == "..tostring(player.userId))
                player:addItem(tonumber(priItem.Detail), priItem.Num, 0)
            end
    
            if (priItem.PriType == PrivilegeConfig.priType.gift_money) then
                -- player.money = player.money + tonumber(priItem.Detail)
                local money = player.money + tonumber(priItem.Detail)
                player:setCurrency(money)
                GameAnalytics:design(player.userId, 0, { "g1048addMoney", tostring(player.userId), tonumber(priItem.Detail)})
                HostApi.log("gift_money userId == "..tostring(player.userId))
            end
    
            if (priItem.PriType == PrivilegeConfig.priType.gift_equip_tool) then
                
                HostApi.log("gift_equip_tool userId == "..tostring(player.userId))
                local itmeList = StringUtil.split(priItem.Detail, "#")
                for _, item_id in pairs(itmeList) do    
                    player:addItem(tonumber(item_id), 1, 0)
                end
            end
    
            if (priItem.PriType == PrivilegeConfig.priType.pri_of_npc) then
                HostApi.log("pri_of_npc userId == "..tostring(player.userId))
                local priNpcList = self:getPriNpcIdList()
                for _, npcId in pairs(priNpcList) do   
                    self:unLockNpc(player, tonumber(npcId), 1)
                end
            end

            if (priItem.PriType == PrivilegeConfig.priType.unlock_common_npc) then
                HostApi.log("unlock_common_npc userId == "..tostring(player.userId)) 
                local itmeList = StringUtil.split(priItem.Detail, "#")
                for _, item_id in pairs(itmeList) do
                    self:unLockNpc(player, tonumber(item_id), 1)
                end
            end
    
            if (priItem.PriType== PrivilegeConfig.priType.forever_all_pri 
                or priItem.PriType== PrivilegeConfig.priType.forever_new_all_pri) then
                HostApi.log("forever_all_pri userId == "..tostring(player.userId))
                local itmeList = StringUtil.split(priItem.Detail, "#")
                for _, item_id in pairs(itmeList) do
                    self:buyPrivilege(player, tonumber(item_id), 1, true)
                end
            end
        end
    end

    for k, v in pairs(player.privilege) do
        if shopItem.Id == v.id then
            if shopItem.Forever == false then
                if v.time < os.time() then
                    v.time = os.time()
                end
                v.time = v.time + shopItem.Time * num
            else
                v.forever = shopItem.Forever
            end

            if not v.recive then
                v.recive = cur_recive
            end

            PrivilegeConfig:setplayerPrivilegeItem(v, player)
            PrivilegeConfig:setPriImgById(player, shopItem.Id)
            return 
        end
    end

    local data ={
        id = shopItem.Id,
        itemId = shopItem.ItemId,
        forever = shopItem.Forever,
        time = os.time() + shopItem.Time * num,
        recive = cur_recive
    }
    
    table.insert(player.privilege, data)
    PrivilegeConfig:setplayerPrivilegeItem(data, player)
    PrivilegeConfig:setPriImgById(player, shopItem.ItemId)
end

function AppShopConfig:UnLockPrivilegeByOtherPri(player, index, num, recive)
    if not player:CanSavePlayerShareData() then
        return
    end

    local shopItem = self.shopList[index]
    if shopItem == nil then
        return
    end

    if not player:checkIsCanBuyPri(shopItem.ItemId) then
        return
    end

    HostApi.log(tostring(player.userId).."  UnLockPrivilegeByOtherPri  index == "..index.."num == "..num)

    local cur_pri = PrivilegeConfig:getPrivilege(shopItem.ItemId)

    if cur_pri == nil then
        return
    end
    
    local cur_recive = recive 

    if shopItem.Forever then
        cur_recive = true
    end

    for k, v in pairs(player.privilege) do
        if shopItem.Id == v.id then
            if shopItem.Forever == false then
                if v.time < os.time() then
                    v.time = os.time()
                end
                v.time = v.time + shopItem.Time * num
            else
                v.forever = shopItem.Forever
            end

            if not v.recive then
                v.recive = cur_recive
            end

            PrivilegeConfig:setplayerPrivilegeItem(v, player)
            PrivilegeConfig:setPriImgById(player, shopItem.ItemId)
            return
        end
    end

    local data ={
        id = shopItem.Id,
        itemId = shopItem.ItemId,
        forever = shopItem.Forever,
        time = os.time() + shopItem.Time * num,
        recive = cur_recive
    }

    table.insert(player.privilege, data)
    PrivilegeConfig:setplayerPrivilegeItem(data, player)
    PrivilegeConfig:setPriImgById(player, shopItem.ItemId)
end

function AppShopConfig:unLockNpc(player, index, num)

    local shopItem = self.shopList[index]
    if shopItem == nil then
        return 
    end

    for k, v in pairs(player.products) do
        if shopItem.ItemId == v.Id and v.Time == -1 then
            return
        end
    end

    HostApi.log("shopItem.ItemId =="..shopItem.ItemId)
    player:insertProductsInfo(shopItem.ItemId, shopItem.Id, num)
    PrivilegeConfig:checkUnlockThreeNpc(player)
    player:sendSkyBlockShopInfo()
    player.taskController:packingProgressData(player, Define.TaskType.have_products, GameMatch.gameType, shopItem.ItemId, 1, 0)
end

function AppShopConfig:canBuyCurItemByInv(player, index, num)
    local shopItem =  self:getItemCfgById(index)
    if shopItem == nil then
        return false
    end

    if shopItem.GoodType == AppShopConfig.GoodType.privilege then
        if PrivilegeConfig:checkCanBuyPri(player, index) then
            return true
        else
            return false
        end
    end

    if shopItem.GoodType == AppShopConfig.GoodType.item or shopItem.GoodType == AppShopConfig.GoodType.block then
        local inv = player:getInventory()
        if inv then
            if inv:isCanAddItem(shopItem.ItemId, shopItem.Meta, num) then
                return true
            else
                return false
            end
        end
    end

    return true
end
return AppShopConfig