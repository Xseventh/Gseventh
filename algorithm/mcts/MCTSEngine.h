//
// Created by seventh on 2020/3/22.
//

#ifndef GSEVENTH_ALGORITHM_MCTS_MCTSENGINE_H_
#define GSEVENTH_ALGORITHM_MCTS_MCTSENGINE_H_
#include <chrono>
#include <vector>
#include <memory>
#include <random>
#include <cfloat>
#include <cassert>
#include <algorithm>
#include <type_traits>
#include "MCTSEngineImpl.h"

namespace algorithm {

namespace mcts {

enum class StatusResult {
    LOSE,
    DRAW,
    WIN,
    NOTEND
};

template<typename MCTSInput>
class MCTSEngine {
  private:
    using Status = typename MCTSInput::Status;
    using Operate = typename MCTSInput::Operate;
    using Player = typename MCTSInput::Player;
    static const int timelimit = MCTSInput::timelimit;

    class MCTSNode {
      public:
        friend class MCTSEngine<MCTSInput>;
        explicit MCTSNode(::std::unique_ptr<Status> &&status)
                : mQ(0),
                  mN(0),
                  player(MCTSInput::getNextPlayer(*status)),
                  mpStatus(std::move(status)) { //如果没有std::move应该无法通过编译
            if (!MCTSInput::getAllOpt(*mpStatus, mOpts)) {
                mResult = MCTSInput::getEndResult(*mpStatus);
                mOpts.clear();
                mpStatus.reset();
            } else {
                mResult = StatusResult::NOTEND;
                mSon.reserve(mOpts.size());
            }
        }
        inline void updateWin() { mQ += 2, mN++; }
        inline void updateDraw() { mQ += 1, mN++; }
        inline void updateLose() { mN += 2; }
        inline bool expansionComplete() { return mOpts.empty(); }
        inline bool isTerminal() { return mOpts.empty() && mSon.empty(); }
        inline Player getPlayer() { return player; }
      private:
        uint64_t mQ;
        uint64_t mN;
        const Player player;
        ::std::unique_ptr<const Status> mpStatus;
        StatusResult mResult;
        ::std::vector<Operate> mOpts;
        ::std::vector<::std::unique_ptr<MCTSNode>> mSon{};
    };

    using MCTSNodeUniqPtr = ::std::unique_ptr<MCTSNode>;
    using ConstMCTSNodeUniqPtr = ::std::unique_ptr<const MCTSNode>;

    static MCTSNodeUniqPtr &selection(MCTSNodeUniqPtr *, ::std::vector<MCTSNodeUniqPtr *> &);

    static MCTSNodeUniqPtr &expansion(MCTSNodeUniqPtr *);

    static StatusResult simluation(MCTSNodeUniqPtr &);

