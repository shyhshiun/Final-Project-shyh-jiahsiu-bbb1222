# 使用方式
1. 編譯
```
make
```
2. 執行
```
./demo
```
執行後就可以開始遊玩囉 !!!  

# 遊玩方式
非常簡單，連妳阿罵都會
基本上就享受劇情，然後做出不同的選擇會觸發不同的結局唷 !!!

# 劇本
1. 檔名

```c
title = "Final Project"
author = "GROUP33"

[player]
starter = "room"
inventory = []
```

```c
# 主角家客廳
[scene.livingroomplayer]
name = "The livingroom of player"
background = "assets/scene/livingroomplayer.bmp"
```

```c
# 書店老闆
[character.bookstoreowner]
name = "Owner of bookstore"
avatar = "assets/character/bookstoreowner.bmp"
```

```c
# 啤酒
[item.beer]
name = "Beer"
description = "player's favorite"
icon = "assets/item/beer.bmp"
```

```c
[music.getupmusic]
name = "getup_music"
addr = "assets/music/getupmusic.wav"
```

```c
[music.getupmusic]
name = "getup_music"
addr = "assets/music/getupmusic.wav"
```

```c
[event.room]
scene = "livingroomplayer"
dialogue = "scene1"

[dialogue.scene1]
wav = "getupmusic"
text = """
又是被惡夢喚醒的一天 ⋯
夢裡，我回到了前年聖誕夜
親眼目睹爸爸媽媽被一個失業男子持槍殺死的那天
儘管這件事已經過了兩年
但卻沒有緩和一切
抱著這個沈重的心情
我打算再去冰箱拿一罐酒
"""
[[dialogue.scene1.options]]
text = "再去冰箱拿一罐酒"
next = "scene41"
```

```c
[dialogue.scene2]
text = """
這圖片上⋯⋯好像是寫著HELP⋯？？？
"""
[[dialogue.scene2.options]]
text = "太怪了吧？這信件誰放的？"
next = "scene4"
[[dialogue.scene2.options]]
text = "有人遇到危險了？（再喝一口酒壓壓驚）"
event = "final1"
```

```c
[event.final1]
scene = "lovebeer"
dialogue = "ending1"

[dialogue.ending1]
text = """
你的MBTI⋯是BEER吧？
"""
```

