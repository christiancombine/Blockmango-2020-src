
require "config.XPlayerShopItems"

XPlayerWeapon = class()

XPlayerWeapon.bindPlayer = nil
XPlayerWeapon.bindWeaponId = nil
XPlayerWeapon.spendMoney = 0
XPlayerWeapon.weaponLv = 1
XPlayerWeapon.maxLevel = 0
XPlayerWeapon.lastItemId = 0
XPlayerWeapon.currentItemId = 0
function XPlayerWeapon:ctor(player, weaponId)
    if player ~= nil then
        self.bindPlayer = player
    end
    if weaponId ~= nil then
        self.bindWeaponId = weaponId
    end
    self.spendMoney = 0
    self.maxLevel = XPlayerShopItems:getMaxLevel(self.bindWeaponId)
    self.lastItemId = self.bindWeaponId
    self.currentItemId = self.bindWeaponId
end
-- 升级物品 升级成功之后返回 下一级物品的Id
function XPlayerWeapon:OnUpgrade()
    local price, itemId = XPlayerShopItems:getPriceTyIteamId(self.bindWeaponId, self.weaponLv)
    if price == nil or itemId == nil then
        return nil
    end

    if self.bindPlayer ~= nil then
        local money = self.bindPlayer:OnChangeMoney(-price)
        if money >= 0 then
            self.lastItemId = self.currentItemId
            self.currentItemId = itemId
            self.weaponLv = self.weaponLv + 1
            self.spendMoney = self.spendMoney + price
            local data = XPlayerShopItems:getItemInfoByItemId(itemId)
            if data ~= nil then
                data.status = 0
                data.groupId = "1"
                data.id = tostring(self.bindWeaponId)
                if self.weaponLv == self.maxLevel then
                    data.status = 9
                    data.price = 0
                end
               
                return {data}
            end
        end
    end
    return nil
end

function XPlayerWeapon:getItems()
    return self.lastItemId, self.currentItemId
end
