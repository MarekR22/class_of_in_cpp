#ifndef POLY_FACTORY_HEADER
#define POLY_FACTORY_HEADER

#include <memory>
#include <type_traits>

namespace pofa {

namespace detail {
template<typename T, typename Overload>
struct can_construct_with_overload : std::false_type
{};

template<typename T, typename...Args>
struct can_construct_with_overload<T, void(Args...)> : std::is_constructible<T, Args...>
{};

template<typename Base>
using safe_unique_ptr = std::conditional_t<
	std::has_virtual_destructor_v<Base>,
		std::unique_ptr<Base>,
		std::unique_ptr<Base, void (*)(Base *)>
	>;

template<typename Base, typename CtorOverloads>
class ctor_interface;

template<typename Base, typename ...Args>
class ctor_interface<Base, void(Args...)>{
public:
    virtual ~ctor_interface() {}
    
    virtual auto create_unique(Args...) const -> safe_unique_ptr<Base> = 0;
    virtual auto create_shared(Args...) const -> std::shared_ptr<Base> = 0;
};
}

template<typename Base, typename...CtorOverload>
class creator_of : public detail::ctor_interface<Base, CtorOverload>...
{
public:
    template<typename Child>
    static auto instance() -> const creator_of*;
    
    template<typename Child>
    static auto strict_instance() -> std::enable_if_t<(detail::can_construct_with_overload<Child, CtorOverload>::value && ...), const creator_of*>
    {
		return instance<Child>();
    }
    
    using detail::ctor_interface<Base, CtorOverload>::create_unique ...;
    using detail::ctor_interface<Base, CtorOverload>::create_shared ...;
};

namespace detail {
template <typename Base, typename Child, typename Interface, typename...CtorOverload>
class creator_of_instance;

template <typename Base, typename Child, typename Interface>
class creator_of_instance<Base, Child, Interface> : public Interface
{};


template <typename Base, typename Child, typename Interface, typename...FirstCtorArgs, typename...CtorOverload>
class creator_of_instance<Base, Child, Interface, void(FirstCtorArgs...), CtorOverload...>
	: public creator_of_instance<Base, Child, Interface, CtorOverload...>
{
public:
    auto create_unique(FirstCtorArgs...args) const -> safe_unique_ptr<Base> override
    {
        if constexpr (std::is_constructible_v<Child, FirstCtorArgs...>)
        {
        	return std::make_unique<Child>(std::forward<FirstCtorArgs>(args)...);
        }
        throw std::logic_error{""};
    }
    
    auto create_shared(FirstCtorArgs...args) const -> std::shared_ptr<Base> override
    {
        if constexpr (std::is_constructible_v<Child, FirstCtorArgs...>)
        {
        	return std::make_shared<Child>(std::forward<FirstCtorArgs>(args)...);
        }
        throw std::logic_error{""};
    }
};
}

template<typename Base, typename...CtorOverload>
template<typename Child>
auto creator_of<Base, CtorOverload...>::instance() -> const creator_of*
{
    using Interface = creator_of<Base, CtorOverload...>;
    
    static_assert((detail::can_construct_with_overload<Child, CtorOverload>::value || ...),
                  "ensure class can be constructed with at least one required overloads");
    
    static const detail::creator_of_instance<Base, Child, Interface, CtorOverload...> creator;
    return &creator;
}
}

#endif // POLY_FACTORY_HEADER

