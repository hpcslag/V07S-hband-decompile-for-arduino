# V07S-hband-decompile-for-arduino
(中文) - 反編譯並解析 V07S 運動手環通訊，令手環直接透過 Arduino 連接。

# Require (需求)

### 中文
我使用 `Arduino 101` 當作 Central 設備，對手還進行連接，這裡面的程式碼是 `Intel Curie` 的函式庫，目前只支援 `Arduino 101` 這個產品，我找尋過 `nRF8001, nRF51x` 之類的設備，很不幸沒有好的結果，所以要輕鬆做 Arduino 和 手環的連接，需要 `Arduino 101` 。

 - Hband V07S 產品
 - Arduino 101

### English
I use `Arduino 101` to be a `Bluetooth 4.0 Low Engry Central Device`,this program is based on `Intel Curie` library, and this library only support `Arduino 101` Product, I tried to use `nRF8001,nRF51x` to did that, but it was unsuccessful.

 - Hband V07S Product
 - Arduino 101 Product

# Description (說明)

### 中文
請將程式跑起來，打開 Terminal 介面，待程式啟動，連接到手環之後，輸入指令就可以獲得結果:

| Command | Descritpion |
| ------- | ----------- |
|    H    | Read Current Hard Rate |


# 參考
