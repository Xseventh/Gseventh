#include <iostream>
#include <string>
#include "game/reversi/ReversiEngine.h"
#include "external/jsoncpp/json.h"

int main() {
    freopen("input.txt","r",stdin);
    int x, y;
    MCTSReversiStatus nowStatus;
    // 初始化棋盘
    nowStatus.set(3, 4, 1);
    nowStatus.set(4, 3, 1);
    nowStatus.set(3, 3, 2);
    nowStatus.set(4, 4, 2);

    // 读入JSON
    ::std::string str;
    getline(::std::cin, str);
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);

    // 分析自己收到的输入和自己过往的输出，并恢复状态
    int turnID = input["responses"].size();
    int my = input["requests"][(Json::Value::UInt) 0]["x"].asInt() < 0 ? 1 : 2; // 第一回合收到坐标是-1, -1，说明我是黑方
    for (int i = 0; i < turnID; i++) {
        // 根据这些输入输出逐渐恢复状态到当前回合
        x = input["requests"][i]["x"].asInt();
        y = input["requests"][i]["y"].asInt();
        if (x >= 0) {
            nowStatus.setNextPlayer(3 - my);
            MCTSReversiOperate opt
                    {static_cast<uint8_t>(x), static_cast<uint8_t>(y)};
            MCTSReversiInput::newStatus(nowStatus, opt, nowStatus); // 模拟对方落子
        }
        x = input["responses"][i]["x"].asInt();
        y = input["responses"][i]["y"].asInt();
        if (x >= 0) {
            nowStatus.setNextPlayer(my);
            MCTSReversiOperate opt
                    {static_cast<uint8_t>(x), static_cast<uint8_t>(y)};
            MCTSReversiInput::newStatus(nowStatus, opt, nowStatus); // 模拟我方落子
        }
    }
    // 看看自己本回合输入
    x = input["requests"][turnID]["x"].asInt();
    y = input["requests"][turnID]["y"].asInt();
    if (x >= 0) {
        nowStatus.setNextPlayer(3 - my);
        MCTSReversiOperate
                opt{static_cast<uint8_t>(x), static_cast<uint8_t>(y)};
        MCTSReversiInput::newStatus(nowStatus, opt, nowStatus); // 模拟对方落子
    }

    nowStatus.setNextPlayer(my);
    // 找出合法落子点
    MCTSReversiOperate opt = MCTSReversiEngine::GetStep(nowStatus);

    Json::Value ret;
    // 决策结束，输出结果（你只需修改以上部分）
    ret["response"]["x"] = opt.x;
    ret["response"]["y"] = opt.y;
//    ret["response"]["cnt"] = cnt;
    if (opt.x == 8 && opt.y == 8) {
        ret["response"]["x"] = -1;
        ret["response"]["y"] = -1;
    }
    Json::FastWriter writer;
    ::std::cout << writer.write(ret) << ::std::endl;
    ::std::cout << ::std::flush;

    return 0;
}
