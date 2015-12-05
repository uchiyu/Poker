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

  ---------------------------------------------------------------------*/


//====================================================================
//  前処理
//====================================================================

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../../src/Poker.h"

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

#define CARDNUM 52 //カード総数

/*--------------------------------------------------------------------*/

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
  for ( i = 0; i < 5; i++) {
    deck[hd[i]] = -1;
  }

  for ( i = 0; i < cg; i++) {
    deck[fd[i]] = -1;
  }
  for ( i = 0; i < us; i++) {
    deck[ud[i]] = -1;
  }
  
  for ( i = 0; i < CARDNUM; i++) {
    if ( deck[i] == 0 ) { deck[i] = 1; }
  }
}

// デッキの残り札 cg, usとか使えばいらないかも
int deckNum( int deck[] ) {
  int i;
  int num = 0;

  for ( i = 0; i < CARDNUM; i++) {
    if ( deck[i] == 1 ) { num++; }
  }
  return num;
}

void copyHd( int hd[], int nextHd[]) {
  int i;
  for ( i = 0; i < 5; i++) {
    nextHd[i] = hd[i];
  }
}

double calcExp( int hd[], int fd[], int cg, int tk, int ud[], int us, int deck[], int changeCard ) {
  int i;
  int nextHd[5]; //次の手札
  double exp = 0;

  for ( i = 0; i < CARDNUM; i++) {
    if ( deck[i] == 1 ) {
      copyHd(hd, nextHd);
      nextHd[changeCard] = i;
      exp += poker_point(nextHd)/(double)deckNum(deck);
    }
  }
  return exp;
}

// 捨てる手札でもっとも期待値が高くなる札番号を返す
int selectCard( int hd[], int fd[], int cg, int tk, int ud[], int us, int deck[] ) {
  int i;
  double hightestExp = poker_point(hd); // 現在の手札の点数
  double exp;
  int select = -1; //捨てる手札の番号

  for ( i = 0; i < 5; i++ ) {
    exp = calcExp( hd, fd, cg, tk , ud, us, deck, i );
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
  int deck[CARDNUM] = { 0 };

  makeDeck(hd, fd, cg, tk , ud, us, deck );

  return selectCard( hd, fd, cg, tk , ud, us, deck );
}

int strategy( int hd[], int fd[], int cg, int tk, int ud[], int us) {
  int myhd[HNUM];
  int select = -1;
  int point;

  int i;

  //高い役の時終了
  point = poker_point( hd ); //手札の点数
  if ( point == P9 ) { return -1; } //ロイヤルストレートフラッシュ
  if ( point == P8 ) { return -1; } //ストレートフラッシュ
  if ( point == P7 ) { return -1; } //フォーカード
  if ( point == P5 ) { return -1; } //フラッシュ
  if ( point == P6 ) { return -1; } //フルハウス
  if ( point == P4 ) { return -1; } //ストレート

  select = bestExp( hd, fd, cg, tk, ud, us );

  return select;
}

//====================================================================
//  補助関数
//====================================================================

