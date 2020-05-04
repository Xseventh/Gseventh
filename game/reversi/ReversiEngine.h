//
// Created by seventh on 2020/4/3.
//

#ifndef GSEVENTH_GAME_REVERSI_REVERSIENGINE_H_
#define GSEVENTH_GAME_REVERSI_REVERSIENGINE_H_

#include <iostream>
#include <random>
#include "algorithm/mcts/MCTSEngine.h"
#include "algorithm/max-min/AlphaBetaEngine.h"

struct AlphaBetaReversiOpt {
    unsigned x, y, player;
    int score;
};

const int COL = 8, ROW = 8;
const int INF = 10000;

class AlphaBetaReversiStatus {
  public:
    inline const uint8_t *operator[](unsigned x) const { return mMap[x]; }

    inline void setLastPlayer(const unsigned &player) { lastPlayer = player; }

    inline uint8_t getNextPlayer() const { return 3 - lastPlayer; }

    void updateHash(unsigned x, unsigned y, int value);

    void set(unsigned x, unsigned y, const unsigned &player);

    void set(AlphaBetaReversiOpt opt);

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

    mutable uint8_t optNum[3]{};
    uint32_t mhash[2][2]{21523360, 21523360, 21523360, 21523360};

  private:

    uint8_t mMap[ROW][COL]{};
    uint8_t lastPlayer{};
};

class AlphaBetaReversiTraits {
  public:
    using Status = AlphaBetaReversiStatus;
    using Operate = AlphaBetaReversiOpt;
    using Score = int;
    using Player = uint8_t;
    static const int deep = 21;
    static const int timelimit = 970;
};

class AlphaBetaReversiInput : public algorithm::alpha_beta::AlphaBetaInput<AlphaBetaReversiTraits> {
  public:
    static Score getMaxScore();
    static Score getMinScore();
    static Score getScore(const Status &nowStatus,
                          const Player &selfPlayer,
                          bool isEnd = false); //返回当前局面得分
    static bool getAllOpt(const Status &nowStatus,
                          std::vector<Operate> &allOpts); // 处于结束状态返回false
    static void newStatus(const Status &nowStatus,
                          const Operate &opt,
                          Status &newStatus);
    static void setEnd(Operate &opt);
    static Player getNextPlayer(const Status &nowStatus);
};

class MCTSReversiOperate {
  public:
    uint8_t x, y;
};

using MCTSReversiPlayer = uint8_t;

class MCTSReversiStatus {
  public:
    MCTSReversiStatus() : mLastSkip(false) {}

    inline const uint8_t *operator[](unsigned x) const { return mTable[x]; }

    void setOperate(const MCTSReversiOperate &opt);

    inline MCTSReversiPlayer getNextPlayer() const { return mNextPlayer; }

    inline void set(uint8_t x, uint8_t y, MCTSReversiPlayer player) { mTable[x][y] = player; }

    inline void setNextPlayer(MCTSReversiPlayer player) { mNextPlayer = player; }

    inline bool lastIsSkip() const { return mLastSkip; }

    void print() const {
        std::cout << "nextPlayer : " << static_cast<int>(getNextPlayer()) << std::endl;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                std::cout << static_cast<int>(mTable[i][j]) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
  private:
    MCTSReversiPlayer mTable[ROW][COL]{};
    MCTSReversiPlayer mNextPlayer{};
    bool mLastSkip{};
};

class MCTSReversiTraits {
  public:
    using Status = MCTSReversiStatus;
    using Operate = MCTSReversiOperate;
    using Player = MCTSReversiPlayer;
    static const int timelimit = 59700;
};

class MCTSReversiInput : public algorithm::mcts::MCTSInput<MCTSReversiTraits> {
  public:
    using StatusResult = algorithm::mcts::StatusResult;
    static bool getAllOpt(const Status &nowStatus, ::std::vector<Operate> &allOpts);
    static bool quickGetOpt(const Status &nowStatus, Operate &opt);
    static void newStatus(const Status &nowStatus, const Operate &opt, Status &newStatus);
    static StatusResult getEndResult(const Status &nowStatus, const Player &selfPlayer);
    static Player getNextPlayer(const Status &nowStatus);
};

using AlphaBetaReversiEngine = algorithm::alpha_beta::AlphaBetaEngine<AlphaBetaReversiInput>;
using MCTSReversiEngine = algorithm::mcts::MCTSEngine<MCTSReversiInput>;
#endif //GSEVENTH_GAME_REVERSI_REVERSIENGINE_H_
