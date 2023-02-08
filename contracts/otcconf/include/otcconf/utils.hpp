#pragma once

#include <string>
#include <algorithm>
#include <iterator>
#include <eosio/eosio.hpp>
#include "safe.hpp"

#include <eosio/asset.hpp>

using namespace std;

#define EMPTY_MACRO_FUNC(...)

#define PP(prop) "," #prop ":", prop
#define PP0(prop) #prop ":", prop
#define PRINT_PROPERTIES(...) eosio::print("{", __VA_ARGS__, "}")

#ifndef ASSERT
    #define ASSERT(exp) eosio::check(exp, #exp)
#endif

#ifndef TRACE
    #define TRACE(...) print(__VA_ARGS__)
#endif

#define TRACE_L(...) TRACE(__VA_ARGS__, "\n")

#define CHECK(exp, msg) { if (!(exp)) eosio::check(false, msg); }
#define CHECKC(exp, code, msg) \
   { if (!(exp)) eosio::check(false, string("$$$") + to_string((int)code) + string("$$$ ") + msg); }

enum class err: uint32_t {
   NONE                 = 0,
   
   /**
    * @brief 系统
    *        
    */
   ACCOUNT_INVALID      = 10001,  // 账号不存在或不符合格式
   RECORD_NOT_FOUND     = 10002,
   UNINITIALIZED        = 10003,
   

   /**
    * @brief 用户
    * 
    */
   ACCOUNT_NOT_FOUND                = 10008,
   NAME_TOO_LARGE                   = 10009,
   EMAIL_TOO_LARGE                  = 10010,
   DETAL_TOO_LARGE                  = 10011,
   MEMO_TOO_LARGE                   = 10012,
   REJECT_REASON_TOO_LARGE          = 10013,
   ACCOUNT_STATE_MISMATCH           = 10014,
   
   /**
    * @brief 订单
    * 
    */
   ORDRE_OWNER_NO_AUTH          = 10101,
   ORDER_NOT_FOUND              = 10101,
   PARAM_ERROR                  = 10101,
   MEMO_FORMAT_ERROR             = 10102,
   INVALID_ORDER_SIZE            = 10004,
   INVALID_QUANTITY              = 10005,
   INVALID_MIN_QUANTITY          = 10005,
   INVALID_MAX_QUANTITY          = 10005,
   INVALID_MIN_QUANTITY_AMOUNT          = 10005,
   INVALID_MAX_QUANTITY_AMOUNT          = 10005,
   INVALID_PRICE             = 10005,
   PRICE_SYMBOL_NOT_ALLOW    = 10005,
   PRICE_NOT_POSITIVE        = 10005,
   QUANTITY_SYMBOL_NOT_ALLOW = 10005,
   QUANTITY_SYMBOL_MISMATCH  = 10005,   
   QUANTITY_NOT_POSITIVE     = 10005,
   PAY_TYPE_NOT_ALLOW        = 10005,
   ORDER_STATE_MISMATCH      = 10005,
   ORDER_STETE_NOT_RUNNING         = 10006,
   ORDER_STETE_NOT_PAUSED         = 10006,
   ORDER_STETE_NOT_CLOSED         = 10006,


   SYMBOL_MISMATCH      = 10201,
   SYMBOL_UNSUPPORTED   = 10202,
   FEE_INSUFFICIENT     = 10203,
   RATE_EXCEEDED        = 10204,
   QUANTITY_INVALID     = 10205,

   NOT_STARTED          = 10300,
   PAUSED               = 10301,
   TIME_EXPIRED         = 10302,
   TIME_NOT_EXPIRED     = 10303,
   STATE_MISMATCH       = 10304,

   SYSTEM_ERROR         = 20000
};


template<typename T>
int128_t multiply(int128_t a, int128_t b) {
    int128_t ret = a * b;
    CHECK(ret >= std::numeric_limits<T>::min() && ret <= std::numeric_limits<T>::max(),
          "overflow exception of multiply");
    return ret;
}

template<typename T>
int128_t divide_decimal(int128_t a, int128_t b, int128_t precision) {
    int128_t tmp = 10 * a * precision  / b;
    CHECK(tmp >= std::numeric_limits<T>::min() && tmp <= std::numeric_limits<T>::max(),
          "overflow exception of divide_decimal");
    return (tmp + 5) / 10;
}

