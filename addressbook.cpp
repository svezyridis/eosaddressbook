#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>



using namespace eosio;


class addressbook : public contract {
    public:
    using contract::contract;
    addressbook(account_name self): contract(self){}

    [[eosio::action]]
    void print(uint64_t key){
        address_index addresses(_self, _self);
        auto idx = addresses.get_index<N(sec_key)>();
        auto itr = idx.lower_bound(key);
        for(; (itr!=idx.end()&&itr->sec_key==key); itr++){
            eosio::print("city= ",itr->city, " \n");
        }
        eosio::print("new line");

    }

    [[eosio::action]]
    void upsert(account_name user,
        std::string first_name,
        std::string last_name,
        std::string street,
        std::string city,
        std::string state,
        uint64_t sec_key
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
                row.sec_key = sec_key;
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
                row.sec_key=sec_key;
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
        uint64_t sec_key;
        std::string first_name;
        std::string last_name;
        std::string street;
        std::string city;
        std::string state;
        uint64_t primary_key() const {return key;}
        uint64_t secondary_key() const {return sec_key;} 
        EOSLIB_SERIALIZE(person, (key)(sec_key)(first_name)(last_name)(street)(city)(state))  
    };

    typedef eosio::multi_index<N(person), person, 
        indexed_by<N(sec_key), const_mem_fun<person, uint64_t, &person::secondary_key>>
        > address_index;
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

<<<<<<< HEAD
EOSIO_ABI( addressbook, (upsert)(erase)(notify)(print) )
// a new comment
// two new comments
=======
EOSIO_ABI( addressbook, (upsert)(erase)(notify) )
>>>>>>> 7ba99772d0a86214253b2347cfbf79dbf766d429
