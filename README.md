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
   story.toml
3. 格式  
   [TOML](https://toml.io/en/)
4. 名詞解釋  
player 是玩家，inventory 是玩家的背包
```c
title = "Final Project"
author = "GROUP33"

[player]
starter = "room"
inventory = []
```
- scene (場景)  
  name 是場景的名稱，background 是圖檔路徑
```c
# 主角家客廳
[scene.livingroomplayer]
name = "The livingroom of player"
background = "assets/scene/livingroomplayer.bmp"
```
- character  (角色)  
  name 是角色的名稱，avatar 是圖檔路徑
```c
# 書店老闆
[character.bookstoreowner]
name = "Owner of bookstore"
avatar = "assets/character/bookstoreowner.bmp"
```
- item  (物品)  
  name 是物品的名稱，description 是物品的描述，icon 是圖檔路徑
```c
# 啤酒
[item.beer]
name = "Beer"
description = "player's favorite"
icon = "assets/item/beer.bmp"
```
- music  (音樂)
  name 是音樂的名稱，addr 是 wav 檔路徑
```c
[music.getupmusic]
name = "getup_music"
addr = "assets/music/getupmusic.wav"
```
- event  (事件)  
  scene 是事件場景的名稱，dialogue 是對話的名稱
```c
[event.room]
scene = "livingroomplayer"
dialogue = "scene1"
```
- dialogue  (對話)  
  wav 是 wav 檔的名稱，text 是對話的文字內容，雙引號是選項
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
- final  (結局)
```c
[event.final1]
scene = "lovebeer"
dialogue = "ending1"

[dialogue.ending1]
text = """
你的MBTI⋯是BEER吧？
"""
```
# 音、圖檔、ttf 的位置
都在 assets 裡面

# 實作功能
全部都以 C 語言開發
1. GUI 使用 [SDL](https://wiki.libsdl.org/SDL2/FrontPage)
2. 角色、物品疊在場景圖上
3. 音樂播放
4. 場景切換
5. 多選項、多結局
6. 背包
7. 顯示字幕，不超出 GUI 視窗邊界
8. 讀取並解析 TOML
