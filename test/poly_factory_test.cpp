#include <catch2/catch.hpp>
#include <pofa/pofa.hpp>

class BaseVdtor {
public:
    virtual ~BaseVdtor() {}
    
    virtual std::string get() const = 0;
};

class A : public BaseVdtor
{
public:
    explicit A(std::string a)
    : mS{std::move(a)}
    {}
    
    std::string get() const override
    {
        return mS;
    }
    
private:
    std::string mS;
};

class B : public BaseVdtor
{
public:
    std::string get() const override
    {
        return "B simple";
    }
};

class AB : public A
{
public:
    using A::A;
    
    AB() : AB("Default value")
    {}
};

static_assert(pofa::detail::can_construct_with_overload<A, void(std::string)>::value,
              "class A can be constructed with a string");
static_assert(!pofa::detail::can_construct_with_overload<A, void()>::value,
              "class A can't be default constructed");

static_assert(!pofa::detail::can_construct_with_overload<B, void(std::string)>::value,
              "class B can't be constructed with a string");
static_assert(pofa::detail::can_construct_with_overload<B, void()>::value,
              "class B can be default constructed");

static_assert(pofa::detail::can_construct_with_overload<AB, void(std::string)>::value,
              "class AB can be constructed with a string");
static_assert(pofa::detail::can_construct_with_overload<AB, void()>::value,
              "class AB can be default constructed");


TEST_CASE("Polymorphic factory")
{
    SECTION("Expose single overload of constructor") {
        using ClassOf = pofa::creator_of<BaseVdtor, void(std::string)>;
        const ClassOf* maker = nullptr;
        
        SECTION("of existing constructor") {
            maker = ClassOf::instance<A>();
            REQUIRE(maker != nullptr);
            
            std::string s{"demo"};
            auto ptrA = maker->create_unique(s);
            REQUIRE(ptrA != nullptr);
            
            auto ptrB = maker->create_shared(s);
            REQUIRE(ptrB != nullptr);
        }
    }
    
    SECTION("Two overloads of constructors") {
        using ClassOf = pofa::creator_of<BaseVdtor, void(), void(std::string)>;
        const ClassOf* maker = nullptr;
        
        SECTION("Use existing constructor") {
            maker = ClassOf::instance<B>();
            REQUIRE(maker != nullptr);
            REQUIRE(maker->create_unique() != nullptr);
            REQUIRE(maker->create_shared() != nullptr);
        }
        
        SECTION("Use none existing constructor") {
            maker = ClassOf::instance<B>();
            REQUIRE(maker != nullptr);
            std::string s{"demo"};
            REQUIRE_THROWS(maker->create_unique(s));
            REQUIRE_THROWS(maker->create_shared(s));
        }
    }
}

template<typename ClassOf, typename Child, typename U = void>
struct has_strict_instance_of : std::false_type
{};

template<typename ClassOf, typename Child>
struct has_strict_instance_of<
        ClassOf,
		Child,
		std::void_t<decltype(ClassOf::template strict_instance<Child>())>
	>
	: std::true_type
{};

template<typename Base, typename Child>
constexpr bool has_strict_instance_of_v = has_strict_instance_of<Base, Child>::value;

using ConstructAsDefaultAndFromString = pofa::creator_of<BaseVdtor, void(), void(std::string)>;

// verifies that "ConstructAsDefaultAndFromString::strict_instace<A>()" doesn't compile
static_assert(!has_strict_instance_of_v<ConstructAsDefaultAndFromString, A>,
              "string_instance build failure if type do not have all required constructors.");

// verifies that "ConstructAsDefaultAndFromString::strict_instance<B>()" doesn't compile
static_assert(!has_strict_instance_of_v<ConstructAsDefaultAndFromString, B>,
              "string_instance build failure if type do not have all required constructors.");

// verifies that "ConstructAsDefaultAndFromString::strict_instance<A>()" compiles correctly
static_assert(has_strict_instance_of_v<ConstructAsDefaultAndFromString, AB>,
              "string_instance build failure if type do not have all required constructors.");
