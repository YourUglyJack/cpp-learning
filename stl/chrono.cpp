#include <chrono>
#include <cmath>
#include <random>
#include <string>
#include <vector>
#include "../common/myutils.h"

// RAII 计时器
class ScopedTimer {
private:
    std::string label_;
    std::chrono::steady_clock::time_point start_;
public:
    explicit ScopedTimer(std::string label)
        : label_(std::move(label)), start_(std::chrono::steady_clock::now()) 
        {}
    
    ~ScopedTimer() {
        auto end = std::chrono::steady_clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
        std::cout << "[timer] " << label_ << ": " << us << " us\n";
    }
};

double norm_cdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

double bs_call(double S, double K, double r, double sigma, double T) {
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    return S * norm_cdf(d1) - K * std::exp(-r * T) * norm_cdf(d2);
}

double mc_call(double S, double K, double r, double sigma, double T, int n, std::mt19937_64& rng) {
    std::normal_distribution<double> Z(0.0, 1.0);   // 分布对象很轻，可以局部创建
    double drift = (r - 0.5 * sigma * sigma) * T;
    double vol   = sigma * std::sqrt(T);
    double sum   = 0.0;
    for (int i = 0; i < n; ++i) {
        double ST = S * std::exp(drift + vol * Z(rng));   // Z(rng)：用引擎驱动分布，得到一个样本
        sum += std::max(ST - K, 0.0);
    }
    return std::exp(-r * T) * sum / n;
}

int main() {
    print_title("chrono");

    auto t0 = std::chrono::steady_clock::now();
    double acc = 0.0;
    for (int i = 1; i <= 1'000'000; ++i) acc += 1.0 / i;
    auto t1 = std::chrono::steady_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1-t0).count();
    std::chrono::duration<double, std::milli> ms = t1 - t0;
    std::cout << "harmonic sum = " << acc << "\n";
    std::cout << "elapsed: " << ns << " ns, " << ms.count() << " ms\n";

    using namespace std::chrono_literals;
    auto timeout = 250ms;
    std::cout << "timeout in " << std::chrono::duration_cast<std::chrono::microseconds>(timeout).count() << " us \n"; 

    {
        ScopedTimer t{"build vector of 1e6"};
        std::vector<int> v;
        for(int i = 1; i <= 1'000'000; ++i) v.push_back(i);
    }

    print_title("random engieen and distribution");
    std::mt19937_64 rng(42);

    std::uniform_int_distribution<int> dice(1, 6);
    std::uniform_real_distribution<double> u01(0.0, 1.0);
    std::normal_distribution<double> ret(0.005, 0.01); // mean=0.005, std=0.01

    std::cout << "dice :";
    for (int i = 0; i < 10; ++i) std::cout << dice(rng) << " ";
    std::cout << "\nu01: " << u01(rng) << " " << u01(rng) << "\n";

    double px = 100.0;
    std::cout << "path: ";
    for (int d = 0; d < 10; ++d) {
        px *= (1.0 + ret(rng));
        std::cout << px << " ";
    }
    std::cout << "\n";


    double S = 100, K = 105, r = 0.03, sigma = 0.2, T = 1.0;
    double exact = bs_call(S, K, r, sigma, T);
    std::cout << "Black-Scholes = " << exact << "\n";
    for (int n : {1'000, 10'000, 100'000, 1'000'000}) {
        ScopedTimer t("MC n=" + std::to_string(n));
        double est = mc_call(S, K, r, sigma, T, n, rng);
        std::cout << "MC n=" << n << "  price=" << est << "  error=" << std::abs(est - exact) << "\n";
    }
}