# Livelli JSON

I livelli usano il formato del level editor: `categories` contiene i nomi delle categorie e `objects` le regioni da convertire in oggetti di gioco. `LevelData` legge e valida il documento, converte le geometrie e applica le impostazioni della scena. Nei quattro prototipi `LevelLoader::load()` legge il documento e crea la scena; `loadJson()` riceve la scena e il `LevelData` già letto, poi crea gli oggetti specifici del gioco.

I livelli inclusi sono:

- `CustomPlatformer/levels/overworld.json`
- `CustomPlatformerSlopes/levels/1-1.json`
- `Box2DPlatformer/levels/level0.json`
- `ActionRPG/levels/overworld.json`

Tutti e quattro i prototipi caricano `levels/<name>.json` dalla cartella dell'eseguibile e usano lo stesso formato: `scene`, `categories` e `objects`. Nuovi file possono essere caricati passando il loro nome a `LevelLoader::load()`. Cambiano le categorie e le proprietà specifiche del gioco, non la struttura del documento.

## Documento e geometrie

```json
{
   "core_version": "3.7.3",
   "categories": ["Static", "Mario"],
   "scene": {
      "rect": {"x": 0, "y": -20, "width": 224, "height": 50, "yUp": false},
      "pixelUnitSize": {"x": 16, "y": 16},
      "dt": 0.01,
      "backgroundColor": [92, 148, 252, 255],
      "view": {"x": 0, "y": -12, "width": 16, "height": 15, "yUp": false}
   },
   "objects": [
      {
         "category": 0,
         "rect": {"x": 0, "y": 1, "width": 68, "height": 2, "yUp": false},
         "sprite": "terrain",
         "layer": 0
      },
      {
         "category": 1,
         "rect": {"x": 2.5625, "y": 0, "width": 1, "height": 1, "yUp": false}
      }
   ]
}
```

`scene` è obbligatorio in tutti e quattro i prototipi e contiene `rect`, `pixelUnitSize` e `dt`. `backgroundColor` (RGBA) e `view` sono opzionali. `dt` è il passo fisico in secondi.

Ogni oggetto ha `category` (indice in `categories`) e una sola geometria:

- `rect`: `x`, `y`, `width`, `height`, `yUp`; posizione e dimensioni del rettangolo.
- `rotRect`: `cx`, `cy`, `width`, `height`, `angle`, `yUp`; centro, dimensioni e angolo **in gradi**. `LevelData::rotRect()` converte l'angolo in radianti per il gioco.
- `multiline`: array di punti `{"x": ..., "y": ...}`, almeno due. I segmenti di lunghezza nulla vengono saltati dai loader.

`name` è opzionale: normalmente basta la categoria. Si usa quando serve identificare una regione specifica, come i due portali `"A"` di ActionRPG, collegati dal loader tramite il nome. Gli attributi come `sprite`, `layer` e `range` sono proprietà autonome e non richiedono un nome. L'editor mostra l'etichetta solo se presente e omette `name` dal salvataggio quando è vuoto.

## Oggetti e proprietà

| Gioco / categoria | Geometria | Proprietà aggiuntive |
| --- | --- | --- |
| CustomPlatformer / `Static` | Rettangolo allineato agli assi | `sprite` opzionale, `layer` (default 0) |
| CustomPlatformer / `HammerBrother` | Rettangolo del personaggio | `layer`; la posizione viene convertita nello spawn atteso dal costruttore |
| CustomPlatformer / `Lift` | Rettangolo allineato agli assi | `vertical` (default true), `range` (default 3), `layer`; il costruttore richiede lo sprite `platform` |
| CustomPlatformer / `Mario` | Rettangolo del personaggio | `layer`; deve essere presente esattamente un Mario |
| CustomPlatformerSlopes / `Slope` | Rettangolo allineato agli assi, `yUp: false` | `risesRight` (default true), `color` RGBA (default marrone), `layer` (default 1); collider e disegno triangolari senza sprite |
| CustomPlatformerSlopes / `Static`, `Pipe` | Rettangolo allineato agli assi | `sprite` e `color` opzionali, `layer` |
| CustomPlatformerSlopes / `Block`, `Brick`, `Wood` | Rettangolo del blocco | `content`: `coin`, `powerup`, `life`, `starman` o stringa vuota; `layer`. Mantiene le implementazioni di SMB3 (starman e foglia non ancora implementati) |
| CustomPlatformerSlopes / `Coin`, `Goomba`, `Mario` | Rettangolo del personaggio/oggetto | `layer`; esattamente un Mario; il loader compensa l'offset dello spawn |
| CustomPlatformerSlopes / `Platform` | `multiline` di segmenti orizzontali | `layer`; collisione solo dall’alto per tutti i dinamici; attraversabili dal basso e dai lati |
| CustomPlatformerSlopes / `Lift` | Rettangolo allineato agli assi | `vertical`, `range` positivo, `sprite` e `color` opzionali, `layer` |
| Box2DPlatformer / `Renderable` | `rect` o `rotRect` | `sprite` opzionale, `layer`; decorazione senza fisica |
| Box2DPlatformer / `Terrain` | `multiline` | `layer`; un collider per segmento, la grafica del terreno è in `backgroundImages` |
| Box2DPlatformer / `Static` | `rect` o `rotRect` | `sprite` opzionale, `layer` |
| Box2DPlatformer / `Gear` | `rect` o `rotRect` | `layer`; il costruttore richiede lo sprite `gear` |
| Box2DPlatformer / `Box` | `rect` o `rotRect` | `layer`; mantiene lo sprite e l'impulso iniziale del costruttore |
| Box2DPlatformer / `Slime` | `rect` o `rotRect` | `layer`; il centro indica lo spawn |
| Box2DPlatformer / `Player` | `rect` o `rotRect` | `layer`; il centro indica lo spawn, deve essere presente esattamente un player |
| ActionRPG / `Static`, `Bush` | `rect` o `rotRect` | Collider statico, senza sprite |
| ActionRPG / `Static`, `Cliff` | `multiline` | Un collider statico per segmento valido |
| ActionRPG / `Portal` | `rect` o `rotRect` | `name` non vuoto; collega la coppia con lo stesso nome |
| ActionRPG / `Clipper` | `rect` o `rotRect` | Regione di clipping del rendering |
| ActionRPG / `NPC` | `rect` o `rotRect` | La posizione del rettangolo indica lo spawn |
| ActionRPG / `Soldier` | `rect` o `rotRect` | Posizione di spawn e `patrolRect`, con gli stessi campi di `rect` |
| ActionRPG / `Link` | `rect` o `rotRect` | La posizione del rettangolo indica lo spawn; deve essere presente esattamente un Link |

