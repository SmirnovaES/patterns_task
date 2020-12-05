#include <iostream>
#include <memory>
#include <cassert>
#include <tuple>
#include <any>

namespace mystd
{

template <typename ... T>
struct InitialArgs {};

template <typename ... T>
class Function;

template <typename ... initial_args, typename return_value, typename ... full_args>
class Function<return_value(full_args...), InitialArgs<initial_args...>> 
{
public:
    Function() = delete;

    template <typename func>
    Function(func t, initial_args ... args): funcStorage_(std::make_unique<FuncStorage<func>>(t)), argsStorage_(args...)
    { }

    template <typename ... later_args>
	return_value operator()(later_args... args) const
    {
        std::tuple<full_args...> fullArgs = std::tuple_cat(argsStorage_, std::make_tuple(args...));
		return funcStorage_->callFunc(fullArgs);
	}

private:
    class FuncStorageWrapper
    {
    public:
        virtual return_value callFunc(std::tuple<full_args...> &) = 0;
    };

    template <typename T>
    class FuncStorage : public FuncStorageWrapper
    {
    public:
        FuncStorage(const T& t): t_(t)
        { }

        return_value callFunc(std::tuple<full_args...> & args) override
        {
            return std::apply(t_, args);
        }

    private:
        T t_;
    };

	std::unique_ptr<FuncStorageWrapper> funcStorage_;
    std::tuple<initial_args...> argsStorage_;
};

}

void func() {
	std::cout << "func" << std::endl;
}

void drawWindow(float x, float y, float z)
{
    std::cout << "x=" << x << ";y=" << y << ";z=" << z << std::endl;
}

int main()
{
    mystd::Function<void(), mystd::InitialArgs<>> f(func);
    f();
    mystd::Function<void(float, float, float), mystd::InitialArgs<float, float>> drawWindowF(drawWindow, 5, 1);
    drawWindowF(2);
    drawWindowF(2.52);
    
    return 0;
}