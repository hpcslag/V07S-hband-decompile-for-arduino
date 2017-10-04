/*
    Author: 資工系 - Mac Taylor
    Date:   2017/10/4
    Reference:
     - Timer Library : https://github.com/JChristensen/Timer
*/
#include <CurieBLE.h>
#include "Timer.h"

String readString = "";
String deviceName = "V07S";
Timer tcb;

static unsigned int count = 0;
void addTime(){
  count++;
}

void setup() {
  Serial.begin(9600);

  //系統非同步計數器
  tcb.every(1000, addTime);

  // initialize the BLE hardware
  BLE.begin();

  Serial.println("BLE Central Read");

  //搜尋設備名稱
  BLE.scanForName(deviceName);
}

void loop() {
  //開啟設備連接
  Serial.println("Start peripheral...");
  
  //檢查設備是否被偵測
  BLEDevice peripheral = BLE.available();

  //如果連接上設備
  if (peripheral) {
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

    //停止搜尋
    BLE.stopScan();
    
    //進行連接操作，從 Serial 來進行指令
    Serial.println("Please Enter The Command:\n");
    //組合輸入指令
    while (Serial.available()) {
      delay(3);  //延遲來允許 buffer 可以釋放
      if (Serial.available() >0) {
        char c = Serial.read();
        readString += c;
      } 
    }
    
    //讀取輸入指令
    if (readString.length() >0) {
      Serial.println(readString);

      //回應測試指令
      if(readString == "test"){
        Serial.println("getting test..." + readString.substring(0,1));
      }

      //開啟讀取心律
      if(readString.substring(0,1) == "H"){
        Serial.println("Now Reading HartRate");
        int hartRate = readHartRate(peripheral,60); //30 seconds
        Serial.print("Final Hart Rate: ");
        Serial.print(hartRate);
        Serial.println();
      }
      
      readString="";
    }

    
    //繼續搜尋裝置
    BLE.scanForName(deviceName);
  }
  
  delay(2000);
}

int readHartRate(BLEDevice peripheral, int seconds) {
  //歸 0 系統計數
  count = 0;
    
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return -1;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributesByService("f0080001-0451-4000-b000-000000000000")) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return -1;
  }

  //Write Type 的 Characteristic 值
  BLECharacteristic WriteCharacteristic = peripheral.characteristic("f0080003-0451-4000-b000-000000000000");

  // Read Type 的 Characteristic 值
  BLECharacteristic ReadCharacteristic = peripheral.characteristic("f0080002-0451-4000-b000-000000000000");

  //寫入值，開始偵測心律
  Serial.println("Function: Start To Read Hart Rate...");
  const byte openHartRateByte[] = {0xD0, 0x01};
  WriteCharacteristic.writeValue(openHartRateByte,2);
  
  //開始進行遍歷值
  while (peripheral.connected()) {
    //繼續計時器的計數
    tcb.update();
    
    unsigned char len = ReadCharacteristic.valueLength();
    const unsigned char *val = ReadCharacteristic.value();
    unsigned char i = 0;

    String hartRate = "";
    while(i<len)
    {
      //Serial.print(static_cast<unsigned char>(val[i++]));
      hartRate += static_cast<unsigned char>(val[i++]);
      hartRate += ",";
    }

    char cgy[500];
    sprintf(cgy, "", ReadCharacteristic.value(), ReadCharacteristic.read());
    Serial.print(cgy);

    //一般直接取得新律值的方式
    Serial.print(count);
    Serial.print(",");
    String data = "";
    data += static_cast<unsigned char>(val[1]);
    Serial.print(data);

    //你可以選擇舊的取值方式，或打印 hartRate 不加陣列索引，給電腦更多判斷選項。
    /*Serial.print(hartRate[3]);
    Serial.print(hartRate[4]);*/
    Serial.println("");
    delay(50);

    if(count == seconds){
      //取得最後一個心律值，寫入佔存器
      int currentHartRate = data.toInt();

      //關閉偵測心律
      Serial.println("Function: Stop To Read Hart Rate...");
      const byte ba[] = {0xD0, 0x00};
      WriteCharacteristic.writeValue(ba,2);
      
      //結束後記得一定要斷接
      peripheral.disconnect();
      return currentHartRate;
    }
  }

  Serial.println("Peripheral disconnected");
  return -1;
}

