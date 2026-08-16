# Simple Macro (Geode / Geometry Dash)

Mod Geode qui enregistre tes appuis/relâchements de bouton frame par frame
pendant un run, puis les rejoue automatiquement.

## Contrôles en jeu

- **F2** — Démarrer / arrêter l'enregistrement
- **F3** — Démarrer / arrêter la lecture du macro chargé en mémoire
- **F4** — Sauvegarder le macro enregistré (fichier `last.macro`)
- **F5** — Charger le macro sauvegardé

Le fichier est stocké dans le dossier de sauvegarde du mod
(`Mod::get()->getSaveDir()`), donc pas besoin de gérer les chemins toi-même.

## Compilation

Tu as besoin du [Geode SDK](https://docs.geode-sdk.org/) installé et de la
CLI `geode` configurée.

```bash
git clone <ton repo ou ce dossier>
cd SimpleMacro
geode build
```

ou, sans la CLI :

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

Le `.geode` généré se trouve ensuite dans `build/`. Copie-le (ou utilise
`geode build --install`) dans le dossier `mods` de Geode.

## Obtenir un vrai fichier `.geode` pour Android (sans rien installer)

Je ne peux pas compiler de binaire moi-même (pas d'accès réseau/NDK dans mon
environnement), mais ce dossier contient un workflow GitHub Actions
(`.github/workflows/build.yml`) qui le fait à ta place, gratuitement :

1. Crée un dépôt GitHub (public ou privé) et pousse ce dossier dedans.
2. Va dans l'onglet **Actions** du dépôt → le workflow « Build » se lance
   automatiquement (ou clique sur *Run workflow*).
3. Attends la fin des 4 builds (Windows / MacOS / Android32 / Android64)
   + le job « Assembler le .geode final ».
4. Télécharge l'artifact **SimpleMacro.geode** — c'est ton fichier `.geode`
   multi-plateforme (utilisable directement sur Android, dernière version
   de Geode/GD 2.2081).
5. Transfère ce fichier dans le dossier `geode/mods` sur ton téléphone
   (via un gestionnaire de fichiers ou en le partageant vers l'app Geode).

## Interface (style Silicate)

Le popup reprend la disposition de ta capture : sidebar à gauche (Record actif
+ 4 onglets inertes pour l'instant), panneau bordeaux translucide à droite
avec titre "Record", champ "Replay Name", boutons Load/Save, et radios
Record/Play. Seuls **Record, Play, Load et Save fonctionnent réellement** ;
Assist/Prediction/Render/Settings affichent juste "Bientôt disponible".

Limites assumées, à vérifier/corriger toi-même à la compilation :
- Pas de vrai flou d'arrière-plan (backdrop blur) : cocos2d-x (le moteur de
  GD) n'a pas d'équivalent simple à ça sans shader custom fragile d'une
  version de GD à l'autre. L'effet "verre" est simulé par un panneau
  translucide teinté, pas par un flou du jeu derrière.
- `TextInput` (champ "Replay Name") : l'API exacte (`TextInput::create`,
  `setString`, `getString`) peut différer légèrement selon la version du
  SDK Geode — vérifie contre `Geode/ui/TextInput.hpp` de ta version.
- Les sprites `GJ_checkOn_001.png` / `GJ_checkOff_001.png` (radios
  Record/Play) sont des icônes carrées de coche, pas des cercles comme
  dans Silicate — un vrai rendu circulaire demanderait de fournir tes
  propres textures.
- TPS / Speed / Frame Advance / Intentional Death ne sont pas dans cette
  version : ce sont des fonctionnalités bien plus lourdes (contrôle de la
  vitesse de simulation, pas-à-pas, détection de mort) que je n'ai pas
  implémentées. Dis-moi si tu veux qu'on s'y attaque ensuite.

## ⚠️ À vérifier avant de compiler

Je n'ai pas pu compiler ni vérifier ce code contre les vrais headers Geode
(pas d'accès réseau/SDK ici), donc quelques points sont à valider toi-même
selon ta version exacte de Geode/GD :

- La signature exacte de `PlayLayer::pushButton` / `releaseButton`
  (`PlayerButton` vs `int`) peut varier selon la version des bindings.
- `mod.json` : j'ai mis `"gd": "2.2074"` par défaut — remplace par la
  version exacte que Geode attend chez toi (« 2.208 » n'est pas un
  identifiant de version GD standard, donc vérifie dans ta CLI Geode /
  la doc quelle chaîne utiliser).
- Le format binaire `.macro` n'est pas portable entre versions de la
  struct `MacroAction` — si tu modifies la struct, les anciens fichiers
  ne se rechargeront plus.
- Il n'y a pas de bouton dans le menu pause : tout passe par les touches
  F2-F5. Dis-moi si tu veux plutôt un bouton dans le menu pause ou une
  fenêtre Geode (`FLAlertLayer`) pour gérer plusieurs macros nommés.

## Idées d'amélioration

- Overlay UI pour voir l'état (enregistrement / lecture) au lieu des
  notifications.
- Plusieurs slots de macro nommés au lieu d'un seul fichier `last.macro`.
- Export en JSON lisible plutôt qu'en binaire.
