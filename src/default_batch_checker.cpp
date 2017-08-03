#include "batch.hpp"
using namespace std;

// ofstream in, out, ok

/*
 * Read input from in
 * contestan't answer from out
 * official answer from ok
 */

/*
 *  To comunicate the result, use OK or WA.
 *  For a specific message, use OK("message") or WA("wa message")
 */

int main(int argc, char** argv) {
    Init(argc, argv); // keep this

    BasicDiff(); // make sure to delete this
    
    OK();
}
