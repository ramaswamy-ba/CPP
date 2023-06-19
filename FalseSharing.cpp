#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace chrono;

const int N = 10000000;
const int PADDING = 16; // Set PADDING to at least 16 to avoid false sharing

struct PaddedInt {
    int value;
    char padding[PADDING - sizeof(int)];
};

void increment(PaddedInt& x) {
    for (int i = 0; i < N; ++i) {
        ++x.value;
    }
}

int main() {
    PaddedInt x[2];
    x[0].value = 0;
    x[1].value = 0;

    auto start = high_resolution_clock::now();
    thread t1(increment, ref(x[0]));
    thread t2(increment, ref(x[1]));
    t1.join();
    t2.join();
    auto end = high_resolution_clock::now();

    cout << "Elapsed time: " << duration_cast<milliseconds>(end - start).count() << "ms" << endl;
    cout << "x[0] = " << x[0].value << ", x[1] = " << x[1].value << endl;

    return 0;
}


//g++ -g -pthread -o false_sharing false_sharing.cpp
//perf stat -e cache-misses,cache-references,L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores -o perf_results.txt ./false_sharing
//Check the output file perf_results.txt for cache miss events. A high number of cache misses may indicate that false sharing occurred.
