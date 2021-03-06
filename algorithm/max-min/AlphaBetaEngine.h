//
// Created by seventh on 2020/2/6.
//

#ifndef GSEVENTH_ALPHA_BETA_H
#define GSEVENTH_ALPHA_BETA_H

#include <vector>
#include <algorithm>
#include <chrono>
#include <cassert>
#include <iostream>
#include <map>
namespace algorithm {

namespace alpha_beta {

template<class AlphaBetaInput>
class AlphaBetaEngine {
  public:
    using Status = typename AlphaBetaInput::Status;
    using Operate = typename AlphaBetaInput::Operate;
    using Score = typename AlphaBetaInput::Score;
    using Player = typename AlphaBetaInput::Player;
    static Operate GetStep(const Status &nowStatus) {
        return maxminFirst(nowStatus, clock() + AlphaBetaInput::timelimit);
    }

  private:
    static Operate maxminFirst(const Status &nowStatus, const int &timelimit) {
        Player player = AlphaBetaInput::getNextPlayer(nowStatus);
        assert(AlphaBetaInput::deep > 0);
        ::std::vector<Operate> allOpts;
        Operate opt;
        if (!AlphaBetaInput::getAllOpt(nowStatus, allOpts)) {//达到终结局面
            AlphaBetaInput::setEnd(opt);
            return opt;
        }
        Score score = AlphaBetaInput::getMinScore();
        int timeSplit = allOpts.size() * (allOpts.size() + 1) / 2;
        for (int i = 0; i < allOpts.size(); i++) {
            Status newStatus;
            AlphaBetaInput::newStatus(nowStatus, allOpts[i], newStatus);  // 遍历当前局面 的所有子局面
            clock_t nowTime = clock();
            Score val = maxmin(newStatus,
                               player,
                               score,
                               AlphaBetaInput::getMaxScore(),
                               AlphaBetaInput::deep - 1,
                               nowTime + (timelimit - nowTime)
                                       * (static_cast<long>(allOpts.size()) - i)
                                       / timeSplit);// 把新产生的局面交给对方，对方返回一个新局面的估价值
//            ::std::cout << allOpts[i].x << " " << allOpts[i].y << " " << val << ::std::endl;
//            ::std::cout << "canUseTime: " << (timelimit - nowTime) * (static_cast<long>(allOpts.size()) - i) / timeSplit
//                      << " useTime: " << clock() - nowTime << ::std::endl;
            timeSplit -= (static_cast<long>(allOpts.size()) - i);
            if (i == 0 || score < val) {
                opt = allOpts[i];
                score = val;
            }
        }
        opt.score = score;
//        ::std::cout << "unuseTime:" << timelimit - clock() << ::std::endl;
        return opt;
    }

    static Score maxmin(const Status &nowStatus,
                        const Player selfPlayer,
                        Score alpha,
                        Score beta,
                        int deep,
                        const clock_t &timelimit) {
        Player player = AlphaBetaInput::getNextPlayer(nowStatus);
        if (deep == 0 || clock() >= timelimit) {
            return AlphaBetaInput::getScore(nowStatus, selfPlayer);
        }
        ::std::vector<Operate> allOpts;
        if (!AlphaBetaInput::getAllOpt(nowStatus, allOpts)) {//达到终结局面
            return AlphaBetaInput::getScore(nowStatus, selfPlayer, true);
        }
        int timeSplit = allOpts.size() * (allOpts.size() + 1) / 2;
        if (player == selfPlayer) {
            for (int i = 0; i < allOpts.size(); i++) {
                Status newStatus;
                AlphaBetaInput::newStatus(nowStatus, allOpts[i], newStatus);  // 遍历当前局面 的所有子局面
                clock_t nowTime = clock();
                Score val = maxmin(newStatus,
                                   selfPlayer,
                                   alpha,
                                   beta,
                                   deep - 1,
                                   nowTime + (timelimit - nowTime)
                                           * (static_cast<long>(allOpts.size()) - i)
                                           / timeSplit);// 把新产生的局面交给对方，对方返回一个新局面的估价值
                timeSplit -= (static_cast<long>(allOpts.size()) - i);
                if (alpha < val) {
                    alpha = val;
                }
                if (!(alpha < beta)) {
                    return alpha;
                }
            }
            return alpha;
        } else {// 轮到对方走
            for (int i = 0; i < allOpts.size(); i++) {
                Status newStatus;
                AlphaBetaInput::newStatus(nowStatus, allOpts[i], newStatus);  // 遍历当前局面 的所有子局面
                clock_t nowTime = clock();
                Score val = maxmin(newStatus,
                                   selfPlayer,
                                   alpha,
                                   beta,
                                   deep - 1,
                                   nowTime + (timelimit - nowTime)
                                           * (static_cast<long>(allOpts.size()) - i)
                                           / timeSplit);// 把新产生的局面交给对方，对方返回一个新局面的估价值
                timeSplit -= (static_cast<long>(allOpts.size()) - i);
                if (val < beta) {
                    beta = val;
                }
                if (!(alpha < beta)) {
                    return beta;
                }
            }
            return beta;
        }
    }
};

template<typename Input>
class AlphaBetaInput {
  public:
    // Interface
    using Status = typename Input::Status;
    using Operate = typename Input::Operate;
    using Score = typename Input::Score;
    using Player = typename Input::Player;
    static const int deep = Input::deep;
    static const int timelimit = Input::timelimit;

    static Score getMaxScore();
    static Score getMinScore();
    static Score getScore(const Status &nowStatus, const Player &selfPlayer, bool isEnd = false); //返回当前局面得分
    static bool getAllOpt(const Status &nowStatus, ::std::vector<Operate> &allOpts); // 处于结束状态返回false
    static void newStatus(const Status &nowStatus, const Operate &opt, Status &newStatus);
    static void setEnd(Operate &opt);
    static Player getNextPlayer(const Status &nowStatus);
};

}

}
#endif //GSEVENTH_ALPHA_BETA_H