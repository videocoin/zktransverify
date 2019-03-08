#include <apps/mergesort_merkle.h>
#include <storage/ram_impl.h>
#include <storage/hasher.h>
#include <storage/configurable_block_store.h>

void mergesort_merkle_input_gen (mpq_t * input_q, int num_inputs, char *argv[]) {

    char db_file_path[BUFLEN];
    snprintf(db_file_path, BUFLEN - 1, "%s/block_stores/prover_%s", FOLDER_STATE, "default_shared_db");
    ConfigurableBlockStore bs(db_file_path);
    RAMImpl ram(&bs);
    HashType* hash = ram.getRootHash();

    int i = 0;
    for (HashType::HashVec::const_iterator itr = hash->GetFieldElts().begin();
         itr != hash->GetFieldElts().end(); ++itr) {
        mpz_set(mpq_numref(input_q[i]), (*itr).get_mpz_t());
        mpq_canonicalize(input_q[i]);
        i++;
    }

    int j;
    srand(time(NULL));
    for(j = 0 ; j < MAX_SIZE; j++) {
        mpq_set_ui(input_q[i+j], rand() % 1024, 1);
    }

    for(int k=i+j; k<num_inputs; k++) {
        mpq_set_ui(input_q[k], 0, 1);
    }
}
