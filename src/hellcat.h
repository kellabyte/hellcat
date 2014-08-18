#pragma once
#include "string_ref.h"

using namespace std;

typedef struct
{
    std::string_ref keyspace;
    std::string_ref key;
    void* value;
    uint32_t value_length;
} hcat_keypair;

class hcat_transaction
{
public:
    virtual ~hcat_transaction() { };
    virtual int commit() = 0;
    virtual int abort() = 0;
    virtual int get(hcat_keypair* pair) = 0;
    virtual int set(hcat_keypair* pair) = 0;
};

#define HCAT_SUCCESS              0
#define HCAT_FAIL                 1
#define HCAT_KEYSPACENOTFOUND     300
#define HCAT_KEYNOTFOUND          301
