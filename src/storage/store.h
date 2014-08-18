#pragma once
#include "../hellcat.h"

namespace hellcat {
    namespace storage {
        
        class Store
        {
        public:
            virtual int open(const char *path, bool durable) = 0;
            virtual void close() = 0;
            virtual int get(hcat_keypair* pair, void* transaction_context) = 0;
            virtual int set(hcat_keypair* pair, void* transaction_context) = 0;
            virtual int begin_transaction(hcat_transaction** tx, int read_only) = 0;
            virtual int commit_transaction(void* transaction_context) = 0;
            virtual int abort_transaction(void* transaction_context) = 0;
            virtual int sync() = 0;
        };
    }
}