/* Copyright 1990-2016 by Chris Osborn <fozztexx@fozztexx.com>
 * http://insentricity.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* devNXWin.c generated from devNXWin.psw
   by unix pswrap V1.009  Wed Apr 19 17:50:24 PDT 1989
 */

#include <dpsclient/dpsfriends.h>
#include <string.h>

#line 1 "devNXWin.psw"
#line 10 "devNXWin.c"
void getDepth(int *min, int *max, int *iscolor, int *width, int *height)
{
  typedef struct {
    unsigned char tokenType;
    unsigned char topLevelCount;
    unsigned short nBytes;

    DPSBinObjGeneric obj0;
    DPSBinObjGeneric obj1;
    DPSBinObjGeneric obj2;
    DPSBinObjGeneric obj3;
    DPSBinObjGeneric obj4;
    DPSBinObjGeneric obj5;
    DPSBinObjGeneric obj6;
    DPSBinObjGeneric obj7;
    DPSBinObjGeneric obj8;
    DPSBinObjGeneric obj9;
    DPSBinObjGeneric obj10;
    DPSBinObjGeneric obj11;
    DPSBinObjGeneric obj12;
    DPSBinObjGeneric obj13;
    DPSBinObjGeneric obj14;
    DPSBinObjGeneric obj15;
    DPSBinObjGeneric obj16;
    DPSBinObjGeneric obj17;
    DPSBinObjGeneric obj18;
    DPSBinObjGeneric obj19;
    DPSBinObjGeneric obj20;
    DPSBinObjGeneric obj21;
    DPSBinObjGeneric obj22;
    DPSBinObjGeneric obj23;
    DPSBinObjGeneric obj24;
    DPSBinObjGeneric obj25;
    char obj26[17];
    char obj27[15];
    char obj28[17];
    } _dpsQ;
  static const _dpsQ _dpsF = {
    DPS_DEF_TOKENTYPE, 26, 261,
    {DPS_EXEC|DPS_NAME, 0, 15, 225},	/* workspaceWindow */
    {DPS_EXEC|DPS_NAME, 0, 17, 240},	/* currentdeviceinfo */
    {DPS_LITERAL|DPS_INT, 0, 0, 2},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 119},	/* printobject */
    {DPS_LITERAL|DPS_INT, 0, 0, 1},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 119},	/* printobject */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 119},	/* printobject */
    {DPS_EXEC|DPS_NAME, 0, 15, 225},	/* workspaceWindow */
    {DPS_EXEC|DPS_NAME, 0, 17, 208},	/* windowdeviceround */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 308},	/* initclip */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 21},	/* clippath */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 115},	/* pathbbox */
    {DPS_LITERAL|DPS_INT, 0, 0, 4},
    {DPS_LITERAL|DPS_INT, 0, 0, -2},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 135},	/* roll */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 117},	/* pop */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 117},	/* pop */
    {DPS_LITERAL|DPS_INT, 0, 0, 4},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 119},	/* printobject */
    {DPS_LITERAL|DPS_INT, 0, 0, 3},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 119},	/* printobject */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 5},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 119},	/* printobject */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 70},	/* flush */
    {'w','i','n','d','o','w','d','e','v','i','c','e','r','o','u','n','d'},
    {'w','o','r','k','s','p','a','c','e','W','i','n','d','o','w'},
    {'c','u','r','r','e','n','t','d','e','v','i','c','e','i','n','f','o'},
    }; /* _dpsQ */
  register DPSContext _dpsCurCtxt = DPSPrivCurrentContext();
  char pad[3];
  DPSResultsRec _dpsR[5];
  static const DPSResultsRec _dpsRstat[] = {
    { dps_tInt, -1 },
    { dps_tInt, -1 },
    { dps_tBoolean, -1 },
    { dps_tInt, -1 },
    { dps_tInt, -1 },
    };
    _dpsR[0] = _dpsRstat[0];
    _dpsR[0].value = (char *)min;
    _dpsR[1] = _dpsRstat[1];
    _dpsR[1].value = (char *)max;
    _dpsR[2] = _dpsRstat[2];
    _dpsR[2].value = (char *)iscolor;
    _dpsR[3] = _dpsRstat[3];
    _dpsR[3].value = (char *)width;
    _dpsR[4] = _dpsRstat[4];
    _dpsR[4].value = (char *)height;

  DPSSetResultTable(_dpsCurCtxt, _dpsR, 5);
  DPSBinObjSeqWrite(_dpsCurCtxt,(char *) &_dpsF,261);
  DPSAwaitReturnValues(_dpsCurCtxt);
  if (0) *pad = 0;    /* quiets compiler warnings */
}
#line 5 "devNXWin.psw"

