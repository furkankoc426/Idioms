#ifndef VISITOR_H_
#define VISITOR_H_

#include <stdexcept>
#include <string>
#include <sstream>
#include <type_traits>
#include <typeinfo>
#include <utility>


template<typename... Ts>
struct Visitor {
    virtual ~Visitor() = default;
};

template<typename T>
struct Visitor<T>: public virtual Visitor<> {
    virtual void visit(T&) = 0;
};

template<typename T, typename... Us>
struct Visitor<T, Us...>: public Visitor<T>, public Visitor<Us>... { };

template<typename... Ts>
using ConstVisitor = Visitor<const Ts...>;


struct AbstractVisitable {
    virtual ~AbstractVisitable() = default;
    virtual void accept(Visitor<>&) = 0;
    virtual void accept(Visitor<>&) const = 0;

protected:
    template<typename T>
    static inline void acceptImpl(Visitor<>& v, T& p) {
        if (auto* av = dynamic_cast<Visitor<T>*>(&v))
            av->visit(p);
        else if (auto* av = dynamic_cast<Visitor<const T>*>(&v))
            av->visit(p);
        else
            throw std::domain_error(std::string() + typeid(v).name() +
                    " does not implement Visitor< " +
                    typeid(T).name() + " >");
    }
};


#define CONCRETE_VISITABLE \
    void accept(Visitor<>& v) override { \
        using TypeOfThis = std::remove_pointer<decltype(this)>::type; \
        AbstractVisitable::acceptImpl<TypeOfThis>(v, *this); } \
    \
    void accept(Visitor<>& v) const override { \
        using TypeOfThis = std::remove_pointer<decltype(this)>::type; \
        AbstractVisitable::acceptImpl<TypeOfThis>(v, *this); }



template<typename Ret, typename... Ts>
struct VisitorFunction;

template<typename Ret, typename Arg, typename... VisitableTypes>
struct VisitorFunction<Ret(Arg), VisitableTypes...>:
        Visitor<VisitableTypes...> {
    VisitorFunction(Arg parameter, Ret initialResult = Ret()):
        parameter(parameter), result(initialResult) {}
    Arg parameter;
    Ret result;

    template<typename T>
    Ret accept(T& v) {
        v.accept(*this);
        return result;
    }
};

template<typename Arg, typename... VisitableTypes>
struct VisitorFunction<void(Arg), VisitableTypes...>:
        Visitor<VisitableTypes...> {
    VisitorFunction(Arg parameter): parameter(parameter) {}
    Arg parameter;

    template<typename T>
    void accept(T& v) {
        v.accept(*this);
    }
};

template<typename Ret, typename... VisitableTypes>
struct VisitorFunction<Ret(), VisitableTypes...>:
        Visitor<VisitableTypes...> {
    VisitorFunction(Ret initialResult = Ret()):
        result(initialResult) {}
    Ret result;

    template<typename T>
    Ret accept(T& v) {
        v.accept(*this);
        return result;
    }
};


#endif /* VISITOR_H_ */