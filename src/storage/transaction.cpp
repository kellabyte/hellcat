#include <iostream>
#include <stdlib.h>
#include "../hellcat.h"
#include "store.h"
#include "transaction.h"

namespace hellcat {
    namespace storage {
        
        Transaction::Transaction(Store* store, void* transaction_context)
        {
            this->store = store;
            this->transaction_context = transaction_context;
        }
        
        Transaction::~Transaction()
        {
            free(this->transaction_context);
        }
        
        int Transaction::commit()
        {
            this->store->commit_transaction(this->transaction_context);
            return HCAT_SUCCESS;
        }
        
        int Transaction::abort()
        {
            this->store->abort_transaction(this->transaction_context);
            return HCAT_SUCCESS;
        }
        
        int Transaction::get(hcat_keypair* pair)
        {
            int rc = this->store->get(pair, this->transaction_context);
            return rc;
        }
        
        int Transaction::set(hcat_keypair* pair)
        {
            this->store->set(pair, this->transaction_context);
            return HCAT_SUCCESS;
        }
    }
}