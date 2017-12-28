#pragma once

#include <utility>

namespace komb {

// Based on Andrei Alexandrescu's talk "Systematic Error Handling in C++" at C++ and Beyond 2012.
// Slides: https://onedrive.live.com/view.aspx?resid=F1B8FF18A2AEC5C5!1158&app=WordPdf&authkey=!APo6bfP5sJ8EmH4

template<class Fun>
class ScopeGuard
{
public:
    explicit ScopeGuard(Fun f) : f_(std::move(f)), active_(true) {}

    ~ScopeGuard() noexcept(false)
    {
        if (active_)
        {
            f_();
        }
    }

    void dismiss() { active_ = false; }
    ScopeGuard() = delete;
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

    ScopeGuard(ScopeGuard && rhs) : f_(std::move(rhs.f_)), active_(rhs.active_)
    {
        rhs.dismiss();
    }

private:
    Fun  f_;
    bool active_;
};

template<class Fun>
ScopeGuard<Fun> scopeGuard(Fun f)
{
    return ScopeGuard<Fun>(std::move(f));
}

namespace detail {

enum class ScopeGuardOnExit { };

template<typename Fun>
ScopeGuard<Fun> operator+(ScopeGuardOnExit, Fun&& fn) // NOLINT: false positive
{
    return ScopeGuard<Fun>(std::forward<Fun>(fn));
}

} // namespace detail

#define CONCATENATE_IMPL(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_IMPL(s1, s2)

#ifdef __COUNTER__
#   define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __COUNTER__)
#else
#   define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)
#endif

/**
 * @brief Usage: SCOPE_EXIT{ fclose(file); };
 * @details SCOPE_EXIT is very useful when dealing with code that is not RAII-wrapped.
 * This includes closing of FILE:s and pipes.
 *
 * Example:
 *
 *     void foo()
 *     {
 *         File* file = fopen(...);
 *         SCOPE_EXIT{ fclose(file); };
 *
 *         if (bar) { return; }
 *         if (baz) { exit(); }
 *         might_throw_an_exception(file);
 *
 *         // fclose will be called automagically here.
 *     }
 */
#define SCOPE_EXIT \
    auto ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) = ::komb::detail::ScopeGuardOnExit() + [&]()

} // namespace komb
