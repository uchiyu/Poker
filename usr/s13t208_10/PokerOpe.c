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
#include <stdbool.h>

#include "Poker.h"

#define RECURSION_LIMIT 2 //1で1手先まで
#define LAST_RECURSION_LIMIT 3 //1で1手先まで
//int LAST_RECURSION_LIMIT = 0; //テイク5の再帰回数
int MAX_EXP;

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
int point_convert( int point ) {
  int convertpoint = 0;
  
  switch (point) {
    case P1: convertpoint = point * 681; break;
    case P2: convertpoint = point * 57; break;
    case P3: convertpoint = point * 581; break;
    case P4: convertpoint = point * 3973; break;
    case P5: convertpoint = point * 2217; break;
    case P6: convertpoint = point * 802; break;
    case P7: convertpoint = point * 9520; break;
    case P8: convertpoint = point * 1720; break;
    case P9: convertpoint = point * 2035; break;
  }
  return convertpoint;
}

int make_deck( int hd[], int ud[], int us, int deck[]) {
  int i;
  int decknum = 0;
  // 手札の収納
  for ( i = 0; i < HNUM; i++ ) {
    deck[hd[i]] = -1;
    decknum++;
  }
  // 捨て札の収納
  for ( i = 0; i < us; i++ ) {
    deck[ud[i]] = -1;
    decknum++;
  }
  return CNUM - decknum;
}

void calc_exp( int hd[], int cg, int tk, int pointhd[], int pointnum, int deck[], int decknum, int recursioncount, int starthd ) {
  int nexthd[HNUM];
  int nextdeck[CNUM];
  int point = 0;
  int i, j;

  for ( i = starthd; i < HNUM; i++) {
    for ( j = 0; j < CNUM; j++) {
      // 山札にカードがあれば
      if ( deck[j] == 0 ) {
        // 手札とデッキの複製
        arr_copy( nexthd, hd, HNUM );
        nexthd[i] = j;
        arr_copy( nextdeck, deck, CNUM );
        nextdeck[j] = -1;

        if ( recursioncount == 1 ) {
          // 一手目の期待値の加算
          pointhd[i] += point_convert(poker_point(nexthd))*decknum;
        } else {
          // 二手目の期待値の加算
          point = point_convert(poker_point(nexthd));
          pointhd[pointnum] += point;
          if ( pointnum < i ) {
            pointhd[i] += point;
          }
        }

        if ( cg > 7 - RECURSION_LIMIT ) { continue; }
        if ( recursioncount >= RECURSION_LIMIT ) { continue; }

        // 再帰呼び出し
        if ( pointnum == -1 ) {
          calc_exp( nexthd, cg+1, tk, pointhd, i, nextdeck, decknum-1, recursioncount+1, i );
        } else {
          calc_exp( nexthd, cg+1, tk, pointhd, pointnum, nextdeck, decknum-1, recursioncount+1, i );
        }
      }
    }
  }
}

int select_card( int hd[], int cg, int tk, int ud[], int us, int deck[], int decknum ) {
  int hightexp = ( decknum > 1 ) ? poker_point(hd)*decknum*(decknum-1) : poker_point(hd)*decknum;
  int select = -1;
  int recursioncount = 1;
  int pointhd[HNUM] = { 0 }; // 期待値を格納
  int i;

  calc_exp( hd, cg, tk, pointhd, -1, deck, decknum, recursioncount, 0);

  for ( i = 0; i < HNUM; i++) {
    if ( pointhd[i] > hightexp ) {
      hightexp = pointhd[i];
      select = i;
    }
  }
  return select;
}

void seek_hightest( int hd[], int cg, int tk, int changecard, int deck[], int recursioncount ) {
  int nexthd[HNUM];
  int nextdeck[CNUM];
  int currentpoint;
  int i, k;

  for ( i = 0; i < CNUM; i++) {
    if ( deck[i] == 0 ) {
      arr_copy( nexthd, hd, HNUM );
      nexthd[changecard] = i;
      arr_copy( nextdeck, deck, CNUM );
      nextdeck[i] = -1;
      currentpoint = poker_point(nexthd);
      if ( currentpoint > MAX_EXP ) { MAX_EXP = currentpoint; }

      if ( cg > 7 - LAST_RECURSION_LIMIT ) { continue; }
      if ( recursioncount >= LAST_RECURSION_LIMIT ) { continue; }
      for ( k = 0; k < HNUM; k++ ) {
          seek_hightest( nexthd, cg+1, tk, k, nextdeck, recursioncount+1 );
      }
    }
  }
}

int select_card_last( int hd[], int cg, int tk, int ud[], int us, int deck[], int decknum ) {
  int hightexp = poker_point(hd);
  int exp;
  int select = -1;
  int recursioncount = 1;
  int i;

  for ( i = 0; i < HNUM; i++ ) {
    MAX_EXP = 0;
    seek_hightest( hd, cg, tk, i, deck, recursioncount);
    if ( hightexp < MAX_EXP ) {
      hightexp = MAX_EXP;
      select = i;
    }
  }
  MAX_EXP = hightexp;

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
  int lastchangehd = 0;

  // 最初から高い点なら手札を変えずに終了
  arr_copy( hdcopy, hd, HNUM);
  currentpoint = poker_point(hdcopy);
  if ( currentpoint >= P6 ) { return -1; }

  arr_copy( udcopy, ud, us );
  decknum = make_deck( hdcopy, udcopy, us, deck );
  if ( tk == 4 && decknum <= 7 - cg ) {
    //LAST_RECURSION_LIMIT = decknum;
    select = select_card_last( hdcopy, cg, tk, udcopy, us, deck, decknum );
  } else {
    select = select_card( hdcopy, cg, tk, udcopy, us, deck, decknum );
  }
  return select;
  
}

//====================================================================
//  補助関数
//====================================================================

