---
--- Created by Jimmy.
--- DateTime: 2017/11/14 0014 16:06
---
require "config.Scene"

SceneConfig = {}

SceneConfig.scenes = {}
SceneConfig.beginIndex = 0
SceneConfig.sceneCount = 0
SceneConfig.lastScene = 0
SceneConfig.lastSceneTimes = 0

function SceneConfig:addScene(sceneConfig)
    self.sceneCount = self.sceneCount + 1
    SceneConfig.scenes[tostring(self.sceneCount)] = Scene.new(self.beginIndex, sceneConfig)
end

function SceneConfig:randomScene()

    local get = {}
    for i = 1, self.sceneCount do
        table.insert(get, i, i)
    end

    local random
    for i = 1, #get do
        math.randomseed(os.clock()*1000000)
        local getIndex = math.random(1, #get)
        random = get[getIndex]
        if self.lastSceneTimes < 3 or random ~= self.lastScene then
            break
        end
        table.remove(get, getIndex)
    end

    local scene = self.scenes[tostring(random)]

    self.lastScene = random
    self.lastSceneTimes = (self.lastSceneTimes < 3 and {self.lastSceneTimes + 1} or {1})[1]

    if scene == nil then
        self.lastScene = 1
        self.lastSceneTimes = 1
        scene = self.scenes["1"]
    end

    return scene
end

return SceneConfig