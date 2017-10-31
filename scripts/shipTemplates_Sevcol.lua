--[[                  Corvette
Corvettes are the common large ships. Larger then a frigate, smaller then a dreadnaught.
They generally have 4 or more shield sections. Run with a crew of 20 to 250.
This class generally has jumpdrives or warpdrives. But lack the manouverbility that is seen in frigates.

They come in 3 different subclasses:
* Destroyer: Combat oriented ships. No science, no transport. Just death in a large package.
* Support: Large scale support roles. Drone carriers fall in this category. As well as mobile repair centers.
* Freighter: Large scale transport ships. Most common here are the jump freighters, using specialized jumpdrives to cross large distances with large amounts of cargo.
----------------------------------------------------------]]

--[[----------------------Destroyers----------------------]]


--celestra
template = ShipTemplate():setName("Celestra"):setModel("Celestra"):setType("playership"):setClass("Corvette", "Freighters")

-- Visual positions of the beams/missiletubes (blender: -X, Y, Z)
--                  Arc, Dir, Range, CycleTime, Dmg
template:setBeam(0, 15, -60, 1300.0, 1, 2)
template:setBeam(1, 15, 60, 1300.0, 1, 2)
template:setBeam(2, 45,  -90, 1000.0, 10, 20)
template:setBeam(3, 45,  90, 1000.0, 10, 20)
template:setRadarTrace("radar_cruiser.png")

template:setTubes(2, 8.0) -- Amount of torpedo tubes, and loading time of the tubes.
template:setHull(200)
template:setShields(160, 160)
template:setSpeed(90, 10, 20)
template:setWarpSpeed(0)
template:setJumpDrive(false)
template:setCloaking(false)
template:setWeaponStorage("Homing", 10)
template:setWeaponStorage("Nuke", 0)
template:setWeaponStorage("Mine", 0)
template:setWeaponStorage("EMP", 1)

template:addRoomSystem(0, 3, 1, 3, "Impulse"); 
template:addRoomSystem(0, 2, 1, 1, "Maneuver");
template:addRoomSystem(0, 6, 1, 1, "Maneuver");
template:addRoomSystem(11, 2, 1, 5, "MissileSystem");
template:addRoomSystem(9, 5, 1, 1, "BeamWeapons");
template:addRoomSystem(9, 3, 1, 1, "BeamWeapons");
template:addRoomSystem(5, 5, 1, 1, "FrontShield");
template:addRoomSystem(5, 3, 1, 1, "RearShield");
template:addRoomSystem(2, 3, 2, 3, "Reactor");
template:addRoomSystem(4, 4, 6, 1, "Warp");
template:addRoomSystem(1, 3, 1, 1, "JumpDrive");
template:addRoomSystem(1, 5, 1, 1, "JumpDrive");

template:addRoom(7, 5, 1, 1);
template:addRoom(7, 3, 1, 1);
template:addRoom(4, 3, 1, 1);
template:addRoom(4, 5, 1, 1);
template:addRoom(6, 3, 1, 1);
template:addRoom(6, 5, 1, 1);
template:addRoom(8, 3, 1, 1);
template:addRoom(8, 5, 1, 1);
template:addRoom(1, 4, 1, 1);
template:addRoom(10, 3, 1, 3);


template:addDoor(0, 3, true);
template:addDoor(0, 6, true);
template:addDoor(2, 5, false);
template:addDoor(2, 3, false);
template:addDoor(4, 4, false);
template:addDoor(1, 4, false);
template:addDoor(2, 4, false);
template:addDoor(5, 4, true);
template:addDoor(5, 5, true);
template:addDoor(4, 4, true);
template:addDoor(4, 5, true);
template:addDoor(6, 4, true);
template:addDoor(6, 5, true);
template:addDoor(7, 4, true);
template:addDoor(7, 5, true);
template:addDoor(8, 4, true);
template:addDoor(8, 5, true);
template:addDoor(10, 3, false);
template:addDoor(10, 5, false);
template:addDoor(10, 4, false);
template:addDoor(11, 4, false);