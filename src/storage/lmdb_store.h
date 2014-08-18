#pragma once
#include <memory>
#include <map>
#include "lmdb.h"
#include "../hellcat.h"
#include "store.h"

using namespace std;

namespace hellcat {
    namespace storage {
        
        typedef struct
        {
            MDB_txn* transaction;
        } lmdb_transaction_context;
        
        class LMDBStore : public Store
        {
        public:
            LMDBStore();
            ~LMDBStore();
            int open(const char *path, bool durable);
            void close();
            int get(hcat_keypair* pair, void* transaction_context);
            int set(hcat_keypair* pair, void* transaction_context);
            int begin_transaction(hcat_transaction** transaction, int read_only);
            int commit_transaction(void* transaction_context);
            int abort_transaction(void* transaction_context);
            int sync();
        private:
            MDB_env* env;
            MDB_dbi dbi;
            unique_ptr<map<string, MDB_dbi>> keyspaces;
        };
        
    }
}
