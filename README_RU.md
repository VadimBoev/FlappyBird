# 🐦 Flappy Bird на Си: Вес APK < 100 Килобайт! 🚀

## 📜 История:

Всё началось в 2021 году, когда я наткнулся на [rawdrawandroid](https://github.com/cnlohr/rawdrawandroid). Моя цель была проста: создать игру с минимальным весом APK, но при этом, чтобы она была понятной и интересной. 🎯

Идея сделать клон Flappy Bird казалась логичной, учитывая, что эта игра уже была портирована на множество языков. 🐤

В 2021 году я также изучил [Raylib](https://github.com/raysan5/raylib), но первая попытка сделать игру на C++ с использованием [ImGui](https://github.com/ocornut/imgui/) потерпела неудачу. 💔

Проблемы были везде: вес APK был около 1 МБ, игра вылетала, и в APK была только библиотека для armeabi-v7a (требования Google от 2022 года это наличие arm64-v8a библиотеки!). 🤯

## 💡 Мотивация:

В сентябре 2024 года, увидев Flappy Bird на C# в дискорд-канале Raylib, я решил попробовать реализовать эту игру на Си для Android с весом APK менее 100 КБ. 🚀  

Идея казалась безумной, но спортивный интерес взял верх. 💪

## 🛠️ Реализация:

Начал с компиляции "Hello World" на Си и упаковки библиотеки в APK. 📦

Звуки были сжаты до формата MP3, а для их воспроизведения использовался OpenSLES. 🎵

Для декодирования PNG файлов я выбрал [upng](https://github.com/elanthis/upng). 🖼️

Всё это было объединено с помощью OpenGL ES 2, шейдеров и Android Native Activity. 🎮

## 🔧 Сборка:

### Visual Studio Code (Windows)

1. Создать .env из .env.example
2. Запустить build.bat

### Linux и macOS (Makefile)

Для пользователей Linux и macOS можно собрать проект с помощью предоставленного Makefile:

1. Установите инструменты командной строки Android.
2. Настройте переменные окружения в файле `.env` в корневом каталоге проекта (смотрите `.env.example` для примера).
3. Соберите проект:
   ```bash
   cd FlappyBird
   make
   ```
   
Для более подробных инструкций, пожалуйста, обратитесь к [BUILDING.md](BUILDING.md).

Подписанный APK будет сгенерирован в:
`FlappyBird/app/build/outputs/apk/FlappyBird-signed.apk`

## 📄 Авторское право:

Я не претендую на авторские права. Право на игру и ресурсы принадлежит **DotGEARS**. 📜

## 🌟 Вдохновение:

- [rawdrawandroid](https://github.com/cnlohr/rawdrawandroid)
- [Flapper](https://github.com/its-Lyn/Flapper)
- [Raylib](https://github.com/raysan5/raylib)
- [ImGui](https://github.com/ocornut/imgui/)

## 🌠 Star History

[![Star History Chart](https://star-history.dera.page/svg?repos=VadimBoev/FlappyBird&type=Timeline)](https://star-history.dera.page/#VadimBoev/FlappyBird&Timeline)

---

🎉 Наслаждайтесь игрой и не забудьте поставить звезду! 🌟
