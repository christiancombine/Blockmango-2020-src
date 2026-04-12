WeaponConfig = {}

WeaponConfig.Gun = {}
WeaponConfig.Bullet = {}

function WeaponConfig:init(config)
    if config.gun ~= nil then
        if #config.gun > 0 then
            for i, v in pairs(config.gun) do
                local item = {}
                item.id = tonumber(v.id)
                item.damage = tonumber(v.damage)
                item.bulletId = tonumber(v.bulletId or "441")
                item.knockback = tonumber(v.knockback or "-1")
                item.maxBulletNum = tonumber(v.maxBulletNum)
                self.Gun[i] = item
            end
        end
    end

    if config.bullet ~= nil then
        if #config.bullet > 0 then
            for i, v in pairs(config.bullet) do
                local item = {}
                item.id = tonumber(v.id)
                item.capacity = tonumber(v.capacity)
                self.Bullet[i] = item
            end
        end
    end
end

function WeaponConfig:prepareWeapon()
    for i, v in pairs(self.Gun) do
        HostApi.setGunSetting(WeaponConfig.newGunSetting(v))
    end

    for i, v in pairs(self.Bullet) do
        HostApi.setBulletClipSetting(v.id, v.capacity)
    end
end

function WeaponConfig.newGunSetting(item)
    local setting = GunPluginSetting.new()
    setting.gunId = item.id
    setting.gunType = 0
    setting.damage = item.damage
    setting.maxBulletNum = item.maxBulletNum
    setting.knockback = item.knockback
    setting.bulletId = item.bulletId
    setting.shootRange = 0
    setting.cdTime = 0
    setting.sniperDistance = -1
    setting.bulletPerShoot = -1
    setting.bulletOffset = -1
    return setting
end

function WeaponConfig:getGunMaxBullet(gunId)
    for i, v in pairs(self.Gun) do
        if v.id == gunId then
            return v.maxBulletNum
        end
    end
    return 10
end

function WeaponConfig:getBulletIdByWeapon(gunId)
    for i, v in pairs(self.Gun) do
        if v.id == gunId then
            return v.bulletId
        end
    end
    return 441
end

return WeaponConfig


