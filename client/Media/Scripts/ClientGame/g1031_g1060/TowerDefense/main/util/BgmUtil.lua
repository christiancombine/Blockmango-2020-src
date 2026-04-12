BgmUtil = {}

local isStart = false

function BgmUtil:startBgm(id)
    if isStart then
        return
    end
    local sound = SoundSetting.getSoundSetting(id)
    if sound.fMinInterval == 0 then
        return
    end
    isStart = true
    BgmUtil:playBgm(sound)
end

function BgmUtil:playBgm(sound)
    SoundUtil.playSound(sound.nSoundId)
    local randomDelay = math.random(math.floor(sound.fMinInterval * 1000), math.floor(sound.fMaxInterval * 1000))
    LuaTimer:schedule(function()
        self:playBgm(sound)
    end, randomDelay)
end