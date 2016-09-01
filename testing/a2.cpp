#include <iostream>
using namespace std;
int main() {
    srand(time(NULL));
    int a, b; cin >> a >> b;
    cout << a + b + (rand() % 3 == 0) << '\n';
    return 0;   
}
