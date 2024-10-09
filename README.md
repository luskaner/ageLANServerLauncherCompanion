# Age of Empires 2 Lan Server - Launcher Companion

AoE2:DE LAN Server - Launcher Companion aims to provide DLL files to enhance or fix certain behaviours of the game to better be used with [AOE2:DE Lan Server](https://github.com/luskaner/aoe2DELanServer) launcher when using a custom game launcher.

## Libraries

* `FakeOnline.dll`: Fakes an internet always online behaviour. It is essential so that [AOE2:DE Lan Server](https://github.com/luskaner/aoe2DELanServer) can be used 100% offline.
* *more to come...*

## Minimum system requirements

* [Microsoft Visual C++ Redistributable](https://aka.ms/vs/17/release/vc_redist.x64.exe) (*it is very likely you have it installed already*).

*Note: It is assumed the requirements for the [game itself](https://support.ageofempires.com/hc/en-us/articles/360054372531-What-are-the-minimum-requirements-for-Age-of-Empires-II-Definitive-Edition) are met too*.

## Binaries

See the [releases page](https://github.com/luskaner/aoe2DELanServerLauncherCompanion/releases)

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

## Configuratiom

**⚠️ ONLY FOLLOW THIS IF YOU HAVE A LEGAL LICENSE IN THE RESPECTIVE PLATFORM FOR THE GAME AND THE CONFIGURED DLCS ⚠️**

You will need to use a custom launcher that either emulates Steam or Xbox Live (depending on the version you are using of the game) and configure it to inject the DLLs you want.

<details>
    <summary>Steam</summary>

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
5. Modify `ColdClientLoader.ini` and set the following values:
   * `[SteamClient]`:
     * `Exe`:  `Drive:\Path\To\SteamLibrary\steamapps\common\AoE2DE\AoE2DE_s.exe`.
     * `AppId`: 813780.
   * `[Injection]`
     * `DllsToInjectFolder`: `dlls`.
6. Create `steam_settings\supported_languages.txt` with notepad and copy the following text as-is:
```
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
7. Create `steam_settings\configs.app.ini` with notepad and copy the following text:
```
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
8. Remove the starting `#` for the DLCs **you legally own** and are installed.
9. Create `steam_settings\configs.user.ini` with notepad and copy the following text:
```
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
10. Modify the fields according to the comments.
11. Copy the DLLs you have download from here to `dlls` folder (*uncompressed*).
12. Edit [AoE2:DE Lan Server - Launcher](https://github.com/luskaner/aoe2DELanServer/tree/main/launcher) launcher `resources\config.ini` and set:
   * `[Client]`:
      * `Executable`:  `Drive:\Path\To\steamclient_loader_x64.exe`.

*Note: Up-to-date as of 09/10/2024 and using release `Release 2024 10 06`*.
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
* Release workflow: See [Github Workflow](https://github.com/luskaner/aoe2DELanServerLauncherCompanion/blob/main/.github/workflows/release.yml)
  
</details>

## Terms of Use

You may only use the provided binaries if you use it in conjuction with [AoE2:DE Lan Server](https://github.com/luskaner/aoe2DELanServer?) and comply with its [ToU](https://github.com/luskaner/aoe2DELanServer?tab=readme-ov-file#terms-of-use).

Disclaimer: This software is not affiliated with Xbox Game Studios, Microsoft Corporation, Forgotten Empires LLC,
World's Edge LLC, or any other entity that is involved in the development of Age of Empires 2 Definitive Edition.
