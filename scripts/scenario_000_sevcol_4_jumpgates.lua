-- Name: Sevcol 4 Jumpgates
-- Description: Empty scenario, no enemies, no friendlies. Can be used by a GM player to setup a scenario in the GM screen. The F5 key can be used to copy the current layout to the clipboard for use in scenario scripts.
-- Type: Basic

require("utils.lua")

-- Init is run when the scenario is started. Create your initial stuff here.
function init()
    memObjects = {}
    Objects = {}
    Worm = {}
    nextArea = nil
    targetx = -500000
    targety = -500000
    jumpgate_target = 0


    -- Create the main ship for the players.
    player = PlayerSpaceship():setFaction("Friendly Vessel"):setShipTemplate("Celestra")
    player:setPosition(1000, 1000)
    player:setCallSign("SCSS. Celestra")

    
    -- GM functions, I make a variable, since I do not want big function references in the init.
    
    -- addGMFunction("Area 1", function() Area = 1;
    -- end)
    -- addGMFunction("Area 2", function() Area = 2;
    -- end)
    -- addGMFunction("Area 3", function() Area = 3;
    -- end)
    
    addGMFunction("Jump target: UpperLeft", function()    
        jumpgate_target = 0
    end)
    addGMFunction("Jump target: UpperRight", function()    
        jumpgate_target = 1
    end)
    addGMFunction("Jump target: BottomLeft", function()        
        jumpgate_target = 2
    end)
    addGMFunction("Jump target: BottomRight", function()    
        jumpgate_target = 3
    end)

    addGMFunction("Jump!", function()    
        jumpToNewPlace(jumpgate_target)
    end)
    
    player:addCustomButton('Science', 'EMP_BUTTON', 'Generate EMP', function()
        energy = player:getEnergy();
        emp = player:getWeaponStorage('emp');

        if energy > 800 then
            if emp == 0 then
                energy = energy - 500;
                player:setEnergy(energy);
                player:setWeaponStorage('emp', 1);
            end
        end
    end) 

    placePlayerAndJumpgates()
    createRandomSpace()

end

function placePlayerAndJumpgates(jumpgate_target)
    --insert the first area here. A wormhole is required to target x and target y.
    -- jumpgate_target =random(0,4)
    
    Positionx = random(0,-60000)-20000
    Positiony = random(0,-60000)-20000  
    WormHole():setPosition(Positionx,Positiony):setTargetPosition(targetx,targety)
    if (jumpgate_target == 0) then
        player:setPosition(Positionx+2000, Positiony+2000)
    end;

    Positionx = random(0,65000)+15000
    Positiony = random(0,-70000)-20000
    WormHole():setPosition(Positionx,Positiony):setTargetPosition(targetx,targety)
    if (jumpgate_target == 1)  then
        player:setPosition(Positionx+2000, Positiony+2000)
    end;


    Positionx = random(0,-55000)-25000
    Positiony = random(0,70000)+20000
    WormHole():setPosition(Positionx,Positiony):setTargetPosition(targetx,targety)
    if (jumpgate_target == 2) then
        player:setPosition(Positionx+2000, Positiony+2000)
    end;  


    Positionx = random(0,65000)+5000
    Positiony = random(0,70000)+8000
    WormHole():setPosition(Positionx,Positiony):setTargetPosition(targetx,targety)
    if (jumpgate_target == 3) then
        player:setPosition(Positionx+2000, Positiony+2000)
    end;
end


function jumpToNewPlace(jumpgate_target)
    cleanup()
    placePlayerAndJumpgates(jumpgate_target)
    createRandomSpace()
end


-- Utility Function
-- Check if a player is sucked in the jumpgate. If so, destroy everything and proceed to next.
function vernietigCheck(delta)
    
    local x1, y1 = player:getPosition()
    local xd, yd = (x1 - targetx), (y1 - targety)
    
    Afstpw = math.sqrt(xd * xd + yd * yd)
    
    if Afstpw<1000 then
        Afstpw=9999
        
        jumpToNewPlace(jumpgate_target)
        
    end
end

function cleanup()
    --Clean up the current play field. Find all objects and destroy everything that is not a player.
    -- If it is a player, position him in the center of the scenario.
    for _, obj in ipairs(getAllObjects()) do
        if obj.typeName ~= "PlayerSpaceship" then
            obj:destroy()
        end
    end
end


function createRandomSpace()
    placeRandomObjects(Asteroid, 30, 0.3, 0, 0, 10, 10)
    placeRandomObjects(VisualAsteroid, 30, 0.3, 0, 0, 15, 15)
    placeRandomObjects(Nebula, 15, 0.3, 0, 0, 15, 15)
end


--The function which runs constantly.
function update(delta)
    vernietigCheck(delta)
end