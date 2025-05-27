#pragma once
// 简单工厂模式，这里通过计算器的例子来说明，Operator为基类，有一个纯虚函数，其余子类都实现了该函数，最后通过一个类来通过父类指针构造子类实例对象；
class Operator {
protected:
    int a;
    int b;
public:
    Operator(int a, int b) : a{ a }, b{ b } {}
    virtual int Compute() = 0;
};

class Add : public Operator {
public:
    Add(int a, int b) : Operator(a, b) {}
    virtual int Compute() override {
        return a + b;
    }
};

class Sub : public Operator {
public:
    Sub(int a, int b) : Operator(a, b) {}
    virtual int Compute() override {
        return a - b;
    }
};

class Mul : public Operator {
public:
    Mul(int a, int b) : Operator(a, b) {}
    virtual int Compute() override {
        return a + b;
    }
};


class Div : public Operator {
public:
    Div(int a, int b) : Operator(a, b) {}
    virtual int Compute() override {
        return a + b;
    }
};

class Easy_Factory {
public:
    Operator* Choose_Operation(char op, int a, int b) {
        Operator* p = nullptr;
        switch (op)
        {
        case '+':
            p = new Add(a, b);
            break;
        case '-':
            p = new Sub(a, b);
            break;
        case '*':
            p = new Mul(a, b);
            break;
        case '/':
            p = new Div(a, b);
            break;
        default:
            break;
        }
        return p;
    }
};

 //int main() {
 //    Easy_Factory f;
 //    auto op = f.Choose_Operation('+', 1, 2);
 //    cout << op->Compute();
 //}