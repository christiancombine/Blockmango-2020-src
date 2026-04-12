GunConfig = {}

local GunConfigs = {}

function GunConfig:init(guns)
    self:initGuns(guns)
end

function GunConfig:initGuns(guns)
    for id, gun in pairs(guns) do
        local item = {
            gunId = tonumber(id),
            itemId = tonumber(gun.itemId),
            type = tonumber(gun.type),
            name = gun.name,
            image = gun.image,
            spareBullet = tonumber(gun.spareBullet),
            bulletId = tonumber(gun.bulletId),
            bulletNum = tonumber(gun.maxBulletNum),
            hurtBlock = tonumber(gun.hurtBlock),
            pickupBulletNum = tonumber(gun.pickupBulletNum),
            attack = tonumber(gun.attack),
            range = tonumber(gun.range),
            speed = tonumber(gun.speed),
            knockback = tonumber(gun.knockback),
            recoil = tonumber(gun.recoil),
            sniperDistance = tonumber(gun.sniperDistance),
            bulletPerShoot = tonumber(gun.bulletPerShoot),
            bulletOffset = tonumber(gun.bulletOffset),
            gunHeadShot = tonumber(gun.gunHeadShot),
            mouseSensitivity = tonumber(gun.mouseSensitivity),
            bulletEffect = gun.bulletEffect or "",
            flameEffect = gun.flameEffect or "",
            isPiercing = tonumber(gun.isPiercing),
            explosionRange = tonumber(gun.explosionRange),
            gravityVelocity = tonumber(gun.gravityVelocity),
            bounceTimes = tonumber(gun.bounceTimes),
            isAutoShoot = tonumber(gun.isAutoShoot),
            property = self:parseProperties(gun.property),
        }
        GunConfigs[gun.itemId] = item
    end
    self:prepareGuns()
end

function GunConfig:prepareGuns()
    for i, gun in pairs(GunConfigs) do
        HostApi.setBulletClipSetting(gun.bulletId, 64)
        HostApi.setGunSetting(GunConfig.newGunSetting(gun))
    end
end

function GunConfig.newGunSetting(item)
    local setting = GunPluginSetting.new()
    setting.gunId = item.itemId
    setting.gunType = item.type
    setting.maxBulletNum = item.bulletNum
    setting.damage = item.attack
    setting.knockback = item.knockback
    setting.bulletId = item.bulletId
    setting.shootRange = item.range
    setting.cdTime = item.speed
    setting.sniperDistance = item.sniperDistance
    setting.mouseSensitivity = item.mouseSensitivity
    setting.bulletPerShoot = item.bulletPerShoot
    setting.bulletOffset = item.bulletOffset
    setting.headshotDamage = item.gunHeadShot
    setting.recoil = item.recoil
    if #item.bulletEffect > 7 then
        setting.bulletEffect = item.bulletEffect
    end
    if #item.flameEffect > 7 then
        setting.flameEffect = item.flameEffect
    end
    setting.isPiercing = item.isPiercing
    setting.explosionRange = item.explosionRange
    setting.gravityVelocity = item.gravityVelocity
    setting.bounceTimes = item.bounceTimes
    setting.isAutoShoot = item.isAutoShoot
    setting.property = item.property
    return setting
end

function GunConfig:getGunByItemId(id)
    return GunConfigs[tostring(id)]
end

function GunConfig:parseProperties(Propertys)
    return StringUtil.split(Propertys,"#")
end

function GunConfig:getGunPropertyByItemId(id)
    local gun = GunConfigs[tostring(id)]
    if gun ~= nil then
        return gun.property
    end
    return nil
end

return GunConfig