I personaggi mantengono dimensioni e collider definiti dalle rispettive classi; il rettangolo nell'editor ne determina la posizione. CustomPlatformer supporta rettangoli allineati agli assi: rotazioni e polilinee richiederebbero un supporto fisico aggiuntivo e vengono segnalate come errori.

ActionRPG descrive nel JSON anche il colore e la vista iniziale della scena, gli sfondi, NPC, Soldier e Link. Il loader crea prima Link, poi le altre entità e i portali: il collegamento non dipende dall'ordine delle regioni nel documento. L'esempio mantiene le stesse posizioni, geometrie e regole di gameplay.

Il trigger di CustomPlatformer è un esempio hardcoded nel loader: dopo aver creato Mario e gli ascensori, viene istanziato in `RectF(1, -12, 0.5f, 13)` passando il giocatore e una lambda che alterna lo stato `freezed` degli ascensori. `Trigger` resta generico e riceve un task `std::function<void()>`, come negli altri prototipi. In CustomPlatformer il task viene eseguito all'ingresso e all'uscita dell'oggetto osservato; in Box2DPlatformer e ActionRPG soltanto all'ingresso, secondo il comportamento originale. Il trigger di esempio non è una regione del JSON e non richiede ID, azioni o riferimenti da risolvere.

Le classi specifiche scelgono lo sprite nel proprio costruttore. Il campo `sprite` rimane per gli oggetti generici, come `StaticObject` e `RenderableObject`, e per le immagini di sfondo.

In CustomPlatformerSlopes, Box2DPlatformer e ActionRPG le immagini fisse del mondo sono nell'array principale `backgroundImages`: il terreno del platformer, oppure l'overworld e la casa di Link. Per esempio:

```json
{
   "rect": {"x": 0, "y": 0, "width": 96, "height": 13, "yUp": true},
   "sprite": "fg_ground",
   "layer": -1
}
```

CustomPlatformerSlopes usa lo sprite `level_1_1` per lo sfondo originale SMB3, con `layer: 0`. Le pendenze sono rettangoli di categoria `Slope`: il rettangolo ne definisce larghezza e altezza; `risesRight` seleziona il verso della salita. Le proprietà `risesRight`, `color` e `content` sono indipendenti dall’etichetta `name`. Il trigger della linea di morte rimane nel loader C++.

Queste immagini non hanno categoria né nome; accettano `rect` o `rotRect`, uno `sprite` e un `layer` (default -1). Restano visibili nel gioco e nell'editor, senza generare rettangoli modificabili che coprano i collider. Il profilo fisico del terreno rimane una `multiline` di categoria `Terrain` in `objects`.

In Box2DPlatformer gli sfondi e gli effetti con parallasse sono nell'array principale `overlays`, nell'ordine di disegno:

```json
{
   "placement": "background",
   "sprite": "bg_houses",
   "parallax": {"x": 0.2, "y": 1},
   "seamless": true
}
```

`placement` può essere `background` o `foreground`. `parallax` ha default `(0,0)` e `seamless` ha default false; gli sfondi ripetuti richiedono uno sprite compatibile (`FilledSprite`).

## Modifica e salvataggio

Il tasto `E` apre il JSON effettivamente caricato dal livello. L'editor modifica geometrie, categorie ed etichette e conserva le altre proprietà degli oggetti e del documento, comprese impostazioni della scena e sfondi. Quando crea un nuovo documento, l'editor include `scene` ricavandolo dalla scena di gioco. Le proprietà specifiche del gioco si impostano direttamente nel JSON; non è stato aggiunto un pannello delle proprietà.

CMake copia i JSON accanto all'eseguibile a ogni build, anche senza modifiche C++. L'editor salva quella **copia di esecuzione**: riportare le modifiche nel file sorgente del progetto prima della build successiva, che ricopia i livelli distribuiti. Per applicare un livello modificato occorre ricaricarlo o riavviare il gioco.

File mancanti, documenti malformati, categorie fuori intervallo e geometrie non valide producono un errore esplicito. Non è previsto un formato alternativo senza `scene`.
