#include<iostream>
#include"easy_factory.h"
using namespace std;


int main() {
    Easy_Factory f;
    auto op = f.Choose_Operation('+', 1, 2);
    cout << op->Compute();
}