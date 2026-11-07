# Figure per la guida alle slope

Figure vettoriali tratte dalla relazione **Slope in SimplePlatformer** di
Alessandro Bria, creata con Claude e fornita dall'autore per questa wiki:
https://claude.ai/artifact/86ze9SdbxTd1KGXnAZWAxw

Gli SVG originali sono stati esportati dal documento e resi autonomi includendo
stili, colori del tema chiaro e marcatori delle frecce. Geometrie e dati sono
quelli della relazione; i font usano fallback di sistema. Nessuno script o
collegamento a risorse esterne è necessario per visualizzarli.

Le etichette usano i nomi di CustomPlatformerSlopes, non più le sigle P, R, U
dei blocchi del prototipo didattico del Drive: *prepare* indica
`Slope::prepareMovement`, *finish* indica `Slope::Motion::finishMovement`
(entrambi coordinati da `DynamicObject`) e *piano* il rettangolo pianificato
prima delle altre collisioni. La guida nella wiki spiega la corrispondenza con
il prototipo del Drive.
In particolare `restore-y.svg` mostra la simulazione didattica con g = 24,
dt = 0.01 e accelerazione orizzontale 12: non è una misura del prototipo SMB3,
che usa altri parametri.

`support-y.svg`, `valley-rest.svg` e `speed-factor.svg` sono state disegnate per
CustomPlatformerSlopes, nello stesso stile, dopo la rifinitura del 5 novembre 2026:
penetrazione affusolata (`Slope::supportY`), arresto nel tratto piano delle valli
e moltiplicatori orizzontali costanti. Le altre figure omettono la penetrazione
(al massimo 0.25 unità) e mostrano i piedi sul segmento.

Guida: https://github.com/abria/agpunicas/wiki/CustomPlatformerSlopes-Movimento-e-Collisioni
