#include <eosio.token/eosio.token.hpp>
#include <otcfeesplit/safemath.hpp>
#include <otcfeesplit/otcfeesplit.hpp>
#include <otcfeesplit/utils.hpp>

using namespace eosio;
using namespace std;
using std::string;

namespace otc {

using namespace std;
using namespace eosio;
using namespace wasm::safemath;


inline int64_t get_precision(const symbol &s) {
    int64_t digit = s.precision();
    CHECK(digit >= 0 && digit <= 18, "precision digit " + std::to_string(digit) + " should be in range[0,18]");
    return calc_precision(digit);
}

inline int64_t get_precision(const asset &a) {
    return get_precision(a.symbol);
}


/**
 * reset the global with default values
 */
void otcfeesplit::init(const name& admin) {
    require_auth(get_self());

    _gstate.admin = admin;
}

void otcfeesplit::setratios(const map<name, uint32_t>& ratios, const bool& to_add) {
    require_auth( _self );

    for ( const auto &ratio : ratios ) {
        if (to_add) {
            _gstate.split_ratios[ ratio.first ] = ratio.second;

        } else { //to delete
            if (_gstate.split_ratios.find( ratio.first ) != _gstate.split_ratios.end()) {
                _gstate.split_ratios.erase( ratio.first );
            }
        }
    }
}

void otcfeesplit::ontransfer(const name& from, const name& to, const asset& quantity, const string& memo) {
    CHECKC( to == _self, err::ACCOUNT_INVALID, "not to self" )
    CHECKC( quantity.amount > 0, err::PARAM_ERROR, "negative amount" )

    auto precision              = get_precision(quantity.symbol);
    auto quant                  = quantity;

    for ( const auto &ratio : _gstate.split_ratios ) {
        auto to                 = ratio.first;
        quant.amount            = wasm::safemath::mul( wasm::safemath::div(ratio.second, percent_boost, precision),
                                    quantity.amount, precision);

        TRANSFER( MIRROR_BANK, to, quant, "feesplit" )
    }
}

}  //end of namespace::otc
