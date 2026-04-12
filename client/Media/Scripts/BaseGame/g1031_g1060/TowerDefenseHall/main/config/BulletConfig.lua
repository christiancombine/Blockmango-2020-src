BulletConfig = {}

local Config = {}

local defineConfig = {
    bulletBox = VectorUtil.newVector3(1, 1, 1),
    bulletEffect = "tower_bomb.effect",
    bulletEffectMultiple = 1,
    bulletSprite = {},
    bulletSpriteMultiple = 1,
    bulletSpeed = 0.3,
    ejectionsSpeed = 0.1,
    boomEffect = "",
    boomRange = ""
}

function BulletConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.Id = tonumber(vConfig.Id) or 0
        local box = StringUtil.split(vConfig.bulletBox, "#")
        data.bulletBox = VectorUtil.newVector3(box[1] or "1", box[2] or "1", box[3] or "1")
        data.bulletEffect = tostring(vConfig.bulletEffect) or "tower_bomb.effect"
        data.bulletEffectMultiple = tonumber(vConfig.bulletEffectMultiple) or 1
        data.bulletSprite = StringUtil.split(vConfig.bulletSprite, "#")
        data.bulletSpriteMultiple = tonumber(vConfig.bulletSpriteMultiple) or 1
        data.bulletSpeed = tonumber(vConfig.bulletSpeed) or 0.3
        data.ejectionsSpeed = tonumber(vConfig.ejectionsSpeed) or 0.1
        data.boomEffect = tostring(vConfig.boomEffect) or ""
        data.boomRange = tonumber(vConfig.boomRange) or 1
        Config[data.Id] = data
    end
end

function BulletConfig:getConfigById(id)
    return Config[id] or defineConfig
end

BulletConfig:init(FileUtil.getConfigFromCsv("setting/Bullet.csv"))