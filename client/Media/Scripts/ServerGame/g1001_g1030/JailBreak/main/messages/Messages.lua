---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(98)
end

function Messages:enterLocation(name)
    return 94, self.getLocationName(name)
end

function Messages:quitLocation(name)
    return 95, self.getLocationName(name)
end

function Messages.getLocationName(name)
    return TextFormat:getTipType(name)
end

function Messages:showPoliceInfo(merit)
    return 96, merit
end

function Messages:showCriminalInfo(threat)
    return 97, threat
end

function Messages:getChestLocationName(name)
    return 99, self.getLocationName(name)
end

function Messages:captureCriminal(criminal, police)
    return 100, criminal .. TextFormat.argsSplit .. police
end

function Messages:killCriminal(criminal, police)
    return 101, criminal .. TextFormat.argsSplit .. police
end

function Messages:killPolice(police, criminal)
    return 102, police .. TextFormat.argsSplit .. criminal
end

function Messages:openDoor(name)
    return 103, self.getLocationName(name)
end

function Messages:policeKillPrisoner()
    return 128
end

function Messages:policeOpenChestMsg()
    return 129
end

function Messages:policeLevelUp(name)
    return 152, name
end

function Messages:criminalLevelUp(name)
    return 153, name
end

function Messages:addWage(wage, remainingWage)
    return 154, wage .. TextFormat.argsSplit .. remainingWage
end

return Messages