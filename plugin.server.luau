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

local function getInstanceByPath(instancePath, allowFolderSwap)
    local instance = world
    local instancePathKeys = string.split(instancePath, ".")

    for instancePathKeyIndex, instancePathKey in ipairs(instancePathKeys) do
        local nextInstancePathKey = instancePathKeys[instancePathKeyIndex + 1]

        if nextInstancePathKey == "init" then
            local child = instance:FindChild(instancePathKey)

            if allowFolderSwap and child and child.ClassName == "Folder" then
                return nil
            end

            return child
        end

        instance = instance:FindChild(instancePathKey)

        if not instance then
            return nil
        end
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
            local existingInstance = currentInstance:FindChild(instancePathKey)

            if existingInstance then
                existingInstance.Name = instancePathKey .. "_DELETE"
            end

            instance.Name = instancePathKey
            instance.Parent = currentInstance

            if existingInstance then
                for _, child in ipairs(existingInstance:GetChildren()) do
                    child.Parent = instance
                end

                existingInstance:Destroy()
            end

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

        local success, events = pcall(function()
            return json.parse(response)
        end)

        if not success then
            continue
        end

        if not events then
            continue
        end

        for _, event in ipairs(events) do
            if event.type == "createScript" then
                local existingScript = getInstanceByPath(event.scriptPath, true)

                if existingScript then
                    logWarning("Failed to create script. Instance already exists '%s'", event.scriptPath)

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