#line 108 "devNXWin.c"
void initWindow(int x, int y, int width, int height, int lwin, int show, int *windownum)
{
  typedef struct {
    unsigned char tokenType;
    unsigned char topLevelCount;
    unsigned short nBytes;

    DPSBinObjGeneric obj0;
    DPSBinObjGeneric obj1;
    DPSBinObjGeneric obj2;
    DPSBinObjGeneric obj3;
    DPSBinObjGeneric obj4;
    DPSBinObjGeneric obj5;
    DPSBinObjGeneric obj6;
    DPSBinObjGeneric obj7;
    DPSBinObjGeneric obj8;
    DPSBinObjGeneric obj9;
    DPSBinObjGeneric obj10;
    DPSBinObjGeneric obj11;
    DPSBinObjGeneric obj12;
    DPSBinObjGeneric obj13;
    DPSBinObjGeneric obj14;
    DPSBinObjGeneric obj15;
    DPSBinObjGeneric obj16;
    DPSBinObjGeneric obj17;
    DPSBinObjGeneric obj18;
    DPSBinObjGeneric obj19;
    DPSBinObjGeneric obj20;
    DPSBinObjGeneric obj21;
    DPSBinObjGeneric obj22;
    DPSBinObjGeneric obj23;
    DPSBinObjGeneric obj24;
    DPSBinObjGeneric obj25;
    DPSBinObjGeneric obj26;
    DPSBinObjGeneric obj27;
    DPSBinObjGeneric obj28;
    DPSBinObjGeneric obj29;
    DPSBinObjGeneric obj30;
    DPSBinObjGeneric obj31;
    DPSBinObjGeneric obj32;
    DPSBinObjGeneric obj33;
    DPSBinObjGeneric obj34;
    DPSBinObjGeneric obj35;
    DPSBinObjGeneric obj36;
    DPSBinObjGeneric obj37;
    DPSBinObjGeneric obj38;
    DPSBinObjGeneric obj39;
    DPSBinObjGeneric obj40;
    DPSBinObjGeneric obj41;
    DPSBinObjGeneric obj42;
    DPSBinObjGeneric obj43;
    DPSBinObjGeneric obj44;
    DPSBinObjGeneric obj45;
    DPSBinObjGeneric obj46;
    DPSBinObjGeneric obj47;
    DPSBinObjGeneric obj48;
    DPSBinObjGeneric obj49;
    DPSBinObjGeneric obj50;
    DPSBinObjGeneric obj51;
    DPSBinObjGeneric obj52;
    DPSBinObjGeneric obj53;
    DPSBinObjGeneric obj54;
    DPSBinObjGeneric obj55;
    DPSBinObjGeneric obj56;
    DPSBinObjGeneric obj57;
    DPSBinObjGeneric obj58;
    DPSBinObjGeneric obj59;
    DPSBinObjGeneric obj60;
    DPSBinObjGeneric obj61;
    DPSBinObjGeneric obj62;
    DPSBinObjGeneric obj63;
    DPSBinObjGeneric obj64;
    DPSBinObjGeneric obj65;
    DPSBinObjGeneric obj66;
    DPSBinObjGeneric obj67;
    DPSBinObjGeneric obj68;
    DPSBinObjGeneric obj69;
    DPSBinObjGeneric obj70;
    DPSBinObjGeneric obj71;
    DPSBinObjGeneric obj72;
    DPSBinObjGeneric obj73;
    DPSBinObjGeneric obj74;
    DPSBinObjGeneric obj75;
    DPSBinObjGeneric obj76;
    DPSBinObjGeneric obj77;
    DPSBinObjGeneric obj78;
    DPSBinObjGeneric obj79;
    DPSBinObjGeneric obj80;
    DPSBinObjGeneric obj81;
    DPSBinObjGeneric obj82;
    DPSBinObjGeneric obj83;
    DPSBinObjGeneric obj84;
    DPSBinObjGeneric obj85;
    DPSBinObjGeneric obj86;
    DPSBinObjGeneric obj87;
    DPSBinObjGeneric obj88;
    DPSBinObjGeneric obj89;
    DPSBinObjGeneric obj90;
    DPSBinObjGeneric obj91;
    DPSBinObjGeneric obj92;
    DPSBinObjGeneric obj93;
    DPSBinObjGeneric obj94;
    DPSBinObjGeneric obj95;
    DPSBinObjGeneric obj96;
    DPSBinObjGeneric obj97;
    DPSBinObjGeneric obj98;
    DPSBinObjGeneric obj99;
    DPSBinObjGeneric obj100;
    DPSBinObjGeneric obj101;
    DPSBinObjGeneric obj102;
    DPSBinObjGeneric obj103;
    DPSBinObjGeneric obj104;
    DPSBinObjGeneric obj105;
    DPSBinObjGeneric obj106;
    DPSBinObjGeneric obj107;
    DPSBinObjGeneric obj108;
    DPSBinObjGeneric obj109;
    DPSBinObjGeneric obj110;
    DPSBinObjGeneric obj111;
    DPSBinObjGeneric obj112;
    DPSBinObjGeneric obj113;
    DPSBinObjGeneric obj114;
    DPSBinObjGeneric obj115;
    DPSBinObjGeneric obj116;
    DPSBinObjGeneric obj117;
    DPSBinObjGeneric obj118;
    DPSBinObjGeneric obj119;
    DPSBinObjGeneric obj120;
    DPSBinObjGeneric obj121;
    DPSBinObjGeneric obj122;
    DPSBinObjGeneric obj123;
    DPSBinObjGeneric obj124;
    DPSBinObjGeneric obj125;
    DPSBinObjGeneric obj126;
    DPSBinObjGeneric obj127;
    DPSBinObjGeneric obj128;
    DPSBinObjGeneric obj129;
    DPSBinObjGeneric obj130;
    DPSBinObjGeneric obj131;
    DPSBinObjGeneric obj132;
    DPSBinObjGeneric obj133;
    DPSBinObjGeneric obj134;
    DPSBinObjGeneric obj135;
    DPSBinObjGeneric obj136;
    DPSBinObjGeneric obj137;
    char obj138[13];
    char obj139[5];
    char obj140[11];
    char obj141[5];
    char obj142[13];
    char obj143[4];
    char obj144[12];
    char obj145[9];
    char obj146[4];
    char obj147[17];
    char obj148[13];
    char obj149[5];
    char obj150[6];
    char obj151[5];
    } _dpsQ;
  static const _dpsQ _dpsStat = {
    DPS_DEF_TOKENTYPE, 123, 1230,
    {DPS_LITERAL|DPS_NAME, 0, DPSSYSNAME, 161},	/* showpage */
    {DPS_EXEC|DPS_ARRAY, 0, 1, 1096},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 14},	/* bind */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 51},	/* def */
    {DPS_LITERAL|DPS_NAME, 0, 5, 1210},	/* WDICT */
    {DPS_LITERAL|DPS_BOOL, 0, 0, 1},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 158},	/* setshared */
    {DPS_LITERAL|DPS_INT, 0, 0, 1},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 53},	/* dict */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 56},	/* dup */
    {DPS_LITERAL|DPS_NAME, 0, 5, 1221},	/* level */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 120},	/* put */
    {DPS_LITERAL|DPS_BOOL, 0, 0, 0},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 158},	/* setshared */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 51},	/* def */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: x */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: y */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_LITERAL|DPS_INT, 0, 0, 2},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 1},	/* add */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_LITERAL|DPS_INT, 0, 0, 24},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 1},	/* add */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_EXEC|DPS_NAME, 0, 6, 1215},	/* window */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 56},	/* dup */
    {DPS_EXEC|DPS_NAME, 0, 5, 1210},	/* WDICT */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 62},	/* exch */
    {DPS_EXEC|DPS_NAME, 0, 13, 1197},	/* setwindowdict */
    {DPS_EXEC|DPS_NAME, 0, 17, 1180},	/* windowdeviceround */
    {DPS_LITERAL|DPS_BOOL, 0, 0, 0},	/* param: show */
    {DPS_EXEC|DPS_ARRAY, 0, 7, 984},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 84},	/* if */
    {DPS_LITERAL|DPS_NAME, 0, 4, 1176},	/* mwin */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 79},	/* gstate */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 34},	/* currentgstate */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 51},	/* def */
    {DPS_EXEC|DPS_NAME, 0, 9, 1167},	/* Allevents */
    {DPS_EXEC|DPS_NAME, 0, 13, 1104},	/* currentwindow */
    {DPS_EXEC|DPS_NAME, 0, 12, 1155},	/* seteventmask */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 150},	/* setgray */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_LITERAL|DPS_INT, 0, 0, 2},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 1},	/* add */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_LITERAL|DPS_INT, 0, 0, 2},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 1},	/* add */
    {DPS_LITERAL|DPS_INT, 0, 0, 22},
    {DPS_EXEC|DPS_NAME, 0, 4, 1151},	/* Copy */
    {DPS_EXEC|DPS_NAME, 0, 13, 1138},	/* compositerect */
    {DPS_LITERAL|DPS_INT, 0, 0, 2},
    {DPS_LITERAL|DPS_INT, 0, 0, 3},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 54},	/* div */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 150},	/* setgray */
    {DPS_LITERAL|DPS_INT, 0, 0, 1},
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_LITERAL|DPS_INT, 0, 0, 3},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 1},	/* add */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 107},	/* moveto */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 20},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 133},	/* rlineto */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_LITERAL|DPS_INT, 0, 0, 1},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 169},	/* sub */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 133},	/* rlineto */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 167},	/* stroke */
    {DPS_LITERAL|DPS_INT, 0, 0, 1},
    {DPS_LITERAL|DPS_INT, 0, 0, 3},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 54},	/* div */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 150},	/* setgray */
    {DPS_LITERAL|DPS_INT, 0, 0, 2},
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_LITERAL|DPS_INT, 0, 0, 3},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 1},	/* add */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 107},	/* moveto */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_LITERAL|DPS_INT, 0, 0, 2},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 169},	/* sub */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 133},	/* rlineto */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 19},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 133},	/* rlineto */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 167},	/* stroke */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 150},	/* setgray */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 1},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 107},	/* moveto */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_LITERAL|DPS_INT, 0, 0, 2},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 1},	/* add */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 99},	/* lineto */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_LITERAL|DPS_INT, 0, 0, 1},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 1},	/* add */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_LITERAL|DPS_INT, 0, 0, 2},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 1},	/* add */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 99},	/* lineto */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_LITERAL|DPS_INT, 0, 0, 1},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 1},	/* add */
    {DPS_LITERAL|DPS_INT, 0, 0, 1},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 99},	/* lineto */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 22},	/* closepath */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 167},	/* stroke */
    {DPS_LITERAL|DPS_INT, 0, 0, 1},
    {DPS_LITERAL|DPS_INT, 0, 0, 1},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 173},	/* translate */
    {DPS_EXEC|DPS_NAME, 0, 13, 1104},	/* currentwindow */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 119},	/* printobject */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 1},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 119},	/* printobject */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 70},	/* flush */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: lwin */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 61},	/* eq */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 112},	/* not */
    {DPS_EXEC|DPS_ARRAY, 0, 3, 1072},
    {DPS_EXEC|DPS_ARRAY, 0, 4, 1040},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 85},	/* ifelse */
    {DPS_EXEC|DPS_NAME, 0, 5, 1133},	/* Above */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_EXEC|DPS_NAME, 0, 13, 1104},	/* currentwindow */
    {DPS_EXEC|DPS_NAME, 0, 11, 1122},	/* orderwindow */
    {DPS_EXEC|DPS_NAME, 0, 5, 1117},	/* Below */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: lwin */
    {DPS_EXEC|DPS_NAME, 0, 13, 1104},	/* currentwindow */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 309},	/* initgraphics */
    {'c','u','r','r','e','n','t','w','i','n','d','o','w'},
    {'B','e','l','o','w'},
    {'o','r','d','e','r','w','i','n','d','o','w'},
    {'A','b','o','v','e'},
    {'c','o','m','p','o','s','i','t','e','r','e','c','t'},
    {'C','o','p','y'},
    {'s','e','t','e','v','e','n','t','m','a','s','k'},
    {'A','l','l','e','v','e','n','t','s'},
    {'m','w','i','n'},
    {'w','i','n','d','o','w','d','e','v','i','c','e','r','o','u','n','d'},
    {'s','e','t','w','i','n','d','o','w','d','i','c','t'},
    {'W','D','I','C','T'},
    {'w','i','n','d','o','w'},
    {'l','e','v','e','l'},
    }; /* _dpsQ */
  _dpsQ _dpsF;	/* local copy  */
  register DPSContext _dpsCurCtxt = DPSPrivCurrentContext();
  char pad[3];
  register DPSBinObjRec *_dpsP = (DPSBinObjRec *)&_dpsF.obj0;
  DPSResultsRec _dpsR[1];
  static const DPSResultsRec _dpsRstat[] = {
    { dps_tInt, -1 },
    };
    _dpsR[0] = _dpsRstat[0];
    _dpsR[0].value = (char *)windownum;

  _dpsF = _dpsStat;	/* assign automatic variable */

  _dpsP[16].val.integerVal = x;
  _dpsP[17].val.integerVal = y;
  _dpsP[18].val.integerVal =
  _dpsP[47].val.integerVal =
  _dpsP[65].val.integerVal =
  _dpsP[80].val.integerVal =
  _dpsP[99].val.integerVal =
  _dpsP[106].val.integerVal = width;
  _dpsP[21].val.integerVal =
  _dpsP[44].val.integerVal =
  _dpsP[58].val.integerVal =
  _dpsP[76].val.integerVal =
  _dpsP[95].val.integerVal =
  _dpsP[102].val.integerVal = height;
  _dpsP[123].val.integerVal =
  _dpsP[135].val.integerVal = lwin;
  _dpsP[31].val.booleanVal = (long) (0 != show);
  DPSSetResultTable(_dpsCurCtxt, _dpsR, 1);
  DPSBinObjSeqWrite(_dpsCurCtxt,(char *) &_dpsF,1230);
  DPSAwaitReturnValues(_dpsCurCtxt);
  if (0) *pad = 0;    /* quiets compiler warnings */
}
#line 42 "devNXWin.psw"

