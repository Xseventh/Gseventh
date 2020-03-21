//
// Created by seventh on 2020/2/17.
//

#ifndef GSEVENTH_GAME_REVERSI_ALPHABETA_H_
#define GSEVENTH_GAME_REVERSI_ALPHABETA_H_

#include "../algorithm/max-min/alpha-beta.h"
#include <iostream>
#include <random>

const int COL = 8, ROW = 8;
const int INF = 10000;

inline uint8_t nextPlayer(uint8_t player) {
    return 3 - player;
}

inline bool checkPoint(const unsigned &x, const unsigned &y) {
    return x < ROW && y < COL;
}

struct AlphaBetaReversiOpt {
    unsigned x, y, player;
    int score;
    bool operator<(const AlphaBetaReversiOpt &oth) const {
        return score < oth.score;
    }
};

constexpr int go[8][2] = {1, 0, 0, 1, -1, 0, 0, -1, 1, 1, 1, -1, -1, 1, -1, -1};
constexpr unsigned origin[64][2] = {0,0,0,7,7,0,7,7,0,2,0,5,2,0,2,7,5,0,5,7,7,2,7,5,0,3,0,4,3,0,3,7,4,0,4,7,7,3,7,4,2,2,2,3,2,4,2,5,3,2,3,5,4,2,4,5,5,2,5,3,5,4,5,5,1,3,1,4,3,1,3,6,4,1,4,6,6,3,6,4,0,1,0,6,1,0,1,7,3,3,3,4,4,3,4,4,6,0,6,7,7,1,7,6,1,2,1,5,2,1,2,6,5,1,5,6,6,2,6,5,1,1,1,6,6,1,6,6};
constexpr uint32_t pow3[17] =
        {1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147, 531441, 1594323, 4782969, 14348907, 43046721};

constexpr int trans[] = {1, 0, 2};
class AlphaBetaReversiStatus {
  private:
    uint8_t mMap[ROW][COL]{};
    uint8_t lastPlayer{};
  public:
    mutable uint8_t optNum[3]{};
    uint32_t mhash[2][2]{21523360, 21523360, 21523360, 21523360};
    inline const uint8_t *operator[](unsigned x) const {
        return mMap[x];
    }
    void updateHash(unsigned x, unsigned y, int value) {
        if (!value)return;
        uint32_t &hash = mhash[x / 4][y / 4];
        if (x >= 4) x = 7 - x;
        if (y >= 4) y = 7 - y;
        hash += value * pow3[x * 4 + y];
    }
    void set(unsigned x, unsigned y, const unsigned &player) {
        assert(player < 3);
        updateHash(x, y, -trans[mMap[x][y]]);
        updateHash(x, y, trans[player]);
        mMap[x][y] = player;
    }
    void set(AlphaBetaReversiOpt opt) {
        set(opt.x, opt.y, opt.player);
        for (auto &k:go) {
            unsigned x = opt.x + k[0], y = opt.y + k[1];
            while (checkPoint(x, y) && mMap[x][y] == nextPlayer(opt.player)) {
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
    void setLastPlayer(const unsigned &player) {
        lastPlayer = player;
    }
    uint8_t getNextPlayer() const {
        return 3 - lastPlayer;
    }
    void print() const {
        std::cout << "nextPlayer : " << static_cast<int>(getNextPlayer()) << std::endl;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                std::cout << static_cast<int>(mMap[i][j]) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};

class ReversiTraits {
  public:
    using Status = AlphaBetaReversiStatus;
    using Operate = AlphaBetaReversiOpt;
    using Score = int;
    using Player = uint8_t;
    static const int deep = 21;
    static const int timelimit = 970;
};

constexpr static const int sc[ROW][COL] = {
        20, -3, 11, 8, 8, 11, -3, 20,
        -3, -7, -4, 1, 1, -4, -7, -3,
        11, -4, 2, 2, 2, 2, -4, 11,
        8, 1, 2, -3, -3, 2, 1, 8,
        8, 1, 2, -3, -3, 2, 1, 8,
        11, -4, 2, 2, 2, 2, -4, 11,
        -3, -7, -4, 1, 1, -4, -7, -3,
        20, -3, 11, 8, 8, 11, -3, 20
};
class AlphaBetaReversiInput : public AlphaBetaInput<ReversiTraits> {
  public:
    Score getMaxScore() override;
    Score getMinScore() override;
    Score getScore(const Status &nowStatus,
                   const Player &selfPlayer,
                   bool isEnd = false) override; //返回当前局面得分
    bool getAllOpt(const Status &nowStatus,
                   std::vector<Operate> &allOpts) override; // 处于结束状态返回true
    bool newStatus(const Status &nowStatus,
                   const Operate &opt,
                   Status &newStatus) override; // 成功返回true
    void setEnd(Operate &opt) override;
    Player getNextPlayer(const Status &nowStatus) override;

    int getScore2(const Status &nowStatus,
                  const Player &selfPlayer) {
        int res = 0;
        bool isEnd = true;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (nowStatus[i][j] == selfPlayer) {
                    if (nowStatus[i / 4 * 7][j / 4 * 7] && sc[i][j] != 1 &&
                            sc[i][j] != 5 && (i % 7 != 0 || j % 7 != 0)) {
                        if (abs(i - i / 4 * 7) <= 1 && abs(j - j / 4 * 7) <= 1)
                            res += 15;
                        else res += 5;
                    } else {
                        res += sc[i][j];
                    }
                } else if (nowStatus[i][j] == nextPlayer(selfPlayer)) {
                    if (nowStatus[i / 4 * 7][j / 4 * 7] && sc[i][j] != 1 &&
                            sc[i][j] != 5 && (i % 7 != 0 || j % 7 != 0)) {
                        if (abs(i - i / 4 * 7) <= 1 && abs(j - j / 4 * 7) <= 1)
                            res -= 15;
                        else res -= 5;
                    } else {
                        res -= sc[i][j];
                    }
                } else isEnd = false;
            }
        }
        return res;
    }
};

typedef AlphaBetaEngine<AlphaBetaReversiInput> AlphaBetaReversiEngine;
typedef Singleton<AlphaBetaReversiInput> SingletonAlphaBetaReversiInput;

#endif //GSEVENTH_GAME_REVERSI_ALPHABETA_H_
