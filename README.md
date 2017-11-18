# V07S-hband-decompile-for-arduino
(中文) - 反編譯並解析 V07S 運動手環通訊，令手環直接透過 Arduino 連接。

(English) - Decompile V07S Band product, use arduino to connect `V07S Band`, no need to use cellphone connect.

# Require (需求)

### 中文
我使用 `Arduino 101` 當作 Central 設備，對手還進行連接，這裡面的程式碼是 `Intel Curie` 的函式庫，目前只支援 `Arduino 101` 這個產品，我找尋過 `nRF8001, nRF51x` 之類的設備，很不幸沒有好的結果，所以要輕鬆做 Arduino 和 手環的連接，需要 `Arduino 101` 。

 - Hband V07S 產品
 - Arduino 101

### English
I use `Arduino 101` to be a `Bluetooth 4.0 Low Engry Central Device`,this program is based on `Intel Curie` library, and this library only support `Arduino 101` Product, I tried to use `nRF8001,nRF51x` to did that, but it was unsuccessful.

 - Hband V07S Product
 - Arduino 101 Product
 
# Setup & Usage (設定和使用方式)

### 中文
`Control` 這個專案是舊版的，只針對單一裝置命名為 `V07S` 進行連接，而且只提供心律功能。

`async_control` 這個專案是目前的版本，使用前要先到這個檔案的 [27 行](https://github.com/hpcslag/V07S-hband-decompile-for-arduino/blob/251357eefb5931e331094a69cbe4aa0e1a8a7a8f/async_control/Control/Control.ino#L27) 修改要連接的裝置數量，然後再把 Mac 地址加入到 [34行](https://github.com/hpcslag/V07S-hband-decompile-for-arduino/blob/251357eefb5931e331094a69cbe4aa0e1a8a7a8f/async_control/Control/Control.ino#L34) 的字串陣列中，記得要先被連接的裝置要放在第一個陣列中。

### English

`Control` folder is old project, it just can connect to device which name is `V07S`, and only provide Heart Rate Feature.

`async_control` is current version project, before using this project, you have to modify [line 27](https://github.com/hpcslag/V07S-hband-decompile-for-arduino/blob/251357eefb5931e331094a69cbe4aa0e1a8a7a8f/async_control/Control/Control.ino#L27) change to the number of current devices, and add Mac Address of each BT devices in string arrays to [line 34](https://github.com/hpcslag/V07S-hband-decompile-for-arduino/blob/251357eefb5931e331094a69cbe4aa0e1a8a7a8f/async_control/Control/Control.ino#L34). (Notice that the first device to be connected is placed on the first index of array)

# Description (說明)

### 中文
請將程式跑起來，打開 Terminal 介面，待程式啟動，連接到手環之後，輸入指令就可以獲得結果:


| Command | Descritpion |
| ------- | ----------- |
|    H    | 讀取目前心律  |
|    B    | 讀取目前血壓  |
|  SET[n] | 設定藍芽裝置， [n] 請換成佇列號(從 0 開始)，在 [27 行](https://github.com/hpcslag/V07S-hband-decompile-for-arduino/blob/251357eefb5931e331094a69cbe4aa0e1a8a7a8f/async_control/Control/Control.ino#L27) 先更改裝置數量，再把 MAC Address 加入到 [34行](https://github.com/hpcslag/V07S-hband-decompile-for-arduino/blob/251357eefb5931e331094a69cbe4aa0e1a8a7a8f/async_control/Control/Control.ino#L34) 的字串陣列中。 | 

### English
This command run in terminal, after `Arduino 101` connect to Band, enter command to get result:

| Command |       Descritpion      |
| ------- | ---------------------- |
|    H    | Read Current Hard Rate |
|    B    | Read Current Blood Pressure |
|  SET[n] | Set to connect to other Bluetooth Device, Please setup in [line 27](https://github.com/hpcslag/V07S-hband-decompile-for-arduino/blob/251357eefb5931e331094a69cbe4aa0e1a8a7a8f/async_control/Control/Control.ino#L27) change to the number of current devices, and add Mac Address of each BT devices in string arrays to [line 34](https://github.com/hpcslag/V07S-hband-decompile-for-arduino/blob/251357eefb5931e331094a69cbe4aa0e1a8a7a8f/async_control/Control/Control.ino#L34). |

# 參考
