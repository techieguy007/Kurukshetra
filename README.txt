KURUKSHETRA - THREE PERSPECTIVES, ONE STORY
Windows PC source prototype / version 0.1.0 / 2 October 2026

STATUS
Source project created. NOT compiled, opened, rendered, packaged, or playtested:
Unreal Engine is not installed on the development computer. This is not yet
a Windows executable. Android work is deferred until the PC prototype is working.
Source/configuration checks are separate from Unreal compilation and gameplay
validation. See VALIDATION.txt for the checks performed so far.

THE FIRST CHAPTER
The Banner Road is an ORIGINAL PRACTICE SCENARIO, not a claim about an episode
in the Mahabharata. It establishes a shared location with three perspectives.
All characters and scenery use engine primitive placeholders. There are no
finished character models, cinematics, music, voice performances, historical
costumes, or a complete epic campaign in this build.

Exactly three character choices:
  ARJUNA  - Faster movement and arrows; Focus increases fire rate for 5 seconds.
  KRISHNA - Chariot platform and a four-marker route; Swift Passage increases
            speed for 5 seconds. No attacking or offensive ability.
  BHISHMA - More health, slower powerful arrows; Steadfast reduces incoming
            damage by 75% for 5 seconds.
All abilities have a 12-second cooldown. These mechanics are design inventions.
The combat perspectives reach a rally marker and face four soldiers and a
captain. Krishna completes four ordered checkpoints within 150 seconds.
Completion is saved locally per character. Mid-mission progress is not saved.

FIRST RUN ON WINDOWS
1. Install Unreal Engine 5.8 through Epic Games Launcher for Windows development.
   Android platform support is NOT needed now. Other engine versions are untested.
   Installation: https://www.unrealengine.com/download
2. Ensure Visual Studio has Game development with C++ and a Windows SDK.
   Use the compiler/toolchain supported by your installed Unreal version.
   To build and launch the game directly after installing the engine, run:

   .\Scripts\Launch-Prototype.ps1

   It detects a standard/registered UE 5.8 installation. For a custom location,
   supply -EngineRoot 'D:\YourEngineFolder'. The first build may take time.
   The following alternative opens the editor for development instead:
3. Open PowerShell in this project folder and run:

   .\Scripts\Prepare-Project.ps1 -EngineRoot 'C:\Program Files\Epic Games\UE_5.8' -OpenEditor

   Substitute the real engine folder. This compiles the editor module, then
   runs Unreal's Python commandlet to create the map and seven materials.
   It preserves existing generated assets on subsequent runs.
4. In Unreal Editor, open /Game/Maps/Chapter01 and press Play.
   Most scenery is generated at runtime, so the editor map initially looks empty.
   Choose a character. The default pawn and UI are native C++ classes.
5. If the script cannot run under your PowerShell execution policy, use the
   normal approved script-running method for your computer. No persistent
   execution-policy or security settings are changed by these scripts.

WINDOWS CONTROLS
W/A/S/D: move. Mouse: aim/look. Hold left click: fire (Arjuna or Bhishma).
Q: ability. Escape: pause/resume. Menus use the mouse.
Controller axes and basic gameplay buttons are mapped, but menu controller
navigation has not been implemented. Steam controller support is not claimed.

WINDOWS BUILD
The initial window is 1280 x 720. The frame limit is 60 FPS, a target that has
not been benchmarked. Lumen, virtual shadow maps, and motion blur are disabled.
There are five active combat opponents; distant figures are static silhouettes.
After the editor playtest passes, package a Windows DEVELOPMENT build:

   .\Scripts\Package-Game.ps1 -EngineRoot 'C:\Program Files\Epic Games\UE_5.8'

Build artifacts go in Builds/Win64 inside this project. Test the packaged game
separately; an editor playtest does not confirm packaged-build readiness.

ANDROID - DEFERRED
The project targets Windows first, with an Android port later. The packaging
script currently accepts Win64 only.
Future reference phone: realme GT7, medium graphics. There is no Android build.
Dormant Android input/HUD branches remain in the source as a starting point,
but SDK setup, touch refinement, device profiles, packaging, and testing are
future work. Do not assume the PC game will port without additional work.

WHAT MUST BE TESTED AFTER INSTALLING UNREAL
See ACCEPTANCE_CHECKLIST.txt. Compilation, asset generation, UMG layout,
collision, mouse/keyboard input, saves, cooking, and PC performance
are all unverified until they run in Unreal/on hardware. Fix build errors before
claiming the project is playable. The source validator is not a game test.

Run the engine-independent source checks with:
   python .\Scripts\validate_source.py

NEXT DEVELOPMENT MILESTONES
1. Compile and playtest this prototype on Windows, including a packaged build.
2. Refine archery feel, enemy feedback, camera, and mouse/keyboard controls.
3. Select source episodes and map the three connected story perspectives.
4. Commission/create original character models, rigs, animation, and chariot art.
5. Add dialogue presentation, cinematics, audio, checkpoint saves, and settings.
6. Optimize and expand one complete narrative chapter before a full campaign.
7. Integrate Steam features. Port the working PC game to Android in a later phase.

SOURCE STRUCTURE
Source/Kurukshetra/KuruGameMode.*   chapter, arena, objectives, completion saves
Source/Kurukshetra/KuruHero.*       movement, camera, archery, role abilities
Source/Kurukshetra/KuruEnemy.*      simple pursuit and close-range damage
Source/Kurukshetra/KuruArrow.*      projectile collision and damage
Source/Kurukshetra/KuruController.* input modes, pause, mobile quality
Source/Kurukshetra/KuruHUD.*        selection, HUD, touch actions, results
Source/Kurukshetra/KuruTypes.h      roles, chapter phases, local save format
Config/                           platform, rendering, input, packaging defaults
Scripts/                          build, asset bootstrap, packaging, validation

No third-party plugins or paid marketplace assets are required by this source.
Generated materials are project-owned placeholders; engine primitives remain
subject to Unreal Engine's license. Unreal licensing and distribution terms:
https://www.unrealengine.com/license

The working title is provisional. The existing repository LICENSE applies to
the project's original source; Unreal Engine and its assets are separately
licensed by Epic. No Unreal Engine source, binaries, or marketplace content
is included in this repository.
