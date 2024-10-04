<img src="flappy.gif" alt="game" width="250px">

**[Readme на русском языке](README_RU.md)**  
  
[Dev blog in Telegram (ENG/RU)](https://t.me/boevdev)

# 🐦 Flappy Bird in C: APK Size < 100 Kilobytes! 🚀

## 📜 History:

It all started in 2021 when I stumbled upon [rawdrawandroid](https://github.com/cnlohr/rawdrawandroid). My goal was simple: to create a game with the minimal APK size, yet still be understandable and interesting. 🎯

The idea of making a Flappy Bird clone seemed logical, given that this game had already been ported to many languages. 🐤

In 2021, I also studied [Raylib](https://github.com/raysan5/raylib), but my first attempt to make a game in C++ using [ImGui](https://github.com/ocornut/imgui/) failed. 💔

The problems were everywhere: the APK size was around 1 MB, the game crashed, and the APK only contained the armeabi-v7a library (Google's 2022 requirements state that the arm64-v8a library must be included!). 🤯

## 💡 Motivation:

In September 2024, seeing Flappy Bird in C# in the Raylib Discord channel, I decided to try implementing this game in C for Android with an APK size of less than 100 KB. 🚀  

The idea seemed crazy, but the competitive interest took over. 💪

## 🛠️ Implementation:

I started by compiling a "Hello World" in C and packaging the library into an APK. 📦

Sounds were compressed to MP3 format, and OpenSLES was used for playback. 🎵

For PNG file decoding, I chose [upng](https://github.com/elanthis/upng). 🖼️

All of this was combined using OpenGL ES 2, shaders, and Android Native Activity. 🎮

## 🔧 Build:

1. Download [Visual Studio 2022](https://visualstudio.microsoft.com/).
2. Install components: C++ Desktop Development and C++ Mobile Development.
3. Download Android Studio (for apktool, sdk, ndk).
4. Install NDK 27.1.12297006.
5. Configure the project for "Debug ARM" and make changes to build.bat.
6. Compile via CTRL + B. 🛠️

## 📄 Copyright:

I do not claim any copyright. The rights to the game and resources belong to **DotGEARS**. 📜

## 🌟 Inspiration:

- [rawdrawandroid](https://github.com/cnlohr/rawdrawandroid)
- [Flapper](https://github.com/its-Lyn/Flapper)
- [Raylib](https://github.com/raysan5/raylib)
- [ImGui](https://github.com/ocornut/imgui/)

## 🌠 Star History

[![Star History Chart](https://api.star-history.com/svg?repos=VadimBoev/FlappyBird&type=Timeline)](https://star-history.com/#VadimBoev/FlappyBird&Timeline)

---

🎉 Enjoy the game and don't forget to star it! 🌟
