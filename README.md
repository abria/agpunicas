# Algoritmi e Programmazione dei Videogiochi @ Università di Cassino

Un framework per programmare videogiochi 2D in C++ con SDL3.

> [!IMPORTANT]
> Questo è il repository ufficiale dell'insegnamento di ***Algoritmi e Programmazione dei Videogiochi*** del corso di laurea in _Ingegneria Informatica e delle Telecomunicazioni_ dell'Università di Cassino. La cartella Drive del corso, che include slide, esercizi e codice sorgente, è pubblicamente accessibile a [questo indirizzo](https://drive.google.com/open?id=1Qk3xPAt2qOVHL6Si3TY1qLDPabbv_kcsZ92gnlwyDK176RSNcc92Jy5ALNNzp6Kzfj_IKN8s&usp=drive_fs).

<img src="https://github.com/abria/agpunicas/blob/main/cover.png">


## utils
Libreria header-only di utilità per la programmazione di videogiochi, che include:
- geometria (forme geometriche di base e vettori 2D)
- tempo (timer, FPS, profiler)
- manipolazione di stringhe
- SDL (disegno di forme complesse e composizione di immagini)
- matematica (interpolazione, percentili, ecc.)
- gestione di file e cartelle (elenco dei file in una cartella, ecc.)
- collisioni (SAT, Swept AABB contro AABB, Swept AABB contro segmenti, ecc.)
- shader CPU (dissolvenze, illuminazione, transizioni di scena, ecc.)

## Core
Motore di rendering e audio basato su SDL, utilizzato da tutti i prototipi di gioco.
- game loop con semi-fixed timestep
- framework Scene/View/Window con adattamento automatico alla risoluzione dello schermo e scene sovrapposte
- separazione tra scene dell'interfaccia e scene di gioco
- modello base degli oggetti con metodi per posizionamento, rendering, aggiornamento e scheduling
- raycasting
- sistema di sprite (`AnimatedSprite`, `TiledSprite`, `FilledSprite`) con blitting GPU dalle spritesheet
- camera manuale o agganciata al giocatore
- sistema audio con suoni e musiche riproducibili, sospendibili e ripristinabili
- parallax e overlay di scena
- sprite testuali basati su SDL_ttf
- funzioni di supporto per le spritesheet (autotiling ed estrazione delle componenti connesse)
- editor dei livelli con persistenza JSON (geometrie supportate: rettangoli, rettangoli ruotati e spezzate)
- finestra opzionale con shader CPU o GPU tramite OpenGL

<img src="https://github.com/abria/agpunicas/blob/main/demo_leveleditor.png">

#### Diagramma delle classi
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_Core.png">

## CustomPlatformer
Prototipo per semplici platform 2D basato su SDL e su un motore fisico personalizzato.
A scopo dimostrativo implementa una piccola porzione di <i>Super Mario Bros</i> (NES).

<img src="https://github.com/abria/agpunicas/blob/main/demo_SuperMarioBros.png">

#### Diagramma delle classi
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_CustomPlatformer.png">

#### Funzionalità
- continuous collision detection (CCD) con Swept AABB
- collider AABB
- collision response con sliding e correzione delle compenetrazioni
- filtri di collisione basati sul tipo
- dinamica lineare configurabile con attrito e slittamento semplici
- categorie di oggetti statici, dinamici e cinematici
- notifica a tutti gli oggetti collidibili dell'inizio e della fine delle collisioni, con normali e metadati
- trigger, detti anche sensori
- interfaccia di base (HUD e menu)
- selezione tra CCD e collision detection discreta AABB

#### Limitazioni
- nessun collider composto: ogni oggetto può avere un solo collider
- nessuna pendenza
- broad phase basata sulla scansione lineare degli oggetti; narrow phase con test Swept AABB o AABB

## Box2DPlatformer
Prototipo per platform 2D complessi basato su SDL e sul motore fisico Box2D.

<img src="https://github.com/abria/agpunicas/blob/main/demo_Box2DPlatformer.png">

#### Diagramma delle classi
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_Box2DPlatformer.png">

#### Funzionalità
- simulazione del world, collision detection e collision response gestite da Box2D 3.x
- body con shape composte
- body statici, dinamici e cinematici
- notifica a tutti gli oggetti collidibili dell'inizio e della fine delle collisioni, con normali e metadati
- trigger tramite sensor shape
- interfaccia di base (HUD e menu)
- esempi di parallax e overlay di scena
- esempio di fisica del player con camminata, salto, dash e compensazione della forza tangenziale sulle pendenze
- esempio di oggetto cinematico composto (ingranaggio)
- esempi di oggetti dinamici (cassa e proiettile `Fire`)
- esempio di nemico

#### Limitazioni
- il giocatore non resta stabile sulle piattaforme mobili; il problema può essere corretto compensando le forze come sulle pendenze
- nessun esempio di joint; consultare la documentazione di Box2D

## ActionRPG
Prototipo per giochi di ruolo d'azione basato su SDL e su un sistema di collisioni personalizzato.
A scopo dimostrativo implementa una piccola porzione di <i>Legend of Zelda: A Link to the Past</i> (NES).

<img src="https://github.com/abria/agpunicas/blob/main/demo_ActionRPG.png">

#### Diagramma delle classi
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_ActionRPG.png">

#### Funzionalità
- collision detection e collision response basate su OBB
- categorie di oggetti statici e dinamici
- notifica a tutti gli oggetti collidibili dell'inizio e della fine delle collisioni, con normali e metadati
- trigger, detti anche sensori
- interfaccia avanzata (HUD e inventario)
- dialoghi testuali (`DialogBox`)
- portali per il teletrasporto del giocatore
- esempio di attacco del giocatore con la spada
- esempio di animazione di un collider (spada)
- esempio di NPC (soldato con pattugliamento e inseguimento)
- esempio di importazione del JSON prodotto dall'editor dei livelli
- esempio di transizione di scena con maschera circolare e dissolvenza
- pathfinding tramite BFS

#### Limitazioni
- collision detection discreta: la narrow phase usa SAT su OBB, senza CCD per oggetti molto veloci