template<typename T>
int128_t multiply_decimal(int128_t a, int128_t b, int128_t precision) {
    int128_t tmp = 10 * a * b / precision;
    CHECK(tmp >= std::numeric_limits<T>::min() && tmp <= std::numeric_limits<T>::max(),
          "overflow exception of multiply_decimal");
    return (tmp + 5) / 10;
}

#define divide_decimal64(a, b, precision) divide_decimal<int64_t>(a, b, precision)
#define multiply_decimal64(a, b, precision) multiply_decimal<int64_t>(a, b, precision)
#define multiply_i64(a, b) multiply<int64_t>(a, b)


inline constexpr int64_t power(int64_t base, int64_t exp) {
    int64_t ret = 1;
    while( exp > 0  ) {
        ret *= base; --exp;
    }
    return ret;
}

inline constexpr int64_t power10(int64_t exp) {
    return power(10, exp);
}

inline constexpr int64_t calc_precision(int64_t digit) {
    return power10(digit);
}

string_view trim(string_view sv) {
    sv.remove_prefix(std::min(sv.find_first_not_of(" "), sv.size())); // left trim
    sv.remove_suffix(std::min(sv.size()-sv.find_last_not_of(" ")-1, sv.size())); // right trim
    return sv;
}

uint64_t to_uint64(string_view s, const char* err_title) {
    errno = 0;
    uint64_t ret = std::strtoull(s.data(), nullptr, 10);
    CHECK(errno == 0, string(err_title) + ": convert str to uint64 error: " + std::strerror(errno));
    return ret;
}


uint64_t to_uint8(string_view s, const char* err_title) {
    errno = 0;
    uint8_t ret = std::strtoull(s.data(), nullptr, 10);
    CHECK(errno == 0, string(err_title) + ": convert str to uint8 error: " + std::strerror(errno));
    return ret;
}

vector<string_view> split(string_view str, string_view delims = " ")
{
    vector<string_view> res;
    std::size_t current, previous = 0;
    current = str.find_first_of(delims);
    while (current != std::string::npos) {
        res.push_back(trim(str.substr(previous, current - previous)));
        previous = current + 1;
        current = str.find_first_of(delims, previous);
    }
    res.push_back(trim(str.substr(previous, current - previous)));
    return res;
}

bool starts_with(string_view sv, string_view s) {
    return sv.size() >= s.size() && sv.compare(0, s.size(), s) == 0;
}

template <class T>
void to_int(string_view sv, T& res) {
    res = 0;
    T p = 1;
    for( auto itr = sv.rbegin(); itr != sv.rend(); ++itr ) {
        CHECK( *itr <= '9' && *itr >= '0', "invalid numeric character of int");
        res += p * T(*itr-'0');
        p   *= T(10);
    }
}
template <class T>
void precision_from_decimals(int8_t decimals, T& p10)
{
    CHECK(decimals <= 18, "precision should be <= 18");
    p10 = 1;
    T p = decimals;
    while( p > 0  ) {
        p10 *= 10; --p;
    }
}

asset asset_from_string(string_view from)
{
    string_view s = trim(from);

    // Find space in order to split amount and symbol
    auto space_pos = s.find(' ');
    CHECK(space_pos != string::npos, "Asset's amount and symbol should be separated with space");
    auto symbol_str = trim(s.substr(space_pos + 1));
    auto amount_str = s.substr(0, space_pos);

    // Ensure that if decimal point is used (.), decimal fraction is specified
    auto dot_pos = amount_str.find('.');
    if (dot_pos != string::npos) {
        CHECK(dot_pos != amount_str.size() - 1, "Missing decimal fraction after decimal point");
    }

    // Parse symbol
    uint8_t precision_digit = 0;
    if (dot_pos != string::npos) {
        precision_digit = amount_str.size() - dot_pos - 1;
    }

    symbol sym = symbol(symbol_str, precision_digit);

    // Parse amount
    safe<int64_t> int_part, fract_part;
    if (dot_pos != string::npos) {
        to_int(amount_str.substr(0, dot_pos), int_part);
        to_int(amount_str.substr(dot_pos + 1), fract_part);
        if (amount_str[0] == '-') fract_part *= -1;
    } else {
        to_int(amount_str, int_part);
    }

    safe<int64_t> amount = int_part;
    safe<int64_t> precision; precision_from_decimals(sym.precision(), precision);
    amount *= precision;
    amount += fract_part;

    return asset(amount.value, sym);
}