    static void backpropagation(::std::vector<MCTSNodeUniqPtr *> &, StatusResult);
  public:
    static Operate GetStep(const Status &nowStatus);
};

template<typename MCTSInput>
typename MCTSEngine<MCTSInput>::Operate MCTSEngine<MCTSInput>::GetStep(const Status &nowStatus) {
    clock_t endTime = clock() + MCTSEngine<MCTSInput>::timelimit;
    MCTSNodeUniqPtr root(::std::make_unique<MCTSNode>(::std::make_unique<Status>(nowStatus)));
    ::std::vector<Operate> rootOpts;
    MCTSInput::getAllOpt(nowStatus, rootOpts);
    assert(rootOpts.size());
    ::std::reverse(rootOpts.begin(), rootOpts.end());
    while (clock() < endTime) {
        ::std::vector<MCTSNodeUniqPtr *> path;
        MCTSNodeUniqPtr &selectNode = selection(&root, path);
        MCTSNodeUniqPtr &expandNode = expansion(&selectNode);
        StatusResult result = simluation(expandNode);
        backpropagation(path, result);
    }

    uint64_t maxM = 0;
    Operate bestOpt(rootOpts[0]);
    for (size_t i = 0; i < root->mSon.size(); i++) {
        ::std::cout << root->mSon[i]->mN << ::std::endl;
        if (root->mSon[i]->mN > maxM) {
            maxM = root->mSon[i]->mN;
            bestOpt = rootOpts[i];
        }
    }
    return bestOpt;
}

template<typename MCTSInput>
typename MCTSEngine<MCTSInput>::MCTSNodeUniqPtr &MCTSEngine<MCTSInput>::selection(MCTSNodeUniqPtr *root,
                                                                                  ::std::vector<MCTSNodeUniqPtr *> &path) {
    path.clear();
    path.emplace_back(root);
    Player rootPlayer = (*root)->getPlayer();
    while ((*root)->expansionComplete() && !(*root)->isTerminal()) {
        MCTSNodeUniqPtr *maxSon = root;
        if (rootPlayer == (*root)->getPlayer()) {
            float bestUctScore = ::std::numeric_limits<float>::lowest();
            for (MCTSNodeUniqPtr &son:(*root)->mSon) {
                // score = Q'/N' + sqrt(2*log(N) / N') 因为减少浮点数运算 存储为整数，且mQ和mN为原来的两倍，所以以下计算有差异
                float uctExploitation = static_cast<float>(son->mQ) / (son->mN + FLT_EPSILON);
                float uctExploration =
                        sqrt(4 * log(static_cast<float>((*root)->mN >> 1) + 1) / (son->mN + FLT_EPSILON));
                float uctScore = uctExploitation + uctExploration;
                if (uctScore > bestUctScore) {
                    bestUctScore = uctScore;
                    maxSon = &son;
                }
            }
        } else {
            float bestUctScore = ::std::numeric_limits<float>::max();
            for (MCTSNodeUniqPtr &son:(*root)->mSon) {
                // score = Q'/N' + sqrt(2*log(N) / N') 因为减少浮点数运算 存储为整数，且mQ和mN为原来的两倍，所以以下计算有差异
                float uctExploitation = static_cast<float>(son->mQ) / (son->mN + FLT_EPSILON);
                float uctExploration =
                        sqrt(4 * log(static_cast<float>((*root)->mN >> 1) + 1) / (son->mN + FLT_EPSILON));
                float uctScore = uctExploitation - uctExploration;
                if (uctScore < bestUctScore) {
                    bestUctScore = uctScore;
                    maxSon = &son;
                }
            }
        }
        root = maxSon;
        path.emplace_back(root);
    }
    return *root;
}

template<typename MCTSInput>
typename MCTSEngine<MCTSInput>::MCTSNodeUniqPtr &MCTSEngine<MCTSInput>::expansion(MCTSNodeUniqPtr *node) {
    if ((*node)->expansionComplete())return *node;
    ::std::unique_ptr<Status> sonStatus = ::std::make_unique<Status>();
    MCTSInput::newStatus(*(*node)->mpStatus, (*node)->mOpts.back(), *sonStatus);
    (*node)->mSon.emplace_back(::std::make_unique<MCTSNode>(::std::move(sonStatus)));
    (*node)->mOpts.pop_back();
    if ((*node)->mOpts.empty())
        (*node)->mpStatus.reset();
    return (*node)->mSon.back();
}
template<typename MCTSInput>
StatusResult MCTSEngine<MCTSInput>::simluation(MCTSNodeUniqPtr &node) {
    if (node->mResult != StatusResult::NOTEND) {
        return node->mResult;
    }
    if constexpr (has_quickGetOpt<MCTSInput, bool(const Status &, Operate &)>::value) {
        Operate opt;
        Status status = *node->mpStatus;
        while (MCTSInput::quickGetOpt(status, opt)) {
            MCTSInput::newStatus(status, opt, status);
        }
        return MCTSInput::getEndResult(status);
    } else {
        ::std::vector<Operate> allOpts;
        Status status = *node->mpStatus;

        static ::std::mt19937 randomEngine(::std::random_device{}());//random

        while (MCTSInput::getAllOpt(status, allOpts)) {
            ::std::uniform_int_distribution<size_t> range(0, allOpts.size() - 1);
            MCTSInput::newStatus(status, allOpts[range(randomEngine)], status);
        }
        return MCTSInput::getEndResult(status);
    }
}
template<typename MCTSInput>
void MCTSEngine<MCTSInput>::backpropagation(std::vector<MCTSNodeUniqPtr *> &path, StatusResult result) {
    switch (result) {
        case StatusResult::LOSE:
            for (MCTSNodeUniqPtr *node :path) {
                (*node)->updateLose();
            }
            break;
        case StatusResult::DRAW:
            for (MCTSNodeUniqPtr *node :path) {
                (*node)->updateDraw();
            }
            break;
        case StatusResult::WIN:
            for (MCTSNodeUniqPtr *node :path) {
                (*node)->updateWin();
            }
            break;
        case StatusResult::NOTEND:assert(0);
            break;
    }
}

template<typename Input>
class MCTSInput {
  public:
    using Status = typename Input::Status;
    using Player = typename Input::Player;
    using Operate = typename Input::Operate;
    static const int timelimit = Input::timelimit;

    static bool getAllOpt(const Status &nowStatus, ::std::vector<Operate> &allOpts); // 处于结束状态返回false
//    static bool quickGetOpt(const Status &nowStatus, Operate &opt);
//    处于结束状态返回false (可选实现)
    static void newStatus(const Status &nowStatus, const Operate &opt, Status &newStatus);
    static StatusResult getEndResult(const Status &nowStatus);
    static Player getNextPlayer(const Status &nowStatus);
};

}
}

#endif //GSEVENTH_ALGORITHM_MCTS_MCTSENGINE_H_
