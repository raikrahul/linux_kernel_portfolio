#include <string>
#include <vector>
using namespace std;

auto x1 = 3;
auto v1 = vector<int>{1, 2, 3};

vector<int> doSomething(string name) {
    auto v2 = vector<int>(3);
    auto x2 = 7;
    // ... do something
    return v2;
}

int main() { auto v3 = doSomething("hi"); }
