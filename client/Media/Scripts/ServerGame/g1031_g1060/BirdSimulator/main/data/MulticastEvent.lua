require "data.EventCallback"
MulticastEvent = class("MulticastEvent")

function MulticastEvent:ctor()
    self.multicastEvents = {}
end

function MulticastEvent:push(key, func, obj)
    local callback = EventCallback.new(func, obj)
    self.multicastEvents[key] = callback
end

function MulticastEvent:Invoke(...)
    for key, callback in pairs(self.multicastEvents) do
        if type(callback) == "table" then
            if callback:Invoke(...) == true then
                self.multicastEvents[key] = nil
            end
        end
    end
end

function MulticastEvent:Remove(key)
    if self.multicastEvents[key] then
        self.multicastEvents[key] = nil
    end
end
