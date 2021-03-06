//====================================================================
//  工学部「情報環境実験２」(富永)  C言語プログラミング  例題
//  ポーカーゲームの戦略
//--------------------------------------------------------------------
//  Poker  PokerOpe.c
//  WinXP DOS  Borland C/C++ 5.5
//--------------------------------------------------------------------
//  安藤研究室 s13t208 内山豊
//  2015.11.10
//====================================================================


//====================================================================
//  仕様
//====================================================================

/*--------------------------------------------------------------------

  手札、場札、チェンジ数、テイク数、捨札を引数とし、捨札を決める。
  返却値は、捨札の位置である。-1のときは、交換しないで、手札を確定させる。
  関数 strategy() は、戦略のインタフェースであり、この関数内で、
  実際の戦略となる関数を呼び出す。手札と捨札は、不正防止のため、
  変更不可なので、局所配列にコピーして、整列などの処理を行う。
  複数の戦略を比較したり、パラメタを変化させて、より強い戦略を目指す。

#define  GNUM  10000    // ゲーム最大回数
#define  CNUM     52    // カードの枚数
#define  HNUM      5    // 手札の枚数
#define  FNUM     10    // 場札の最大枚数(5×2)

#define POINT_MAX       9    // 役番号の最大点
#define POINT_MIN       0    // 役番号の最小点
#define POINT_NUM      10    //

//----  役の配点
#define P0    0    // ノーペア
#define P1    1    // ワンペア
#define P2    2    // ツーペア
#define P3    8    // スリーカインズ
#define P4   32    // ストレート
#define P5   24    // フラッシュ
#define P6   16    // フルハウス
#define P7   64    // フォーカード
#define P8  128    // ストレートフラッシュ
#define P9  256    // ロイヤルストレートフラッシュ

  ---------------------------------------------------------------------*/


//====================================================================
//  前処理
//====================================================================

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Poker.h"

#define LIMIT 2 //1で1手先まで

//--------------------------------------------------------------------
//  関数宣言
//--------------------------------------------------------------------



//====================================================================
//  戦略
//====================================================================

/*--------------------------------------------------------------------
//  ユーザ指定
//--------------------------------------------------------------------

最初の手札のまま交換しない。

hd : 手札配列
fd : 場札配列(テイク内の捨札)
cg : チェンジ数
tk : テイク数
ud : 捨札配列(過去のテイクも含めた全ての捨札)
us : 捨札数

--------------------------------------------------------------------*/
int pointconvert( int point ) {
  int result = 0;
  
  switch (point) {
    //case P3: point = point * 2.2; break;
    case P4: result = point * 2.2; break;
    case P5: result = point * 2.0; break;
    //case P6: point = point * 2.2; break;
    //case P7: point = point * 2.2; break;
    //case P8: point = point * 2.2; break;
    //case P9: point = point * 2.2; break;
  }
  return result;
}

int makedeck( int hd[], int ud[], int us, int deck[]) {
  int i;
  int count = 0;
  // 手札の収納
  for ( i = 0; i < HNUM; i++ ) {
    deck[hd[i]] = -1;
    count++;
  }
  // 捨て札の収納
  for ( i = 0; i < us; i++ ) {
    deck[ud[i]] = -1;
    count++;
  }
  return CNUM - count;
}

void calcexp( int hd[], int cg, int tk, int pointhd[], int pointnum, int deck[], int decknum, int recursioncount, int starthd ) {
  int nexthd[HNUM];
  int nextdeck[CNUM];
  int point = 0;
  int i, j;

  for ( i = starthd; i < HNUM; i++) {
    if ( pointnum != -1 ) {
      //printf("%d, %d, %d\n", pointnum, i, decknum);
    }
    for ( j = 0; j < CNUM; j++) {
      if ( deck[j] == 0 ) {
        arr_copy( nexthd, hd, HNUM );
        nexthd[i] = j;
        arr_copy( nextdeck, deck, CNUM );
        nextdeck[j] = -1;

        if ( pointnum == -1 ) {
          // 一手目の期待値の加算
          //pointhd[i] += poker_point(nexthd)*(decknum*(LIMIT+1-recursioncount));
          pointhd[i] += poker_point(nexthd)*decknum;
          //pointhd[i] += pointconvert(poker_point(nexthd))*decknum;
        } else {
          // 二手目以降の期待値の加算
          point = poker_point(nexthd);
          //point = pointconvert(poker_point(nexthd));

          pointhd[pointnum] += point;

          if ( pointnum < i ) {
            pointhd[i] += point;
          }
        }

        //if ( tk < 1 || tk > 3 ) { continue; }
        if ( cg > 7 - LIMIT ) { continue; }
        if ( recursioncount >= LIMIT ) { continue; }

        if ( pointnum == -1 ) {
          calcexp( nexthd, cg+1, tk, pointhd, i, nextdeck, decknum-1, recursioncount+1, i );
        } else {
          calcexp( nexthd, cg+1, tk, pointhd, pointnum, nextdeck, decknum-1, recursioncount+1, i );
        }

      }
    }
  }
}

int selectcard( int hd[], int cg, int tk, int ud[], int us, int deck[], int decknum ) {
  int hightexp = poker_point(hd)*decknum;
  int select = -1;
  int recursioncount = 1;
  int pointhd[HNUM] = { 0 }; // 期待値を格納
  int i;

  calcexp( hd, cg, tk, pointhd, -1, deck, decknum, recursioncount, 0);

  for ( i = 0; i < HNUM; i++) {
    if ( pointhd[i] > hightexp ) {
      hightexp = pointhd[i];
      select = i;
    }
  }
  return select;
}

int strategy( const int hd[], const int fd[], int cg, int tk, const int ud[], int us) {
  int myhd[HNUM];
  int select = -1;
  int hdcopy[CNUM];
  int udcopy[us];
  int currentpoint = 0;
  int deck[CNUM] = { 0 };
  int decknum = CNUM;
  int k;

  // 最初から高い点なら手札を変えずに終了
  arr_copy( hdcopy, hd, HNUM);
  currentpoint = poker_point(hdcopy);
  if ( currentpoint >= P6 ) { return -1; }

  arr_copy( udcopy, ud, us );
  decknum = makedeck( hdcopy, udcopy, us, deck );
  select = selectcard(hdcopy, cg, tk, udcopy, us, deck, decknum );
  return select;
}

//====================================================================
//  補助関数
//====================================================================

