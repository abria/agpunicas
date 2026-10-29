# Livelli JSON

I livelli usano il formato del level editor: `categories` contiene i nomi delle categorie e `objects` le regioni da convertire in oggetti di gioco. `LevelData` legge, valida e salva il documento e converte le geometrie. Nei quattro prototipi `LevelLoader::load()` legge il documento e crea la scena; `loadJson()` riceve la scena e il `LevelData` già letto, poi crea gli oggetti specifici del gioco.

I livelli inclusi sono:

- `CustomPlatformer/levels/overworld.json`
- `CustomPlatformerSlopes/levels/1-1.json`
- `Box2DPlatformer/levels/level0.json`
- `ActionRPG/levels/overworld.json`

Tutti e quattro i prototipi caricano `levels/<name>.json` dalla cartella dell'eseguibile e usano lo stesso formato: `categories` e `objects`, con `core_version` come metadato. Nuovi file possono essere caricati passando il loro nome a `LevelLoader::load()`. Cambiano le categorie e le proprietà specifiche del gioco, non la struttura del documento.

## Documento e geometrie

```json
{
   "core_version": "3.7.3",
   "categories": ["Static", "Mario"],
   "objects": [
      {
         "category": 0,
         "rect": {"x": 0, "y": 1, "width": 68, "height": 2, "yUp": false},
         "layer": 0
      },
      {
         "category": 1,
         "rect": {"x": 2.5625, "y": 0, "width": 1, "height": 1, "yUp": false}
      }
   ]
}
```

Dimensioni della scena (`sceneRect`), unità in pixel (`pixelUnitSize`), passo fisico
(`dt`, in secondi) e colore di sfondo sono definiti nel `LevelLoader.cpp` e
passati al costruttore della scena. La vista iniziale è definita nel costruttore
della scena specifica. Questi parametri non fanno parte del JSON.

Per esempio, in CustomPlatformerSlopes il loader crea la scena con:

```cpp
auto* world = new PlatformerGameScene(
    RectF(0, -20, 176, 50), {16, 16}, 0.01f, {156, 252, 240, 255});
world->setJsonPath(path); // file che sarà aperto e salvato dall'editor
```

Ogni oggetto ha `category` (indice in `categories`) e una sola geometria:

- `rect`: `x`, `y`, `width`, `height`, `yUp`; posizione e dimensioni del rettangolo.
- `rotRect`: `cx`, `cy`, `width`, `height`, `angle`, `yUp`; centro, dimensioni e angolo **in gradi**. `LevelData::rotRect()` converte l'angolo in radianti per il gioco.
- `multiline`: array di punti `{"x": ..., "y": ...}`, almeno due. In CustomPlatformerSlopes i costruttori rifiutano segmenti di lunghezza nulla; negli altri loader di polilinee vengono saltati.

`name` è opzionale: normalmente basta la categoria. Si usa quando serve identificare una regione specifica, come i due portali `"A"` di ActionRPG, collegati dal loader tramite il nome. Gli attributi come `layer` e `range` sono proprietà autonome e non richiedono un nome. L'editor mostra l'etichetta solo se presente e omette `name` dal salvataggio quando è vuoto.

## Oggetti e proprietà

| Gioco / categoria | Geometria | Proprietà aggiuntive |
| --- | --- | --- |
| CustomPlatformer / `Static` | Rettangolo allineato agli assi | Collider trasparente senza sprite, `layer` (default 0) |
| CustomPlatformer / `HammerBrother` | Rettangolo del personaggio | `layer`; la posizione viene convertita nello spawn atteso dal costruttore |
| CustomPlatformer / `Lift` | Rettangolo allineato agli assi | `vertical` (default true), `range` (default 3), `layer`; il costruttore carica lo sprite `platform` |
| CustomPlatformer / `Mario` | Rettangolo del personaggio | `layer`; deve essere presente esattamente un Mario |
| CustomPlatformerSlopes / `Slope` | `multiline` di segmenti inclinati, scena con `yUp: false` | Verso ricavato dagli estremi, indipendente dal loro ordine; `layer` (default 1); collider trasparente, collisione sul segmento |
| CustomPlatformerSlopes / `Static`, `Pipe` | Rettangolo allineato agli assi | Collider trasparente senza sprite, `layer` |
| CustomPlatformerSlopes / `Block`, `Brick`, `Wood` | Rettangolo del blocco | `content`: `coin`, `powerup`, `life`, `starman` o stringa vuota; `layer`. Mantiene le implementazioni di SMB3 (starman e foglia non ancora implementati) |
| CustomPlatformerSlopes / `Coin`, `Goomba`, `Mario` | Rettangolo del personaggio/oggetto | `layer`; esattamente un Mario; il loader compensa l'offset dello spawn |
| CustomPlatformerSlopes / `Platform` | `multiline` di segmenti orizzontali | `layer`; collisione solo dall’alto per tutti i dinamici; attraversabili dal basso e dai lati |
| CustomPlatformerSlopes / `Lift` | Rettangolo allineato agli assi | `vertical`, `range` positivo, `layer`; il costruttore carica la sprite `platform` |
| Box2DPlatformer / `Renderable` | `rect` o `rotRect` | `layer`; oggetto trasparente senza fisica |
| Box2DPlatformer / `Terrain` | `multiline` | `layer`; un collider per segmento, la grafica del terreno è creata nel loader C++ |
| Box2DPlatformer / `Static` | `rect` o `rotRect` | Collider trasparente senza sprite, `layer` |
| Box2DPlatformer / `Gear` | `rect` o `rotRect` | `layer`; il costruttore carica lo sprite `gear` |
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

