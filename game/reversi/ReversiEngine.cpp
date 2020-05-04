//
// Created by seventh on 2020/4/3.
//

#include <fstream>
#include "ReversiEngine.h"

inline bool checkPoint(const unsigned &x, const unsigned &y) {
    return x < ROW && y < COL;
}

inline uint8_t nextPlayer(uint8_t player) {
    return 3 - player;
}

constexpr unsigned origin[64][2] =
        {0, 0, 0, 7, 7, 0, 7, 7, 0, 2, 0, 5, 2, 0, 2, 7, 5, 0, 5, 7, 7, 2, 7, 5, 0, 3, 0, 4, 3, 0, 3, 7, 4, 0, 4, 7,
         7, 3, 7, 4, 2, 2, 2, 3, 2, 4, 2, 5, 3, 2, 3, 5, 4, 2, 4, 5, 5, 2, 5, 3, 5, 4, 5, 5, 1, 3, 1, 4, 3, 1, 3, 6,
         4, 1, 4, 6, 6, 3, 6, 4, 0, 1, 0, 6, 1, 0, 1, 7, 3, 3, 3, 4, 4, 3, 4, 4, 6, 0, 6, 7, 7, 1, 7, 6, 1, 2, 1, 5,
         2, 1, 2, 6, 5, 1, 5, 6, 6, 2, 6, 5, 1, 1, 1, 6, 6, 1, 6, 6};
constexpr int go[8][2] = {1, 0, 0, 1, -1, 0, 0, -1, 1, 1, 1, -1, -1, 1, -1, -1};
constexpr uint32_t pow3[17] =
        {1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147, 531441, 1594323, 4782969, 14348907, 43046721};
constexpr int trans[] = {1, 0, 2};

void AlphaBetaReversiStatus::set(unsigned x, unsigned y, const unsigned &player) {
    assert(player < 3);
    updateHash(x, y, -trans[mMap[x][y]]);
    updateHash(x, y, trans[player]);
    mMap[x][y] = player;
}
void AlphaBetaReversiStatus::set(AlphaBetaReversiOpt opt) {
    set(opt.x, opt.y, opt.player);
    for (auto &k:go) {
        unsigned x = opt.x + k[0], y = opt.y + k[1];
        while (checkPoint(x, y) && mMap[x][y] == 3 - opt.player) {
            x = x + k[0];
            y = y + k[1];
        }
        if (checkPoint(x, y) && mMap[x][y] == opt.player) {
            x = x - k[0];
            y = y - k[1];
            while (x != opt.x || y != opt.y) {
                set(x, y, opt.player);
                x = x - k[0];
                y = y - k[1];
            }
        }
    }
    setLastPlayer(opt.player);
}

void AlphaBetaReversiStatus::updateHash(unsigned x, unsigned y, int value) {
    if (!value)return;
    uint32_t &hash = mhash[x / 4][y / 4];
    if (x >= 4) x = 7 - x;
    if (y >= 4) y = 7 - y;
    hash += value * pow3[x * 4 + y];
}

