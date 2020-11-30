# Robosys2020
ロボットシステム学2020で作成したデバイスドライバ


## 使用したもの
・Raspberry Pi4

・ブレットボード

・ジャンパー線

・LED 4個

・抵抗　4つ(200Ω)


## 実行方法
```
$ sudo make  
                             
$ sudo insmod myled.ko
                              
$ chmod 666 /dev/myled0   
 
$ echo [コマンド] > /dev/myled0
```

## コマンドの動作内容
・1のとき：4つのLEDが点滅する

・2のとき：4つのLEDが順番に点滅(速度が変化)

・3のとき：4つのLEDが対角線上に点滅(速度が変化)

・0のとき：LEDが消える


## 動画URL
[Youtube](https://youtu.be/rtgkgXmA7jo)

