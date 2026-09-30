# Algoritmi e Programmazione dei Videogiochi @ Università di Cassino

Un framework per programmare videogiochi 2D in C++ con SDL3.

> [!IMPORTANT]
> Questo è il repository ufficiale dell'insegnamento di ***Algoritmi e Programmazione dei Videogiochi*** del corso di laurea in _Ingegneria Informatica e delle Telecomunicazioni_ dell'Università di Cassino. La cartella Drive del corso, che include slide, esercizi e codice sorgente, è pubblicamente accessibile a [questo indirizzo](https://drive.google.com/open?id=1Qk3xPAt2qOVHL6Si3TY1qLDPabbv_kcsZ92gnlwyDK176RSNcc92Jy5ALNNzp6Kzfj_IKN8s&usp=drive_fs).

La [wiki](https://github.com/abria/agpunicas/wiki) raccoglie le guide al framework e ai quattro prototipi. Nel menu laterale trovi gli approfondimenti e i diagrammi delle classi di ciascun progetto.

## utils

Libreria header-only di utilità condivise: vettori e forme geometriche, test di collisione, autotiling ed estrazione dei fotogrammi dalle spritesheet, oltre a funzioni per tempo, matematica, file e SDL.

[Geometria e collisioni](https://github.com/abria/agpunicas/wiki/Utils-Geometria) · [Autotiling e spritesheet](https://github.com/abria/agpunicas/wiki/Utils-Autotiling) · [Esplora il codice](https://github.com/abria/agpunicas/tree/main/utils)

## Core

Infrastruttura comune dei quattro prototipi: game loop, scene, oggetti, rendering, sprite, camera, audio e interfaccia. Include un editor visuale e il caricamento dei livelli JSON tramite LevelData; ciascun gioco definisce la propria fisica e le proprie regole.

![Editor dei livelli](https://github.com/abria/agpunicas/blob/main/demo_leveleditor.png)

[Guida al core](https://github.com/abria/agpunicas/wiki/Core) · [Editor e livelli](https://github.com/abria/agpunicas/wiki/Core-Editor) · [Diagramma delle classi](https://github.com/abria/agpunicas/wiki/Core-Diagramma-delle-classi)

## Post-processing

Effetti visivi applicati al frame completo, con elaborazione CPU o GPU. Il modulo raccoglie le risorse per il post-processing GPU usato dalle finestre del core.

[Guida agli shader](https://github.com/abria/agpunicas/wiki/Core-Shader) · [Esplora il codice](https://github.com/abria/agpunicas/tree/main/postprocessing)

## CustomPlatformer

Platform 2D ispirato a *Super Mario Bros*, con movimento e collisioni AABB implementati da zero. È il punto di partenza per studiare la fisica personalizzata e le interazioni di un platform.

![CustomPlatformer](https://github.com/abria/agpunicas/blob/main/demo_SuperMarioBros.png)

[Guida al prototipo](https://github.com/abria/agpunicas/wiki/CustomPlatformer) · [Diagramma delle classi](https://github.com/abria/agpunicas/wiki/CustomPlatformer-Diagramma-delle-classi)

## CustomPlatformerSlopes

Platform 2D ispirato a *Super Mario Bros 3*, con fisica personalizzata estesa alle pendenze: salita rallentata, scivolamento a riposo e piattaforme attraversabili dal basso. Riusa core, utils e formato JSON condivisi; il livello include rampe triangolari senza sprite.

![CustomPlatformerSlopes](https://github.com/abria/agpunicas/blob/main/demo_SuperMarioBros3.png)

[Guida al prototipo](https://github.com/abria/agpunicas/wiki/CustomPlatformerSlopes) · [Diagramma delle classi](https://github.com/abria/agpunicas/wiki/CustomPlatformerSlopes-Diagramma-delle-classi)

## Box2DPlatformer

Platform 2D con simulazione fisica affidata a Box2D 3.x: corpi rigidi, forme composte, pendenze e contatti. Mostra come integrare un motore fisico esterno con scene, rendering e servizi del core.

![Box2DPlatformer](https://github.com/abria/agpunicas/blob/main/demo_Box2DPlatformer.png)

[Guida al prototipo](https://github.com/abria/agpunicas/wiki/Box2DPlatformer) · [Diagramma delle classi](https://github.com/abria/agpunicas/wiki/Box2DPlatformer-Diagramma-delle-classi)

## ActionRPG

Prototipo con vista dall'alto ispirato a *The Legend of Zelda: A Link to the Past*. Combina collisioni personalizzate, combattimento, NPC, pathfinding, dialoghi e portali fra scene.

![ActionRPG](https://github.com/abria/agpunicas/blob/main/demo_ActionRPG.png)

[Guida al prototipo](https://github.com/abria/agpunicas/wiki/ActionRPG) · [Diagramma delle classi](https://github.com/abria/agpunicas/wiki/ActionRPG-Diagramma-delle-classi)
