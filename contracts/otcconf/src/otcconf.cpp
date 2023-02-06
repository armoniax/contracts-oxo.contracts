#include <eosio.token/eosio.token.hpp>
#include <otcconf/safemath.hpp>
#include <otcconf/otcconf.hpp>
#include <otcconf/utils.hpp>

using namespace eosio;
using namespace std;
using std::string;

namespace otc {

using namespace std;
using namespace eosio;
using namespace wasm::safemath;

/**
 * reset the global with default values
 */
void otcconf::init(const name& admin) {
    require_auth(get_self());

    _gstate.app_info = {
        "meta.balance"_n,
        "0.2.0",
        "https://metabalance.app",
        "https://nftstorage.link/ipfs/bafkreicrlzf4rix5wt6bcnslosdgw7px5gg6fkdi5inva6y7hkyc3fu4ua"
    };
    
    // check(_gstate.status==status_type::UN_INITIALIZE, "contract has initialzed");
    _gstate = {};
    _gstate.status = uint8_t(status_type::INITIALIZED);
    _gstate.app_info = {
        "meta.balance"_n,
        "0.2.0",
        "https://m.oxo.cash",
        "https://nftstorage.link/ipfs/bafkreicrlzf4rix5wt6bcnslosdgw7px5gg6fkdi5inva6y7hkyc3fu4ua"
    };
    _gstate.managers = {
        {manager_type::admin,admin},
        {manager_type::otcbook,"meta.book"_n},
        {manager_type::settlement,"meta.settle"_n},
        {manager_type::swaper, "meta.swap"_n},
        {manager_type::aplinkfarm, "aplink.farm"_n},
        {manager_type::feetaker,"oxo.feeadmin"_n},
        {manager_type::arbiter,"casharbitoo1"_n},
        {manager_type::cashbank, "amax.mtoken"_n},
        {manager_type::scorebank, "meta.token"_n}
    };
    _gstate.pay_type = { BANK, WECHAT, ALIPAY };
    _gstate.fiat_type = CNY;
    _gstate.fee_pct = 80;
    _gstate.stake_assets_contract = {
        // {STAKE_AMAX, AMAX_BANK},
        // {STAKE_CNYD, CNYD_BANK},
        {STAKE_USDT, MIRROR_BANK}
    };
    _gstate.coin_as_stake = {
        {AMAX_ARC20, STAKE_AMAX},
        {CNYD_ARC20, STAKE_CNYD},
        {USDT_ERC20, STAKE_USDT},
        {USDT_TRC20, STAKE_USDT},
        {USDT_BEP20, STAKE_USDT}
    };
    _gstate.buy_coins_conf = {
        // AMAX_ARC20,
        // CNYD_ARC20,
        USDT_ERC20,
        USDT_TRC20,
        USDT_BEP20
    };
    _gstate.sell_coins_conf = {
        // AMAX_ARC20,
        // CNYD_ARC20,
        USDT_ERC20,
        USDT_TRC20,
        USDT_BEP20
    };
    _gstate.accepted_timeout = 1800;
    _gstate.payed_timeout = 10800;

    _gstate.settle_levels = {
        {0,0,0},
        {100000000000, 1000, 4000},
        {1000000000000, 2500, 5000},
        {20000000000000, 4000, 6000}
    };

    _gstate.farm_lease_id = 3;
    _gstate.farm_scales = {
        {STAKE_USDT.code(), 25000},
        {STAKE_AMAX.code(), 6250000}
    };

    _gstate.swap_steps = {
        {0, 1500}, {2000000000, 2500}, {10000000000, 3500}, {25000000000, 5000}};
}

void otcconf::setmanager(const name& type, const name& account,const name& contract_name){
    auto fiat_conf = fiat_conf_t( contract_name );
    CHECKC( _db.get(fiat_conf),err::RECORD_FOUND, "conf not existing : " + contract_name.to_string())
    CHECKC( has_auth(_self) || has_auth(fiat_conf.managers.at(manager_type::admin)), err::NO_AUTH, "Missing required authority of admin or managers" )
    CHECKC( type == manager_type::admin
            || type == manager_type::feetaker
            || type == manager_type::arbiter
            || type == manager_type::otcbook
            || type == manager_type::settlement
            || type == manager_type::swaper
            || type == manager_type::cashbank
            || type == manager_type::scorebank
            || type == manager_type::aplinkfarm , err::PARAM_ERROR, "manager type error: " + type.to_string())

    // require_auth(_gstate.managers.at(manager_type::admin));
    // CHECKC(is_account(account), err::ACCOUNT_INVALID, "invalid account: " + account.to_string());
    fiat_conf.managers[type] = account;
    
    _db.set(fiat_conf);
}

void otcconf::addcoin(const bool& is_buy, const symbol& coin, const symbol& stake_coin,const name& contract_name){

    auto fiat_conf = fiat_conf_t( contract_name );
    CHECKC( _db.get(fiat_conf),err::RECORD_FOUND, "conf not existing : " + contract_name.to_string())
    CHECKC( has_auth(_self) || has_auth(fiat_conf.managers.at(manager_type::admin)), err::NO_AUTH, "Missing required authority of admin or managers" )

    // require_auth(_gstate.managers.at(manager_type::admin));
    CHECKC( fiat_conf.stake_assets_contract.count(stake_coin), err::SYMBOL_MISMATCH, "stake coin not supported" )
    if(is_buy){
        CHECKC( !fiat_conf.buy_coins_conf.count(coin), err::RECORD_FOUND, "coin already in buy coin list" )
        fiat_conf.buy_coins_conf.insert(coin);
    }
    else {
        CHECKC( !fiat_conf.sell_coins_conf.count(coin), err::RECORD_FOUND, "coin already in sell coin list" ) 
        fiat_conf.sell_coins_conf.insert(coin);
    }
    fiat_conf.coin_as_stake[coin] = stake_coin;
    _db.set(fiat_conf);
}

void otcconf::deletecoin(const bool& is_buy, const symbol& coin,const name& contract_name){
    auto fiat_conf = fiat_conf_t( contract_name );
    CHECKC( _db.get(fiat_conf),err::RECORD_FOUND, "conf not existing : " + contract_name.to_string())
    CHECKC( has_auth(_self) || has_auth(fiat_conf.managers.at(manager_type::admin)), err::NO_AUTH, "Missing required authority of admin or managers" )

    if(is_buy){
        CHECKC( fiat_conf.buy_coins_conf.count(coin), err::RECORD_NOT_FOUND, "coin not in buy coin list" )
        fiat_conf.buy_coins_conf.erase(coin);
    }
    else {
        CHECKC( fiat_conf.sell_coins_conf.count(coin), err::RECORD_NOT_FOUND, "coin not in sell coin list" ) 
        fiat_conf.sell_coins_conf.erase(coin);
    }
    _db.set(fiat_conf);
}

void otcconf::setfeepct(const uint64_t& feepct,const name& contract_name){
    auto fiat_conf = fiat_conf_t( contract_name );
    CHECKC( _db.get(fiat_conf),err::RECORD_FOUND, "conf not existing : " + contract_name.to_string())
    CHECKC( has_auth(_self) || has_auth(fiat_conf.managers.at(manager_type::admin)), err::NO_AUTH, "Missing required authority of admin or managers" )

    // require_auth(_gstate.managers.at(manager_type::admin));
    CHECKC(feepct >= 0, err::NOT_POSITIVE, "unsupport negtive fee");
    fiat_conf.fee_pct = feepct;
    _db.set(fiat_conf);
}

void otcconf::setsettlelv(const vector<settle_level_config>& configs,const name& contract_name){
    auto fiat_conf = fiat_conf_t( contract_name );
    CHECKC( _db.get(fiat_conf),err::RECORD_FOUND, "conf not existing : " + contract_name.to_string())
    CHECKC( has_auth(_self) || has_auth(fiat_conf.managers.at(manager_type::admin)), err::NO_AUTH, "Missing required authority of admin or managers" )

    fiat_conf.settle_levels = configs;
    _db.set(fiat_conf);
}

void otcconf::setswapstep(const vector<swap_step_config> rates,const name& contract_name)
{
    auto fiat_conf = fiat_conf_t( contract_name );
    CHECKC( _db.get(fiat_conf),err::RECORD_FOUND, "conf not existing : " + contract_name.to_string())
    CHECKC( has_auth(_self) || has_auth(fiat_conf.managers.at(manager_type::admin)), err::NO_AUTH, "Missing required authority of admin or managers" )

    fiat_conf.swap_steps = rates;
    _db.set(fiat_conf);
}

void otcconf::setfarm(const name& farmname, const uint64_t& farm_lease_id, const symbol_code& symcode, const uint32_t& farm_scale,const name& contract_name){
    auto fiat_conf = fiat_conf_t( contract_name );
    CHECKC( _db.get(fiat_conf),err::RECORD_FOUND, "conf not existing : " + contract_name.to_string())
    CHECKC( has_auth(_self) || has_auth(fiat_conf.managers.at(manager_type::admin)), err::NO_AUTH, "Missing required authority of admin or managers" )

    fiat_conf.managers[manager_type::aplinkfarm] = farmname;
    fiat_conf.farm_lease_id = farm_lease_id;
    CHECKC(farm_scale >= 0, err::NOT_POSITIVE, "farm scale value invalid");
    fiat_conf.farm_scales[symcode] = farm_scale;
    _db.set(fiat_conf);
}

void otcconf::setappname(const name& otc_name,const name& contract_name) {
    auto fiat_conf = fiat_conf_t( contract_name );
    CHECKC( _db.get(fiat_conf),err::RECORD_FOUND, "conf not existing : " + contract_name.to_string())
    CHECKC( has_auth(_self) || has_auth(fiat_conf.managers.at(manager_type::admin)), err::NO_AUTH, "Missing required authority of admin or managers" )

    fiat_conf.app_info.app_name = otc_name;
    _db.set(fiat_conf);
}

void otcconf::setstatus(const name& status,const name& contract_name){
    auto fiat_conf = fiat_conf_t( contract_name );
    CHECKC( _db.get(fiat_conf),err::RECORD_FOUND, "conf not existing : " + contract_name.to_string())
    CHECKC( has_auth(_self) || has_auth(fiat_conf.managers.at(manager_type::admin)), err::NO_AUTH, "Missing required authority of admin or managers" )

    CHECKC( status == conf_status::UN_INITIALIZE
            || status == conf_status::INITIALIZED
            || status == conf_status::RUNNING
            || status == conf_status::MAINTAINING , err::PARAM_ERROR, "status type error: " + status.to_string())

    fiat_conf.status = status;
    _db.set(fiat_conf);
}

void otcconf::settimeout(const uint64_t& accepted_timeout, const uint64_t& payed_timeout,const name& contract_name) {
    auto fiat_conf = fiat_conf_t( contract_name );
    CHECKC( _db.get(fiat_conf),err::RECORD_FOUND, "conf not existing : " + contract_name.to_string())
    CHECKC( has_auth(_self) || has_auth(fiat_conf.managers.at(manager_type::admin)), err::NO_AUTH, "Missing required authority of admin or managers" )

    fiat_conf.accepted_timeout = accepted_timeout;
    fiat_conf.payed_timeout = payed_timeout;
    _db.set(fiat_conf);
}

void otcconf::setconf( const fiat_conf_t& conf ) {
    require_auth( _self );
    CHECKC( is_account(conf.contract_name), err::PARAM_ERROR,"contract name invalid: " + conf.contract_name.to_string());
    CHECKC( conf.status == conf_status::UN_INITIALIZE
            || conf.status == conf_status::INITIALIZED
            || conf.status == conf_status::RUNNING
            || conf.status == conf_status::MAINTAINING , err::PARAM_ERROR, "status type error: " + conf.status.to_string())
    CHECKC( conf.fee_pct >= 0, err::NOT_POSITIVE, "unsupport negtive fee");

    auto fiat_conf = fiat_conf_t( conf.contract_name );
    if ( !_db.get(fiat_conf) ){
        _db.set(_self.value,conf,false);
    } else {
        _db.set(conf);
    }
}

}  //end of namespace:: otc
