# 測試「讀取與解析 toml」
1. 編譯
```
make
```
2. 執行
```
./demo
```
你會看到以下:
```
-------------------------------------------------------  
number of item: 12
number of scene: 15
number of character: 9
number of event: 25
number of dialogue: 44
-------------------------------------------------------
Found the item: 'envelope'
name: Envelope
description: Red
icon: 
-------------------------------------------------------
Found the scene: 'street'
name: The street of Victoria
background: 
-------------------------------------------------------
Found the character: 'frienddad'
name: Dad of Billy (Dad of your friend)
avatar: 
tachie: 
-------------------------------------------------------
Found the event: 'findtreasure'
scene: findtreasure
dialog: scene32
-------------------------------------------------------
Found the dialogue: 'scene23'
text: （寶藏打開了！裡面是滿滿的金條！）
option (1):
    text: （馬上按下回去按鈕！）
    event: victoria_old
option (2):
    text: 既然有了這筆錢，我想幹嘛就幹嘛！（那個手錶的設定應該是唬我的吧？）
    event: final3
```

# toml 的合法格式 (重要)
每個區塊必須以 [] 表示，然後 [] 內和 key 只能是英文，但是 value 的話則是中英都可，然後 key value 之間要用 = 進行區隔，請見以下範例
- 錯誤範例 1 ([] 和 key 有中文)
```
[城市]
    圖片 = "modern_city.bmp"
    描述 = "一個繁華的現代都市，充滿了高樓大廈和繁忙的街道。"
    事件 = "玩家的基地和主要活動地點。"
```
  
- 錯誤範例 2 (key value 間的分隔符不是等號)
```
[city]
    圖片 : "modern_city.bmp"
    描述 : "一個繁華的現代都市，充滿了高樓大廈和繁忙的街道。"
    事件 : "玩家的基地和主要活動地點。"
```
- 正確範例
```
[city]
    img = "modern_city.bmp"
    desc = "一個繁華的現代都市，充滿了高樓大廈和繁忙的街道。"
    event = "玩家的基地和主要活動地點。"
```
  
