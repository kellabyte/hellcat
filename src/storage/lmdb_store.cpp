#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include "lmdb.h"
#include "../string_ref.h"
#include "../hellcat.h"
#include "lmdb_store.h"
#include "transaction.h"

typedef map<string, MDB_dbi> keyspace_map;

namespace hellcat {
    namespace storage {
        
        LMDBStore::LMDBStore()
        {
            keyspaces = unique_ptr<keyspace_map>(new keyspace_map());
        }
        
        LMDBStore::~LMDBStore()
        {
        }
        
        int LMDBStore::open(const char *path, bool durable)
        {
            int rc;
            rc = mdb_env_create(&env);
            rc = mdb_env_set_mapsize(env, size_t(1048576000));
            rc = mdb_env_set_maxdbs(env, 1024);

            rc = mdb_env_open(env, path, MDB_NOSYNC | MDB_WRITEMAP, 0664);

            MDB_txn *txn;
            rc = mdb_txn_begin(env, NULL, 0, &txn);
            rc = mdb_open(txn, NULL, 0, &dbi);
            rc = mdb_txn_commit(txn);
            
            return HCAT_SUCCESS;
        }
        
        void LMDBStore::close()
        {
            mdb_close(env, dbi);
            mdb_env_close(env);
        }
        
        int LMDBStore::commit_transaction(void* transaction_context)
        {
            lmdb_transaction_context* context = (lmdb_transaction_context*)transaction_context;
            int rc = mdb_txn_commit(context->transaction);
            return (rc == 0 ? HCAT_SUCCESS : HCAT_FAIL);
        }
        
        int LMDBStore::abort_transaction(void* transaction_context)
        {
            lmdb_transaction_context* context = (lmdb_transaction_context*)transaction_context;
            mdb_txn_abort(context->transaction);
            return HCAT_SUCCESS;
        }
        
        int LMDBStore::set(hcat_keypair* pair, void* transaction_context)
        {
            int rc;
            lmdb_transaction_context* context = (lmdb_transaction_context*)transaction_context;

            MDB_dbi db_instance = dbi;
            if (pair->keyspace.length() > 0)
            {
                db_instance = keyspaces->operator[](pair->keyspace.data());
            
                if (db_instance == NULL)
                {
                    rc = mdb_dbi_open(context->transaction, pair->keyspace.data(), MDB_CREATE, &db_instance);
                    if (rc == MDB_SUCCESS)
                    {
                        keyspaces->operator[](pair->keyspace.data()) = db_instance;
                    }
                    else
                    {
                        // TODO: Return an error.
                    }
                }
            }

            MDB_val mdb_key;
            MDB_val mdb_value;
            
            mdb_key.mv_size = pair->key.length() + 1;
            mdb_key.mv_data = (void*)pair->key.data();
            
            //mdb_value.mv_size = pair->value.length() + 1;
            //mdb_value.mv_data = (void*)pair->value.data();
            mdb_value.mv_size = pair->value_length + 1;
            mdb_value.mv_data = pair->value;
            
            rc = mdb_put(context->transaction, db_instance, &mdb_key, &mdb_value, 0);

            return (rc == 0 ? HCAT_SUCCESS : HCAT_FAIL);
        }
        
        int LMDBStore::get(hcat_keypair* pair, void* transaction_context)
        {
            int rc;
            lmdb_transaction_context* context = (lmdb_transaction_context*)transaction_context;

            MDB_dbi db_instance = dbi;
            if (pair->keyspace.length() > 0)
            {
                db_instance = keyspaces->operator[](pair->keyspace.data());

                //if (db_instance == NULL)
                //{
                    rc = mdb_dbi_open(context->transaction, pair->keyspace.data(), MDB_CREATE, &db_instance);
                    if (rc == MDB_SUCCESS)
                    {
                        keyspaces->operator[](pair->keyspace.data()) = db_instance;
                    }
                    else
                    {
                        // TODO: Return an error.
                    }
                //}
            }

            MDB_val mdb_key;
            MDB_val mdb_value;
            MDB_cursor *cursor;
            
            mdb_key.mv_size = pair->key.length() + 1;
            auto blah = pair->key.data();
            mdb_key.mv_data = (char*)pair->key.data();
            
            // TODO: Why am I using a cursor here? I forget.
            // Use mdb_get() instead.
            int return_code = HCAT_SUCCESS;
            rc = mdb_cursor_open(context->transaction, db_instance, &cursor);
            rc = mdb_cursor_get(cursor, &mdb_key, &mdb_value, MDB_SET);
            //rc = mdb_cursor_get(cursor, &mdb_key, &mdb_value, MDB_NEXT);
            
            switch(rc)
            {
                case MDB_SUCCESS:
                {
                    //char* buffer = new char[mdb_value.mv_size];
                    //memcpy(buffer, (char **)mdb_value.mv_data, mdb_value.mv_size);
                    ////pair->value = string_ref(buffer, mdb_value.mv_size);
                    //pair->value = buffer;
                    pair->value = mdb_value.mv_data;
                    pair->value_length = mdb_value.mv_size;
                    break;
                }
                case MDB_NOTFOUND:
                {
                    return_code = HCAT_KEYNOTFOUND;
                    break;
                }
            }
            
            //printf("%s\t%s\n", (char *)mdb_key.mv_data, (char *)mdb_value.mv_data);
            //while ((rc = mdb_cursor_get(cursor, &mdb_key, &mdb_value, MDB_NEXT)) == 0) {
            //	printf("key: %p %.*s, data: %p %.*s\n",
            //		mdb_key.mv_data,  (int) mdb_key.mv_size,  (char *) mdb_key.mv_data,
            //		mdb_value.mv_data, (int) mdb_value.mv_size, (char *) mdb_value.mv_data);
            //}
            
            mdb_cursor_close(cursor);
            return return_code;
        }
        
        int LMDBStore::begin_transaction(hcat_transaction** tx, int read_only)
        {
            int rc;
            MDB_txn* txn = NULL;
            if (txn == NULL)
            {
                unsigned int transaction_flag = 0;
                if (read_only)
                {
                    transaction_flag = MDB_RDONLY;
                }
                rc = mdb_txn_begin(env, NULL, transaction_flag, &txn);
            }
            
            lmdb_transaction_context* context = new lmdb_transaction_context();
            context->transaction = txn;
            Transaction* trans = new Transaction(this, context);
            *tx = trans;
            
            return HCAT_SUCCESS;
        }
        
        int LMDBStore::sync()
        {
            int rc = mdb_env_sync(env, 1);
            return (rc == 0 ? HCAT_SUCCESS : HCAT_FAIL);
        }
    }
}
