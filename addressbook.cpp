#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>



using namespace eosio;

class addressbook : public contract {
    public:
    using contract::contract;
    addressbook(account_name self): contract(self){}

    [[eosio::action]]
    void upsert(account_name user,
        std::string first_name,
        std::string last_name,
        std::string street,
        std::string city,
        std::string state
    ){
        require_auth(user);
        address_index addresses(_self, _self);
        auto iterator=addresses.find(user);
        if(addresses.find(user)==addresses.end())
        {
            addresses.emplace(user, [&](auto& row){
                row.key = user;
                row.first_name= first_name;
                row.last_name=last_name;
                row.street=street;
                row.city=city;
                row.state=state;
            });
            send_summary(user, "successfully emplaced");
            increment_counter(user, "emplace");


        }
        else{
            addresses.modify(iterator , user, [&](auto & row){
                row.first_name=first_name;
                row.last_name=last_name;
                row.street=street;
                row.city=city;
                row.state=state;
            });
            send_summary(user, "successfully modified");
            increment_counter(user, "modify");

        }     

    }
    [[eosio::action]]
    void erase(account_name user){
        require_auth(user);
        address_index addresses(_self, _self);
        auto iterator=addresses.find(user);
        eosio_assert(iterator != addresses.end(), "Record does not exist");
        addresses.erase(iterator);
        send_summary(user, "successfully erased");
        increment_counter(user, "erase");
    }
      [[eosio::action]]

    void notify(account_name user, std::string msg) {
        require_auth(get_self());
        require_recipient(user);
    }
  
    private:
      struct [[eosio::table]] person {
        account_name key;
        std::string first_name;
        std::string last_name;
        std::string street;
        std::string city;
        std::string state;
        uint64_t primary_key() const {return key;}   
    };

    typedef eosio::multi_index<N(people), person> address_index;
    void send_summary(account_name user, std::string message){
        action(
            permission_level(get_self(), N(active)),
            get_self(),
            N(notify),
            std::make_tuple(user, name{user}.to_string()+message)
        ).send();
    }

     void increment_counter(account_name user, std::string type){
        action(
            permission_level(get_self(), N(active)),
            N(abcounter),
            N(count),
            std::make_tuple(user, type)
        ).send();
    }
    

    

};

EOSIO_ABI( addressbook, (upsert)(erase)(notify) )
// a new comment
// two new comments
