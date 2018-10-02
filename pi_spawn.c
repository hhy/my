#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>

int worker(int argc, char *argv[]) 
{
   printf("<<<<<<<<<worker starting\n");
   
   int size;
   for(int i=0; i<argc; i++){
       printf("[worker] %d- %s\n", i, argv[i]);
   }

   MPI_Comm parent; 
   MPI_Init(&argc, &argv);
   
   int rank;
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );

   
   MPI_Comm_get_parent(&parent); 
   if (parent == MPI_COMM_NULL) error(1,1,"No parent!"); 
   MPI_Comm_remote_size(parent, &size); 
   if (size != 1) error(1, 1,"Something's wrong with the parent");
   printf("[worker %d] size: %d\n", rank, size);

   /*
   int MPI_Recv(void *buf, int count, MPI_Datatype datatype,
    int source, int tag, MPI_Comm comm, MPI_Status *status)
   */
   char buf[3333];
   MPI_Status status;
   printf("[worker %d] start receiving\n");
   int r= MPI_Recv(buf, 39, MPI_CHAR, 0, 1, parent, &status);
   printf("receive from parent: %d\n", r);


   
   /* 
    * Parallel code here.  
    * The manager is represented as the process with rank 0 in (the remote 
    * group of) the parent communicator.  If the workers need to communicate 
    * among themselves, they can use MPI_COMM_WORLD. 
    */

   

   MPI_Finalize();
   printf("worker is done\n");
   return 0; 
} 


int main(int argc, char *argv[]) 
{ 
   int world_size, universe_size, *universe_sizep, flag; 
   MPI_Comm everyone;           /* intercommunicator */ 
   char worker_program[100];
   /*
   for(int i=0; i<argc; i++){
       printf("[worker] %d- %s\n", i, argv[i]);
       //return 0;
   }
   */

   //if(0==strcmp("worker", argv[1])){
   //if(0!=strcmp("master", argv[1])){
   //if(argc<2 && 0!=strcmp("master", argv[1])){
   if(argc!=2){
       printf("---------------this is worker process\n");
       return worker(argc, argv);
   }

   
   MPI_Init(&argc, &argv); 
   MPI_Comm_size(MPI_COMM_WORLD, &world_size);
   printf("world size: %d\n", world_size);
   
 
   if (world_size != 1)    error(1, 1, "Top heavy with management"); 
 
   MPI_Comm_get_attr(MPI_COMM_WORLD, MPI_UNIVERSE_SIZE,  
                     &universe_sizep, &flag);
   
   *universe_sizep=3;
   printf("universe size: %d, flag: %d, invalid value: %d\n", *universe_sizep, flag, MPI_KEYVAL_INVALID);
   //MPI_Finalize();
   //return 0;
   
   if (!flag) { 
        printf("This MPI does not support UNIVERSE_SIZE. How many\n \
processes total?"); 
        scanf("%d", &universe_size); 
   } else universe_size = *universe_sizep; 
   if (universe_size == 1) error(1, 1, "No room to start workers");
   int rank;
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   printf("[master rank]: %d\n", rank);
   /*  
    * Now spawn the workers. Nopte that there is a run-time determination 
    * of what type of worker to spawn, and presumably this calculation must 
    * be done at run time and cannot be calculated before starting 
    * the program. If everything is known when the application is  
    * first started, it is generally better to start them all at once 
    * in a single MPI_COMM_WORLD.  
    */ 
 
   //choose_worker_program(worker_program);

   strcpy(worker_program,argv[0]);//, strlen("./a.out"));
   printf("worker program : %s\n", worker_program);

    printf("[master]start spawning\n");
    MPI_Comm_spawn(worker_program, MPI_ARGV_NULL/* workerargv*/, universe_size-1,  
             MPI_INFO_NULL, 0, MPI_COMM_SELF, &everyone,  
             MPI_ERRCODES_IGNORE);
    
    printf("[master] spawned\n");
    char b[]="msg from master";
    //int b=444;
    
    //MPI_Bcast( &b, strlen(b), MPI_CHAR, MPI_ROOT, everyone);
    /*
    int MPI_Bsend(const void *buf, int count, MPI_Datatype datatype,
    int dest, int tag, MPI_Comm comm)
    */

    MPI_Bsend( &b, strlen(b), MPI_CHAR, 0, 1,everyone);
    MPI_Bsend( &b, strlen(b), MPI_CHAR, 1, 1,everyone);
   

   //char myport[]="masterport";//[MPI_MAX_PORT_NAME];
    
    /* ... */

 
    //MPI_Comm_accept(myport, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercomm);


   
   /* 
    * Parallel code here. The communicator "everyone" can be used 
    * to communicate with the spawned processes, which have ranks 0,.. 
    * MPI_UNIVERSE_SIZE-1 in the remote group of the intercommunicator 
    * "everyone". 
    */ 
 
   MPI_Finalize();
   printf("master is done\n");
   return 0; 
} 


