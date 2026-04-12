--region *.lua
require "config.GameConfig"

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(36);
end

function Messages:gameStartWelcome()
    return 37;
end

function Messages:gameOverHint(seconds)
    return 28, tostring(seconds);
end

function Messages:gameResetTimeHint()
    return 29;
end

function Messages:gameData(teams)
    return 38, teams[1].color .. teams[1].flags .. TextFormat.argsSplit ..
               teams[2].color .. teams[2].flags .. TextFormat.argsSplit ..
               teams[1].color .. teams[1].kills .. TextFormat.argsSplit ..
               teams[2].color .. teams[2].kills .. TextFormat.argsSplit ..
               teams[1].color .. teams[1].curPlayers .. TextFormat.argsSplit ..
               teams[2].color .. teams[2].curPlayers .. TextFormat.argsSplit ..
               teams[1].color .. teams[1].score .. TextFormat.argsSplit ..
               teams[2].color .. teams[2].score;
end

function Messages:moveFlagArea()
    return 39;
end

function Messages:moveOtherBornArea()
    return 40;
end

function Messages:moveGameNotStart()
    return 41;
end

function Messages:getFlagHint()
    local name = "";
    if BaseMain:isChina() then
        name = name .. "旗帜放置于此"
    else
        name = name .. "Put the flag here"
    end
    return name
end

return Messages


--endregion
