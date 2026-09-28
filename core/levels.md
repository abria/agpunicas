# Livelli JSON

I livelli usano il formato del level editor: `categories` contiene i nomi delle categorie e `objects` le regioni da convertire in oggetti di gioco. `LevelData` legge e valida il documento, converte le geometrie e applica le impostazioni della scena. Ogni `LevelLoader` mantiene soltanto la creazione degli oggetti specifici del gioco e i loro collegamenti.

I livelli inclusi sono:

- `CustomPlatformer/levels/overworld.json`
- `Box2DPlatformer/levels/level0.json`
- `ActionRPG/editor/EditorScene.json` (formato precedente, ancora supportato)

CustomPlatformer e Box2DPlatformer caricano `levels/<name>.json` dalla cartella dell'eseguibile. Nuovi file possono essere caricati passando il loro nome a `LevelLoader::load()`. ActionRPG mantiene il suo livello e il percorso `EditorScene.json`.

## Documento e geometrie

```json
{
   "core_version": "3.7.1",
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
         "id": "mario",
         "rect": {"x": 2.5625, "y": 0, "width": 1, "height": 1, "yUp": false}
      }
   ]
}
```

`scene` è richiesto dai due platformer. Nei documenti precedenti di ActionRPG può essere assente. `backgroundColor` (RGBA) e `view` sono opzionali. `dt` è il passo fisico in secondi.

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
| CustomPlatformer / `Lift` | Rettangolo allineato agli assi | `sprite` (default `platform`), `vertical` (default true), `range` (default 3), `layer`, `id` |
| CustomPlatformer / `Mario` | Rettangolo del personaggio | `id`, `layer`; deve essere presente esattamente un Mario |
| CustomPlatformer / `Trigger` | Rettangolo allineato agli assi | `action`, `watched`, `targets` |
| Box2DPlatformer / `Renderable` | `rect` o `rotRect` | `sprite`, `layer`; decorazione senza fisica |
| Box2DPlatformer / `Terrain` | `multiline` | `sprite` opzionale, `layer`; un terreno per segmento |
| Box2DPlatformer / `Static` | `rect` o `rotRect` | `sprite` opzionale, `layer` |
| Box2DPlatformer / `Gear` | `rect` o `rotRect` | `sprite` (default `gear`), `layer` |
| Box2DPlatformer / `Box` | `rect` o `rotRect` | `layer`; mantiene lo sprite e l'impulso iniziale del costruttore |
| Box2DPlatformer / `Slime` | `rect` o `rotRect` | `layer`; il centro indica lo spawn |
| Box2DPlatformer / `Player` | `rect` o `rotRect` | `layer`; il centro indica lo spawn, deve essere presente esattamente un player |

I personaggi mantengono dimensioni e collider definiti dalle rispettive classi; il rettangolo nell'editor ne determina la posizione. CustomPlatformer supporta rettangoli allineati agli assi: rotazioni e polilinee richiederebbero un supporto fisico aggiuntivo e vengono segnalate come errori.

Il trigger di CustomPlatformer usa `"action": "toggleFreezed"`, `"watched": "mario"` e `"targets": ["lift1", "lift2"]`. I riferimenti puntano agli `id`, che devono essere univoci, e vengono risolti dopo la creazione degli oggetti: l'ordine delle voci nel JSON non conta. `name` può essere cambiato nell'editor senza modificare questi collegamenti.

In Box2DPlatformer le immagini fisse del mondo, come la grafica del terreno, sono nell'array principale `backgroundImages`:

```json
{
   "rect": {"x": 0, "y": 0, "width": 96, "height": 13, "yUp": true},
   "sprite": "fg_ground",
   "layer": -1
}
```

Queste immagini non hanno categoria né nome; accettano `rect` o `rotRect`, uno `sprite` e un `layer` (default -1). Restano visibili nel gioco e nell'editor, senza generare rettangoli modificabili che coprano i collider. Il profilo fisico del terreno rimane una `multiline` di categoria `Terrain` in `objects`.

Gli sfondi e gli effetti con parallasse sono nell'array principale `overlays`, nell'ordine di disegno:

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

Il tasto `E` apre il JSON effettivamente caricato dal livello. L'editor modifica geometrie, categorie ed etichette e conserva le altre proprietà degli oggetti e del documento, comprese impostazioni della scena, ID, trigger e sfondi. Le proprietà specifiche del gioco si impostano direttamente nel JSON; non è stato aggiunto un pannello delle proprietà.

CMake copia i JSON accanto all'eseguibile a ogni build, anche senza modifiche C++. L'editor salva quella **copia di esecuzione**: riportare le modifiche nel file sorgente del progetto prima della build successiva, che ricopia i livelli distribuiti. Per applicare un livello modificato occorre ricaricarlo o riavviare il gioco.

File mancanti, documenti malformati, categorie fuori intervallo e geometrie non valide producono un errore esplicito. I vecchi JSON di ActionRPG rimangono leggibili senza conversione.
