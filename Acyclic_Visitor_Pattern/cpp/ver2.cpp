#include "Visitor.h"

#include <functional>
#include <iostream>


struct Node: public AbstractVisitable { };

struct A: public Node {
    CONCRETE_VISITABLE
};

struct B: public Node {
    CONCRETE_VISITABLE
};

struct C: public Node {
    CONCRETE_VISITABLE
};

struct ZC: public C {
    CONCRETE_VISITABLE
};


struct BasicVisitor: Visitor<A, B, C, ZC> {
    void visit(A&) override {
        std::cout << "A" << std::endl;
    }
    void visit(B&) override {
        std::cout << "B" << std::endl;
    }
    void visit(C&) override {
        std::cout << "C" << std::endl;
    }
    void visit(ZC&) override {
        std::cout << "ZC" << std::endl;
    }
};


struct BasicConstVisitor: ConstVisitor<A, B> {
    void visit(const A&) override {
        std::cout << "A" << std::endl;
    }
    void visit(const B&) override {
        std::cout << "B" << std::endl;
    }
};


struct FuncVisitor: VisitorFunction<int(std::string), A, B, C> {
    using VisitorFunction::VisitorFunction;

    void visit(A&) override {
        std::cout << "A " << parameter << std::endl;
        result += 1;
    }
    void visit(B&) override {
        std::cout << "B" << std::endl;
        result += 10;
    }
    void visit(C&) override {
        std::cout << "C" << std::endl;
        result += 100;
    }
};


struct VoidFuncVisitor: VisitorFunction<void(std::string), A, B, C> {
    using VisitorFunction::VisitorFunction;

    void visit(A&) override {
        std::cout << "A" << parameter << std::endl;
    }
    void visit(B&) override {
        std::cout << "B" << std::endl;
    }
    void visit(C&) override {
        std::cout << "C" << std::endl;
    }
};

int main() {
    A a; Node& n1 = a;
    B b; Node& n2 = b;
    C c; Node& n3 = c;

    BasicVisitor basic;
    Visitor<>& vis1 = basic;
    n1.accept(vis1);
    n2.accept(vis1);
    n3.accept(vis1);

    BasicConstVisitor basicConst;
    Visitor<>& visConst1 = basicConst;
    n1.accept(visConst1);
    n2.accept(visConst1);

    ZC zc;
    zc.accept(vis1);

    int result = FuncVisitor("PARAM", 50000).accept(n1);
    std::cout << "Result: " << result << std::endl;

    return 0;
}