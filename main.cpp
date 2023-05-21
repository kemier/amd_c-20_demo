#include <iostream>
#include <chrono>
#include <format>
#include <iterator>
#include <thread>
#include <bit>
#include <cstdlib>
#include <new>
#include <limits>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <limits>
#include <numbers>
#include <string_view>


using namespace std::chrono;

auto egamma_aprox(const unsigned iterations) {
    long double s = 0;
    for (unsigned m = 2; m < iterations; ++m) {
        if (const long double t = std::riemann_zetal(m) / m; m % 2)
            s -= t;
        else
            s += t;
    }
    return s;
};


template<class T>
struct Mallocator {
    typedef T value_type;

    Mallocator() = default;

    template<class U>
    constexpr Mallocator(const Mallocator<U> &) noexcept {}

    [[nodiscard]] T *allocate(std::size_t n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            throw std::bad_array_new_length();

        if (auto p = static_cast<T *>(std::malloc(n * sizeof(T)))) {
            report(p, n);
            return p;
        }

        throw std::bad_alloc();
    }

    void deallocate(T *p, std::size_t n) noexcept {
        report(p, n, 0);
        std::free(p);
    }

private:
    void report(T *p, std::size_t n, bool alloc = true) const {
        std::cout << (alloc ? "Alloc: " : "Dealloc: ") << sizeof(T) * n
                  << " bytes at " << std::hex << std::showbase
                  << reinterpret_cast<void *>(p) << std::dec << '\n';
    }
};

template<class T, class U>
bool operator==(const Mallocator<T> &, const Mallocator<U> &) { return true; }

template<class T, class U>
bool operator!=(const Mallocator<T> &, const Mallocator<U> &) { return false; }

int main() {
    using milliseconds_tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
    using zone_tm = std::chrono::zoned_time<std::chrono::milliseconds>;
    std::cout << "Hello, World!" << std::endl;
    milliseconds_tp time = floor < milliseconds > (system_clock::now());
    zone_tm localTime = zoned_time<milliseconds>(current_zone(), time);

    zone_tm berlinTime = zoned_time<milliseconds>("Europe/Berlin", time);

    zone_tm newYorkTime = zoned_time<milliseconds>("America/New_York", time);

    zone_tm tokyoTime = zoned_time<milliseconds>("Asia/Tokyo", time);

    std::cout << time << '\n'; // 2020-05-23 19:07:20.290
    std::cout << localTime << '\n'; // 2020-05-23 21:07:20.290 CEST
    std::cout << berlinTime << '\n'; // 2020-05-23 21:07:20.290 CEST
    std::cout << newYorkTime << '\n'; // 2020-05-23 15:07:20.290 EDT
    std::cout << tokyoTime << '\n'; // 2020-05-24 04:07:20.290 JST
    std::string message = std::format("The answer is {}.", 42);
    std::string s = std::format("I'd rather be {1} than {0}.", "right", "happy");
    std::string buffer;
    std::format_to(std::back_inserter(buffer), "{}", 42); // replaces itoa(42, buffer, 10)
    std::format_to(std::back_inserter(buffer), "{:x}", 42);// replaces itoa(42, buffer, 16)
    std::cout << buffer.c_str() << std::endl;


    std::cout << '\n';

    std::jthread nonInterruptible([] {

        int counter{0};
        while (counter < 10) {

            std::this_thread::sleep_for(0.2s);

            std::cerr << "nonInterruptible: " << counter << '\n';

            ++counter;
        }

    });

    std::jthread interruptible([](std::stop_token stoken) {

        int counter{0};
        while (counter < 10) {
            std::this_thread::sleep_for(0.2s);
            if (stoken.stop_requested()) return;
            std::cerr << "interruptible: " << counter << '\n';
            ++counter;
        }

    });
    std::this_thread::sleep_for(1s);

    std::cerr << '\n';
    std::cerr << "Main thread interrupts both jthreads" << std::endl;
    nonInterruptible.request_stop();
    interruptible.request_stop();
    nonInterruptible.join();
    if constexpr (std::endian::native == std::endian::big)
        std::cout << "big-endian\n";
    else if constexpr (std::endian::native == std::endian::little)
        std::cout << "little-endian\n";
    else std::cout << "mixed-endian\n";

    std::cout << '\n';


    std::vector<int, Mallocator<int>> v(8);
    v.push_back(42);

    using namespace std::numbers;

    const auto
            x = sqrt(inv_pi) / inv_sqrtpi + ceil(exp2(log2e)) + sqrt3 * inv_sqrt3 + exp(0),
            t = (phi * phi - phi) + 1 / log2(sqrt2) + log10e * ln10 + pow(e, ln2) - cos(pi);

    std::cout << "The answer is " << x * t << '\n';

    using namespace std::string_view_literals;
    constexpr auto γ = "0.577215664901532860606512090082402"sv;

    std::cout
            << "γ as 10⁶ sums of ±ζ(m)/m   = "
            << egamma_aprox(1'000'000) << '\n'
            << "γ as egamma_v<float>       = "
            << std::setprecision(std::numeric_limits<float>::digits10 + 1)
            << egamma_v<float> << '\n'
            << "γ as egamma_v<double>      = "
            << std::setprecision(std::numeric_limits<double>::digits10 + 1)
            << egamma_v<double> << '\n'
            << "γ as egamma_v<long double> = "
            << std::setprecision(std::numeric_limits<long double>::digits10 + 1)
            << egamma_v<long double> << '\n'
            << "γ with " << γ.length() - 1 << " digits precision = " << γ << '\n';

    return 0;
}