#line 461 "devNXWin.c"
void compositeImageMono(int width, int height)
{
  typedef struct {
    unsigned char tokenType;
    unsigned char topLevelCount;
    unsigned short nBytes;

    DPSBinObjGeneric obj0;
    DPSBinObjGeneric obj1;
    DPSBinObjGeneric obj2;
    DPSBinObjGeneric obj3;
    DPSBinObjGeneric obj4;
    DPSBinObjGeneric obj5;
    DPSBinObjGeneric obj6;
    DPSBinObjGeneric obj7;
    DPSBinObjGeneric obj8;
    DPSBinObjGeneric obj9;
    DPSBinObjGeneric obj10;
    DPSBinObjGeneric obj11;
    DPSBinObjGeneric obj12;
    DPSBinObjGeneric obj13;
    DPSBinObjGeneric obj14;
    DPSBinObjGeneric obj15;
    DPSBinObjGeneric obj16;
    DPSBinObjGeneric obj17;
    DPSBinObjGeneric obj18;
    DPSBinObjGeneric obj19;
    DPSBinObjGeneric obj20;
    DPSBinObjGeneric obj21;
    DPSBinObjGeneric obj22;
    DPSBinObjGeneric obj23;
    DPSBinObjGeneric obj24;
    DPSBinObjGeneric obj25;
    DPSBinObjGeneric obj26;
    DPSBinObjGeneric obj27;
    DPSBinObjGeneric obj28;
    char obj29[6];
    char obj30[6];
    } _dpsQ;
  static const _dpsQ _dpsStat = {
    DPS_DEF_TOKENTYPE, 11, 248,
    {DPS_LITERAL|DPS_NAME, 0, 6, 232},	/* picstr */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 165},	/* string */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 51},	/* def */
    {DPS_LITERAL|DPS_NAME, 0, 6, 238},	/* screen */
    {DPS_EXEC|DPS_ARRAY, 0, 14, 88},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 51},	/* def */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 139},	/* scale */
    {DPS_EXEC|DPS_NAME, 0, 6, 238},	/* screen */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_LITERAL|DPS_INT, 0, 0, 8},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 260},	/* [ */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 110},	/* neg */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 261},	/* ] */
    {DPS_EXEC|DPS_ARRAY, 0, 4, 200},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 86},	/* image */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 31},	/* currentfile */
    {DPS_EXEC|DPS_NAME, 0, 6, 232},	/* picstr */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 126},	/* readstring */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 117},	/* pop */
    {'p','i','c','s','t','r'},
    {'s','c','r','e','e','n'},
    }; /* _dpsQ */
  _dpsQ _dpsF;	/* local copy  */
  register DPSContext _dpsCurCtxt = DPSPrivCurrentContext();
  char pad[3];
  register DPSBinObjRec *_dpsP = (DPSBinObjRec *)&_dpsF.obj0;
  _dpsF = _dpsStat;	/* assign automatic variable */

  _dpsP[1].val.integerVal =
  _dpsP[11].val.integerVal =
  _dpsP[15].val.integerVal =
  _dpsP[7].val.integerVal = width;
  _dpsP[12].val.integerVal =
  _dpsP[18].val.integerVal =
  _dpsP[21].val.integerVal =
  _dpsP[8].val.integerVal = height;
  DPSBinObjSeqWrite(_dpsCurCtxt,(char *) &_dpsF,248);
  if (0) *pad = 0;    /* quiets compiler warnings */
}
#line 52 "devNXWin.psw"

