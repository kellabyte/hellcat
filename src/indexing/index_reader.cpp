#include <iostream>
//#include <chrono>
//#include <thread>
//#include "../simd_compression/codecfactory.h"
//#include "../simd_compression/intersection.h"
#include "index_reader.h"

using namespace std;
//using namespace chrono;

namespace hellcat {
    namespace indexing {
        
        const int amount = 250000000;
        
        // We pick a CODEC, could use others, e.g., "varint", "s-fastpfor-1"
        //IntegerCODEC &codec =  * CODECFactory::getFromName("s4-bp128-m");
        // using SIMD intersection
        //intersectionfunction inter;
        
        vector<uint32_t> docsets [1024];
        vector<uint32_t> instersect_with_docsets [1024];
        
        void IndexReader::Read()
        {
            
        }
        
        /*
         int main2(int argc, char* argv[]) {
         std::stringstream output;
         std::locale comma_locale(std::locale(), new comma_numpunct());
         output.imbue(comma_locale);
         
         int num_threads = 1;
         if (argc > 1)
         {
         num_threads = std::stoi(argv[1]);
         }
         
         inter = IntersectionFactory::getFromName("simd");
         std::chrono::time_point<std::chrono::high_resolution_clock> start;
         
         if (num_threads > 1)
         {
         vector<std::thread> threads;
         
         // Create docsets.
         for (int i=0; i<num_threads; i++)
         {
         threads.emplace_back(std::thread(create_docset, i));
         }
         
         for(auto& thread : threads)
         {
         thread.join();
         }
         threads.clear();
         
         // Insersect.
         cout << "Intersecting..." << endl;
         start = high_resolution_clock::now();
         
         for (int i=0; i<num_threads; i++)
         {
         threads.emplace_back(std::thread(intersect_docset, i));
         }
         for(auto& thread : threads)
         {
         thread.join();
         }
         }
         else
         {
         create_docset(0);
         cout <<"Skipping threading" << endl;
         cout << "Intersecting..." << endl;
         start = high_resolution_clock::now();
         intersect_docset(0);
         }
         
         auto end = high_resolution_clock::now();
         nanoseconds elapsed = duration_cast<nanoseconds>(end - start);
         print_results(output, amount * num_threads, elapsed);
         cout << output.str();
         }
         
         void create_docset(int threadid)
         {
         docsets[threadid] = create_and_compress_docset(amount);
         instersect_with_docsets[threadid] = create_docset_to_interect_with(amount);
         }
         
         void intersect_docset(int threadid)
         {
         intersect_docs(docsets[threadid], instersect_with_docsets[threadid]);
         }
         
         vector<uint32_t> create_and_compress_docset(size_t amount)
         {
         ////////////
         //
         // create a container with some integers in it
         //
         // We need the integers to be in sorted order.
         //
         // (Note: You don't need to use a vector.)
         //
         size_t N = amount;
         vector<uint32_t> mydata(N);
         for (uint32_t i = 0; i < N; ++i) mydata[i] = 3 * i;
         ///////////
         //
         // You need some "output" container. You are responsible
         // for allocating enough memory.
         //
         vector<uint32_t> compressed_output(N + 1024);
         // N+1024 should be plenty
         //
         //
         size_t compressedsize = compressed_output.size();
         codec.encodeArray(mydata.data(), mydata.size(),
         compressed_output.data(), compressedsize);
         //
         // if desired, shrink back the array:
         compressed_output.resize(compressedsize);
         compressed_output.shrink_to_fit();
         // display compression rate:
         cout << setprecision(3);
         cout << "You are using " << 32.0 * static_cast<double>(compressed_output.size()) /
         static_cast<double>(mydata.size()) << " bits per integer. " << endl;
         
         return mydata;
         }
         
         vector<uint32_t> decompress_docset(vector<uint32_t>& compressed_docset, size_t amount)
         {
         ///
         // decompressing is also easy:
         //
         vector<uint32_t> mydataback(amount);
         size_t recoveredsize = mydataback.size();
         //
         codec.decodeArray(compressed_docset.data(),
         compressed_docset.size(), mydataback.data(), recoveredsize);
         mydataback.resize(recoveredsize);
         
         //
         // That's it for compression!
         //
         //if (mydataback != mydata) throw runtime_error("bug!");
         
         return mydataback;
         }
         
         vector<uint32_t> create_docset_to_interect_with(size_t amount)
         {
         //
         // Next we are going to test out intersection...
         //
         vector<uint32_t> mydata2(amount);
         for (uint32_t i = 0; i < amount; ++i) mydata2[i] = 6 * i;
         
         return mydata2;
         }
         
         void intersect_docs(vector<uint32_t>& docset, vector<uint32_t>& instersect_with_docset)
         {
         size_t intersize = inter(instersect_with_docset.data(), instersect_with_docset.size(), docset.data(), docset.size(), instersect_with_docset.data());
         //cout << "Resizing..." << endl;
         //instersect_with_docset.resize(intersize);
         //instersect_with_docset.shrink_to_fit();
         //cout << "Intersection size: " << instersect_with_docset.size() << "  integers. " << endl;
         }
         */
    }
}
