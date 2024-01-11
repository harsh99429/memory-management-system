/*
All the main functions with respect to the MeMS are inplemented here
read the function discription for more details

NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
you are only allowed to implement the functions 
you can also make additional helper functions a you wish

REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add othter headers as required


// intiitalize void* mem
//test adress conituity and write code for new main node creation for hole not big enough0
#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<stdint.h>
struct main_node
{
   struct main_node* next;
   struct main_node* prev;

   struct  sub* head_sub;
   size_t size;

   void* start_this;
   void* page_ptr;
   size_t rem_mem;

   uintptr_t vir_start;
   uintptr_t vir_end;
   
};

struct sub
{
    struct sub* next;
    struct sub* prev;

    void* start_this;
   

    void* mem_start;
    void* mem_end;
    size_t mem_size;
    int hole;

    uintptr_t mem_phys_start;
   uintptr_t mem_phys_end;
   uintptr_t mem_vir_start;
   uintptr_t mem_vir_end;
    //hole=0 indicates a process hole=1 indicates its a hole
    
};

void printSubStructure(const struct sub* sub) {
    printf("Sub Structure:\n");
    printf("next = %lu\n", (uintptr_t)sub->next);
    printf("prev = %lu\n", (uintptr_t)sub->prev);
    printf("start_this = %lu\n", (uintptr_t)sub->start_this);
    printf("mem_start = %lu\n", (uintptr_t)sub->mem_start);
    printf("mem_end = %lu\n", (uintptr_t)sub->mem_end);
    printf("mem_size = %lu\n", sub->mem_size);
    printf("hole = %d\n", sub->hole);
    printf("mem_phys_start = %lu\n", sub->mem_phys_start);
    printf("mem_phys_end = %lu\n", sub->mem_phys_end);
    printf("mem_vir_start = %lu\n", sub->mem_vir_start);
    printf("mem_vir_end = %lu\n", sub->mem_vir_end);
}
void printMainNodeStructure(const struct main_node* main_node) {
    printf("Main Node Structure:\n");
    printf("next = %lu\n", (uintptr_t)main_node->next);
    printf("prev = %lu\n", (uintptr_t)main_node->prev);
    printf("head_sub = %lu\n", (uintptr_t)main_node->head_sub);
    printf("size = %lu\n", main_node->size);
    printf("start_this = %lu\n", (uintptr_t)main_node->start_this);
    printf("page_ptr = %lu\n", (uintptr_t)main_node->page_ptr);
    printf("rem_mem = %lu\n", main_node->rem_mem);
    printf("vir_start = %lu\n", main_node->vir_start);
    printf("vir_end = %lu\n", main_node->vir_end);
}
/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this 
macro to make the output of all system same and conduct a fair evaluation. 
*/

struct main_node* head;
void* curr;
int tot_pages;
size_t rem_mem_struct;
uintptr_t  vir;
#define PAGE_SIZE 4096


