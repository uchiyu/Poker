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

#define recursionLimit 1    // 先読み回数の指定 0が一手先

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
  double parameter = 1.0;
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
  
  switch ((int)point) {
    /*
    case P3: point = point * 2.9; break;
    case P4: point = point * 2.4; break;
    case P5: point = point * 2.1; break;
    case P6: point = point * 3.9; break;
    case P7: point = point * 2.1; break;
    case P8: point = point * 2.7; break;
    case P9: point = point * 3.9; break;
    */
  }

  switch (tk) {
    case 0: rtPoint = point * TK1; break;
    case 1: rtPoint = point * TK2; break;
    case 2: rtPoint = point * TK3; break;
    case 3: rtPoint = point * TK4; break;
    case 4: rtPoint = point * TK5; break;
  }
  return rtPoint;
}

double calcExp( int hd[], int fd[], int cg, int tk, int ud[], int us, int deck[], int changeCard, int recursionCount ) {
  int i, k;
  int nextHd[HNUM]; //次の手札
  int nextDeck[CNUM] = { 0 };
  double exp = 0;
  double currentExp = 0;
  double point;
        
  if ( recursionCount > recursionLimit ) { return 0.0; } // 再帰回数の制限
      
  for ( i = 0; i < CNUM; i++) {
    if ( deck[i] == 1 ) {
      copyHd(nextHd, hd);
      nextHd[changeCard] = i;
      copyDeck(nextDeck, deck);
      nextDeck[i] = -1;
      //point = pointConvert( (double)poker_point(nextHd), tk);
      point = poker_point(nextHd);
      currentExp = (double)point/recursionExp( nextHd, nextDeck, recursionCount );
      exp += currentExp;
  
      if ( tk != 2 ) { continue; }
      if ( tk < 1 || tk > 3  ) { continue; } // 配点が低い最初と最後のテイクは二手先は無視
      if ( cg > 6 - recursionLimit ) { continue; } // チェンジ回数による再帰の制限
      //if ( currentExp < 1 && recursionCount > 0 ) { return 0; }
      // 二手先以降の期待値
      for ( k = 0; k < HNUM; k++) {
        exp += calcExp( nextHd, fd, cg, tk , ud, us, nextDeck, k, recursionCount+1 );
      }
    }
  }
  return exp;
}

// 捨てる手札でもっとも期待値が高くなる札番号を返す
int selectCard( int hd[], int fd[], int cg, int tk, int ud[], int us, int deck[] ) {
  int i;
  double hightestExp = (double)poker_point(hd); // 現在の手札の点数
  double exp;
  int select = -1; //捨てる手札の番号
  int recursionCount = 0; //再帰回数

  for ( i = 0; i < HNUM; i++ ) {
    exp = calcExp( hd, fd, cg, tk , ud, us, deck, i, recursionCount );
    // 現在より期待値が高ければselectを変更
    if ( exp > hightestExp ) { 
      select = i;
      hightestExp = exp;
    }
  }
  return select;
}

// 捨てると最も期待値が高くなるカードを返す
int bestExp( int hd[], int fd[], int cg, int tk, int ud[], int us ) {
  int deck[CNUM] = { 0 };

  makeDeck(hd, fd, cg, tk , ud, us, deck );

  return selectCard( hd, fd, cg, tk , ud, us, deck );
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
  select = bestExp( hdcopy, fdcopy, cg, tk, udcopy, us );
  return select;
}

//====================================================================
//  補助関数
//====================================================================

