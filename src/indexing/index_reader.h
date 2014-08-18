#pragma once
#include "../storage/store.h"

using namespace hellcat::storage;

namespace hellcat {
    namespace indexing {
        
        class IndexReader
        {
        public:
            IndexReader();
            ~IndexReader();
            void Read();
        private:
            Store* store;
        };
    }
}