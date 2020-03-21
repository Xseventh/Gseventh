//
// Created by seventh on 2020/2/17.
//
#include "reversi_alphabeta.h"
#include "../common/scanner.h"
#include <fstream>
#include <algorithm>

int AlphaBetaReversiInput::getMinScore() {
    return -INF;
}
int AlphaBetaReversiInput::getMaxScore() {
    return INF;
}
bool AlphaBetaReversiInput::newStatus(const Status &nowStatus,
                                      const Operate &opt,
                                      Status &newStatus) {
    newStatus = nowStatus;
    if (!checkPoint(opt.x, opt.y)) {
        newStatus.setLastPlayer(opt.player);
        return true;
    }
    newStatus.set(opt);
    return true;
}

bool AlphaBetaReversiInput::getAllOpt(const Status &nowStatus,
                                      std::vector<Operate> &allOpts) {
    const Player player = nowStatus.getNextPlayer();
    allOpts.clear();
    bool notEnd = false;
    for (auto &ori:origin) {
        const unsigned &i = ori[0], &j = ori[1];
        if (nowStatus[i][j] == 0) {
            notEnd = true;
            for (auto &k : go) {
                unsigned x = i + k[0], y = j + k[1];
                if (checkPoint(x, y) && nowStatus[x][y] == nextPlayer(player)) {
                    while (checkPoint(x, y) && nowStatus[x][y] == nextPlayer(player)) {
                        x = x + k[0];
                        y = y + k[1];
                    }
                    if (checkPoint(x, y) && nowStatus[x][y] == player) {
                        allOpts.emplace_back(Operate{i, j, player, sc[i][j]});
                        break;
                    }
                }
            }
        }
    }
    if (allOpts.empty()) {
        if (notEnd) {
            notEnd = false;
            for (auto &ori : origin) {
                const unsigned &i = ori[0], &j = ori[1];
                if (!notEnd && nowStatus[i][j] == 0) {
                    for (auto &k : go) {
                        unsigned x = i + k[0], y = j + k[1];
                        if (checkPoint(x, y) && nowStatus[x][y] == player) {
                            while (checkPoint(x, y) && nowStatus[x][y] == player) {
                                x = x + k[0];
                                y = y + k[1];
                            }
                            if (checkPoint(x, y) && nowStatus[x][y] == nextPlayer(player)) {
                                notEnd = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
        allOpts.emplace_back(Operate{8, 8, player});
    }
    nowStatus.optNum[player] = allOpts.size();
    return notEnd;
}
int16_t scoreTable[43046721];
int AlphaBetaReversiInput::getScore(const Status &nowStatus,
                                    const Player &selfPlayer,
                                    bool isEnd) {
    static bool init = false;
    if (!init) {
        std::ifstream input("scoreTableObject", std::ios::binary);
        input.read(reinterpret_cast<char *>(scoreTable), 43046721 * 2);
        init = true;
    }
    if (isEnd) {
        int res = 0;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (nowStatus[i][j] == selfPlayer)
                    res++;
                else if (nowStatus[i][j] == nextPlayer(selfPlayer))
                    res--;
            }
        }
        if (res > 0)
            return getMaxScore();
        else
            return getMinScore() + 64 + res;
    }
    if (selfPlayer == 1)
        return scoreTable[nowStatus.mhash[0][0]] + scoreTable[nowStatus.mhash[0][1]]
                + scoreTable[nowStatus.mhash[1][0]] + scoreTable[nowStatus.mhash[1][1]]
                + (nowStatus.optNum[selfPlayer] - nowStatus.optNum[nextPlayer(selfPlayer)]) * 3;
    else
        return -(scoreTable[nowStatus.mhash[0][0]] + scoreTable[nowStatus.mhash[0][1]]
                + scoreTable[nowStatus.mhash[1][0]] + scoreTable[nowStatus.mhash[1][1]])
                + (nowStatus.optNum[selfPlayer] - nowStatus.optNum[nextPlayer(selfPlayer)]) * 3;
}
void AlphaBetaReversiInput::setEnd(Operate &opt) {
    opt.x = opt.y = 8;
}
AlphaBetaReversiInput::Player AlphaBetaReversiInput::getNextPlayer(const Status &nowStatus) {
    return nowStatus.getNextPlayer();
}
