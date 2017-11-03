neutral = FactionInfo():setName("Neutral Vessel")
neutral:setGMColor(128, 128, 128)
neutral:setDescription([[Neutral Vessel]])

friendly = FactionInfo():setName("Friendly Vessel")
friendly:setGMColor(255, 255, 255)
friendly:setDescription([[Friendly Vessel]])

enemy = FactionInfo():setName("Enemy Vessel")
enemy:setGMColor(255, 0, 0)
enemy:setEnemy(friendly)
enemy:setDescription([[Enemy Vessel]])

surrendered = FactionInfo():setName("Surrendered Vessel")
surrendered:setGMColor(255, 128, 0)
surrendered:setDescription([[Surrendered Vessel]])