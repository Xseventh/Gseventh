//
// Created by seventh on 2020/4/24.
//

#ifndef GSEVENTH_ALGORITHM_MCTS_MCTSENGINEIMPL_H_
#define GSEVENTH_ALGORITHM_MCTS_MCTSENGINEIMPL_H_
template<typename, typename T>
struct has_quickGetOpt {
    static_assert(
            std::integral_constant<T, false>::value,
            "Second template parameter needs to be of function type.");
};

template<typename C, typename Ret, typename... Args>
struct has_quickGetOpt<C, Ret(Args...)> {
  private:
    template<typename T>
    static constexpr auto check(T *) ->
    typename std::is_same<decltype(std::declval<T>().quickGetOpt(std::declval<Args>()...)), Ret>::type;

    template<typename>
    static constexpr std::false_type check(...);

    typedef decltype(check<C>(0)) type;

  public:
    static constexpr bool value = type::value;
};
#endif //GSEVENTH_ALGORITHM_MCTS_MCTSENGINEIMPL_H_
