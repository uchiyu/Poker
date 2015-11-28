# Poker

* デッキの生成  
```
$ cd src/  
$ gcc -o DeckInit DeckInit.c CardLib.c  
$ cp DeckInit.exe ../sys/  
```
* コンパイル  
```
$ cd usr/s13t...  
$ sh ../../sys/Make2015.sh  
```
* 山札の生成  
```
$ ../../sys/DeckInit.exe 山札数(整数) > 山札ファイル(例：Deck_01.txt)  
```
* プログラムの実行  
```  
$ ./PokerExec.exe s13t... 1000(トライ数) 山札のファイル名 0  
```

---
* 自動での実行  
```  
$ cd cd usr/s13t208_??/  
$ sh auto_exe.sh  
```
