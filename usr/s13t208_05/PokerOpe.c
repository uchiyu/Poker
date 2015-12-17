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

void outputCard( int deck[], int num ) {
  int i;

  for ( i = 0; i < 13; i++) {
    printf("%2d ", i+1);
  }
  printf("\n");
  for ( i = 1; i <= num; i++) {
    printf("%2d ", deck[i-1]);
    if ( i%13 == 0 && i > 0 ) { printf("\n"); }
  }
  printf("\n\n");
}
 
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

double pointConvert( double point, int tk ) {
  double rtPoint = 0.0;
  switch (tk) {
    case 0: rtPoint = point * TK1; break;
    case 1: rtPoint = point * TK2; break;
    case 2: rtPoint = point * TK3; break;
    case 3: rtPoint = point * TK4; break;
    case 4: rtPoint = point * TK5; break;
  }
  return rtPoint;
}

double calcExp( int hd[], int fd[], int cg, int tk, int ud[], int us, int deck[], int changeCard, int count ) {
  int i, k;
  int nextHd[HNUM]; //次の手札
  int nextDeck[CNUM] = { 0 };
  double exp = 0;
        
  for ( i = 0; i < CNUM; i++) {
    if ( deck[i] == 1 ) {
      copyHd(nextHd, hd);
      nextHd[changeCard] = i;
      copyDeck(nextDeck, deck);
      nextDeck[i] = -1;
      exp += poker_point(nextHd)/(double)recursionExp( nextHd, nextDeck, count );
    }
  }
  return exp;
}

// 捨てる手札でもっとも期待値が高くなる札番号を返す
// 一度目の呼び出しでは、beforepointは0, selectnumは-1
int bestExp( int hd[], int fd[], int cg, int tk, int ud[], int us, int deck[], int checkhdnum[], int count, double beforepoint, int selectnum ) {
  int i;
  double bestpoint = (double)poker_point(hd); // 現在の手札の点数
  int select = selectnum; //捨てる手札の番号
  double valuehd[HNUM] = { 0.0 };
  double averagenum = 0;
  int nextHd[HNUM] = { 0 };
  int nextDeck[CNUM] = { 0 };

  if ( count == recursionlimit ) { return selectnum; }
 
  for ( i = 0; i < HNUM; i++ ) {
    // 評価値の作成 
    valuehd[i] = calcExp( hd, fd, cg, tk , ud, us, deck, i, count );
  

    //printf("%3.0f ", valuehd[i]);
    

    // 期待値の最高点の更新
    if ( bestpoint < valuehd[i] && beforepoint < valuehd[i] ) {
      bestpoint = valuehd[i];
      select = i;
    }
  }


  //printf(" == %-d\n", select);


  //if ( count == 0 ) { return select; }
  averagenum = average( valuehd, i );
  for ( i = 0; i < HNUM; i++ ) {
    if ( averagenum < valuehd[i] ) {
      copyHd(nextHd, hd);
      copyDeck(nextDeck, deck);
      nextDeck[i] = -1;
      bestExp( nextHd, fd, cg+1, tk , ud, us, nextDeck, hd, count+1, bestpoint, select );
    }
  }

  //return select;
}

// 捨てると最も期待値が高くなるカードを返す
int selectCard( int hd[], int fd[], int cg, int tk, int ud[], int us ) {
  int deck[CNUM] = { 0 };
  int num = HNUM;

  makeDeck(hd, fd, cg, tk , ud, us, deck );
  return bestExp( hd, fd, cg, tk , ud, us, deck, hd, 0, 0, -1 );
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
  if ( us >= 46 ) { return -1; }
  select = selectCard( hdcopy, fdcopy, cg, tk, udcopy, us );
  return select;
}


//====================================================================
//  補助関数
//====================================================================

