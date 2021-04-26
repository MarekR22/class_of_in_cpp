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
        
        SECTION("of none existing constructor") {
            maker = ClassOf::instance<B>();
            REQUIRE(maker != nullptr);
            
            std::string s{"demo"};
            REQUIRE_THROWS(maker->create_unique(s));
        }
    }
    
    SECTION("Two overloads of constructors") {
        using ClassOf = pofa::creator_of<BaseVdtor, void(), void(std::string)>;
        const ClassOf* maker = nullptr;
        
        SECTION("Use existing constructor") {
            maker = ClassOf::instance<B>();
            REQUIRE(maker != nullptr);
            REQUIRE(maker->create_unique() != nullptr);
        }
        
        SECTION("Use none existing constructor") {
            maker = ClassOf::instance<B>();
            REQUIRE(maker != nullptr);
            std::string s{"demo"};
            REQUIRE_THROWS(maker->create_unique(s));
        }
    }
}

