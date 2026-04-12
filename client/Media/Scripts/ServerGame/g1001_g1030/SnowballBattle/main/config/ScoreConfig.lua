---
--- Created by Jimmy.
--- DateTime: 2017/10/30 0030 10:51
---

ScoreConfig = {}

ScoreConfig.KILL_SCORE = 2
ScoreConfig.WIN_SCORE = 10
ScoreConfig.KILL_2_SCORE = 4
ScoreConfig.KILL_3_SCORE = 6
ScoreConfig.KILL_4_SCORE = 10
ScoreConfig.KILL_5_SCORE = 15

ScoreConfig.scores = {}
ScoreConfig.winScore = 0

function ScoreConfig:init(scores, winScore)
    for i, v in pairs(scores) do
        self.scores[tostring(i)] = v
    end
    self.winScore = winScore
end

function ScoreConfig:getScore(deaths, win)
    local score = self.scores[tostring(deaths)]
    if score == nil then
        if deaths > 0 then
            score = self.scores["7"]
        else
            score = self.scores["0"]
        end
    end
    if win then
        return score + self.winScore
    else
        return score
    end
end

return ScoreConfig