/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_init() {
    head=NULL;
    curr=NULL;
    vir=0;
    tot_pages=0;
    //printf("size of main node %lu\nsize of sub node %lu\n",sizeof(struct main_node),sizeof(struct sub));
}
/*
This function will be called at the end of the MeMS system and its main job is to unmap the 
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_finish(){
    struct main_node* temp = head->next;
    
    
    while(temp!=NULL){

		
		if(munmap(temp->page_ptr,temp -> size) == -1){
			perror("Error: munmap failed\n");
			exit(1);
		};
		
		temp = temp -> next; 
	}
}


int func(size_t t)
{
    int n=(int )t;
   // printf(" n %d \n\n",n);
    
    if(n%PAGE_SIZE==0)
    {
        return n/PAGE_SIZE;
    }

    else 
    {
        return (n/PAGE_SIZE)+1;
    }
    
}
/*
Allocates memory of the specified size by reusing a segment from the free list if 
a sufficiently large segment is available. 

Else, uses the mmap system call to allocate more memory on the heap and updates 
the free list accordingly.

Note that while mapping using mmap do not forget to reuse the unused space from mapping
by adding it to the free list.
Parameter: The size of the memory the user program wants
Returns: MeMS Virtual address (that is created by MeMS)
*/ 
void* mems_malloc(size_t size){

    if(head==NULL)
    {

        int n=func(sizeof(struct main_node));
        curr=mmap(NULL, n*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(curr == MAP_FAILED){
        	perror("Error: mmap failed");
        	exit(1);
        }
        //printf("intital curr %lu\n",(uintptr_t)curr);
        rem_mem_struct=n*PAGE_SIZE;

      //  printf("headc reated \n");
        head=(struct main_node*)curr;
        curr=(char*)curr+sizeof(struct main_node);
      //  printf("curr after head %lu\n",(uintptr_t)curr);
        rem_mem_struct= rem_mem_struct-sizeof(struct main_node);
        head->next=NULL;
        head->prev=NULL;

      //  printf("new main created");
        void* temp_ptr=curr;
        struct main_node* new_node=(struct main_node*)curr;
        head->next=new_node;
        curr=(char*)curr+sizeof(struct main_node);
     //   printf("curr after new main node %lu\n",(uintptr_t)curr);
        rem_mem_struct=rem_mem_struct-sizeof(struct main_node);
        new_node->start_this=temp_ptr;
        
        new_node->next=NULL;
        new_node->prev=head;
        new_node->head_sub=NULL;
        struct sub* new_sub=(struct sub*)curr;
        void* temp_ptr3=curr;
        curr=(char*)curr+sizeof(struct sub);
       // printf("curr after sub node creation %lu\n",(uintptr_t)curr);
        new_node->head_sub=new_sub;
        new_sub->next=NULL;
        new_sub->prev=NULL;
        new_sub->start_this=temp_ptr3;

        n=func(size);
        new_node->vir_start=vir;
        new_node->vir_end=vir+n*PAGE_SIZE-1;
        vir=vir+n*PAGE_SIZE;
        new_node->size=n*PAGE_SIZE;
        void* temp_mem=mmap(NULL, n*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(temp_mem == MAP_FAILED){
        	perror("Error: mmap failed");
        	exit(1);
        }
       // printf("intitial mempry ptr %lu\n",(uintptr_t)temp_mem);
        new_sub->mem_start=temp_mem;
        new_sub->mem_end=(char*)new_sub->mem_start+size-1;
        new_sub->mem_size=size;
        new_sub->hole=0;

        new_sub->mem_vir_start=0;
        new_sub->mem_vir_end=size-1;

        new_sub->mem_phys_start=(uintptr_t)temp_mem;
        new_sub->mem_phys_end=(uintptr_t)((char*)temp_mem+size-1);
        

        new_node->rem_mem=n*PAGE_SIZE-size;
        new_node->page_ptr=temp_mem;
        rem_mem_struct=rem_mem_struct-sizeof(struct sub);
     
     //   printf("hole sub node creation\n");
        void* temp_ptr4=curr;
        struct sub* new_sub2=(struct sub*)curr;
        new_sub2->start_this=temp_ptr4;
        curr=(char*)curr+sizeof(struct sub);
        rem_mem_struct=rem_mem_struct-sizeof(struct sub);

        new_sub->next=new_sub2;
        new_sub2->prev=new_sub;
        new_sub2->next=NULL;
        new_sub2->mem_start=new_sub->mem_end+1;
        new_sub2->mem_phys_start=(uintptr_t)new_sub2->mem_start;

    
        new_sub2->mem_end=(char*)new_node->page_ptr+new_node->size-1;
        new_sub2->mem_phys_end=(uintptr_t)new_sub2->mem_end;
        new_sub2->mem_vir_start=(uintptr_t)((char*)new_sub->mem_vir_end+1);
        new_sub2->mem_vir_end=new_node->vir_end;
            new_sub2->hole=1;
        new_sub2->mem_size=new_node->size-new_sub->mem_size;


      // printSubStructure(new_sub2);
 
  //    printf("curr after hole %lu\n",(uintptr_t)curr);


        uintptr_t res=new_node->head_sub->mem_vir_start;
        void* res_f= (void*)res;
        return res_f;


    }

    else
    {
        struct main_node* temp_ptr5=head->next;

        struct main_node* found_main;
        struct sub* found_sub;
        int found=0;
        while(temp_ptr5!=NULL)
        {
            struct sub* temp_ptr6=temp_ptr5->head_sub;
            while(temp_ptr6!=NULL)
            {
                if(temp_ptr6->hole==1 &&temp_ptr6->mem_size>=size)
                {
                    found_main=temp_ptr5;
                    found_sub=temp_ptr6;

                    found++;
                    break;

                }
                temp_ptr6=temp_ptr6->next;
            }
            if(found==1)
            {
                break;
            }
            temp_ptr5=temp_ptr5->next;
        }


     //   found=0;        
        
        //check for holes
        if(found==0)
        {
            //new main node to be created
            int n=func(size);
            
            struct main_node* node=head->next;
            struct main_node* last;
            while(node!=NULL)
            {
                last=node;
                node=node->next;
            }
    //        printf("new main created");
            
            
            if(rem_mem_struct<sizeof(struct main_node)) 
            {
                curr=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                if(curr == MAP_FAILED){
					perror("Error: mmap failed");
					exit(1);
				}
      //          printf("new page %lu\n",(uintptr_t)curr);
                rem_mem_struct=PAGE_SIZE;
            }
            void* temp_ptr=curr;
            struct main_node* new_node=(struct main_node*)curr;
            last->next=new_node;
             new_node->prev=last;
            new_node->size=size;
            curr=(char*)curr+sizeof(struct main_node);
           
         //   printf("curr after new main node %lu\n",(uintptr_t)curr);
            rem_mem_struct=rem_mem_struct-sizeof(struct main_node);
            new_node->start_this=temp_ptr;
            new_node->vir_start=vir;
            new_node->vir_end=vir+n*PAGE_SIZE-1;
            vir=vir+n*PAGE_SIZE;
            
            new_node->next=NULL;
            new_node->prev=head;
            new_node->head_sub=NULL;
            struct sub* new_sub=(struct sub*)curr;
            void* temp_ptr3=curr;
            if(rem_mem_struct<sizeof(struct sub))
            {
                curr=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                if(curr == MAP_FAILED){
					perror("Error: mmap failed");
					exit(1);
				}
           //     printf("new page %lu\n",(uintptr_t)curr);
                rem_mem_struct=PAGE_SIZE;
            }
            curr=(char*)curr+sizeof(struct sub);
            rem_mem_struct=rem_mem_struct-sizeof(struct sub);
            //printf("curr after sub node creation %lu\n",(uintptr_t)curr);
            new_node->head_sub=new_sub;
            new_sub->next=NULL;
            new_sub->prev=NULL;
            new_sub->start_this=temp_ptr3;

            new_node->size=n*PAGE_SIZE;
            void* temp_mem=mmap(NULL, n*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if(temp_mem == MAP_FAILED){
		    	perror("Error: mmap failed");
		    	exit(1);
		    }
            //printf("intitial mempry ptr %lu\n",(uintptr_t)temp_mem);
            new_sub->mem_start=temp_mem;
            new_sub->mem_end=(char*)new_sub->mem_start+size-1;
            new_sub->mem_size=size;
            new_sub->hole=0;

            new_sub->mem_vir_start=new_node->vir_start;
            new_sub->mem_vir_end=new_node->vir_start+size-1;

            new_sub->mem_phys_start=(uintptr_t)temp_mem;
            new_sub->mem_phys_end=(uintptr_t)((char*)temp_mem+size-1);
            

            new_node->rem_mem=n*PAGE_SIZE-size;
            new_node->page_ptr=temp_mem;

            
             //printMainNodeStructure(new_node);

             //printf("new sub created \n\n");
         //   printSubStructure(new_sub);


            //printf("hole sub node creation\n");
            void* temp_ptr4=curr;
            struct sub* new_sub2=(struct sub*)curr;
            new_sub2->start_this=temp_ptr4;
            if(rem_mem_struct<sizeof(struct sub))
            {
                curr=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                if(curr == MAP_FAILED){
					perror("Error: mmap failed");
					exit(1);
				}
               // printf("new page %lu\n",(uintptr_t)curr);
                rem_mem_struct=PAGE_SIZE;
            }
            curr=(char*)curr+sizeof(struct sub);
            rem_mem_struct=rem_mem_struct-sizeof(struct sub);

            new_sub->next=new_sub2;
            new_sub2->prev=new_sub;
            new_sub2->next=NULL;
            new_sub2->mem_start=new_sub->mem_end+1;
            new_sub2->mem_phys_start=(uintptr_t)new_sub2->mem_start;


            new_sub2->mem_end=(char*)new_node->page_ptr+new_node->size-1;
            new_sub2->mem_phys_end=(uintptr_t)new_sub2->mem_end;
            new_sub2->mem_vir_start=(uintptr_t)((char*)new_sub->mem_vir_end+1);
            new_sub2->mem_vir_end=new_node->vir_end;
                new_sub2->hole=1;
            new_sub2->mem_size=new_node->size-new_sub->mem_size;


           // printSubStructure(new_sub2);

            //printf("curr after hole %lu\n",(uintptr_t)curr);


            uintptr_t res=new_sub->mem_vir_start;
          //  printf("reached here\n");
            void* res_f= (void*)res;
            return res_f;

            
        }

        else
        {
           if(rem_mem_struct<sizeof(struct sub))
            {
                curr=mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                if(curr == MAP_FAILED){
					perror("Error: mmap failed");
					exit(1);
				}
              //  printf("new page %lu\n",(uintptr_t)curr);
                rem_mem_struct=PAGE_SIZE;
            }


            struct sub* new_sub=(struct sub*)curr;
            new_sub->next=found_sub->next;
            found_sub->next=new_sub;
            new_sub->prev=found_sub;
            new_sub->start_this=curr;
            curr=(char*)curr+sizeof(struct sub);
            
            rem_mem_struct=rem_mem_struct-sizeof(struct sub);
            void* temp_found_main=found_sub->mem_end;
            size_t temp_size= found_sub->mem_size;

            uintptr_t temp_vir_start=found_sub->mem_vir_start;
            uintptr_t temp_vir_end=found_sub->mem_vir_end;


           
            new_sub->mem_end=temp_found_main;


            found_sub->mem_end=(char*)found_sub->mem_start+size-1;
            found_sub->mem_phys_end=(uintptr_t)found_sub->mem_end;
            found_sub->mem_size=size;

            found_sub->mem_vir_end=found_sub->mem_vir_start+found_sub->mem_size-1;
            found_sub->hole=0;

            new_sub->mem_vir_start=found_sub->mem_vir_end+1;
            new_sub->mem_vir_end=temp_vir_end;
            new_sub->mem_start=(char*)found_sub->mem_end+1;
            new_sub->mem_phys_start=(uintptr_t)new_sub->mem_start;
            new_sub->mem_phys_end=(uintptr_t)temp_found_main;

            
            new_sub->mem_size=temp_size-size;
            new_sub->hole=1;

            // printf("found_sub\n");
            // printSubStructure(found_sub);
            // printf("\n\n\n");
            // printSubStructure(new_sub);

            return (void *)found_sub->mem_vir_start;
            }
    
    }
    

}

/*
this function print the stats of the MeMS system like
1. How many pages are utilised by using the mems_malloc
2. how much memory is unused i.e. the memory that is in freelist and is not used.
3. It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing
Returns: Nothing but should print the necessary information on STDOUT
*/
void mems_print_stats() {
    printf("---------------Printing Stats [mems_print_stats]---------------\n");
    printf("----MeMS SYSTEM STATS -----\n");

    size_t size = 0;
    size_t free = 0;
    size_t mainLength = 0;

    struct main_node* temp = head->next;
	
    while (temp != NULL) {
        printf("MAIN[%ld:%ld] -> ", temp->vir_start, temp->vir_end);

        struct sub* chain = temp->head_sub; // Use temp->head_sub here

        while (chain != NULL) {
            if (chain->hole == 0) {
                printf("P[%ld -> %ld]  <-> ", chain->mem_vir_start, chain->mem_vir_end);
            } else {
                printf("H[%ld -> %ld]  <-> ", chain->mem_vir_start, chain->mem_vir_end);
                free += chain->mem_size;
            }
            chain = chain->next;
        }
        printf("NULL\n");

        mainLength++;
        size += temp->size;
        temp = temp->next;
    }

    printf("Pages used: %ld pages\n", size / PAGE_SIZE);
    printf("Space unused: %ld bytes\n", free);
    printf("Main Chain Length: %ld\n", mainLength);

    temp = head->next;
    printf("Sub-Chain Length array: [");

    while (temp != NULL) {
        struct sub* chain = temp->head_sub;
        size_t subLength = 0;

        while (chain != NULL) {
            chain = chain->next;
            subLength++;
        }

        printf("%ld, ", subLength);
        temp = temp->next;
    }

    printf(" ]\n");
    printf("-------------------------\n");
}

/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/
void *mems_get(void*v_ptr){
	uintptr_t checker = (uintptr_t)v_ptr;
	struct main_node* temp = head;
	while(temp!=NULL){
		if(temp->vir_start <= checker && temp->vir_end >= checker){
			struct sub* chain = temp -> head_sub;
			while(chain!=NULL){
				if(chain->mem_vir_start <= checker && chain->mem_vir_end >= checker){
					uintptr_t ans = checker - chain->mem_vir_start + chain -> mem_phys_start;
					return (void *)ans;
				}
				chain = chain-> next;
			}
		}
		temp = temp->next;
	}
	
	
}


/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS) 
Returns: nothing
*/
void mems_free(void *v_ptr){
    uintptr_t checker = (uintptr_t)v_ptr;
	struct main_node* temp = head;
	while(temp!=NULL){
		if(temp->vir_start <= checker && temp->vir_end >= checker){
			struct sub* chain = temp -> head_sub;
			while(chain!=NULL){
				if(chain->mem_vir_start <= checker && chain->mem_vir_end >= checker){
					chain->hole = 1;
					break;
				}
				chain = chain-> next;
			}
		}
		temp = temp->next;
	}
	

	temp = head->next;
	while(temp!=NULL){
	struct sub* chain = temp-> head_sub -> next;
		while(chain!=NULL){
			struct sub* temp1 = chain -> next;
			if(chain -> prev -> hole == 1 && chain -> hole == 1){
				
				chain -> prev -> mem_end = chain -> mem_end;
				
				chain -> prev -> mem_phys_end = chain -> mem_phys_end;
				
				chain -> prev -> mem_size = chain -> prev -> mem_size + chain -> mem_size;
				
				chain -> prev -> mem_vir_end = chain -> mem_vir_end;
				
				chain -> prev -> next = chain -> next;
				if(chain->next!=NULL)
					chain -> next -> prev = chain -> prev;
				
				chain -> prev = NULL;
				chain -> next = NULL;
			
				
			}
			chain  = temp1;
		}
		temp = temp-> next;
		}
	return ;
}
