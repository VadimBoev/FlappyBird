**ENG:**

# Flappy Bird, only C, without Java/Kotlin, weight APK (armeabi-v7a + arm64-v8a) < 100 kilobytes

## History:  
  
It all started in 2021. Then I came across the [rawdrawandroid](https://github.com/cnlohr/rawdrawandroid) repository. 
There was a motivation to make some kind of game with the lowest possible APK weight, but at the same time, so that the game would be simple and understandable.  
At that moment, the idea came up to make a clone of the long-forgotten Flappy Bird game. Which has already been ported to many programming languages.  
Then, later in 2021, I found another interesting repository [Raylib](https://github.com/raysan5/raylib).  
But, the first attempt to make this game was in C++, using [ImGui](https://github.com/ocornut/imgui/), because I already knew him.  
And so, all the difficulties were presented in Android Native Activity and building a clean APK from apktool without Android Studio.  
The first attempt failed.  
Firstly, the weight of the APK was about 1 Megabyte.  
Secondly, there could be crashes of the game.  
Thirdly, there was only a library for armeabi-v7a inside the APK, and since 2022 Google's rules require the presence of arm64-v8a libraries.  
Fourthly, the structure of the project and its organization were terrible, it created a mess in the eyes and made it difficult to navigate the project normally.  
In general, I tried something, it didn't work out, the thought was stored in my head throughout this time, but no more attempts were made.  

## Motivation:  
  
Around September 14, 2024, in the Raylib discord channel, I saw a guy make a Flappy Bird in C#.  
Then it became very interesting to me to try a crazy idea, to make this game in C, for Android, with an APK weighing less than 100 Kilobytes.
The idea seemed crazy, as well as unsuccessful.  
Just imagine, today, when the weight of the APK reaches 500 Megabytes, you only need to keep less than 100 Kilobytes.
What are these frames for? It's a sporting interest, will it work out? It worked! But it wasn't easy at all.
  
## Implementation:  
  
At first, I put together a solution that compiled Hello World in C, packaged the library into an APK, everything was signed and sent to my device via USB.  
As soon as everything was ready, I went on to explore the resources of the game. The sounds were in ogg format at first, I compressed them, but there were some problems, I don't remember this moment anymore.  
Then the sounds still became mp3 format, compressed at 16 (kilobytes per second) each, thereby reducing the weight as much as possible, and the sound quality remained tolerable.  
The first difficulty arose if I had previously used [BASS](https://www.un4seen.com/) to play the sound, and it's heavy for my purpose, I had to study OpenSLES, which reads MP3 format without problems.  
Further, png images remain from the resources. There is no other way to use the format. Then it was necessary to find something easier than [stb_image](https://github.com/nothings/stb ).  
So I came across [upng](https://github.com/elanthis/upng), which completely solved the issue of decoding png files for their further rendering.

In general, everything is simpler than it seems.  
OpenGL ES 2 + shaders for rendering, OpenSLES for sounds, upng for decoding png format and of course Android Native Activity.

## Copyright: 
I do not claim copyright. The right to this game and resources belongs to **DotGEARS**.

## Inspiration:
- [rawdrawandroid](https://github.com/cnlohr/rawdrawandroid)
- [Flapper](https://github.com/its-Lyn/Flapper)
- [Raylib](https://github.com/raysan5/raylib)
- [ImGui](https://github.com/ocornut/imgui/)
  
  

**RU:**  

# Flappy Bird, только Си, без Java/Kotlin, вес APK (armeabi-v7a + arm64-v8a) < 100 kilobytes

## История:  
  
Всё началось в 2021 году. Тогда я наткнулся на репозиторий [rawdrawandroid](https://github.com/cnlohr/rawdrawandroid). 
Появилась мотивация сделать какую-нибудь игру с максимально меньшим весом APK, но при этом, что бы игра была простой и понятной.  
В моменте появилась идея сделать клон давно забытой игры Flappy Bird. Которую уже портировали на многие языки программирования.  
Тогда, позднее в 2021 году, я нашел ещё один интересный репозиторий [Raylib](https://github.com/raysan5/raylib).  
Но, первая попытка сделать эту игру была на C++, при использовании [ImGui](https://github.com/ocornut/imgui/), потому что я уже был с ним знаком.  
А так, все трудности были представлены в Android Native Activity и сборке чистого APK из apktool без Android Studio.  
Первая попытка потерпела крах.  
Во-первых, вес APK был примерно 1 Мегабайт.  
Во-вторых, могли случаться вылеты игры.  
В-третьих, внутри APK была только библиотека для armeabi-v7a, а с 2022 года правила Google требуют наличие arm64-v8a библиотек.  
В-четвертых, структура проекта и его организация были ужасными, это создавало кашу в глазах и мешало нормально ориентироваться в проекте.  
В целом, что-то попробовал, не получилось, мысль в голове хранилась на протяжении всего этого времени, но попыток больше не предпринималось.  

## Мотивация:  
  
Примерно 14 сентября 2024 года, в дискорд-канале Raylib я увидел как один парень сделал Flappy Bird на языке C#.  
Тогда мне стало очень интересно, попробовать безумную идею, сделать эту игру на Си, для Android, весом APK меньше 100 Килобайт.
Идея казалось безумной, а также, безуспешной.  
Просто представьте, сегодня, когда вес APK достигает по 500 Мегабайт, нужно уложиться всего лишь меньше, чем 100 Килобайт.
Для чего такие рамки? Это спортивный интерес, получится ли такое? Получилось! Но было совсем не просто.
  
## Реализация:  
  
По началу я собрал себе решение которое компилировало Hello World на Си, упаковывало библиотеку в APK, всё подписывалось и отправлялось мне на устройство по USB.  
Как только всё было готово, дальше я пошел изучать ресурсы игры. Звуки сначала были в формате ogg, я их сжал, но были какие-то проблемы, я уже не помню этот момент.  
Дальше звуки всё же стали форматом mp3, сжатым по 16 (килобайт в секунду) каждый, тем самым максимально уменьшив вес, а также качество звука оставалось терпимым.  
Возникла первая трудность, если раньше для воспроизведения звука я использовал [BASS](https://www.un4seen.com/), а он тяжелый для моей цели, то пришлось изучить OpenSLES который без проблем читает формат MP3.  
Дальше из ресурсов остаются картинки, формата png. Иного вариант использования формата нет. Тогда надо было найти что-то легче, чем [stb_image](https://github.com/nothings/stb).  
Так я наткнулся на [upng](https://github.com/elanthis/upng), которая полностью решила вопрос с декодированием png файлов для дальнейшего их рендера.

В целом всё проще чем кажется.  
OpenGL ES 2 + шейдеры для отрисовки, OpenSLES для звуков, upng для декодирования png формата и конечно же Android Native Activity.

## Авторское право: 
Я не претендую на авторское право. Право на эту игру и ресурсы принадлежит **DotGEARS**.

## Вдохновление:
- [rawdrawandroid](https://github.com/cnlohr/rawdrawandroid)
- [Flapper](https://github.com/its-Lyn/Flapper)
- [Raylib](https://github.com/raysan5/raylib)
- [ImGui](https://github.com/ocornut/imgui/)
