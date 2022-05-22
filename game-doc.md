Projectile based. 

Projectile paramters:
- Different speeds
- Different sizes
- Different onCollision effects
- Different damages

Projectile types:
- Destructable
- Indestructable
- Ricochet 
- Lazers?
- Cluster bomb

Powerups/items:
- Shield
- HP UP
- HP recovery
- Projectile number up
- Damage up
- Slow motion (might be hard)

Enemies:
- Basic turrent  :  Looks at you and shoots with delay
- Advanced turret:  Looks at you and shoots bursts
- Rotating turret:  Rotates and shoots in a pattern
- Shotgunner     :  Shoots multiple projectiles in an arc

Player:
- Move forwards, backward, left, right.
- Can jump.
- Can shoot with different effects.
- Mouselook

Event queue:  
For delayed events. It's a queue that holds events.
Each event contains the following:
- Initial delay 
- Reptitions
- Delay between repititions
- Function to call 
Behaviour:
Each frame, all events are decremented by dt. Then when time < dt. set time = delay between repitions, decrement repetitions, call function.
Functions will be handled by lambda to capture needed variables from outside the function.
