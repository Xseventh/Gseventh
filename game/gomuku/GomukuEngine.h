//
// Created by seventh on 2020/4/29.
//

#ifndef GSEVENTH_GAME_GOMUKU_GOMUKUENGINE_H_
#define GSEVENTH_GAME_GOMUKU_GOMUKUENGINE_H_

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <queue>
#include "algorithm/max-min/AlphaBetaEngine.h"

class AlphaBetaGomukuOpt {
  public:
    unsigned x, y;
    int score;
    bool operator>(AlphaBetaGomukuOpt opt) const {
        return score > opt.score;
    }
};

const int GomukuCOL = 15, GomukuROW = 15;
const int GomukuINF = 1000000000;

class AlphaBetaGomukuStatus {
  public:
    inline const uint8_t *operator[](unsigned x) const { return mMap[x]; }

    inline void setLastPlayer(const unsigned &player) { lastPlayer = player; }

    inline uint8_t getNextPlayer() const { return 3 - lastPlayer; }

    void set(unsigned x, unsigned y, const unsigned &player);

    void set(AlphaBetaGomukuOpt opt);

    void print() const {
        std::cout << "nextPlayer : " << static_cast<int>(getNextPlayer()) << std::endl;
        for (int i = 0; i < GomukuROW; i++) {
            for (int j = 0; j < GomukuCOL; j++) {
                std::cout << static_cast<int>(mMap[i][j]) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    uint8_t mMap[GomukuROW][GomukuCOL]{};
  private:
    uint8_t lastPlayer{};
};

///////////////以下计分/////////////


class ScoreTable {
  public:
    const static int maxsig = 3;
    struct node {
        int next[maxsig];
        int fail, cnt, tw;
    };
    std::vector<node> stateTable;
    int size;
    void init() {
        stateTable.resize(1);
        memset(stateTable[0].next, 0, sizeof(stateTable[0].next));
        stateTable[0].cnt = 0;
        size = 1;
    }
    int getid(char c) {
        if (c == 'o')return 1;
        else if (c == 'x')return 2;
        else return 0;
    }
    void insert(const char *s, int res, int tw = 0) {
        int n = strlen(s);
        int now = 0;
        for (int i = 0; i < n; i++) {
            int c = getid(s[i]);
            if (!stateTable[now].next[c]) {
                stateTable.push_back(node{});
                memset(stateTable[size].next, 0, sizeof(stateTable[size].next));
                stateTable[size].cnt = 0;
                stateTable[now].next[c] = size++;
            }
            now = stateTable[now].next[c];
        }
        stateTable[now].cnt = res;
        stateTable[now].tw = tw;
    }
    void build() {
        std::queue<int> q;
        stateTable[0].fail = -1;
        q.push(0);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int i = 0; i < maxsig; i++)
                if (stateTable[u].next[i]) {
                    int v = stateTable[u].fail;
                    while (v != -1) {
                        if (stateTable[v].next[i]) {
                            stateTable[stateTable[u].next[i]].fail = stateTable[v].next[i];
                            break;
                        }
                        v = stateTable[v].fail;
                    }
                    if (v == -1)stateTable[stateTable[u].next[i]].fail = 0;
                    q.push(stateTable[u].next[i]);
                }
        }
    }
    inline int getd(char x, int player) {
        if (x == 0)return 0;
        else if (x == player)return 1;
        else return 2;
    }
    void Get(int u, int &sum, int &sum2, bool ty) {
        while (u != -1) {
            if (stateTable[u].cnt) {
                if (ty)sum += 3;
                sum += stateTable[u].cnt;
                sum2 += stateTable[u].tw;
            }
            u = stateTable[u].fail;
        }
    }
    int find(const uint8_t table[][GomukuCOL], int player) {
        int now, sum = 0, sum2 = 0;
        for (int i = 0; i < GomukuROW; i++) {
            now = 0;
            for (int j = 0; j < GomukuCOL; j++) {
                int c = getd(table[i][j], player);
                if (stateTable[now].next[c])
                    now = stateTable[now].next[c];
                else {
                    int v = stateTable[now].fail;
                    while (v != -1 && !stateTable[v].next[c])
                        v = stateTable[v].fail;
                    if (v == -1)now = 0;
                    else now = stateTable[v].next[c];
                }
                if (stateTable[now].cnt)
                    Get(now, sum, sum2, 0);
                else if (stateTable[now].fail)Get(stateTable[now].fail, sum, sum2, 0);
            }
        }
        for (int i = 0; i < GomukuCOL; i++) {
            now = 0;
            for (int j = 0; j < GomukuROW; j++) {
                int c = getd(table[j][i], player);
                if (stateTable[now].next[c])
                    now = stateTable[now].next[c];
                else {
                    int v = stateTable[now].fail;
                    while (v != -1 && !stateTable[v].next[c])
                        v = stateTable[v].fail;
                    if (v == -1)now = 0;
                    else now = stateTable[v].next[c];
                }
                if (stateTable[now].cnt)
                    Get(now, sum, sum2, 0);
                else if (stateTable[now].fail)Get(stateTable[now].fail, sum, sum2, 0);
            }
        }
        for (int i = 0; i < GomukuROW; i++) {
            now = 0;
            for (int j = 0; i + j < GomukuROW && j < GomukuCOL; j++) {
                int c = getd(table[i + j][j], player);
                if (stateTable[now].next[c])
                    now = stateTable[now].next[c];
                else {
                    int v = stateTable[now].fail;
                    while (v != -1 && !stateTable[v].next[c])
                        v = stateTable[v].fail;
                    if (v == -1)now = 0;
                    else now = stateTable[v].next[c];
                }
                if (stateTable[now].cnt)
                    Get(now, sum, sum2, 1);
                else if (stateTable[now].fail)Get(stateTable[now].fail, sum, sum2, 1);
            }
        }
        for (int i = 0; i < GomukuCOL; i++) {
            now = 0;
            for (int j = 0; i + j < GomukuCOL && j < GomukuROW; j++) {
                int c = getd(table[j][i + j], player);
                if (stateTable[now].next[c])
                    now = stateTable[now].next[c];
                else {
                    int v = stateTable[now].fail;
                    while (v != -1 && !stateTable[v].next[c])
                        v = stateTable[v].fail;
                    if (v == -1)now = 0;
                    else now = stateTable[v].next[c];
                }
                if (stateTable[now].cnt)
                    Get(now, sum, sum2, 1);
                else if (stateTable[now].fail)Get(stateTable[now].fail, sum, sum2, 1);
            }
        }

        for (int i = 0; i < GomukuROW; i++) {
            now = 0;
            for (int j = 0; i - j >= 0 && j < GomukuCOL; j++) {
                int c = getd(table[i - j][j], player);
                if (stateTable[now].next[c])
                    now = stateTable[now].next[c];
                else {
                    int v = stateTable[now].fail;
                    while (v != -1 && !stateTable[v].next[c])
                        v = stateTable[v].fail;
                    if (v == -1)now = 0;
                    else now = stateTable[v].next[c];
                }
                if (stateTable[now].cnt)
                    Get(now, sum, sum2, 1);
                else if (stateTable[now].fail)Get(stateTable[now].fail, sum, sum2, 1);
            }
        }

        for (int i = 0; i < GomukuCOL; i++) {
            now = 0;
            for (int j = 0; i + j < GomukuCOL && j < GomukuROW; j++) {
                int c = getd(table[GomukuROW - j - 1][i + j], player);
                if (stateTable[now].next[c])
                    now = stateTable[now].next[c];
                else {
                    int v = stateTable[now].fail;
                    while (v != -1 && !stateTable[v].next[c])
                        v = stateTable[v].fail;
                    if (v == -1)now = 0;
                    else now = stateTable[v].next[c];
                }
                if (stateTable[now].cnt)
                    Get(now, sum, sum2, 1);
                else if (stateTable[now].fail)Get(stateTable[now].fail, sum, sum2, 1);
            }
        }
        if (sum2 > 1024)sum += 200000;
        else if (sum2 != 1024 && sum2 >= 2)sum += 30000;
        return sum;
    }
};


//////////////以上计分/////////////

class AlphaBetaGomukuTraits {
  public:
    using Status = AlphaBetaGomukuStatus;
    using Operate = AlphaBetaGomukuOpt;
    using Score = int;
    using Player = uint8_t;
    static const int deep = 5;
    static const int timelimit = 970;
};

class AlphaBetaGomukuInput : public algorithm::alpha_beta::AlphaBetaInput<AlphaBetaGomukuTraits> {
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

using AlphaBetaGomukuEngine = algorithm::alpha_beta::AlphaBetaEngine<AlphaBetaGomukuInput>;
#endif //GSEVENTH_GAME_GOMUKU_GOMUKUENGINE_H_