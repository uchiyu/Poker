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

#define TK1 1.0
#define TK2 1.5
#define TK3 2.0
#define TK4 1.5
#define TK5 1.0

#define TRUE 1
#define FALSE 0

#define recursionlimit 1    // 先読み回数の指定 0が一手先
//int recursionlimit = 1;    // 先読み回数の指定 0が一手先

double BestPoint = 0;
int Select = -1;

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

// 山札と既に場に出た札を合わせたものを生成
// 山札 +1, それ以外 -1
void makeDeck( int hd[], int fd[], int cg, int tk, int ud[], int us, int deck[] ) {
  int i;
  for ( i = 0; i < HNUM; i++) {
    deck[hd[i]] = -1;
  }

  for ( i = 0; i < us; i++) {
    deck[ud[i]] = -1;
  }
  
  for ( i = 0; i < CNUM; i++) {
    if ( deck[i] == 0 ) { deck[i] = 1; }
  }
}

// デッキの残り札 cg, usとか使えばいらないかも
int deckNum( int deck[] ) {
  int i;
  int num = 0;

  for ( i = 0; i < CNUM; i++) {
    if ( deck[i] == 1 ) { num++; }
  }
  return num;
}

double average( double array[], int num ) {
  int sum = 0;
  int i = 0;
  
  for ( i = 0; i < num; i++) {
    sum += array[i];
  }
  return sum/(double)num;
}

void copyHd( int nextHd[], const int hd[]) {
  int i;
  for ( i = 0; i < HNUM; i++) {
    nextHd[i] = hd[i];
  }
}

void copyDeck( int nextDeck[], int deck[]) {
  int i;
  for ( i = 0; i < CNUM; i++) {
    nextDeck[i] = deck[i];
  }
}

void copyArray( int next[], const int current[], int num) {
  int i;
  for ( i = 0; i < num; i++) {
    next[i] = current[i];
  }
}

double recursionExp ( int hd[], int deck[], int recursionCount ) {
  double parameter = 1;
  int decknum = 0;
  int i;

  decknum = deckNum( deck );
  for ( i = 0; i <= recursionCount; i++ ) {
    parameter *= (double)( decknum + i );
  }
  return parameter;
}

int strategy( const int hd[], const int fd[], int cg, int tk, const int ud[], int us) {
  int myhd[HNUM];
  int select = -1;
  int hdcopy[CNUM];
  int fdcopy[cg];
  int udcopy[us];

  copyHd(hdcopy, hd);
  copyArray( fdcopy, fd, cg );
  copyArray( udcopy, ud, us );

  printf("%d\n", cg);

  if ( us >= 46 ) { return -1; }
  return select;
}


//====================================================================
//  補助関数
//====================================================================

