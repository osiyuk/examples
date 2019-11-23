// Задача: реализовать алгоритм решета Эратосфена
// замерить скорость алгоритма и потребление памяти
// корректность работы сравнить по простому отпечатку
// реализовать оптимизацию по памяти с помощью битовой упаковки


#include <ctime>
#include <iostream>

const unsigned N = 1 << 31;

#define MB(bytes) (bytes >> 20)
#define FINGERPRINT_LENGTH 80
#define FINGERPRINT(getter) std::cout << "Fingerprint:\n"; \
    for (int i = 0; i < FINGERPRINT_LENGTH; i++) \
        std::cout << (getter ? 1 : 0); \
    std::cout << "\n\n";


void output_time(const char *title, time_t start_time)
{
    float output = (float)(clock() - start_time) / CLOCKS_PER_SEC;
    std::cout << title << ": " << output << " sec\n";
}

void output_memory(unsigned bytes)
{
    std::cout << "Memory: " << MB(bytes) << " MB\n";
}


#include <cmath>
#include <vector>


void test_eratosthenes_sieve()
{
    std::vector<char> prime ((N + 1) / 2, true);
    prime[0] = false;

    time_t processing = clock();

    for (int i = 3; i <= std::sqrt(N); i += 2)
        if (prime[(i - 1) / 2])
            for (int j = i*i; j <= N; j +=  2 * i)
                prime[(j - 1) / 2] = false;

    output_time("std::vector", processing);
    output_memory(prime.size() * sizeof(char));

    FINGERPRINT(prime[i])
}

// Признак простоты числа хранится в массиве для всех нечетных не больше N
// Индекс для доступа к массиву для числа n вычисляется так (n - 1) / 2
// Например числа 1, 3, 5 будут иметь индексы 0, 1, 2
// Тогда prime[5] содержит признак для числа 11


struct Bitset {
    char *a;
    unsigned sz;

    Bitset(unsigned n):
        a(0), sz(n / 8 + 1)
    {
        a = new char[sz];
        std::memset(a, 0, sz);
    }

    ~Bitset() {
        delete [] a;
    }

    bool get(unsigned i) {
        return (a[i >> 3] & (1u << (i % 8))) != 0;
    }

    void set(unsigned i, bool val) {
        if (val)
            a[i >> 3] |= 1u << (i % 8);
        else
            a[i >> 3] &= ~(1u << (i % 8));
    }

    unsigned size() { return sz * sizeof(char); }

    void _all_ones() {
        std::memset(a, ~0, sz * sizeof(char));
    }

};

void test_eratosthenes_sieve_with_bitset()
{
    Bitset prime ((N + 1) / 2);
    prime._all_ones();
    prime.set(0, false);

    time_t processing = clock();

    for (int i = 3; i <= std::sqrt(N); i += 2)
        if (prime.get((i - 1) / 2))
            for (int j = i*i; j <= N; j +=  2 * i)
                prime.set((j - 1) / 2, false);

    output_time("Bitset", processing);
    output_memory(prime.size());

    FINGERPRINT(prime.get(i))
}


int main()
{
    for (int i = 0; i < 3; ++i) {
        test_eratosthenes_sieve();
        test_eratosthenes_sieve_with_bitset();
    }

    return 0;
}

/*
MacBook Pro Early 2011
2,3 GHz Intel Core i7
8 GB 1600 MHz DDR3

std::vector: 23.5223 sec
Bitset: 27.2006 sec
*/
