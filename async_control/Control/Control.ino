/*
    Author: 資工系 - Mac Taylor
    Date:   2017/10/4
    Reference:
     - Timer Library : https://github.com/JChristensen/Timer

    新連接方式參照:
    https://github.com/01org/corelibs-arduino101/blob/2227ce15e6f2f02bd4648ae46221a374df73b3ba/libraries/CurieBLE/src/BLEDevice.h#L438https://github.com/01org/corelibs-arduino101/blob/2227ce15e6f2f02bd4648ae46221a374df73b3ba/libraries/CurieBLE/src/BLEDevice.h#L438
*/
#include <CurieBLE.h>
#include "HashMap.h"
#include "Timer.h"


String readString = "";
String deviceName = "V07S";
Timer tcb;

//系統計數器
static unsigned int count = 0;
void addTime(){
  count++;
}


//先設定裝置大小
const byte HASH_SIZE = 2;

//預掛暫存器
HashType<int,char*> hashRawArray[HASH_SIZE];
HashMap<int,char*> DEVICES = HashMap<int,char*>( hashRawArray , HASH_SIZE );

//設定所有裝置 mac 地址 (請先設定裝置大小)
String devices[HASH_SIZE] = {"E5:73:8A:4B:7D:85","D4:4E:82:0E:21:B7"};

//現在使用的裝置暫存
int device_use = 0;
 
void setup() {
  Serial.begin(9600);

  //自動設定裝置 MAC 地址
  for(int i = 0 ;i<sizeof(devices);i++){
    DEVICES[i](i, &devices[i][0u]);
  }

  //系統非同步計數器
  tcb.every(1000, addTime);
  
  // initialize the BLE hardware
  BLE.begin();

  Serial.println("BLE Central Read");

  //搜尋設備 MAC
  BLE.scanForAddress(DEVICES.getValueOf(device_use));
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
        
        //把這個值拿來使用即可 (wifi or enc28j60 網路傳輸，請使用這個值)
        Serial.print(hartRate);
        Serial.println();
      }

      //開啟讀取血壓
      if(readString.substring(0,1) == "B"){
        Serial.println("Now Reading Blood Pressure");
        String bp = readBloodPressure(peripheral,60); //30 seconds
        Serial.print("Final BP: ");
        
        //把這個值拿來使用即可 (wifi or enc28j60 網路傳輸，請使用這個值)
        Serial.print(bp);
        Serial.println();
      }
      //

      //設定成其他(陣列中)的裝置
      if(readString.substring(0,3) == "SET"){
        String setid = readString.substring(3,4);
        
        device_use = setid.toInt();

        Serial.println("SET DEVICE TO: " + device_use);
        
        Serial.println("Disconnect Device.");
        peripheral.disconnect();
        BLE.disconnect();
        BLE.begin();
        Serial.println("Connect Device to: " + String(DEVICES.getValueOf(device_use)));
        BLE.scanForAddress(DEVICES.getValueOf(device_use));
        Serial.println();
      }
      
      readString="";
    }

    
    //繼續搜尋裝置
    BLE.scanForAddress(DEVICES.getValueOf(device_use));
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
  WriteCharacteristic.writeValue(openHartRateByte,2); //設定為 2 個 byte
  
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

String readBloodPressure(BLEDevice peripheral, int seconds) {
  //歸 0 系統計數
  count = 0;
    
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return "connect to peripheral failed!";
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributesByService("f0080001-0451-4000-b000-000000000000")) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return "connect to services failed!";
  }

  //Write Type 的 Characteristic 值
  BLECharacteristic WriteCharacteristic = peripheral.characteristic("f0080003-0451-4000-b000-000000000000");

  // Read Type 的 Characteristic 值
  BLECharacteristic ReadCharacteristic = peripheral.characteristic("f0080002-0451-4000-b000-000000000000");

  //寫入值，開始偵測心律
  Serial.println("Function: Start To Read Hart Rate...");
  const byte openBPByte[] = {0x90, 0x01, 0x00};
  WriteCharacteristic.writeValue(openBPByte,3); //設定為 3 個 byte
  
  //開始進行遍歷值
  while (peripheral.connected()) {
    //繼續計時器的計數
    tcb.update();
    
    unsigned char len = ReadCharacteristic.valueLength();
    const unsigned char *val = ReadCharacteristic.value();
    unsigned char i = 0;

    String BP = "";
    while(i<len)
    {
      BP += static_cast<unsigned char>(val[i++]);
      BP += ",";
    }

    char cgy[500];
    sprintf(cgy, "", ReadCharacteristic.value(), ReadCharacteristic.read());
    Serial.print(cgy);

    //一般直接取得新律值的方式
    Serial.print(count);
    Serial.print(",");
    String data = "";
    data += static_cast<unsigned char>(val[1]);
    data += ",";
    data += static_cast<unsigned char>(val[2]);
    Serial.print(data);

    Serial.println("");
    delay(50);

    if(count == seconds){
      //取得最後一個血壓值，寫入佔存器
      String currentBP = data;

      //關閉偵測心律
      Serial.println("Function: Stop To Read BP...");
      const byte ba[] = {0x90, 0x00, 0x00};
      WriteCharacteristic.writeValue(ba,3);
      
      //結束後記得一定要斷接
      peripheral.disconnect();
      return currentBP;
    }
  }

  Serial.println("Peripheral disconnected");
  return "no data";
}
