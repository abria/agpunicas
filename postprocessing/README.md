# Shader post-processing delle demo

Le tre demo caricano gli stessi dieci fragment shader dalla cartella `assets/shaders` accanto all'eseguibile. Premere un numero per attivare l'effetto; premerlo di nuovo per disattivarlo. Gli effetti possono essere combinati e vengono applicati nell'ordine in cui sono stati attivati.

| Tasto | Shader | Risultato |
| --- | --- | --- |
| `1` | `rain` | Pioggia animata, scena più cupa e lampi di luce fredda pseudo-casuali |
| `2` | `fog` | Nebbia densa, stratificata e mobile |
| `3` | `crt` | Curvatura, scanline e maschera RGB |
| `4` | `pixelate` | Mosaicatura, palette ridotta e dithering |
| `5` | `neon` | Contorni luminosi ciano e magenta |
| `6` | `heat` | Rifrazione e aberrazione cromatica |
| `7` | `nightvision` | Visione notturna con grana e vignettatura |
| `8` | `glitch` | Distorsioni orizzontali e separazione dei canali |
| `9` | `bloom` | Alone luminoso sulle zone chiare |
| `0` | `vortex` | Torsione radiale animata |

Ogni effetto ha un sorgente `.frag.hlsl` e tre versioni già compilate: `.msl` per Metal, `.dxil` per Direct3D 12 e `.spv` per Vulkan. CMake copia questi file a ogni build, ma non compila automaticamente l'HLSL: dopo aver modificato un sorgente, rigenerare i tre formati con `shadercross`:

```sh
shadercross assets/shaders/fog.frag.hlsl -o assets/shaders/fog.frag.msl
shadercross assets/shaders/fog.frag.hlsl -o assets/shaders/fog.frag.dxil
shadercross assets/shaders/fog.frag.hlsl -o assets/shaders/fog.frag.spv
```

Eseguire questi comandi dalla cartella `postprocessing`. Se SDL_GPU non trova un backend compatibile, le demo usano il normale renderer SDL: restano giocabili, ma gli effetti GPU non vengono applicati. In ActionRPG gli effetti CPU già usati dal portale e dal tasto `L` restano attivi anche in questo caso.