#line 554 "devNXWin.c"
void compositeImageColor(int width, int height)
{
  typedef struct {
    unsigned char tokenType;
    unsigned char topLevelCount;
    unsigned short nBytes;

    DPSBinObjGeneric obj0;
    DPSBinObjGeneric obj1;
    DPSBinObjGeneric obj2;
    DPSBinObjGeneric obj3;
    DPSBinObjGeneric obj4;
    DPSBinObjGeneric obj5;
    DPSBinObjGeneric obj6;
    DPSBinObjGeneric obj7;
    DPSBinObjGeneric obj8;
    DPSBinObjGeneric obj9;
    DPSBinObjGeneric obj10;
    DPSBinObjGeneric obj11;
    DPSBinObjGeneric obj12;
    DPSBinObjGeneric obj13;
    DPSBinObjGeneric obj14;
    DPSBinObjGeneric obj15;
    DPSBinObjGeneric obj16;
    DPSBinObjGeneric obj17;
    DPSBinObjGeneric obj18;
    DPSBinObjGeneric obj19;
    DPSBinObjGeneric obj20;
    DPSBinObjGeneric obj21;
    DPSBinObjGeneric obj22;
    DPSBinObjGeneric obj23;
    DPSBinObjGeneric obj24;
    DPSBinObjGeneric obj25;
    DPSBinObjGeneric obj26;
    DPSBinObjGeneric obj27;
    DPSBinObjGeneric obj28;
    DPSBinObjGeneric obj29;
    DPSBinObjGeneric obj30;
    char obj31[6];
    char obj32[6];
    } _dpsQ;
  static const _dpsQ _dpsStat = {
    DPS_DEF_TOKENTYPE, 11, 264,
    {DPS_LITERAL|DPS_NAME, 0, 6, 248},	/* picstr */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 165},	/* string */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 51},	/* def */
    {DPS_LITERAL|DPS_NAME, 0, 6, 254},	/* screen */
    {DPS_EXEC|DPS_ARRAY, 0, 16, 88},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 51},	/* def */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 139},	/* scale */
    {DPS_EXEC|DPS_NAME, 0, 6, 254},	/* screen */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_LITERAL|DPS_INT, 0, 0, 8},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 260},	/* [ */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: width */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 110},	/* neg */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: height */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 261},	/* ] */
    {DPS_EXEC|DPS_ARRAY, 0, 4, 216},
    {DPS_LITERAL|DPS_BOOL, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 3},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 267},	/* colorimage */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 31},	/* currentfile */
    {DPS_EXEC|DPS_NAME, 0, 6, 248},	/* picstr */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 126},	/* readstring */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 117},	/* pop */
    {'p','i','c','s','t','r'},
    {'s','c','r','e','e','n'},
    }; /* _dpsQ */
  _dpsQ _dpsF;	/* local copy  */
  register DPSContext _dpsCurCtxt = DPSPrivCurrentContext();
  char pad[3];
  register DPSBinObjRec *_dpsP = (DPSBinObjRec *)&_dpsF.obj0;
  _dpsF = _dpsStat;	/* assign automatic variable */

  _dpsP[1].val.integerVal =
  _dpsP[11].val.integerVal =
  _dpsP[15].val.integerVal =
  _dpsP[7].val.integerVal = width;
  _dpsP[12].val.integerVal =
  _dpsP[18].val.integerVal =
  _dpsP[21].val.integerVal =
  _dpsP[8].val.integerVal = height;
  DPSBinObjSeqWrite(_dpsCurCtxt,(char *) &_dpsF,264);
  if (0) *pad = 0;    /* quiets compiler warnings */
}
#line 62 "devNXWin.psw"

