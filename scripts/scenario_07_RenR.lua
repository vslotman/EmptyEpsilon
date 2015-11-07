-- Name: RenR
-- Description: RenR sevcol event


-- Global data
Neb = {}
Ast= {}
Objects = {}
Worm = {}
targetx = 0
targety = 0
nextArea = nil


-- Utility Function
-- Create something to fill the void
function createRandomSpace(nrOfClusters, xOffset, yOffset)
	for n=1,nrOfClusters do
		local x1 = random(-200000, 200000) + xOffset
		local y1 = random(-200000, 200000) + yOffset
		
		-- Create some random nebulas
		for n=1,random(2, 10) do
			local x2 = random(0, 20000)
			local y2 = random(0, 20000)

			table.insert(Objects, Nebula():setPosition(x1+x2, y1+y2))
		end
		
		-- create some visual random asteroids
		for n=1,random(0, 80) do
			local x2 = random(0, 60000)
			local y2 = random(0, 60000)
			table.insert(Objects, VisualAsteroid():setPosition(x1+x2, y1+y2))
		end
		
		-- create some random asteroids
		for n=1,random(0, 20) do
			local x2 = random(0, 20000)
			local y2 = random(0, 20000)
			table.insert(Objects, Asteroid():setPosition(x1+x2, y1+y2))
		end
		
		
    end
end

-- Utility Function
-- Create some clusters of asteroids within a given sector-size
function createAsteroidClusters(nrOfClusters, xRange, yRange, amount)
	for n=1,nrOfClusters do
		local x1 = random(-xRange, xRange)
		local y1 = random(-yRange, yRange)
		
		-- create some visual random asteroids
		for n=1,random(0, 10*amount) do
			local x2 = random(0, 60000)
			local y2 = random(0, 60000)
			table.insert(Objects, VisualAsteroid():setPosition(x1+x2, y1+y2))
		end
		
		-- create some random asteroids
		for n=1,random(0, 2*amount) do
			local x2 = random(0, 20000)
			local y2 = random(0, 20000)
			table.insert(Objects, Asteroid():setPosition(x1+x2, y1+y2))
		end
	end
end


-- Utility Function
-- Create some object in a circle and add it to the table
function createCircleObject(obj, xPos, yPos, angle, distance, spread)
	obj:setPosition( math.sin(angle / 180 * math.pi) * distance + xPos + random(0, spread), 
					-math.cos(angle / 180 * math.pi) * distance + yPos + random(0, spread))
	table.insert(Objects, obj)
end


-- Utility Function
-- Destroy everything
function destroyObjects()
	for n=1,#Objects do
		Objects[n]:destroy()
	end
	
	Objects = {}
end

-- Utility Function
-- Check if a player is sucked in the jumpgate. If so, destroy everything and proceed to next.
function vernietigCheck(delta)
	
	local x1, y1 = player:getPosition()
	local xd, yd = (x1 - targetx), (y1 - targety)
	
	Afstpw = math.sqrt(xd * xd + yd * yd)
	
	if Afstpw<1000 then
		Afstpw=9999
		
		destroyObjects()
		nextArea()
		
	end
end


function init()
    -- Create the main ship for the players.
	player = PlayerSpaceship():setFaction("Friendly Vessel"):setShipTemplate("Player shuttle1")
	player:setPosition(0, 1000)
	player:setCallSign("MUSS Archibald Laurow")
	
	--mission_state = areaFour
	mission_state = missionStartState
end


function  missionStartState(delta)
	starting_station = SpaceStation():setTemplate("Large Station"):setFaction("Friendly Vessel")
    starting_station:setPosition(0, 0):setCallSign("F-Stat-Artemis1")
	table.insert(Objects, starting_station)
	
	createRandomSpace(30, 0, 0)
	createAsteroidClusters(20, 200000, 200000, 10)

	targetx = -500000
	targety = -500000
	table.insert( Objects, WormHole():setPosition(-225000, -8000):setTargetPosition(targetx, targety))
	
	for n=1,150 do
		createCircleObject(Asteroid(), -225000, -8000, random(0, 360), 15000, 10000)
	end
	
	nextArea = areaTwo
	mission_state = vernietigCheck
end

function areaTwo(delta)
	
	createRandomSpace(20, 0, 0)
	createAsteroidClusters(20, 200000, 200000, 10)
	targetx = -500000
	targety = -200000
	
	player:setPosition(0, 0):setRotation(180)
	--Usable Wormholes
	table.insert( Objects, WormHole():setPosition(20000, 15000  ):setTargetPosition(150000, 100000)) --the previous gate, has cooldown and does not work at the moment
	table.insert( Objects, WormHole():setPosition(-120000, -100000):setTargetPosition(targetx,targety)) --the next gate
	
	nextArea = areaThree
	mission_state = vernietigCheck
end


function areaThree(delta)
	
	createRandomSpace(30, 0, 0)
	createAsteroidClusters(40, 100000, 100000, 10)
	player:setPosition(0, 0)
	
	targetx = -500000
	targety = -800000 
	
	table.insert(Objects, WormHole():setPosition(-36628, -2456):setTargetPosition(-33634, -1858)) --starting wormhole
	table.insert(Objects, WormHole():setPosition(100000, -8000):setTargetPosition(targetx, targety)) --finishing wormhole

	nextArea = areaFour
	mission_state = vernietigCheck
	
end
	
function areaFour(delta)

	createRandomSpace(50, 0, 0)
	targetx = 500000
	targety = 500000
	
	player:setPosition(0, 0):setRotation(220)
	
	table.insert(Objects, WormHole():setPosition(14278, 31744):setTargetPosition(targetx, targety)) --starting wormhole
	
	Station = SpaceStation():setPosition(8130, 123):setTemplate('Medium Station'):setFaction("Neutral Vessel"):setRotation(0)
	
	nextArea = areaTwo
	mission_state = vernietigCheck
end


function update(delta)

    if mission_state ~= nil then
        mission_state(delta)
    end
end