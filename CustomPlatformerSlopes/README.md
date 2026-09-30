# CustomPlatformerSlopes

Quarto prototipo della repo, ottenuto da `2025-SuperMarioBros3/SMB3` e dalle
pendenze di `06-SimplePlatformer-Slopes`. CMake usa `../core` e `../utils`:
non ci sono copie locali delle librerie. Sprite e audio provengono da SMB3.

Il livello iniziale è `levels/1-1.json`, nel [formato condiviso](../core/levels.md).
`LevelLoader::load()` legge `LevelData` e crea la scena; `loadJson()` configura
la scena e istanzia gli oggetti. Spawn di Mario, nemici, sfondo e impostazioni
sono nel JSON. Il contenuto dei blocchi usa `content`, separato da `name`.
La linea di morte rimane un trigger C++ a y = 4, come nel progetto originale.

Sono mantenuti i quattro Goomba già definiti nel livello. La generazione di
100 Goomba casuali è stata rimossa; i segnaposto per nemici non implementati
nel progetto di partenza non sono inclusi nel JSON eseguibile.

## Pendenze

Le sei rampe sono visibili come triangoli marroni, senza sprite:

- x = 5–12: salita, raccordo piano e discesa vicino alla partenza;
- x = 20–27: salita ripida e discesa raccordate al primo tubo;
- x = 45–53: due pendenze adiacenti che formano una valle.

Una slope si aggiunge come oggetto di categoria `Slope` (indice 9 nel livello):

```json
{
   "category": 9,
   "rect": {"x": 5, "y": -1, "width": 3, "height": 2, "yUp": false},
   "risesRight": true,
   "color": [190, 120, 65, 255],
   "layer": 1
}
```

`risesRight: true` indica una salita verso destra; `false` verso sinistra.
Larghezza e altezza devono essere positive. La superficie usa l'angolo del
collider rivolto a monte. Con pendenza `s = height / width`, lo spostamento
orizzontale è moltiplicato per `1 / (1 + s)` in salita e `1 + s` in discesa;
la velocità memorizzata viene ripristinata alla fine del passo.

Quando l'entità è ferma e non impartisce un comando orizzontale, scivola
spontaneamente verso valle. La velocità lungo la superficie è `5 * min(s, 2)`
unità di scena al secondo, proiettata sugli assi della rampa: cresce con la
pendenza ma resta moderata anche sulle rampe ripide. Il comando di movimento
ha precedenza sullo scivolamento, quindi si può ripartire in salita da fermi.
Al rilascio durante la salita, una decelerazione aggiuntiva di `60 * (1 + s)`
smorza rapidamente l'inerzia residua: segue subito lo scivolamento, conservando
un breve tratto di rallentamento. In fondo alle valli l'entità si ferma senza
oscillazioni.

`Slope` definisce superficie, lati solidi e disegno. `DynamicObject`
adatta lo spostamento prima della collisione e registra il contatto dopo la
risoluzione, anche nel modo discreto usato sulle piattaforme cinematiche.
Mario, nemici e power-up ereditano questo comportamento. I salti staccano
l'oggetto dalla pendenza; pareti e soffitti restano solidi.

Le pendenze sono statiche e triangolari; gli oggetti mobili restano AABB.
Non sono supportate slope mobili o ruotate. La risposta alla pendenza è
arcade, con rallentamento in salita e scivolamento a riposo.

## Piattaforme attraversabili

`Platform` gestisce direttamente le collisioni di tutti i dinamici: Mario,
nemici e power-up possono attraversarla dai lati e dal basso, mentre vi
atterrano dall'alto. Il filtro usa il collider all'inizio del passo fisico:
un salto che porta sopra la piattaforma soltanto la testa non aggancia il corpo.
I test della forma rispondono solo verso l'alto, anche alle estremità.
`Mario` eredita il comportamento comune senza un'eccezione dedicata.

Il controllo bilaterale di `collidableWith` era già presente in
`CollidableObject.cpp`, anche per gli statici; non è stato necessario modificarlo.

## Comandi ed editor

Frecce sinistra/destra: movimento; Spazio: salto; Z: corsa; freccia giù:
accovacciamento; Invio/Esc: pausa; T/H: power-up/power-down di prova.
C mostra i collider, R i rettangoli degli oggetti, M attiva la camera manuale,
E apre l'editor. L'editor mostra il rettangolo che delimita ogni slope e
conserva `risesRight`, `color`, `content` e le altre proprietà nel salvataggio.
Queste proprietà si modificano direttamente nel JSON.

L'editor salva la copia del livello accanto all'eseguibile. Riportare le
modifiche nel JSON sorgente prima di ricompilare: ogni build ricopia i livelli.

## Compilazione e verifiche

Dalla radice della repo, con CMake, compilatore C++14 e i pacchetti SDL3,
SDL3_image e SDL3_mixer configurati come per gli altri prototipi:

```sh
cmake -S CustomPlatformerSlopes -B ../build-CustomPlatformerSlopes
cmake --build ../build-CustomPlatformerSlopes
```

I test opzionali verificano salite e discese in entrambi i versi, raccordi,
valli, velocità, salti, atterraggi, soffitti, pareti, trigger e piattaforme.
Verificano inoltre passaggi laterali e dal basso, salti parziali, scivolamento
a riposo con limite di velocità e ripartenza in salita su pendenze diverse.
Una seconda verifica carica il livello e gli asset, muove il vero Mario e
salva/ricarica le geometrie tramite la serializzazione dell'editor.

```sh
cmake -S CustomPlatformerSlopes -B ../build-CustomPlatformerSlopes -DSLOPES_BUILD_TESTS=ON
cmake --build ../build-CustomPlatformerSlopes
ctest --test-dir ../build-CustomPlatformerSlopes --output-on-failure
```

Il test del livello usa video e audio SDL dummy e produce `slopes-preview.png`
nella cartella di compilazione. Il core condiviso carica anche le musiche MP3
originali di SMB3, mantenendo la precedenza dei WAV se hanno lo stesso nome.