ActionRPG descrive nel JSON NPC, Soldier, Link, collider e portali. Il loader crea prima Link, poi le altre entità e i portali: il collegamento non dipende dall'ordine delle regioni nel documento. L'esempio mantiene le stesse posizioni, geometrie e regole di gameplay.

Il trigger di CustomPlatformer è un esempio hardcoded nel loader: dopo aver creato Mario e gli ascensori, viene istanziato in `RectF(1, -12, 0.5f, 13)` passando il giocatore e una lambda che alterna lo stato `freezed` degli ascensori. `Trigger` resta generico e riceve un task `std::function<void()>`, come negli altri prototipi. In CustomPlatformer il task viene eseguito all'ingresso e all'uscita dell'oggetto osservato; in Box2DPlatformer e ActionRPG soltanto all'ingresso, secondo il comportamento originale. Il trigger di esempio non è una regione del JSON e non richiede ID, azioni o riferimenti da risolvere.

Le classi specifiche caricano la propria sprite nel costruttore, senza riceverla
dal loader. I collider generici `Static` non hanno sprite né colore: sono
trasparenti e si appoggiano alla grafica di sfondo. Il JSON non contiene i
campi `sprite`, `color` o `backgroundColor`: ogni classe definisce il proprio
aspetto nel costruttore; per la scena il colore di sfondo arriva dal loader C++. Le classi base di movimento continuano a ricevere la sprite
dalle sottoclassi.

Sfondi e parallassi sono creati direttamente nei rispettivi `LevelLoader.cpp`,
con chiamate esplicite: non fanno parte del JSON perché non sono oggetti
creabili nell'editor.

- CustomPlatformerSlopes: immagine `level_1_1` dietro i collider.
- ActionRPG: immagini `overworld` e `linkhouse`.
- Box2DPlatformer: immagine del terreno `fg_ground`; cielo, case ed erba come
  sfondi; pioggia e nebbia in primo piano. Le velocità di parallasse e la
  ripetizione delle immagini sono argomenti dei costruttori `OverlayScene`.
- CustomPlatformer: terreno, blocchi e tubi sono immagini create nel C++,
  sovrapposte ai collider `Static` del JSON.

Le immagini fisse restano visibili nell'editor, senza rettangoli selezionabili;
si modificano nel C++. I collider restano in `objects`: ad esempio `Terrain`
in Box2DPlatformer e le linee `Slope` in CustomPlatformerSlopes. Gli estremi
delle slope ne determinano il verso, senza `risesRight`. Le slope e i collider generici sono trasparenti nel gioco. Anche il trigger della
linea di morte rimane nel loader C++.

## Modifica e salvataggio

Il tasto `E` apre il JSON effettivamente caricato dal livello. L'editor modifica geometrie, categorie ed etichette e conserva le proprietà specifiche degli oggetti. Il documento salvato contiene soltanto `core_version`, `categories` e `objects`; l'editor usa dimensioni e unità della scena di gioco già creata dal loader. Le proprietà specifiche del gioco si impostano direttamente nel JSON; non è stato aggiunto un pannello delle proprietà.

CMake copia i JSON accanto all'eseguibile a ogni build, anche senza modifiche C++. L'editor salva quella **copia di esecuzione**: riportare le modifiche nel file sorgente del progetto prima della build successiva, che ricopia i livelli distribuiti. Per applicare un livello modificato occorre ricaricarlo o riavviare il gioco.

File mancanti, documenti malformati, categorie fuori intervallo e geometrie non valide producono un errore esplicito.
