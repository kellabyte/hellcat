#pragma once
#include "../hellcat.h"
#include "../string_ref.h"
#include "../storage/store.h"

using namespace hellcat::storage;

namespace hellcat {
    namespace indexing {
        
        class IndexDictionary
        {
        public:
            IndexDictionary(std::string_ref keyspace);
            ~IndexDictionary();
            uint32_t GetCurrentTermCount(hcat_transaction* tx);
            uint32_t AddTerm(std::string_ref name, hcat_transaction* tx);
            uint32_t GetTermId(std::string_ref term, hcat_transaction* tx);
        private:
            std::string_ref keyspace;

            void SetCurrentTermCount(uint32_t count, hcat_transaction* tx);
            void SetTermId(std::string_ref term, uint32_t id, hcat_transaction* tx);
        };
    }
}