int AlphaBetaReversiInput::getMinScore() {
    return -INF;
}
int AlphaBetaReversiInput::getMaxScore() {
    return INF;
}
void AlphaBetaReversiInput::newStatus(const Status &nowStatus,
                                      const Operate &opt,
                                      Status &newStatus) {
    newStatus = nowStatus;
    if (!checkPoint(opt.x, opt.y))
        newStatus.setLastPlayer(opt.player);
    newStatus.set(opt);
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
                        allOpts.emplace_back(Operate{i, j, player});
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

void MCTSReversiStatus::setOperate(const MCTSReversiOperate &opt) {
    if (!checkPoint(opt.x, opt.y)) {
        setNextPlayer(nextPlayer(mNextPlayer));
        mLastSkip = true;
        return;
    }
    set(opt.x, opt.y, mNextPlayer);
    for (auto &k:go) {
        unsigned x = opt.x + k[0], y = opt.y + k[1];
        while (checkPoint(x, y) && mTable[x][y] == nextPlayer(mNextPlayer)) {
            x = x + k[0];
            y = y + k[1];
        }
        if (checkPoint(x, y) && mTable[x][y] == mNextPlayer) {
            x = x - k[0];
            y = y - k[1];
            while (x != opt.x || y != opt.y) {
                set(x, y, mNextPlayer);
                x = x - k[0];
                y = y - k[1];
            }
        }
    }
    setNextPlayer(nextPlayer(mNextPlayer));
    mLastSkip = false;
}

bool MCTSReversiInput::getAllOpt(const Status &nowStatus,
                                 std::vector<Operate> &allOpts) {
    const MCTSReversiPlayer player = nowStatus.getNextPlayer();
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
                        allOpts.emplace_back(Operate{static_cast<uint8_t>(i), static_cast<uint8_t>(j)});
                        break;
                    }
                }
            }
        }
    }
    if (allOpts.empty()) {
        if (notEnd && nowStatus.lastIsSkip())
            return false;
        allOpts.emplace_back(Operate{8, 8});
    }
    return notEnd;
}

template<int N>
struct Factorial {
    enum { value = N * Factorial<N - 1>::value };
};
template<>
struct Factorial<0> {
    enum { value = 1 };
};
template<int N>
void decreaseCantorExpansion(int id, ::std::vector<int> &vec, ::std::vector<bool> &vis) {
    int tmp = id / Factorial<N>::value;
    for (int i = 0;; i++) {
        if (vis[i] == false) {
            tmp--;
            if (tmp < 0) {
                vec[N] = i;
                vis[i] = true;
                break;
            }
        }
    }
    if constexpr (N != 0)
        decreaseCantorExpansion<N - 1>(id % Factorial<N>::value, vec, vis);
}

template<int N>
void decreaseCantorExpansionHelper(int id, ::std::vector<int> &vec) {
    vec.resize(N);
    ::std::vector<bool> vis(N);
    decreaseCantorExpansion<N - 1>(id, vec, vis);
}

bool MCTSReversiInput::quickGetOpt(const Status &nowStatus, Operate &opt) {
    static ::std::mt19937 randomEngine(::std::random_device{}());//random
    ::std::uniform_int_distribution<size_t> range(0, Factorial<8>::value);
    int cantorExpansionRowId = range(randomEngine);
    int cantorExpansionColId = range(randomEngine);
    ::std::vector<int> row, col;
    decreaseCantorExpansionHelper<8>(cantorExpansionRowId, row);
    decreaseCantorExpansionHelper<8>(cantorExpansionColId, col);
    const MCTSReversiPlayer player = nowStatus.getNextPlayer();
    bool notEnd = false;
    for (uint8_t i:row) {
        for (uint8_t j:col) {
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
                            opt = Operate{i, j};
                            return true;
                        }
                    }
                }
            }
        }
    }
    if (notEnd && nowStatus.lastIsSkip())
        return false;
    opt = Operate{8, 8};
    return notEnd;
}

void MCTSReversiInput::newStatus(const Status &nowStatus,
                                 const Operate &opt,
                                 Status &newStatus) {
    newStatus = nowStatus;
    newStatus.setOperate(opt);
}

algorithm::mcts::StatusResult MCTSReversiInput::getEndResult(const MCTSReversiStatus &nowStatus,
                                                             const Player &selfPlayer) {
    int res = 0;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (nowStatus[i][j] == selfPlayer)
                res++;
            else if (nowStatus[i][j] == nextPlayer(selfPlayer))
                res--;
        }
    }
    if (res == 0)return algorithm::mcts::StatusResult::DRAW;
    else if (res > 0)return algorithm::mcts::StatusResult::WIN;
    else return algorithm::mcts::StatusResult::LOSE;
}

MCTSReversiInput::Player MCTSReversiInput::getNextPlayer(const Status &nowStatus) {
    return nowStatus.getNextPlayer();
}