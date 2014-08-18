#include "index_writer.h"
#include <iostream>
#include <sstream>
#include "../simd_compression/codecfactory.h"
#include "../simd_compression/intersection.h"
#include "../hellcat.h"
#include "../string_ref.h"
#include "../storage/store.h"

using namespace hellcat::storage;

//const uint32_t segment_size = 128;
const uint32_t segment_size = 1024;

namespace hellcat {
    namespace indexing {
        IndexWriter::IndexWriter(std::string_ref keyspace)
        {
            this->keyspace = keyspace;
        }
        
        IndexWriter::~IndexWriter()
        {
        }
        
        void IndexWriter::SetRecord(std::string_ref term, uint32_t record_id, hcat_transaction* tx)
        {
            /*
            auto segment_index = record_id % segment_size;
            auto segment_id = (record_id / segment_size) + (segment_size > 0);
            
            std::ostringstream stream;
            stream << term.data() << ":" << segment_id;
            
            hcat_keypair pair;
            pair.keyspace = this->keyspace;
            pair.key = stream.str();
            
            if (tx->get(&pair) == HCAT_SUCCESS)
            {
                //uint32_t count = *reinterpret_cast<uint32_t*>(pair.value);
            }
            else
            {
                uint32_t* segment[segment_size];
                segment[segment_index] =
                
            }
            */
            
            //std::ostringstream key;
            //key << "$term:" << term.data();
            std::string key;
            key.reserve(6 + term.length());
            key.append("$term:");
            key.append(term.data());
            
            hcat_keypair pair;
            pair.keyspace = this->keyspace;
            pair.key = key;
            
            if (tx->get(&pair) == HCAT_SUCCESS)
            {
                //cout << "LEN: " << pair.value_length << endl;
                uint32_t* segment = (uint32_t*)pair.value;
                for (int i=0; i< segment_size; i++)
                {
                    //cout << i << "\t" << segment[i] << endl;
                    if (segment[i] == 0)
                    {
                        //cout << i << "\t" << key.str() << "\t\tREC: " << record_id << endl;
                        segment[i] = record_id;
                        tx->set(&pair);
                        break;
                    }
                }
            }
            else
            {
                uint32_t* segment = new uint32_t[segment_size];
                segment[0] = record_id;
                pair.value = segment;
                pair.value_length = segment_size;
                tx->set(&pair);
            }
        }
    }
}
