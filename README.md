Certainly! Here’s a refined version of your README that improves clarity and readability while maintaining the essential details.

---

<img src="flappy.gif" alt="game" width="250px">

**[Readme на русском языке](README_RU.md)**  
  
[Dev blog in Telegram (ENG/RU)](https://t.me/boevdev)

# Flappy Bird in C (No Java/Kotlin) - APK Size < 100 KB

## History

The journey began in 2021 when I discovered the [rawdrawandroid](https://github.com/cnlohr/rawdrawandroid) repository. My goal was to create a simple game with the smallest possible APK size. This led me to the idea of cloning the iconic Flappy Bird game, which has been ported to various programming languages over the years.

Later that year, I found the [Raylib](https://github.com/raysan5/raylib) library, but my initial attempts were in C++ using [ImGui](https://github.com/ocornut/imgui/). I faced several challenges, particularly with Android Native Activity and building a clean APK without Android Studio. My first attempt resulted in a 1 MB APK with potential crashes, lacking support for arm64-v8a, and a messy project structure.

Though the idea lingered in my mind, I did not revisit it until September 2024, when I saw a C# version of Flappy Bird in the Raylib Discord channel. This sparked my interest to create a C version for Android, aiming for an APK size under 100 KB—a daunting yet exciting challenge in an era of 500 MB APKs.

## Implementation

Initially, I created a basic solution that compiled a Hello World program in C and packaged it into an APK. From there, I explored game resources. 

- **Audio**: Sounds were first in OGG format, but I converted them to MP3 (16 kbps) to minimize size while retaining acceptable quality. I switched from using [BASS](https://www.un4seen.com/) to OpenSLES for audio playback, as it better suited my needs.
- **Images**: For image resources, I sought a simpler alternative to [stb_image](https://github.com/nothings/stb). I discovered [upng](https://github.com/elanthis/upng), which efficiently handles PNG decoding.

The technical stack includes OpenGL ES 2 for rendering, OpenSLES for audio, and Android Native Activity for the core functionality.

## Build Instructions

1. Download [Visual Studio 2022](https://visualstudio.microsoft.com/).
   - Open the Visual Studio Installer.
   - Click "Edit."
   - Check the following options: Development of classic applications in C++, Development of mobile applications in C++.
   
2. Download Android Studio (needed for apktool, SDK, and NDK).
   - Install NDK 25.2.9519653 (or a later version).

3. In the project, set the configuration to "Debug ARM," but update `build.bat` with the correct paths.

4. Compile using `CTRL + B`.

## Copyright

I do not claim copyright. The rights to this game and its resources belong to **DotGEARS**.

## Inspiration

- [rawdrawandroid](https://github.com/cnlohr/rawdrawandroid)
- [Flapper](https://github.com/its-Lyn/Flapper)
- [Raylib](https://github.com/raysan5/raylib)
- [ImGui](https://github.com/ocornut/imgui/)

## Star History
[![Star History Chart](https://api.star-history.com/svg?repos=VadimBoev/FlappyBird&type=Timeline)](https://star-history.com/#VadimBoev/FlappyBird&Timeline)

---

Feel free to adjust any sections further or add specific features you want to highlight!
