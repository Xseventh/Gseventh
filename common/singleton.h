//
// Created by seventh on 2020/3/22.
//

#ifndef GSEVENTH_COMMON_SINGLETON_H_
#define GSEVENTH_COMMON_SINGLETON_H_
template<typename T>
class Singleton {
  public:
    static T &getInstance() {
        static T instance;
        return instance;
    }
  protected:
    Singleton() = default;
    ~Singleton() = default;
  public:
    Singleton(Singleton const &) = delete;
    Singleton &operator=(Singleton const &) = delete;
};
#endif //GSEVENTH_COMMON_SINGLETON_H_
