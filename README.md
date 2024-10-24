# Algorithms and Game Programming @ University of Cassino

A framework for programming 2D videogames with C++/SDL.

<img src="https://github.com/abria/agpunicas/blob/main/cover.png">


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

<img src="https://github.com/abria/agpunicas/blob/main/demo_SimplePlatformer.png">
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_SimplePlatformer.png">

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
- no slopes
- teleports break CCD and must be handled carefully
- narrow collision detection phase is based on linear search with view box intersection


## Proto-ComplexPlatformer
Game prototype for complex 2D platformers with SDL and Box2D physics engine.

<img src="https://github.com/abria/agpunicas/blob/main/demo_ComplexPlatformer.png">
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_ComplexPlatformer.png">

#### Features
- Box2D powered physics and collision detection/resolution
- compound colliders
- fixed timestep
- static, dynamic, kinematic object categories
- all collidable objects are informed when collision starts and ends (with normals and metadatas)
- sprite system with on-the-fly GPU blitting from spritesheets
- triggers (sensors) and schedulers
- basic UI (HUD, menus)
- basic Audio (sounds and music with stop/resume)
- Scene/View/Window framework with Scene (world) -> View -> Screen transforms
- parallax and foreground scenes layering
- example player physics with walking, jumping, dashing and tangent force compensation on slopes
- example compound kinematic object (gear)
- example dynamic object (box)

#### Limitations
- no bullets (can be implemented using Box2D)
- player not steady on moving platforms (can be fixed with force compensation like for slopes)
- no joint examples (see Box2D docs)
- no enemies
