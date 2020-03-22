//
// Created by seventh on 2020/3/22.
//

#ifndef GSEVENTH_ALGORITHM_MCTS_MCTSENGINE_H_
#define GSEVENTH_ALGORITHM_MCTS_MCTSENGINE_H_
template<typename MCTSInput>
class MCTSEngine {

  public:
    static Operate GetStep(const Status &nowStatus) {
        return maxminFirst(nowStatus, clock() + AlphaBetaInput::timelimit);
    }
};

#endif //GSEVENTH_ALGORITHM_MCTS_MCTSENGINE_H_
