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

namespace algorithm {

namespace mcts {

enum class StatusResult {
    LOSS,
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
                : mQ(0), mN(0), mResult(StatusResult::NOTEND), mpStatus(std::move(status)) { //如果没有std::move应该无法通过编译
            MCTSInput::getAllOpt(*status, mOpts);
            mSon.reserve(mOpts.size());
        }
        inline void updateWin() { mQ += 2, mN++; }
        inline void updateDraw() { mQ += 1, mN++; }
        inline void updateLose() { mN += 2; }
        inline bool expansionComplete() { return mOpts.empty(); }
        inline bool isTerminal() { return mOpts.empty() && mSon.empty(); }
      private:
        uint64_t mQ;
        uint64_t mN;
        ::std::unique_ptr<const Status> mpStatus;
        StatusResult mResult;
        ::std::vector<Operate> mOpts;
        ::std::vector<::std::unique_ptr<MCTSNode>> mSon{};
    };

    using MCTSNodeUniqPtr = ::std::unique_ptr<MCTSNode>;
    using ConstMCTSNodeUniqPtr = ::std::unique_ptr<const MCTSNode>;

    static const MCTSNodeUniqPtr &selection(MCTSNodeUniqPtr *, ::std::vector<MCTSNodeUniqPtr *> &);

    static const MCTSNodeUniqPtr &expansion(MCTSNodeUniqPtr *);

    static StatusResult simluation(MCTSNodeUniqPtr &);

    static void backpropagation(::std::vector<MCTSNodeUniqPtr *> &, StatusResult);
  public:
    static Operate GetStep(const Status &nowStatus);
};

template<typename MCTSInput>
typename MCTSEngine<MCTSInput>::Operate MCTSEngine<MCTSInput>::GetStep(const Status &nowStatus) {
    clock_t endTime = clock() + MCTSEngine<MCTSInput>::timelimit;
    MCTSNode root(::std::make_unique<Status>(nowStatus));
    while (clock() < endTime) {
        ::std::vector<MCTSNodeUniqPtr *> path;
        MCTSNodeUniqPtr &selectNode = selection(&root, path);
        MCTSNodeUniqPtr &expandNode = expansion(&selectNode);
        StatusResult result = simluation(expandNode);
        backpropagation(path, result);
    }
}

template<typename MCTSInput>
const typename MCTSEngine<MCTSInput>::MCTSNodeUniqPtr &MCTSEngine<MCTSInput>::selection(MCTSNodeUniqPtr *root,
                                                                                        ::std::vector<MCTSNodeUniqPtr *> &path) {
    path.clear();
    path.emplace_back(root);
    while ((*root)->expansionComplete()) {// Terminal也是完全展开的，所以不需要判Terminal
        MCTSNodeUniqPtr *maxSon = root;
        float maxUctScore = ::std::numeric_limits<float>::min();
        for (MCTSNodeUniqPtr &son:(*root)->mSon) {
            // score = Q'/N' + sqrt(2*log(N) / N') 因为减少浮点数运算 存储为整数，且mQ和mN为原来的两倍，所以以下计算有差异
            float uctExploitation = static_cast<float>(son->mQ) / (son->mN + FLT_EPSILON);
            float uctExploration = sqrt(4 * log(static_cast<float>(root->mN >> 1) + 1) / (son->mN + FLT_EPSILON));
            float uctScore = uctExploitation + uctExploration;
            if (uctScore > maxUctScore) {
                maxUctScore = uctScore;
                maxSon = &son;
            }
        }
        root = maxSon;
        path.emplace_back(root);
    }
    return *root;
}

template<typename MCTSInput>
const typename MCTSEngine<MCTSInput>::MCTSNodeUniqPtr &MCTSEngine<MCTSInput>::expansion(MCTSNodeUniqPtr *node) {
    if ((*node)->expansionComplete())return *node;
    auto sonStatus = ::std::make_unique<Status>();
    MCTSInput::newStatus(*(*node)->mpStatus, (*node)->mOpts.back(), *sonStatus);
    (*node)->mSon.emplace_back(std::move(sonStatus));
    (*node)->mOpts.pop_back();
    return (*node)->mSon.back();
}
template<typename MCTSInput>
StatusResult MCTSEngine<MCTSInput>::simluation(MCTSNodeUniqPtr &node) {
    if (node->mResult != StatusResult::NOTEND) {
        return node->mResult;
    }
    ::std::vector<Operate> allOpts;
    Status status = *node->mpStatus;

    static ::std::mt19937 randomEngine(::std::random_device{}());//random
    ::std::uniform_int_distribution<size_t> range(0, allOpts.size() - 1);

    while (MCTSInput::getAllOpt(allOpts)) {
        MCTSInput::newStatus(status, allOpts[range(randomEngine)], status);
    }
    if (node->isTerminal()) {
        node->mResult = MCTSInput::getEndResult(status);
        return node->mResult;
    } else {
        return MCTSInput::getEndResult(status);
    }
}
template<typename MCTSInput>
void MCTSEngine<MCTSInput>::backpropagation(std::vector<MCTSNodeUniqPtr *> &path, StatusResult result) {
    switch (result) {
        case StatusResult::LOSS:
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
    using Operate = typename Input::Operate;
    static const int timelimit = Input::timelimit;

    static bool getAllOpt(const Status &nowStatus, ::std::vector<Operate> &allOpts);
    static void newStatus(const Status &nowStatus, const Operate &opt, Status &newStatus);
    static StatusResult getEndResult(const Status &nowStatus);
};

}
}

#endif //GSEVENTH_ALGORITHM_MCTS_MCTSENGINE_H_
