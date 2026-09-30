# Algoritmi e Programmazione dei Videogiochi @ Università di Cassino

Un framework per programmare videogiochi 2D in C++ con SDL3.

> [!IMPORTANT]
> Questo è il repository ufficiale dell'insegnamento di ***Algoritmi e Programmazione dei Videogiochi*** del corso di laurea in _Ingegneria Informatica e delle Telecomunicazioni_ dell'Università di Cassino. La cartella Drive del corso, che include slide, esercizi e codice sorgente, è pubblicamente accessibile a [questo indirizzo](https://drive.google.com/open?id=1Qk3xPAt2qOVHL6Si3TY1qLDPabbv_kcsZ92gnlwyDK176RSNcc92Jy5ALNNzp6Kzfj_IKN8s&usp=drive_fs).

<img src="https://github.com/abria/agpunicas/blob/main/demo_leveleditor.png">


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
- funzioni di supporto per le spritesheet (autotiling ed estrazione delle componenti connesse)

## Core
Motore di rendering e audio basato su SDL, utilizzato da tutti i prototipi di gioco. Include:
- game loop con semi-fixed timestep
- framework Scene/View/Window con adattamento automatico alla risoluzione dello schermo e scene sovrapposte
- separazione tra scene dell'interfaccia e scene di gioco
- modello base degli oggetti con metodi per posizionamento, rendering, aggiornamento e scheduling
- query spaziali mediante quadtree e raycasting
- sistema di sprite (`AnimatedSprite`, `TiledSprite`, `FilledSprite`) con blitting GPU dalle spritesheet
- camera manuale o agganciata al giocatore
- sistema audio con suoni e musiche riproducibili, sospendibili e ripristinabili
- parallax e overlay di scena
- sprite testuali generate da font mediante SDL_ttf
- editor dei livelli con persistenza JSON (geometrie supportate: rettangoli, rettangoli ruotati e spezzate)
- caricamento condiviso dei livelli JSON tramite `LevelData`, con validazione e conservazione delle proprietà specifiche dei giochi nell'editor ([formato e utilizzo](core/levels.md))
- finestra opzionale con shader CPU o post-processing GPU tramite SDL_GPU; le tre demo condividono dieci effetti attivabili con i tasti `1`-`9` e `0`

#### Diagramma delle classi
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_Core.png">

## CustomPlatformer
Prototipo per platform 2D con fisica personalizzata realizzata _from scratch_ e pendenze, basato su <i>Super Mario Bros</i>.
Usa le librerie condivise `core` e `utils`.

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
- livello caricato da `levels/overworld.json`, inclusi nemici e piattaforme mobili; esempio di trigger collegato direttamente in C++

#### Limitazioni
- nessun collider composto: ogni oggetto può avere un solo collider
- nessuna pendenza (usare CustomPlatformerSlopes se si desidera questa funzionalità)
- broad phase basata sulla scansione lineare degli oggetti; narrow phase con test Swept AABB o AABB

## CustomPlatformerSlopes
Prototipo per platform 2D con fisica personalizzata realizzata _from scratch_ e pendenze, basato su <i>Super Mario Bros 3</i>.
Usa le librerie condivise `core` e `utils`.

<img src="https://github.com/abria/agpunicas/blob/main/demo_SuperMarioBros3.png">

#### Funzionalità
- tutte quelle di CustomPlatformer, di cui questo è un'estensione
- collider triangolari statici, con salita e discesa in entrambe le direzioni
- aderenza al terreno, raccordi con superfici piane, salti, salita rallentata e scivolamento moderato a riposo
- collisioni con pareti, soffitti, piattaforme attraversabili dal basso per tutti i dinamici, nemici e oggetti raccoglibili
- gameplay SMB3 con blocchi, monete, Goomba, power-up, HUD e menu
- livello `levels/1-1.json` caricato tramite `LevelData`, con sei rampe colorate senza sprite
- geometrie e proprietà delle slope conservate dall'editor condiviso

#### Diagramma delle classi
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_CustomPlatformer.png">

#### Limitazioni
- tutte quelle di CustomPlatformer, di cui questo è un'estensione

## Box2DPlatformer
Prototipo per platform 2D con fisica newtoniana complessa basato sul motore fisico Box2D.
Usa le librerie condivise `core` e `utils`.

<img src="https://github.com/abria/agpunicas/blob/main/demo_Box2DPlatformer.png">

#### Diagramma delle classi
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_Box2DPlatformer.png">

#### Funzionalità
- simulazione del world, collision detection e collision response gestite da Box2D 3.x
- body con shape composte
- body statici, dinamici e cinematici
- notifica a tutti gli oggetti collidabili dell'inizio e della fine delle collisioni, con normali e metadati
- trigger tramite sensor shape
- interfaccia di base (HUD e menu)
- esempi di parallax e overlay di scena
- esempio di fisica del player con camminata, salto, dash e compensazione della forza tangenziale sulle pendenze
- esempio di oggetto cinematico composto (ingranaggio)
- esempi di oggetti dinamici (cassa e proiettile `Fire`)
- esempio di nemico
- livello caricato da `levels/level0.json`, inclusi terreno, ingranaggi, player e sfondi

#### Limitazioni
- il giocatore non resta stabile sulle piattaforme mobili; il problema può essere corretto compensando le forze come sulle pendenze
- nessun esempio di joint; consultare la documentazione di Box2D

## ActionRPG
Prototipo per giochi di ruolo (e più in generale con vista dall'alto) basato su core/SDL e su un motore fisico personalizzabile realizzato _from scratch_.
A scopo dimostrativo implementa una piccola porzione di <i>Legend of Zelda: A Link to the Past</i> (NES).

<img src="https://github.com/abria/agpunicas/blob/main/demo_ActionRPG.png">

#### Diagramma delle classi
<img src="https://github.com/abria/agpunicas/blob/main/classdiagram_ActionRPG.png">

#### Funzionalità
- collision detection e collision response basate su OBB
- categorie di oggetti statici e dinamici
- notifica a tutti gli oggetti collidabili dell'inizio e della fine delle collisioni, con normali e metadati
- trigger, detti anche sensori
- interfaccia avanzata (HUD e inventario)
- dialoghi testuali (`DialogBox`)
- portali per il teletrasporto del giocatore
- esempio di attacco del giocatore con la spada
- esempio di collider animato (spada)
- pathfinding tramite BFS
- esempio di NPC (soldato con pattugliamento e inseguimento)
- livello caricato da `levels/overworld.json`, inclusi scena, sfondi, personaggi, collider e portali
- esempio di transizione di scena con maschera circolare e dissolvenza

#### Limitazioni
- broad phase basata sulla scansione lineare degli oggetti; narrow phase con test SAT su OBB
