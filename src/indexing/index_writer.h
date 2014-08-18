#pragma once
#include <iostream>
#include "../hellcat.h"
#include "../string_ref.h"
#include "../storage/store.h"

using namespace hellcat::storage;

namespace hellcat {
    namespace indexing {
        
        class IndexWriter
        {
        public:
            IndexWriter(std::string_ref keyspace);
            ~IndexWriter();
            void SetRecord(std::string_ref term, uint32_t record_id, hcat_transaction* tx);
        private:
            std::string_ref keyspace;
        };
    }
}
