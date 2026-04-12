ScoreConfig = {}


ScoreConfig.ticket = 0
ScoreConfig.captureCriminal = 0
ScoreConfig.killPolice = 0
ScoreConfig.criminalIncident = 0

function ScoreConfig:init(score)
    self.ticket = score.ticket
    self.captureCriminal = score.captureCriminal
    self.killPolice = score.killPolice
    self.criminalIncident = score.criminalIncident
end

return ScoreConfig
