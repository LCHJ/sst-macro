

#include <stdio.h>
#include <sstmac/replacements/mpi.h>

namespace errring {
#define MPIR_ERR_FATAL 1
#define MPIR_ERR_RECOVERABLE 0
int MPIR_Err_create_code(int, int, char *, int, int, const char [], const char [], ...);
void MPIR_Err_print_stack(FILE * fp, int errcode);

int errring(int argc, char **argv)
{
    int err;
    
    MPI_Init( 0, 0 );

    err = MPIR_Err_create_code(MPI_ERR_INTERN, MPIR_ERR_RECOVERABLE, "main", __LINE__, MPI_ERR_UNKNOWN, "**buffer", 0);
    err = MPIR_Err_create_code(err, MPIR_ERR_RECOVERABLE, "main", __LINE__, MPI_ERR_UNKNOWN, "**count", 0);
    err = MPIR_Err_create_code(err, MPIR_ERR_RECOVERABLE, "main", __LINE__, MPI_ERR_UNKNOWN, "**dtype", 0);
    MPIR_Err_print_stack(stdout, err);
    
    MPI_Finalize();
    return 0;
}


}
