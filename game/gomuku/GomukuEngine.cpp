//
// Created by seventh on 2020/4/29.
//

#include "GomukuEngine.h"
#include <queue>
#include <algorithm>

void AlphaBetaGomukuStatus::set(unsigned x, unsigned y, const unsigned &player) {
    if (x >= GomukuROW || y >= GomukuCOL)return;
    mMap[x][y] = player;
}

void AlphaBetaGomukuStatus::set(AlphaBetaGomukuOpt opt) {
    setLastPlayer(getNextPlayer());
    set(opt.x, opt.y, lastPlayer);
}

inline int nextPlayer(int player) {
    return 3 - player;
}

AlphaBetaGomukuTraits::Player AlphaBetaGomukuInput::getNextPlayer(const AlphaBetaGomukuStatus &nowStatus) {
    return nowStatus.getNextPlayer();
}

bool AlphaBetaGomukuInput::getAllOpt(const AlphaBetaGomukuStatus &nowStatus, std::vector<Operate> &allOpts) {
    static ScoreTable win;
    static bool winInit = false;
    const Player player = nowStatus.getNextPlayer();
    if (!winInit) {
        win.init();
        win.insert("ooooo", 1);
        win.build();
        winInit = true;
    }
    if (win.find(nowStatus.mMap, player) || win.find(nowStatus.mMap, nextPlayer(player))) {
        return false;
    }
    allOpts.clear();
    static bool vis[GomukuROW][GomukuCOL];
    static int go[][2] = {0, 1, 1, 0, 0, -1, -1, 0, 1, 1, 1, -1, -1, 1, -1, -1};
    memset(vis, 0, sizeof(vis));
    std::queue<std::pair<int, int>> q;
    for (int i = 0; i < GomukuROW; i++)
        for (int j = 0; j < GomukuCOL; j++)
            if (nowStatus[i][j])
                q.push(std::make_pair(i, j)), vis[i][j] = 1;
    if (q.size() == 0) {
        Operate opt;
        opt.x = opt.y = 7;
        allOpts.push_back(opt);
        return allOpts.size() != 0;
    }
    for (int step = 1; step <= 3; step++) {
        int sz = q.size();
        while (sz--) {
            int i = q.front().first, j = q.front().second;
            q.pop();
            if (nowStatus[i][j] == 0) {
                Operate opt;
                opt.x = i;
                opt.y = j;
                Status newStatus;
                AlphaBetaGomukuInput::newStatus(nowStatus, opt, newStatus);
                opt.score = AlphaBetaGomukuInput::getScore(newStatus, player) - std::max(abs(7 - i), abs(7 - j));
                /*opt.player=nextPlayer(player);
                opt.score = std::max(opt.score,getOptScore(nowStatus,nextPlayer(player),opt))-std::max(abs(7-i),abs(7-j));*/
                //if(i==6&&j==6)printf("test//  %d\n",opt.score);
                allOpts.push_back(opt);
            }
            for (int k = 0; k < 8; k++) {
                int x = i + go[k][0], y = j + go[k][1];
                if (x < 0 || x >= GomukuROW || y < 0 || y >= GomukuCOL)continue;
                if (!vis[x][y]) {
                    q.push(std::make_pair(x, y));
                    vis[x][y] = 1;
                }
            }
        }
    }
    ::std::sort(allOpts.begin(), allOpts.end(), ::std::greater<Operate>());
    if (allOpts.size() > 12)
        allOpts.erase(allOpts.begin() + 12, allOpts.end());
    return allOpts.size() != 0;
}

int calGetScore(const AlphaBetaGomukuTraits::Status &nowStatus, int player, int nextplayer) {
    static ScoreTable intitate, defensive, win;
    static bool scoreInit = false;
    if (!scoreInit) {
        intitate.init();
        intitate.insert("oooo.", 10000000);//活4
        intitate.insert(".oooo", 10000000);
        intitate.insert("o.ooo", 10000000);
        intitate.insert("oo.oo", 10000000);
        intitate.insert("ooo.o", 10000000);

        intitate.insert("..ooo.", 80000);//活3
        intitate.insert(".ooo..", 80000);
        intitate.insert("..ooo..", -80000);
        intitate.insert(".o.oo.", 80000);
        intitate.insert(".oo.o.", 80000);

        intitate.insert("ooo..", 1000);//眠3
        intitate.insert("..ooo", 1000);
        intitate.insert(".o.oo", 1000);
        intitate.insert("oo.o.", 1000);
        intitate.insert("o.oo.", 1000);
        intitate.insert(".oo.o", 1000);

        intitate.insert(".oo...", 250);//活二
        intitate.insert("...oo.", 250);
        intitate.insert("..oo..", 250);
        intitate.insert("...oo..", -250);
        intitate.insert("..oo...", -250);
        intitate.insert(".o.o..", 250);
        intitate.insert("..o.o.", 250);
        intitate.insert("..o.o..", -250);

        intitate.build();

        defensive.init();

        defensive.insert(".oooo.", 400000);//活4
        defensive.insert("o.ooo.o", 400000);

        defensive.insert("..ooo.", 2500, 1);//活3
        defensive.insert(".ooo..", 2500, 1);
        defensive.insert("..ooo..", -2500, -1);
        defensive.insert(".o.oo.", 2250, 1);
        defensive.insert(".oo.o.", 2250, 1);

        defensive.insert("oo.oo", 5250, 1024);//眠4
        defensive.insert("oooo.", 5350, 1024);
        defensive.insert(".oooo", 5350, 1024);
        defensive.insert("ooo.o", 5250, 1024);
        defensive.insert("o.ooo", 5250, 1024);

        defensive.insert("..oo..", 80);//活2
        defensive.insert(".o.o.", 60);
        defensive.build();

        win.init();
        win.insert("ooooo", 1);
        win.build();
        scoreInit = true;
    }
    if (win.find(nowStatus.mMap, player))return 100000000;
    if (win.find(nowStatus.mMap, nextPlayer(player)))return -100000000;
    if (player == nextplayer)
        return intitate.find(nowStatus.mMap, player) - defensive.find(nowStatus.mMap, nextPlayer(player));
    else return defensive.find(nowStatus.mMap, player) - intitate.find(nowStatus.mMap, nextPlayer(player));
}

AlphaBetaGomukuTraits::Score AlphaBetaGomukuInput::getScore(const AlphaBetaGomukuStatus &nowStatus,
                                                            const unsigned char &selfPlayer,
                                                            bool isEnd) {
    return calGetScore(nowStatus, selfPlayer, nowStatus.getNextPlayer());
}

void AlphaBetaGomukuInput::newStatus(const AlphaBetaGomukuStatus &nowStatus,
                                     const AlphaBetaGomukuOpt &opt,
                                     AlphaBetaGomukuStatus &newStatus) {
    newStatus = nowStatus;
    newStatus.set(opt);
}

AlphaBetaGomukuTraits::Score AlphaBetaGomukuInput::getMaxScore() {
    return GomukuINF;
}

AlphaBetaGomukuTraits::Score AlphaBetaGomukuInput::getMinScore() {
    return -GomukuINF;
}

void AlphaBetaGomukuInput::setEnd(AlphaBetaGomukuOpt &opt) {
    opt.x = GomukuROW;
    opt.y = GomukuCOL;
}