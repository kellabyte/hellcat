#pragma once
#include <stdlib.h>
#include "store.h"

namespace hellcat {
    namespace storage {
        
        class Transaction : public hcat_transaction
        {
        public:
            Transaction(Store* store, void* transaction_context);
            ~Transaction();
            int commit();
            int abort();
            int get(hcat_keypair* pair);
            int set(hcat_keypair* pair);
            
        private:
            Store* store;
            void* transaction_context;
        };
        
    }
}