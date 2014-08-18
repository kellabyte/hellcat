#include <iostream>
#include "index_dictionary.h"
#include "../hellcat.h"
#include "../string_ref.h"
#include "../storage/store.h"

using namespace hellcat::storage;

namespace hellcat {
    namespace indexing {
        
        IndexDictionary::IndexDictionary(std::string_ref keyspace)
        {
            this->keyspace = keyspace;
        }
        
        IndexDictionary::~IndexDictionary()
        {
            
        }
        
        uint32_t IndexDictionary::GetCurrentTermCount(hcat_transaction* tx)
        {
            hcat_keypair pair;
            pair.keyspace = this->keyspace;
            pair.key = string_ref("$current_term");
            
            int rc = tx->get(&pair);
            if (rc == 0)
            {
                uint32_t count = *reinterpret_cast<uint32_t*>(pair.value);
                return count;
            }
            else
            {
                // Key doesn't exist, lets create it.
                //pair.value = string_ref((char*)0);
                //tx->set(&pair);
                //tx->commit();
            }
            return 0;
        }
        
        void IndexDictionary::SetCurrentTermCount(uint32_t count, hcat_transaction* tx)
        {
            hcat_keypair pair;
            pair.keyspace = this->keyspace;
            pair.key = string_ref("$current_term");
            pair.value = &count;
            pair.value_length = sizeof(uint32_t);
            tx->set(&pair);
        }
        
        uint32_t IndexDictionary::GetTermId(std::string_ref term, hcat_transaction* tx)
        {
            //string term_count_key;
            //term_count_key.append(term.data());
            //term_count_key.append("$term");

            hcat_keypair pair;
            pair.keyspace = this->keyspace;
            pair.key = term.data(); //term_count_key;
            if (tx->get(&pair) == HCAT_SUCCESS)
            {
                uint32_t count = *reinterpret_cast<uint32_t*>(pair.value);
                return count;
            }
            return 0;
        }
        
        void IndexDictionary::SetTermId(std::string_ref term, uint32_t id, hcat_transaction* tx)
        {
            //string term_count_key;
            //term_count_key.append(term.data());
            //term_count_key.append("$term");
            
            hcat_keypair pair;
            pair.keyspace = this->keyspace;
            pair.key = term.data(); //term_count_key;
            pair.value = &id;
            pair.value_length = sizeof(uint32_t);
            tx->set(&pair);
        }
        
        uint32_t IndexDictionary::AddTerm(std::string_ref term, hcat_transaction* tx)
        {
            uint32_t term_id = GetTermId(term, tx);
            if (term_id == 0)
            {
                term_id = GetCurrentTermCount(tx);
                term_id++;
                SetTermId(term, term_id, tx);
                SetCurrentTermCount(term_id, tx);
            }
            return term_id;
            //cout << term.data() << " " << term_id << endl;
        }
    }
}
