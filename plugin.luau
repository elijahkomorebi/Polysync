local ADDON_ID = "Polysync"
local PORT = 37630
local URL = "http://localhost:" .. tostring(PORT)
local SYNC_UPDATE_RATE = 0.1

local addon = Addons.Register(ADDON_ID)
addon.AddonName = ADDON_ID
local toggleGUIButton = addon:CreateToolItem("Toggle GUI")
local syncThread = nil
local syncing = false
local polysyncGUI = script.PolysyncGUI
polysyncGUI.Parent = CreatorGUI
local polysyncView = polysyncGUI.PolysyncView
local syncButton = polysyncView.SyncButton
local infoView = polysyncView.InfoView
local portLabel = infoView.PortLabel
portLabel.Text = tostring(PORT)

local function logInfo(log, ...)
    print(string.format("[color=#0097FF][Polysync Info] %s[/color]", log:format(...)))
end

local function logWarning(log, ...)
    print(string.format("[color=#FFBC58][Polysync Warning] %s[/color]", log:format(...)))
end

local function logError(log, ...)
    print(string.format("[color=#F95D5D][Polysync Error] %s[/color]", log:format(...)))
end

local function getInstanceByPath(instancePath)
    local instance = world

    for _, instancePathKey in ipairs(string.split(instancePath, ".")) do
        if not instance:FindChild(instancePathKey) then
            return nil
        end

        instance = instance:FindChild(instancePathKey)
    end

    return instance
end

local function createInstance(instanceClass, instancePath)
    local instance = Instance.New(instanceClass)
    local currentInstance = world
    local instancePathKeys = string.split(instancePath, ".")

    for instancePathKeyIndex, instancePathKey in ipairs(instancePathKeys) do
        local nextInstancePathKey = instancePathKeys[instancePathKeyIndex + 1]

        if instancePathKeyIndex == #instancePathKeys then
            instance.Name = instancePathKey
        elseif nextInstancePathKey == "init" then
            instance.Name = instancePathKey
            instance.Parent = currentInstance

            return instance
        else
            local instanceParent = currentInstance:FindChild(instancePathKey)

            if not instanceParent then
                instanceParent = Instance.New("Folder", currentInstance)
                instanceParent.Name = instancePathKey
            end

            currentInstance = instanceParent
        end
    end

    instance.Parent = currentInstance

    return instance
end

toggleGUIButton.Pressed:Connect(function()
    polysyncGUI.Visible = not polysyncGUI.Visible
end)
syncButton.Clicked:Connect(function()
    syncing = not syncing

    if syncing then
        syncButton.Text = "Stop Syncing"
        syncButton.Color = Color.FromHex("#9a9a9a")
    else
        syncButton.Text = "Start Syncing"
        syncButton.Color = Color.FromHex("#50a4db")
    end
end)
addon.CleanupReceived:Connect(function()
    polysyncGUI:Destroy()
    pcall(function()
        coroutine.close(syncThread)
    end)
end)

syncThread = coroutine.create(function()
    while true do
        wait(SYNC_UPDATE_RATE)

        if not syncing then
            continue
        end

        local response = Http:GetAsync(URL)

        if not response then
            logWarning("No response from server. Make sure you run 'polysync serve'")
        end

        local events = json.parse(response)

        if not events then
            return
        end

        for _, event in ipairs(events) do
            if event.type == "createScript" then
                local existingScript = getInstanceByPath(event.scriptPath)

                if existingScript then
                    logWarning("Failed to create script. Script already exists '%s'", event.scriptPath)

                    continue
                end

                logInfo("Creating script '%s", event.scriptPath)

                local script = nil

                if event.scriptType == 0 then
                    script = createInstance("ServerScript", event.scriptPath)
                elseif event.scriptType == 1 then
                    script = createInstance("ClientScript", event.scriptPath)
                elseif event.scriptType == 2 then
                    script = createInstance("ModuleScript", event.scriptPath)
                end

                script:LinkWithScriptFile(event.filePath)
            elseif event.type == "deleteScript" then
                local script = getInstanceByPath(event.scriptPath)

                if not script then
                    continue
                end

                logInfo("Deleting script '%s", event.scriptPath)
                script:Destroy()
            elseif event.type == "modifyScript" then
            else
                logError("Unknown event type '%s'", event.type)
            end
        end
    end
end)

pcall(function()
    coroutine.resume(syncThread)
end)
logWarning("Listening at %s", URL)