#line 651 "devNXWin.c"
void showWindow(int win1, int win2)
{
  typedef struct {
    unsigned char tokenType;
    unsigned char topLevelCount;
    unsigned short nBytes;

    DPSBinObjGeneric obj0;
    DPSBinObjGeneric obj1;
    DPSBinObjGeneric obj2;
    DPSBinObjGeneric obj3;
    DPSBinObjGeneric obj4;
    DPSBinObjGeneric obj5;
    DPSBinObjGeneric obj6;
    DPSBinObjGeneric obj7;
    DPSBinObjGeneric obj8;
    DPSBinObjGeneric obj9;
    DPSBinObjGeneric obj10;
    DPSBinObjGeneric obj11;
    DPSBinObjGeneric obj12;
    DPSBinObjGeneric obj13;
    DPSBinObjGeneric obj14;
    char obj15[11];
    char obj16[5];
    char obj17[5];
    } _dpsQ;
  static const _dpsQ _dpsStat = {
    DPS_DEF_TOKENTYPE, 7, 145,
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: win2 */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 61},	/* eq */
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 112},	/* not */
    {DPS_EXEC|DPS_ARRAY, 0, 4, 88},
    {DPS_EXEC|DPS_ARRAY, 0, 4, 56},
    {DPS_EXEC|DPS_NAME, 0, DPSSYSNAME, 85},	/* ifelse */
    {DPS_EXEC|DPS_NAME, 0, 5, 136},	/* Above */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: win1 */
    {DPS_EXEC|DPS_NAME, 0, 11, 120},	/* orderwindow */
    {DPS_EXEC|DPS_NAME, 0, 5, 131},	/* Below */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: win2 */
    {DPS_LITERAL|DPS_INT, 0, 0, 0},	/* param: win1 */
    {DPS_EXEC|DPS_NAME, 0, 11, 120},	/* orderwindow */
    {'o','r','d','e','r','w','i','n','d','o','w'},
    {'B','e','l','o','w'},
    {'A','b','o','v','e'},
    }; /* _dpsQ */
  _dpsQ _dpsF;	/* local copy  */
  register DPSContext _dpsCurCtxt = DPSPrivCurrentContext();
  char pad[3];
  register DPSBinObjRec *_dpsP = (DPSBinObjRec *)&_dpsF.obj0;
  _dpsF = _dpsStat;	/* assign automatic variable */

  _dpsP[13].val.integerVal =
  _dpsP[9].val.integerVal = win1;
  _dpsP[0].val.integerVal =
  _dpsP[12].val.integerVal = win2;
  DPSBinObjSeqWrite(_dpsCurCtxt,(char *) &_dpsF,145);
  if (0) *pad = 0;    /* quiets compiler warnings */
}
#line 70 "devNXWin.psw"

