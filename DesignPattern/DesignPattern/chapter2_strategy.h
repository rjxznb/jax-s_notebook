#pragma once
#include<string>
// 设计一个收银系统：有打折活动，有满减活动，这里简化一下，只根据原价总金额计算实际活动优惠后的总金额；

// 首先采用简单工厂模式：
class SaleBase {
public:
	virtual double sell(double money) = 0;
};

// 正常没活动；
class Normal: public SaleBase{
public:
	virtual double sell(double money) override {
		return money;
	}
};

// 打折活动：
class Discount: public SaleBase{
private:
	int discount; // 折扣；
public:
	Discount(int discount) : discount{ discount } {}

	virtual double sell(double money) override {
		return money * 0.1 * discount;
	}
};

class SpendAndSave: public SaleBase {
private:
	double spend;
	double save;
public:
	SpendAndSave(double spend, double save) : spend{ spend }, save{ save } {}

	virtual double sell(double money) override{
		return money >= spend ? money - (money / spend) * save : money;
	}
};

// cmd为1表示normal，2表示discount，3表示spendandsave；
class EasyFactory {
public:
    SaleBase* Choose_Sale(int cmd, double spend_or_discount = 0, double save = 0) {
        SaleBase* p = nullptr;
        switch (cmd)
        {
        case 1:
            p = new Normal();
            break;
        case 2:
            p = new Discount(spend_or_discount);
            break;
        case 3:
            p = new SpendAndSave(spend_or_discount, save);
            break;
        default:
            break;
        }
        return p;
    }
};



// 策略模式：