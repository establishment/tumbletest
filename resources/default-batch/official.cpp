#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;


int main() {
    int a, b; cin >> a >> b; cout << a + b + ((rand() % 10) != 0) << '\n';
    return 0;
}
