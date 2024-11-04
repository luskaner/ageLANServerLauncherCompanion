# Age LAN Server - Launcher Companion

Age LAN Server - Launcher Companion aims to provide DLL files to enhance or fix certain behaviours of the game to better be used with [Age LAN Server](https://github.com/luskaner/ageLANServer) launcher when using a custom game launcher.

## Libraries

* `FakeOnline.dll`: Fakes an internet always online behaviour. It is essential so that [Age LAN Server](https://github.com/luskaner/ageLANServer) can be used 100% offline. Currently, not working for AoE III: DE.
* *more to come...*

## Minimum system requirements

* [Microsoft Visual C++ Redistributable](https://aka.ms/vs/17/release/vc_redist.x64.exe) (*it is very likely you have it installed already*).

*Note: It is assumed the requirements for the [AoE II: DE](https://support.ageofempires.com/hc/en-us/articles/360054372531-What-are-the-minimum-requirements-for-Age-of-Empires-II-Definitive-Edition) or [AoE III: DE](https://support.ageofempires.com/hc/en-us/articles/360049678611-What-are-the-minimum-requirements-for-the-game) are met too*.

## Binaries

See the [releases page](https://github.com/luskaner/ageLANServerLauncherCompanion/releases)

*Note: If you are using Antivirus it may flag one or more binaries as virus, this is a **false positive***.

### Verification

The verification process ensures that the files you download are the same as the ones that were uploaded by the
maintainer.

<details>
    <summary>Verification steps</summary>

1. Check the release tag is verified with the committer's signature key (*as all commits must be*).
2. Download the ```..._checksums_vA.B.C.D.txt``` and ```..._checksums_vA.B.C.D.txt.sig``` files.
3. Import the [release public key](release_public.key) and import it to your keyring if you haven't already.
4. Verify the ```..._checksums_vA.B.C.D.txt``` file with the ```..._checksums_vA.B.C.D.txt.sig``` file.
5. Verify the SHA-256 checksum list inside ```..._checksums_vA.B.C.D.txt``` with the downloaded archives.

</details>

## Configuration

**⚠️ ONLY FOLLOW THIS IF YOU HAVE A LEGAL LICENSE IN THE RESPECTIVE PLATFORM FOR THE GAME AND THE CONFIGURED DLCS ⚠️**

You will need to use a custom launcher that either emulates Steam or Xbox Live (depending on the version you are using of the game) and configure it to inject the DLLs you want.

### Steam

There are multiple Steam emulators you can use, however, the only one tested is the continuation of the so-called [Goldberg Emulator](https://gitlab.com/Mr_Goldberg/goldberg_emulator) hosted [here](https://github.com/Detanup01/gbe_fork).

Here are the basic steps suposing you are using Windows:
1. Download the latest stable [emu-win-release.7z](https://github.com/Detanup01/gbe_fork/releases/latest/download/emu-win-release.7z).
2. Uncompress it in a temporary directory (*to be deleted later*).
3. Copy these files residing in `release\steamclient_experimental` to your preferred folder **outside the game's directory itself**:
   * `steamclient.dll`
   * `steamclient64.dll`
   * `ColdClientLoader.ini`
   * `steamclient_loader_x64.exe`
4. Create the subdirectory `dlls` and `steam_settings` where the other files reside.
5. Follow game-specific steps.

#### Age of Empires II : Definitive Edition

<details>
    <summary>Steps</summary>

1. Modify `ColdClientLoader.ini` and set the following values:
   * `[SteamClient]`:
     * `Exe`:  `Drive:\Path\To\SteamLibrary\steamapps\common\AoE2DE\AoE2DE_s.exe`.
     * `AppId`: 813780.
   * `[Injection]`
     * `DllsToInjectFolder`: `dlls`.
2. Create `steam_settings\supported_languages.txt` with notepad and copy the following text as-is:
```text
english
french
italian
german
japanese
koreana
portuguese
brazilian
russian
schinese
latam
tchinese
turkish
vietnamese
spanish
polish
hindi
malay
```
3. Create `steam_settings\achievements.json` with notepad and copy the following text as-is:
```json
[{"name":"ACHIEVEMENT_ETERNAL_GRATITUDE"},{"name":"AZTEC_VICTORY"},{"name":"BERBERS_VICTORY"},{"name":"BRITONS_VICTORY"},{"name":"BULGARIANS_VICTORY"},{"name":"BURMESE_VICTORY"},{"name":"BYZANTINES_VICTORY"},{"name":"CELTS_VICTORY"},{"name":"CHINESE_VICTORY"},{"name":"CUMANS_VICTORY"},{"name":"ETHIOPIANS_VICTORY"},{"name":"FRANKS_VICTORY"},{"name":"GOTHS_VICTORY"},{"name":"HUNS_VICTORY"},{"name":"INCAS_VICTORY"},{"name":"INDIANS_VICTORY"},{"name":"ITALIANS_VICTORY"},{"name":"JAPANESE_VICTORY"},{"name":"KHMER_VICTORY"},{"name":"KOREANS_VICTORY"},{"name":"LITHUANIANS_VICTORY"},{"name":"MAGYARS_VICTORY"},{"name":"MALAY_VICTORY"},{"name":"MALIANS_VICTORY"},{"name":"MAYANS_VICTORY"},{"name":"MONGOLS_VICTORY"},{"name":"PERSIANS_VICTORY"},{"name":"PORTUGUESE_VICTORY"},{"name":"SARACENS_VICTORY"},{"name":"SLAVS_VICTORY"},{"name":"SPANISH_VICTORY"},{"name":"TATARS_VICTORY"},{"name":"TEUTONS_VICTORY"},{"name":"TURKS_VICTORY"},{"name":"VIETNAMESE_VICTORY"},{"name":"VIKINGS_VICTORY"},{"name":"ACHIEVEMENT_CHAIN_REACTION"},{"name":"ACHIEVEMENT_SHEEP_HOARDER"},{"name":"ACHIEVEMENT_THE_WONDER"},{"name":"ACHIEVEMENT_LOSING_YOUR_RELIGION"},{"name":"ACHIEVEMENT_ANYONE_ORDER_PIZZA"},{"name":"ACHIEVEMENT_DARK_HUMOR"},{"name":"ACHIEVEMENT_HOWDY_NEIGHBOR"},{"name":"ACHIEVEMENT_NO_SUNTZU_LIGHT"},{"name":"ACHIEVEMENT_WOLOLO"},{"name":"ACHIEVEMENT_90_KG"},{"name":"ACHIEVEMENT_FIGHTING_FROM_AFAR"},{"name":"ACHIEVEMENT_CASTLE_CRUSHER"},{"name":"ACHIEVEMENT_CAMPAIGN_WALLACE_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_JOANARC_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_KHAN_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_SALADIN_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_BARBAROSSA_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_ATTILA_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_ELCID_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_MONTEZUMA_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_HISTORICAL_BATTLES_COMPLETE"},{"name":"ACHIEVEMENT_CAMPAIGN_ALARIC_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_BARI_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_DRACULA_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_ELDORADO_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_PRITHVIRAJ_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_SFORZA_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_PORTUGUESE_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_MALIAN_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_BERBER_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_ETHIOPIAN_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_BURMESE_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_MALAY_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_VIETNAMESE_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_KHMER_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_BULGARIAN_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_TATAR_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_CUMAN_COMPLETED"},{"name":"ACHIEVEMENT_NO_WONDER_LEFT_BEHIND"},{"name":"ACHIEVEMENT_CATHEDRAL_RUSH"},{"name":"ACHIEVEMENT_NO_WONDER_ON_MY_WATCH"},{"name":"ACHIEVEMENT_DIPLOMACY_IS_FOR_THE_MEEK"},{"name":"ACHIEVEMENT_KUSHLUK_ASSASSINATION"},{"name":"ACHIEVEMENT_I_WAS_IN_CHINA_BEFORE"},{"name":"ACHIEVEMENT_OFFENSE_IS_THE_BEST_DEFENSE"},{"name":"ACHIEVEMENT_OUT_WITH_A_BANG"},{"name":"ACHIEVEMENT_D_DAY"},{"name":"ACHIEVEMENT_THE_GO_GETTER"},{"name":"ACHIEVEMENT_FUROR_TEUTONICUS"},{"name":"ACHIEVEMENT_ATTILA_SPEED_RUN"},{"name":"ACHIEVEMENT_HOT_N_SPICY"},{"name":"ACHIEVEMENT_BATTLESHIP"},{"name":"ACHIEVEMENT_SURYAVARMAN_RELICS"},{"name":"ACHIEVEMENT_THE_MISSIONARY"},{"name":"ACHIEVEMENT_OTTOMAN_GLORY_SEEKER"},{"name":"ACHIEVEMENT_NO_HOJO"},{"name":"ACHIEVEMENT_FIRE_WITH_FIRE"},{"name":"ACHIEVEMENT_DESTROY_DELHI"},{"name":"ACHIEVEMENT_KILL_KING_TAKAYUTPI"},{"name":"ACHIEVEMENT_NO_KILLING_PORTUGUESE"},{"name":"ACHIEVEMENT_NUMBERS_BEAT_EVERYTHING"},{"name":"ACHIEVEMENT_ALARIC_SPEED_RUN"},{"name":"ACHIEVEMENT_DRAGON_SHIPS"},{"name":"ACHIEVEMENT_100_GBETO"},{"name":"ACHIEVEMENT_SATANS_MAP"},{"name":"ACHIEVEMENT_NO_SUPPORT"},{"name":"ACHIEVEMENT_RUSH_TO_THE_KING"},{"name":"ACHIEVEMENT_NOT_THE_VIPER"},{"name":"ACHIEVEMENT_NO_CASTLE_AGE"},{"name":"ACHIEVEMENT_DEFEAT_AUSTRIAN_DUKE"},{"name":"ACHIEVEMENT_NO_WALLS"},{"name":"ACHIEVEMENT_FRANKLY_MY_DEAR"},{"name":"ACHIEVEMENT_UNEXPECTED"},{"name":"ACHIEVEMENT_SUPREMELY_UNEXPECTED"},{"name":"ACHIEVEMENT_FINAL_COUNTDOWN"},{"name":"ACHIEVEMENT_KABOOM"},{"name":"ACHIEVEMENT_BULLS_EYE"},{"name":"ACHIEVEMENT_HOMELESS"},{"name":"ACHIEVEMENT_OUT_OF_THEIR_ELEMENT"},{"name":"ACHIEVEMENT_CASTLE_OF_DOUBT"},{"name":"ACHIEVEMENT_ITS_A_TREB"},{"name":"ACHIEVEMENT_MARCO_POLO"},{"name":"ACHIEVEMENT_KNOCKING_ON_YOUR_DOOR"},{"name":"ACHIEVEMENT_SUSHI_LOVER"},{"name":"ACHIEVEMENT_EASIEST_AI"},{"name":"ACHIEVEMENT_EASY_AI"},{"name":"ACHIEVEMENT_MODERATE_AI"},{"name":"ACHIEVEMENT_HARD_AI"},{"name":"ACHIEVEMENT_HARDEST_AI"},{"name":"ACHIEVEMENT_EXTREME_AI"},{"name":"ACHIEVEMENT_MANGO_SHOTS"},{"name":"ACHEVEMENT_HUN_WHAT_ARE_YOU_DOING"},{"name":"ACHIEVEMENT_VICTORY_WITH_EVERY_CIV"},{"name":"ACHIEVEMENT_LONG_LIVE_THE_KING"},{"name":"ACHIEVEMENT_RELIC_HUNTER"},{"name":"ACHIEVEMENT_DARK_DINNER"},{"name":"ACHIEVEMENT_PLEASANT_PEASANT"},{"name":"ACHIEVEMENT_BIRD_SHOOTING"},{"name":"ACHIEVEMENT_KARAMBOLAGE"},{"name":"ACHIEVEMENT_MASTERPIECE"},{"name":"ACHIEVEMENT_TOO_LAME_TO_TAME"},{"name":"ACHIEVEMENT_ELEPHANTASTIC"},{"name":"ACHIEVEMENT_CAMPAIGN_BRITONS_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_BURGUNDIANS_COMPLETED"},{"name":"ACHIEVEMENT_CAMPAIGN_SICILIANS_COMPLETED"},{"name":"ACHIEVEMENT_UNCHIVALROUS_PRAGMATIST"},{"name":"ACHIEVEMENT_THRONE_THIEF"},{"name":"ACHIEVEMENT_MALLEUS_SCOTORUM"},{"name":"ACHIEVEMENT_NO_WHEELS"},{"name":"ACHIEVEMENT_A_SECOND_HASTINGS"},{"name":"ACHIEVEMENT_SHUT_UP_LA_HIRE"},{"name":"ACHIEVEMENT_SELFMADE_MAN"},{"name":"ACHIEVEMENT_HAUTE_HAUTE_BRIEF_CANDLE"},{"name":"ACHIEVEMENT_ITALY_JONES_AND_THE_FIRST_CRUSADE"},{"name":"BURGUNDIANS_VICTORY"},{"name":"SICILIANS_VICTORY"},{"name":"ACHIEVEMENT_FLEMISH_REVOLUTION"},{"name":"ACHIEVEMENT_DONJON_SERJEANT"},{"name":"ACHIEVEMENT_CAMPAIGN_LITHUANIANS_COMPLETED"},{"name":"ACHIEVEMENT_TOLERANT_PIETY"},{"name":"ACHIEVEMENT_SCATTERED_HORDE"},{"name":"ACHIEVEMENT_MINT_CONDITION"},{"name":"ACHIEVEMENT_CAMPAIGN_POLES_COMPLETED"},{"name":"ACHIEVEMENT_PROTECTOR_REALM"},{"name":"ACHIEVEMENT_NO_BAGGAGE"},{"name":"ACHIEVEMENT_NO_QUARTER"},{"name":"ACHIEVEMENT_CAMPAIGN_BOHEMIANS_COMPLETED"},{"name":"ACHIEVEMENT_MALEVOLENT_MARAUDER"},{"name":"ACHIEVEMENT_ZIZKOV_HILL"},{"name":"ACHIEVEMENT_AGAINST_ALL"},{"name":"BOHEMIANS_VICTORY"},{"name":"POLES_VICTORY"},{"name":"ACHIEVEMENT_OOF_NICE"},{"name":"ACHIEVEMENT_FAST_FOOD"},{"name":"ACHIEVEMENT_CAMPAIGN_HINDUSTANIS_COMPLETED"},{"name":"ACHIEVEMENT_NEVER_TRUST"},{"name":"ACHIEVEMENT_AN_OFFER"},{"name":"ACHIEVEMENT_THE_RENOVATOR"},{"name":"ACHIEVEMENT_CAMPAIGN_DRAVIDIANS_COMPLETED"},{"name":"ACHIEVEMENT_NO_REST_FOR_THE_WICKED"},{"name":"ACHIEVEMENT_EYE_OF_THE_TIGER"},{"name":"ACHIEVEMENT_NO_WONDER_YOU_WON"},{"name":"ACHIEVEMENT_CAMPAIGN_BENGALIS_COMPLETED"},{"name":"ACHIEVEMENT_HUNA_JOIN_ME"},{"name":"ACHIEVEMENT_CONSTRUCTION_CANCELLATION"},{"name":"ACHIEVEMENT_PARINIRVANA"},{"name":"ACHIEVEMENT_CAMPAIGN_RAJA_OF_THE_PEOPLE"},{"name":"ACHIEVEMENT_CAMPAIGN_HOPELESS_ROMANTIC"},{"name":"ACHIEVEMENT_CAMPAIGN_INSPIRING_POET"},{"name":"BENGALIS_VICTORY"},{"name":"DRAVIDIANS_VICTORY"},{"name":"GURJARAS_VICTORY"},{"name":"ACHIEVEMENT_FACE_MY_RATHA"},{"name":"ACHIEVEMENT_KING_OF_THE_SEAS"},{"name":"ACHIEVEMENT_FORGED_IN_THE_HEAT_OF_BATTLE"},{"name":"ACHIEVEMENT_NOT_JUST_A_MILITIA"},{"name":"ACHIEVEMENT_FAMILY_FEUDAL"},{"name":"ACHIEVEMENT_CAN_I_BE_FRANK_WITH_YOU"},{"name":"ACHIEVEMENT_CHALLENGE_ACCEPTED"},{"name":"ACHIEVEMENT_GOING_FOR_THE_GOLD"},{"name":"ACHIEVEMENT_WINGMAN"},{"name":"ACHIEVEMENT_BABY_BOOMER"},{"name":"ACHIEVEMENT_BULL_MARKET"},{"name":"ACHIEVEMENT_BON_VOYAGE"},{"name":"ACHIEVEMENT_PREPARE_FOR_BOAR"},{"name":"ACHIEVEMENT_CAMPAIGN_SUMERIANS_COMPLETED"},{"name":"ACHIEVEMENT_NOT_CUTTING_CORNERS"},{"name":"ACHIEVEMENT_EXPEDITED_DELIVERY"},{"name":"ACHIEVEMENT_UR_OUT_OF_HERE"},{"name":"ACHIEVEMENT_CAMPAIGN_MACEDONIANS_COMPLETED"},{"name":"ACHIEVEMENT_WHAT_WOULD_ALEXANDER_HAVE_DONE"},{"name":"ACHIEVEMENT_GREECED_LIGHTNING"},{"name":"ACHIEVEMENT_THE_BEST_DEFENSE"},{"name":"ACHIEVEMENT_CAMPAIGN_ROMANS_COMPLETED"},{"name":"ACHIEVEMENT_ARE_YOU_NOT_ENTERTAINED"},{"name":"ACHIEVEMENT_I_AM_LEGION"},{"name":"ACHIEVEMENT_YOU_HAVE_NO_POWER_HERE"},{"name":"ROMAN_VICTORY"},{"name":"POMPEII_ASSYRIAN_VICTORY"},{"name":"POMPEII_BABYLONIAN_VICTORY"},{"name":"POMPEII_CARTHAGINIAN_VICTORY"},{"name":"POMPEII_CHOSON_VICTORY"},{"name":"POMPEII_EGYPTIAN_VICTORY"},{"name":"POMPEII_GREEK_VICTORY"},{"name":"POMPEII_HITTITE_VICTORY"},{"name":"POMPEII_LACVIET_VICTORY"},{"name":"POMPEII_MACEDONIAN_VICTORY"},{"name":"POMPEII_MINOAN_VICTORY"},{"name":"POMPEII_PALMYRAN_VICTORY"},{"name":"POMPEII_PERSIAN_VICTORY"},{"name":"POMPEII_PHOENICIAN_VICTORY"},{"name":"POMPEII_ROMAN_VICTORY"},{"name":"POMPEII_SHANG_VICTORY"},{"name":"POMPEII_SUMERIAN_VICTORY"},{"name":"POMPEII_YAMATO_VICTORY"},{"name":"ACHIEVEMENT_RIDE_FOR_RUIN"},{"name":"ACHIEVEMENT_CALM_COOL_AND_COLLECTED"},{"name":"ACHIEVEMENT_AGILE_AS_A_SPARROW"},{"name":"ACHIEVEMENT_I_WONDER_WHY_I_DID_THAT"},{"name":"ACHIEVEMENT_FORGED_IN_IRON"},{"name":"ACHIEVEMENT_CHARIOTABLE_NUMBERS"},{"name":"ACHIEVEMENT_BUILDER_OF_CIVILIZATIONS"},{"name":"ACHIEVEMENT_TACTICAL_RETREAT"},{"name":"ACHIEVEMENT_PHILHELLENE"},{"name":"ACHIEVEMENT_LAW_MAKER"},{"name":"ACHIEVEMENT_CAMPAIGN_PERSIANS_COMPLETED"},{"name":"ACHIEVEMENT_SNEAK_ATTACK"},{"name":"ACHIEVEMENT_LANDSLIDE"},{"name":"ACHIEVEMENT_NO_GUNS"},{"name":"ACHIEVEMENT_CAMPAIGN_ARMENIANS_COMPLETED"},{"name":"ACHIEVEMENT_BOUNTY_HUNTER"},{"name":"ACHIEVEMENT_RAIDING_PARTY"},{"name":"ACHIEVEMENT_CHURCH_SANCTUARY"},{"name":"ACHIEVEMENT_CAMPAIGN_GEORGIANS_COMPLETED"},{"name":"ACHIEVEMENT_GAME_OVER_YURY"},{"name":"ACHIEVEMENT_AUNTIE_DEAREST"},{"name":"ACHIEVEMENT_ALEXANDRA_THE_GREAT"},{"name":"ARMENIANS_VICTORY"},{"name":"GEORGIANS_VICTORY"},{"name":"ACHIEVEMENT_CAMPAIGN_HEROES_AND_VILLAINS_COMPLETED"},{"name":"ACHIEVEMENT_LOST_VIKINGS"},{"name":"ACHIEVEMENT_THE_OLD_FASHIONED_WAY"},{"name":"ACHIEVEMENT_DAIMYO_OF_THE_NINE_PROVINCES"},{"name":"ACHIEVEMENT_PLEBEIAN_PRESERVER"},{"name":"ACHIEVEMENT_VANDALIZED"},{"name":"ACHIEVEMENT_UNITED_WE_FALL"},{"name":"ACHIEVEMENT_AGE_OF_VIKINGS"},{"name":"ACHIEVEMENT_GENERATIONAL_AMBITIONS"},{"name":"ACHIEVEMENT_BREAD_AND_CIRCUSES"},{"name":"ACHIEVEMENT_GALLEON_BLING"},{"name":"ACHIEVEMENT_SO_EPIC"},{"name":"ACHIEVEMENT_CIVIS_ROMANUS"},{"name":"ACHIEVEMENT_SEEDS_OF_THEIR_OWN_DESTRUCTION"},{"name":"ACHIEVEMENT_A_MOST_CONVENIENT_DEATH"},{"name":"ACHIEVEMENT_FASTER_THAN_A_SPEEDING_CANNONBALL"},{"name":"ACHIEVEMENT_TRUST_NO_ONE"},{"name":"ACHIEVEMENT_COUP_DETAT"},{"name":"ACHIEVEMENT_SAXON_REVENGE"},{"name":"ACHIEVEMENT_SHOGUN"}]
```
4. Create `steam_settings\configs.app.ini` with notepad and copy the following text:
```ini
[app::dlcs]
unlock_all=0
# Expansions
#1389240=Age of Empires II: Definitive Edition - Lords of the West
#1557210=Age of Empires II: Definitive Edition - Dawn of the Dukes
#1869820=Age of Empires II: Definitive Edition - Dynasties of India
#2141580=Age of Empires II: Definitive Edition - Return of Rome
#2555420=Age of Empires II: Definitive Edition - The Mountain Royals
#2805510=Age of Empires II: Definitive Edition - Victors and Vanquished
# Animated Icons
#2672800=Age of Empires II: Definitive Edition – La Hire’s Dry Humor Animated Icons
#2672810=Age of Empires II: Definitive Edition – Joan’s Marvelous Moving Masterpieces Animated Icons
#2672820=Age of Empires II: Definitive Edition – Barbarossa’s Barrel of Pickled Goods Animated Icons
# Other
#1039811=Enhanced Graphics Pack
```
5. Remove the starting `#` for the DLCs **you legally own** and are installed.
6. Create `steam_settings\configs.user.ini` with notepad and copy the following text:
```ini
[user::general]
# Fill your account name as you would normally see.
account_name=
# Fill with your real steamid or leave empty for it to be auto-generated.
account_steamid=
# Choose one from steam_settings\supported_languages.txt (described in https://partner.steamgames.com/doc/store/localization/languages) or leave empty to be set as 'english'
language=
# Choose from 'Alpha-2' country code: https://www.iban.com/country-codes or leave empty to be set as 'US'
ip_country=
```
7. Modify the fields according to the comments.
8. Copy the DLLs you have download from here to `dlls` folder (*uncompressed*).
9. Edit [Age LAN Server - Launcher](https://github.com/luskaner/ageLANServer/tree/main/launcher) `resources\config.aoe2.toml` and set:
   * `[Client]`:
      * `Executable`:  `Drive:\Path\To\steamclient_loader_x64.exe`.

*Note: Up-to-date as of 03/11/2024 and using release `Release 2024 10 25`*.
</details>

### Age of Empires III : Definitive Edition

<details>
    <summary>Steps</summary>

1. Modify `ColdClientLoader.ini` and set the following values:
   * `[SteamClient]`:
     * `Exe`:  `Drive:\Path\To\SteamLibrary\steamapps\common\AoE3DE\AoE3DE_s.exe`.
     * `AppId`: 933110.
   * `[Injection]`
     * `DllsToInjectFolder`: `dlls`.
2. Create `steam_settings\supported_languages.txt` with notepad and copy the following text as-is:
```text
english
french
italian
german
japanese
koreana
brazilian
russian
schinese
tchinese
turkish
vietnamese
spanish
```
3. Create `steam_settings\achievements.json` with notepad and copy the following text as-is:
```json
[{"name":"WHAT_A_BEAUTIFUL_CITY"},{"name":"CONTENT_CREATOR"},{"name":"BLOOD_ICE_AND_STEEL"},{"name":"FIRE_AND_SHADOW"},{"name":"THE_ASIAN_DYNASTIES"},{"name":"THE_TOMMYNATOR"},{"name":"EVERY_MOMENT_I_LIVE_IS_AGONY"},{"name":"RAISING_CONQUERORS"},{"name":"WINNER_WINNER"},{"name":"CARTOGRAPHER"},{"name":"IMPERIAL_AGE"},{"name":"FULL_OF_HOT_AIR"},{"name":"DANCE"},{"name":"ALLIANCE"},{"name":"TRADES_MASTER"},{"name":"WE_NEED_MORE_WOOD"},{"name":"MY_HERO"},{"name":"TREASURE_HUNTER"},{"name":"A_PIRATES_LIFE_FOR_ME"},{"name":"TO_THE_SEAS"},{"name":"WAR_WAGONS"},{"name":"FIERCE_CAVALRYMEN_OF_TARTARS"},{"name":"CATHEDRAL_ASSIST"},{"name":"BIG_BROTHERS"},{"name":"PIKE_AND_SHOT"},{"name":"CONGREVE_ROCKETS"},{"name":"RIDERS"},{"name":"RIBAULDEQUIN"},{"name":"WONDERS"},{"name":"FAITH"},{"name":"ELEPHANT_IN_THE_ROOM"},{"name":"SOCIAL_DOMINANCE"},{"name":"NO_MAINS"},{"name":"BUDGET_CUT"},{"name":"HERE_COMES_THE_CAVALRY"},{"name":"WHATEVER_THE_PRICE_ILL_BEAT_IT"},{"name":"OLD_FASHIONED"},{"name":"ANIMAL_WHISPERER"},{"name":"ANIMAL_HUNTER"},{"name":"SEE_MY_VEST"},{"name":"COMFORTS_OF_HOME"},{"name":"STRIKE"},{"name":"WELL_STUDIED"},{"name":"HOLY_MOUNTAINS"},{"name":"RAIDING_PARTY"},{"name":"BIG_BOMBARDMENT"},{"name":"SAFARI"},{"name":"INFLUENCER"},{"name":"CASH_COW"},{"name":"HAUSA_VICTORY"},{"name":"ETHIOPIAN_VICTORY"},{"name":"RULER_OF_THE_SEVEN_KINGDOMS"},{"name":"KING_OF_KINGS"},{"name":"DEVOUT_LEARNER"},{"name":"TRULY_REVOLTING"},{"name":"COMPANIA_FUSILEROS"},{"name":"ESTADOS_UNIDOS_MEXICANOS"},{"name":"GUERRILLEROS_MEXICANOS"},{"name":"STRENGTH_IN_NUMBERS"},{"name":"DESPERADO"},{"name":"MEXICAN_VICTORY"},{"name":"STRATEGIST"},{"name":"MEXICAN_STATESMAN"},{"name":"TONGUES_OF_EUROPE"},{"name":"DEMOLITION_DERBY"},{"name":"THE_GREAT_SIEGE"},{"name":"HEAVY_METAL_RAIN"},{"name":"KNIGHTS_HOSPITAL"},{"name":"BUSINESS_TRIP"},{"name":"MALTESE_VICTORY"},{"name":"ALONE_AGAINST_THE_WORLD"},{"name":"COSSACK_CAPTAIN"},{"name":"DEFENSE_OF_THE_HEADQUARTERS"},{"name":"SCORCHED_EARTH"},{"name":"UPHILL_BATTLE"},{"name":"SHELL_WITH_YOUR_NAME_ON_IT"},{"name":"INTERNATIONAL_SUPPORTERS"},{"name":"TECHNOLOGICAL_BOOM"},{"name":"INVESTMANT_BANKER"},{"name":"FREE_REAL_ESTATE"},{"name":"BERSAGLIERI_BATTALION"},{"name":"ITALIAN_VICTORY"},{"name":"LA_SERENISSIMA"},{"name":"ECONOMIC_SUPER_POWER"},{"name":"OK_BOOMER"},{"name":"GRAND_EXCHANGE"},{"name":"IMPERIUM"},{"name":"WEALTH_OF_EMPIRES"}]
```
4. Create `steam_settings\configs.app.ini` with notepad and copy the following text:
```ini
[app::dlcs]
unlock_all=0
# Base Game
#2477660=Age of Empires III: Definitive Edition (Base Game)
# Civilizations
#1581450=Age of Empires III: Definitive Edition - United States Civilization
#1817370=Age of Empires III: Definitive Edition - Mexico Civilization
# Expansions
#1581451=Age of Empires III: DE The African Royals
#1817361=Age of Empires III: Definitive Edition - Knights of the Mediterranean
# Cosmetics
#2154360=Age of Empires III: Definitive Edition – Hero Cosmetic Pack – Lizzie
#2154361=Age of Empires III: Definitive Edition – Hero Cosmetic Pack – Kunoichi
#2154362=Age of Empires III: Definitive Edition – Hero Cosmetic Pack – Vol. 1
# Other
#1331250=Boston - 4K Cinematic Pack
```
5. Remove the starting `#` for the DLCs **you legally own** and are installed.
6. Create `steam_settings\configs.user.ini` with notepad and copy the following text:
```ini
[user::general]
# Fill your account name as you would normally see.
account_name=
# Fill with your real steamid or leave empty for it to be auto-generated.
account_steamid=
# Choose one from steam_settings\supported_languages.txt (described in https://partner.steamgames.com/doc/store/localization/languages) or leave empty to be set as 'english'
language=
# Choose from 'Alpha-2' country code: https://www.iban.com/country-codes or leave empty to be set as 'US'
ip_country=
```
7. Modify the fields according to the comments.
8. Copy the DLLs you have download from here to `dlls` folder (*uncompressed*).
9. Edit [Age LAN Server - Launcher](https://github.com/luskaner/ageLANServer/tree/main/launcher) `resources\config.aoe3.toml` and set:
   * `[Client]`:
      * `Executable`:  `Drive:\Path\To\steamclient_loader_x64.exe`.

*Note: Up-to-date as of 03/11/2024 and using release `Release 2024 10 25`*.
</details>

## Development

<details>
    <summary>Instructions</summary>
    
### Requirements
* OS: Windows 10.
* IDE: Visual Studio 2022 or Visual Studio Code.
* Tools: Windows 10 SDK.

### Tasks

Before staring make sure to clone the repo with submodules and install `vcpkg` submodule, you may need to configure VS Studio/Code to its path.

* Release Build: `msbuild /m /p:Configuration=Release`
* Debug Build:  `msbuild /m /p:Configuration=Debug`
* Release workflow: See [Github Workflow](https://github.com/luskaner/ageLANServerLauncherCompanion/blob/main/.github/workflows/release.yml)
  
</details>

## Terms of Use

You may only use the provided binaries if you use it in conjuction with [Age LAN Server](https://github.com/luskaner/ageLANServer) and comply with its [ToU](https://github.com/luskaner/ageLANServer?tab=readme-ov-file#terms-of-use).

Disclaimer: This software is not affiliated with Xbox Game Studios, Microsoft Corporation, Forgotten Empires LLC,
World's Edge LLC, or any other entity that is involved in the development of Age of Empires 2 Definitive Edition.
