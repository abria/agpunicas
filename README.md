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

## Core
Rendering+audio engine based on SDL, utilized by all game prototypes.
- game loop with semi-fixed timestep
- Scene/View/Window framework with automatic scaling to screen resolution and scene layering
- separation between UI scenes and game scenes
- basic object model with positioning, rendering, updating, and scheduling methods
- raycasting
- sprite system (animations, tiling, filling) with on-the-fly GPU blitting from spritesheets
- camera (both manual and follows-the-player)
- audio system with playable/resumable sounds and musics
- parallax and overlay scenes layering
- text sprites based on SDL_ttf
- helper functions for spritesheets (autotiling and connected components extraction)
- level editor with json persistence (supported geometries: rects, rotated rects, multilines)

<img src="https://github.com/abria/agpunicas/blob/main/demo_leveleditor.png">

#### Class Diagram
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_Core.png">

## Proto-SimplePlatformer
Game prototype for simple 2D platformers with SDL and custom physics engine.
For demonstration purposes, a small portion of <i>Super Mario Bros</i> (NES) is implemented.

<img src="https://github.com/abria/agpunicas/blob/main/demo_SuperMarioBros.png">

#### Class Diagram
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_SimplePlatformer.png">

#### Features
- Swept AABB Continous Collision Detection (CCD)
- AABB colliders
- bug-free sliding collision resolution
- collision filters (type-based)
- configurable linear dynamics with simple friction and skidding
- static, dynamic, kinematic object categories
- all collidable objects are informed when collision starts (with normals and metadatas)
- triggers (a.k.a. sensors)
- basic UI (HUD, menus)

#### Limitations
- no "onCollisionEnd" events
- no compound colliders (only one collider per object)
- no slopes
- teleports break CCD and must be handled carefully
- narrow collision detection phase is based on linear search with view box intersection

## Proto-ComplexPlatformer
Game prototype for complex 2D platformers with SDL and Box2D physics engine.

<img src="https://github.com/abria/agpunicas/blob/main/demo_ComplexPlatformer.png">

#### Class Diagram
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_ComplexPlatformer.png">

#### Features
- Box2D powered physics and collision detection/resolution
- compound colliders
- static, dynamic, kinematic object categories
- all collidable objects are informed when collision starts and ends (with normals and metadatas)
- triggers (a.k.a. sensors)
- basic UI (HUD, menus)
- example parallax and overlay scenes
- example player physics with walking, jumping, dashing and tangent force compensation on slopes
- example compound kinematic object (gear)
- example dynamic object (box)
- example enemy

#### Limitations
- no bullets (can be implemented using Box2D)
- player not steady on moving platforms (can be fixed with force compensation like for slopes)
- no joint examples (see Box2D docs)

## Proto-RPG
Game prototype for RPG/action games with SDL and Box2D physics engine.
For demonstration purposes, a small portion of <i>Legend of Zelda: A Link to the Past</i> (NES) is implemented.

<img src="https://github.com/abria/agpunicas/blob/main/demo_RPG.png">

#### Class Diagram
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_RPG.png">

#### Features
- OBB collision detection/resolution
- static and dynamic object categories
- all collidable objects are informed when collision starts (with normals and metadatas) and ends
- triggers (a.k.a. sensors)
- advanced UI (HUD + inventory)
- text dialogs
- portals for player teleporting
- example player attack with sword
- example collider animation (sword)
- example NPC (soldier with patrolling + chasing script)
- example import level editor's json

#### Limitations
- no scene transitions from indoor to outdoor and viceversa
