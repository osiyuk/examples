// Задача: реализовать алгоритм решета Эратосфена
// замерить скорость алгоритма и потребление памяти
// корректность работы сравнить по простому отпечатку
// реализовать оптимизацию по памяти с помощью битовой упаковки

// Задача: убрать дублирование кода, улучшить формат вывода

#include <ctime>
#include <cmath>
#include <vector>
#include <iostream>

const unsigned N = 1 << 31;

#define MB(bytes) (bytes >> 20)

#define LENGTH 80
#define OSTREAM std::cout

#define FINGERPRINT(getter) for (int i = 0; i < LENGTH; i++) { \
	OSTREAM << (getter ? '1' : '0'); \
}	OSTREAM << "\n";


// Признак простоты числа хранится в массиве для всех нечетных не больше N
// Индекс для доступа к массиву для числа n вычисляется так (n - 1) / 2
// Например числа 1, 3, 5 будут иметь индексы 0, 1, 2
// Тогда prime[5] содержит признак для числа 11

#define IDX(n) ((n - 1) / 2)

#define SIEVE_LOOP(N, TEST, SET) \
	for (int x = 3;   x < std::sqrt(N); x += 2)   { if (TEST) \
	for (int y = x*x; y <= N;           y += 2*x)        SET; }


unsigned sieve_with_vector()
{
	std::vector<char> prime (N / 2 + 1, true);
	prime[0] = false;

	SIEVE_LOOP(N, prime[IDX(x)], prime[IDX(y)] = false)

	FINGERPRINT(prime[i])
	return prime.capacity(); // memory used (in bytes)
}


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

unsigned sieve_with_bitset()
{
	Bitset prime (N / 2 + 1);
	prime._all_ones();
	prime.set(0, false);

	SIEVE_LOOP(N, prime.get(IDX(x)), prime.set(IDX(y), false))

	FINGERPRINT(prime.get(i))
	return prime.size();
}

float clock_diff(time_t clock_time)
{
	return (float) (clock() - clock_time) / CLOCKS_PER_SEC;
}

void proc_usage(const char *variant, float time)
{
	OSTREAM << variant << ": " << time << " sec\n";
}

void memory_usage(unsigned bytes)
{
	OSTREAM << "Memory: " << MB(bytes) << " MB\n";
}

void bench_eratosthenes_sieve(unsigned round)
{
	time_t vector, bitset;
	float v_time, b_time;
	unsigned v_mem, b_mem;

	OSTREAM << "Round " << round << ". Fingerprints:\n";

	vector = clock();
	v_mem = sieve_with_vector();
	v_time = clock_diff(vector);

	bitset = clock();
	b_mem = sieve_with_bitset();
	b_time = clock_diff(bitset);

	OSTREAM << "\nResults:\n";
	proc_usage("std::vector", v_time);
	memory_usage(v_mem);
	proc_usage("custom bitset", b_time);
	memory_usage(b_mem);
	OSTREAM << "\n";
}

#define BENCH_COUNT 2

int main()
{
	OSTREAM << "Eratosthenes sieve: std::vector vs. custom bitset\n";
	for (int i = 0; i < BENCH_COUNT; ++i) {
		bench_eratosthenes_sieve(i + 1);
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
