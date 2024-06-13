# 解析 toml
1. 編譯
```
make
```
2. 執行
```
./parse_toml
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
  
