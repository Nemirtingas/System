#pragma once

#include <optional>
#include <functional>
#include <vector>

namespace System {

template<typename T>
struct SharedState
{
    std::mutex m;
    std::condition_variable cv;

    bool completed = false;
    std::optional<T> value;
    std::exception_ptr exception;

    std::vector<std::function<void()>> continuations;
};

template<>
struct SharedState<void>
{
    std::mutex m;
    std::condition_variable cv;

    bool completed = false;
    std::exception_ptr exception;

    std::vector<std::function<void()>> continuations;
};

template<typename TP, typename T>
class Task
{
public:
    using State = SharedState<T>;

    Task(std::shared_ptr<State> s, TP& p)
        : state(std::move(s))
        , pool(&p)
    {
    }

    Task(const Task&) = default;
    Task& operator=(const Task&) = default;

    bool IsCompleted() const
    {
        std::lock_guard<std::mutex> lock(state->m);
        return state->completed;
    }

    T Get()
    {
        std::unique_lock<std::mutex> lock(state->m);
        state->cv.wait(lock, [&]
        {
            return state->completed;
        });

        if (state->exception)
        {
            std::rethrow_exception(state->exception);
        }

        return *state->value;
    }

    // ContinueWith(Task<T>)
    template<typename F>
    auto ContinueWith(F&& f) const
        -> std::enable_if_t<
        std::is_invocable_v<F, Task<TP, T>>,
        Task<TP, std::invoke_result_t<F, Task<TP, T>>>>
    {
        using R = std::invoke_result_t<F, Task<TP, T>>;
        auto nextState = std::make_shared<SharedState<R>>();

        auto schedule =
            [
                prev = state,
                nextState,
                func = std::forward<F>(f),
                pool = pool
            ]() mutable
            {
                pool->Push(
                    [prev, nextState, func, pool]() mutable
                    {
                        try
                        {
                            Task<TP, T> t(prev, *pool);
                            R result = func(t);

                            {
                                std::lock_guard<std::mutex> lock(nextState->m);
                                nextState->value = std::move(result);
                                nextState->completed = true;
                            }
                        }
                        catch (...)
                        {
                            std::lock_guard<std::mutex> lock(nextState->m);
                            nextState->exception = std::current_exception();
                            nextState->completed = true;
                        }

                        nextState->cv.notify_all();

                        for (auto& c : nextState->continuations)
                        {
                            c();
                        }
                    });
            };

        {
            std::lock_guard<std::mutex> lock(state->m);
            if (state->completed)
            {
                schedule();
            }
            else
            {
                state->continuations.push_back(schedule);
            }
        }

        return Task<TP, R>(nextState, *pool);
    }

    // ContinueWith(T)
    template<typename F>
    auto ContinueWith(F&& f) const
        -> std::enable_if_t<
        std::is_invocable_v<F, T>,
        Task<TP, std::invoke_result_t<F, T>>>
    {
        return ContinueWith(
            [func = std::forward<F>(f)](Task<TP, T> t) mutable
            {
                return func(t.Get());
            });
    }

private:
    std::shared_ptr<State> state;
    TP* pool;
};

template<typename TP>
class Task<TP, void>
{
public:
    using State = SharedState<void>;

    Task(std::shared_ptr<State> s, TP& p)
        : state(std::move(s))
        , pool(&p)
    {
    }

    Task(const Task&) = default;
    Task& operator=(const Task&) = default;

    bool IsCompleted() const
    {
        std::lock_guard<std::mutex> lock(state->m);
        return state->completed;
    }

    void Get()
    {
        std::unique_lock<std::mutex> lock(state->m);
        state->cv.wait(lock, [&]
            {
                return state->completed;
            });

        if (state->exception)
        {
            std::rethrow_exception(state->exception);
        }
    }

    // ContinueWith(Task<void>)
    template<typename F>
    auto ContinueWith(F&& f) const
        -> std::enable_if_t<
        std::is_invocable_v<F, Task<TP, void>>,
        Task<TP, std::invoke_result_t<F, Task<TP, void>>>>
    {
        using R = std::invoke_result_t<F, Task<TP, void>>;
        auto nextState = std::make_shared<SharedState<R>>();

        auto schedule =
            [
                prev = state,
                nextState,
                func = std::forward<F>(f),
                pool = pool
            ]()
            {
                pool->Push(
                    [prev, nextState, func, pool]() mutable
                    {
                        try
                        {
                            Task<TP, void> t(prev, *pool);

                            if constexpr (std::is_void_v<R>)
                            {
                                func(t);

                                {
                                    std::lock_guard<std::mutex> lock(nextState->m);
                                    nextState->completed = true;
                                }
                            }
                            else
                            {
                                R result = func(t);

                                {
                                    std::lock_guard<std::mutex> lock(nextState->m);
                                    nextState->value = std::move(result);
                                    nextState->completed = true;
                                }
                            }
                        }
                        catch (...)
                        {
                            std::lock_guard<std::mutex> lock(nextState->m);
                            nextState->exception = std::current_exception();
                            nextState->completed = true;
                        }

                        nextState->cv.notify_all();

                        for (auto& c : nextState->continuations)
                        {
                            c();
                        }
                    });
            };

        {
            std::lock_guard<std::mutex> lock(state->m);
            if (state->completed)
            {
                schedule();
            }
            else
            {
                state->continuations.push_back(schedule);
            }
        }

        return Task<TP, R>(nextState, *pool);
    }

    // ContinueWith()
    template<typename F>
    auto ContinueWith(F&& f) const
        -> std::enable_if_t<
        std::is_invocable_v<F>,
        Task<TP, std::invoke_result_t<F>>>
    {
        return ContinueWith(
            [func = std::forward<F>(f)](Task<TP, void>)
            {
                return func();
            });
    }

private:
    std::shared_ptr<State> state;
    TP* pool;
};

template<typename TP, typename F>
auto RunTask(TP& pool, F&& func)
{
    using R = std::invoke_result_t<F>;
    auto state = std::make_shared<SharedState<R>>();

    pool.Push(
        [state, func = std::forward<F>(func)]()
        {
            try
            {
                if constexpr (std::is_void_v<R>)
                {
                    func();

                    {
                        std::lock_guard<std::mutex> lock(state->m);
                        state->completed = true;
                    }
                }
                else
                {
                    R result = func();

                    {
                        std::lock_guard<std::mutex> lock(state->m);
                        state->value = std::move(result);
                        state->completed = true;
                    }
                }
            }
            catch (...)
            {
                std::lock_guard<std::mutex> lock(state->m);
                state->exception = std::current_exception();
                state->completed = true;
            }

            state->cv.notify_all();

            for (auto& c : state->continuations)
            {
                c();
            }
        });

    return Task<TP, R>(state, pool);
}

}
