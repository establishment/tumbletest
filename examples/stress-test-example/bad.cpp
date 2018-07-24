#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;


int main() {
    int a, b;
    cin >> a >> b;
    if (a == 10) {
        a += 1;
    }

    cout << a + b << '\n';
    return 0;
}
