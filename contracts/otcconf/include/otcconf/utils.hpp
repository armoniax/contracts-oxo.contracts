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
   ACCOUNT_INVALID                      = 10001,    // 帐户无效
   RECORD_EXISTING                      = 10002,    // 数据已存在
   RECORD_NOT_FOUND                     = 10003,    // 数据不存在
   UNINITIALIZED                        = 10004,    // 未初始化完成
   CONF_NOT_FOUND                       = 10005,    // 配置不存在
   SPLIT_NOT_FOUND                      = 10006,    // 分账计划不存在
   NOT_POSITIVE                         = 10007,    // 非正数
   PARAM_ERROR                          = 10008,    // 参数错误
   SYMBOL_MISMATCH                      = 10009,    // 符号未匹配

   /**
    * @brief 用户
    * 
    */
   NO_AUTH                              = 10101,    // 无权操作
   BLACKLISTED                          = 10102,    // 黑名单
   ACCOUNT_NOT_FOUND                    = 10103,    // 用户不存在
   NAME_TOO_LARGE                       = 10104,    // 名称过长
   EMAIL_TOO_LARGE                      = 10105,    // email 过长
   DETAL_TOO_LARGE                      = 10106,    // detal 过长
   MEMO_TOO_LARGE                       = 10107,    // memo 过长
   REJECT_REASON_TOO_LARGE              = 10108,    // REJECT REASON原因过长
   ACCOUNT_STATE_MISMATCH               = 10109,    // 用户状态未匹配
   ACCCOUNT_TYPE_MISMATCH               = 10110,    // 用户类型未匹配
   
   /**
    * @brief 订单
    * 
    */
   ORDER_NOT_FOUND                      = 10201,    // 订单不存在
   ORDER_EXISTING                       = 10202,    // 订单已存在
   INVALID_ORDER_SIZE                   = 10203,    // 订单方无效
   INVALID_QUANTITY                     = 10204,    // 数量无效
   INVALID_MIN_QUANTITY_SYMBOL          = 10205,    // 最小数量符号无效
   INVALID_MAX_QUANTITY_SYMBOL          = 10206,    // 最大数量符号无效
   INVALID_MIN_QUANTITY                 = 10207,    // 最小数量无效
   INVALID_MAX_QUANTITY                 = 10208,    // 最大数量无效
   INVALID_PRICE                        = 10209,    // 价格无效
   PRICE_SYMBOL_NOT_ALLOW               = 10211,    // 价格符号不支持
   PRICE_NOT_POSITIVE                   = 10212,    // 价格非正数
   QUANTITY_MISMATCH                    = 10213,    // 数量不匹配
   QUANTITY_SYMBOL_NOT_ALLOW            = 10214,    // 符号不支持
   QUANTITY_SYMBOL_MISMATCH             = 10215,    // 符号不匹配
   QUANTITY_NOT_POSITIVE                = 10216,    // 数额需正数
   QUANTITY_FROZEN_INSUFFICIENT         = 10217,    // 冻结额不足
   QUANTITY_INSUFFICIENT                = 10218,    // 余额不足

   PAY_TYPE_NOT_ALLOW                   = 10219,    // 支付类型不支持
   ORDER_STATE_MISMATCH                 = 10220,    // 订单状态不匹配
   ORDER_STATE_NOT_RUNNING              = 10221,    // 订单未进行
   ORDER_STATE_NOT_PAUSED               = 10222,    // 订单未暂停
   ORDER_STATE_NOT_CLOSED               = 10223,    // 订单未关闭
   ORDER_STATE_CREATED                  = 10224,    // 订单已创建
   ORDER_STATE_CLOSED                   = 10225,    // 订单已关闭
   ORDER_STATE_CANCELLED                = 10226,    // 订单已取消
   ORDER_STATE_MAKER_ACCEPTED           = 10227,    // 商户已接受
   ORDER_STATE_NOT_ARBITING             = 10228,    // 订单非仲裁中
   ORDER_STATE_UNARBITTED               = 10229,    // 已开始仲裁
   
   TIME_NOT_EXPIRED                     = 10230,    // 交易未到期  
   TIME_NOT_REACHED                     = 10231,    // 未达操作时间
   TIME_TOO_LARGE                       = 10232,    // 时间过长

   SYSTEM_ERROR                         = 20000     // 系统错误
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
