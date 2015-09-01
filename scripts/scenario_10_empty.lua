-- Name: Empty space
-- Description: Empty scenario, no enemies, no friendlies, only a lonely station. Can be used by a GM player to setup a scenario in the GM screen.

function init()
	--SpaceStation():setPosition(1000, 1000):setTemplate('Small Station'):setFaction("Human Navy"):setRotation(random(0, 360))
	--SpaceStation():setPosition(-1000, 1000):setTemplate('Medium Station'):setFaction("Human Navy"):setRotation(random(0, 360))
	--SpaceStation():setPosition(1000, -1000):setTemplate('Large Station'):setFaction("Human Navy"):setRotation(random(0, 360))
	--SpaceStation():setPosition(-1000, -1000):setTemplate('Huge Station'):setFaction("Human Navy"):setRotation(random(0, 360))
	PlayerSpaceship():setFaction("Human Navy"):setShipTemplate("Player Cruiser"):setRotation(200)
	--Nebula():setPosition(-5000, 0)
	--Asteroid():setPosition(1000, 1000
	--ToutatisAsteroid():setPosition(1000, 5000)
	
	for n=1,50 do
       local f = random(0, 1)
       local x = 5130
       local y = 123
       
       local r = random(0, 360)
       local distance = random(5300, 16000)
       x = x + math.cos(r / 180 * math.pi) * distance + 100
       y = y + math.sin(r / 180 * math.pi) * distance + 100

	   WormHole():setPosition(x,y)
       --VisualAsteroid():setPosition(x, y)
   end 
end

function update(delta)
	--No victory condition
end
