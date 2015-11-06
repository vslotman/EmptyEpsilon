--[[ Stations --]]
template = ShipTemplate():setName("Small Station"):setModel("space_station_4")
template:setHull(150)
template:setShields(300, 0)

template = ShipTemplate():setName("Medium Station"):setModel("space_station_3")
template:setHull(400)
template:setShields(800, 0)

template = ShipTemplate():setName("Large Station"):setModel("space_station_2")
template:setHull(500)
template:setShields(1000, 0)

template = ShipTemplate():setName("Huge Station"):setModel("space_station_1")
template:setHull(800)
template:setShields(1200, 0)

--[[ Player ships --]]

--celestra
template = ShipTemplate():setName("Player shuttle1"):setModel("shuttle1") --UC shuttle1
template:setSizeClass(11)
template:setHull(80)
template:setShields(1, 1)
template:setSpeed(90, 6, 10)

template:setWarpSpeed(0)
template:setJumpDrive(false)
template:setCloaking(false)


template:addRoomSystem(3, 0, 1, 1, "Maneuver");
template:addRoomSystem(1, 0, 2, 1, "BeamWeapons");

template:addRoomSystem(0, 1, 1, 2, "RearShield");
template:addRoomSystem(1, 1, 2, 2, "Reactor");
template:addRoomSystem(3, 1, 2, 1, "Warp");
template:addRoomSystem(3, 2, 2, 1, "JumpDrive");
template:addRoomSystem(5, 1, 1, 2, "FrontShield");

template:addRoomSystem(1, 3, 2, 1, "MissileSystem");
template:addRoomSystem(3, 3, 1, 1, "Impulse");

template:addDoor(2, 1, true);
template:addDoor(3, 1, true);
template:addDoor(1, 1, false);
template:addDoor(3, 1, false);
template:addDoor(3, 2, false);
template:addDoor(3, 3, true);
template:addDoor(2, 3, true);
template:addDoor(5, 1, false);
template:addDoor(5, 2, false);

template = ShipTemplate():setName("Player raptor"):setModel("raptor") --UC shuttle3
template:setSizeClass(11)
template:setHull(50)
template:setShields(50, 50)
template:setSpeed(180, 6, 10)
template:setWeaponStorage("Homing", 2)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 30, 0, 100.0, 3.0, 5)
template:setTubes(2, 16.0)
template:setWarpSpeed(0)
template:setJumpDrive(false)
template:setCloaking(false)


template:addRoomSystem(3, 0, 1, 1, "Maneuver");
template:addRoomSystem(1, 0, 2, 1, "BeamWeapons");
template:addRoomSystem(0, 1, 1, 2, "RearShield");
template:addRoomSystem(1, 1, 2, 2, "Reactor");
template:addRoomSystem(3, 1, 2, 1, "Warp");
template:addRoomSystem(3, 2, 2, 1, "JumpDrive");
template:addRoomSystem(5, 1, 1, 2, "FrontShield");
template:addRoomSystem(1, 3, 2, 1, "MissileSystem");
template:addRoomSystem(3, 3, 1, 1, "Impulse");
template:addDoor(2, 1, true);
template:addDoor(3, 1, true);
template:addDoor(1, 1, false);
template:addDoor(3, 1, false);
template:addDoor(3, 2, false);
template:addDoor(3, 3, true);
template:addDoor(2, 3, true);
template:addDoor(5, 1, false);
template:addDoor(5, 2, false);


template = ShipTemplate():setName("dropshuttle"):setModel("dropshuttle") --UC shuttle4
template:setSizeClass(11)
template:setHull(50)
template:setShields(1, 1)
template:setSpeed(90, 6, 10)
template:setDescription([[Manufacturer: ###
Type: 	Civilian/Military
Propulsion: 	 1x fixed engines + 2x truster engines
Crew: 	2 (Pilot and ECO)
Capacity: 	Approximately 12 passengers
Role: 	
* Search and Rescue
* Transport
* Atmospheric drop
Armaments: 	
* None
Defenses: 	
* None
Dimensions
Length: 	(9.5m)]])
