# Algorithms and Game Programming @ University of Cassino

A framework for programming 2D videogames with C++/SDL.

## utils
Header-only library for game programming utils, including:
- geometry (basic shape primitives and 2D vectors)
- time (timer, FPS, profiler)
- string manipulation
- SDL (complex shapes drawing and image mosaicing)
- math (interpolation, percentiles, etc.)
- file/folder management (get files in folder, etc.)
- collision (SAT, Swept AABB vs. AABB, Sweept AABB vs. Line, etc.)

## Proto-SimplePlatformer
Game prototype for simple 2D platformers with SDL and custom physics engine.
#### Features
- Swept AABB Continous Collision Detection (CCD)
- AABB colliders
- bug-free sliding collision resolution
- collision filters (type-based)
- semi-fixed timestep
- configurable linear dynamics with simple friction and skidding
- static, dynamic, kinematic object categories
- all collidable objects are informed when collision starts (with normals and metadatas)
- sprite system with on-the-fly GPU blitting from spritesheets
- triggers (sensors) and schedulers
- basic UI (HUD, menus)
- basic Audio (sounds and music with stop/resume)
- Scene/View/Window framework with Scene (world) -> View -> Screen transforms

#### Limitations
- no "onCollisionEnd" events
- no compound colliders (only one collider per object)
- teleports break CCD and must be handled carefully
- narrow collision detection phase is based on linear search with view box intersection
