/*
Syed Ali 1001725463

*/


// The MIT License (MIT)
//
// Copyright (c) 2022 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

enum TYPE{

  FREE = 0,
  USED = 1
};

// code given by Professor Bakker
struct Node {
  size_t size;
  enum TYPE type;
  void * arena;
  struct Node * next;
  struct Node * prev;
};

// code given by Professor Bakker
struct Node *alloc_list = NULL;
struct Node *previous_node;

// Total size of complete arena
void * arena;

// An enum ALFORITHM that will contain the fit being used
enum ALGORITHM allocation_algorithm;

//DONE
// code given by Professor Bakker
int mavalloc_init( size_t size, enum ALGORITHM algorithm ){

  if(size < 0){
    return -1;
  }

  arena = malloc( ALIGN4( size ) );
  if(arena == NULL){
    return -1;
  }
  allocation_algorithm = algorithm;

  alloc_list = ( struct Node * )malloc( sizeof( struct Node ));

  alloc_list -> arena = arena;
  alloc_list -> size  = ALIGN4(size);
  alloc_list -> type  = FREE;
  alloc_list -> next  = NULL;
  alloc_list -> prev  = NULL;

  previous_node  = alloc_list;

  return 0;
}


// DONE
void mavalloc_destroy( ){

  // free the arena
  free(arena);

  struct Node *current = alloc_list;
  struct Node *x = current;

  //while a current node is not NULL free x node
  while(current != NULL){
    current = current -> next;
    free(x);
    x = current;
  }
  alloc_list = NULL;
  previous_node = NULL;
  return;
}

void * mavalloc_alloc( size_t size ){

  struct Node * node;

  if( allocation_algorithm != NEXT_FIT ){
    node = alloc_list;
  }
  else if ( allocation_algorithm == NEXT_FIT ){
    node = previous_node;
  }
  else{
    printf("ERROR: Unknown allocation algorithm!\n");
    exit(0);
  }

  size_t aligned_size = ALIGN4( size );

  // Implement First Fit
  // will find the first place the the node can be placed and place it there
  // code given by Professor Bakker
  if( allocation_algorithm == FIRST_FIT ){
    while( node ){
      if( node -> size >= aligned_size  && node -> type == FREE ){
        int leftover_size = 0;

        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;

        if( leftover_size > 0 ){
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));

          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;

          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;

      }
      node = node -> next;
    }
  }

  // Implement Next Fit
  // will find the first place the the node can be placed skip that
  // and then place it in next available
  else if( allocation_algorithm == NEXT_FIT){
    struct Node *startNode = node;
    while(node){
      // code given by Professor Bakker
      if( node -> size >= aligned_size  && node -> type == FREE ){
        int leftover_size = 0;

        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;

        if( leftover_size > 0 ){
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));

          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;

          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
    }
    node = alloc_list;
    while(node){
      if(node == startNode){
        printf("All nodes are full");
        return ( void * ) node -> arena;
      }
      // code given by Professor Bakker
      if( node -> size >= aligned_size  && node -> type == FREE ){
        int leftover_size = 0;

        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;

        if( leftover_size > 0 ){
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));

          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;

          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node->next;
    }
  }
  // Implement Best Fit
  // will find the smallest free portion of memory that
  // fit the node and then place the node there
  else if( allocation_algorithm == BEST_FIT){
    struct Node *biggest = NULL;
    int best_size = INT_MAX;

    // while a node exist run through all of them
    while (node)
    {
      if (node -> size >= aligned_size  && node -> type == FREE)
      {
        // calculate the difference between the size
        // of memory vs how much your are allocation
        int diff = (node->size - size);

        if (diff < best_size)
        {
          biggest = node;
          best_size = diff;
        }
      }
      node = node->next;
    }
    node = biggest;
    if (node)
    {
      // code given by Professor Bakker but changed
      int leftover_size = 0;

      node->type = USED;
      leftover_size = node->size - aligned_size;
      node->size = aligned_size;

      if (leftover_size > 0)
      {
        struct Node *previous_next = node->next;
        struct Node *leftover_node = (struct Node *)malloc(sizeof(struct Node));

        leftover_node->arena = node->arena + size;
        leftover_node->type = FREE;
        leftover_node->size = leftover_size;
        leftover_node->next = previous_next;

        node->next = leftover_node;
      }
      previous_node = node;
      return (void *)node->arena;
    }
  }
  // Implement Worst Fit
  // will find the largest free portion of memory that
  // fit the node and then place the node there
  // opposite of the best fit
  else if( allocation_algorithm == WORST_FIT){
      // node to hold the biggest portion of memory
      struct Node *max;
      while(node -> type == USED){
        node = node -> next;
      }
      max = node;
      while(node -> next != NULL){
        node = node -> next;
        if(node -> type == FREE){
          // check to see if any node is bigger than chosen max
          if(max -> size < node -> size){
            max = node;
          }
        }
      }
      node = max;
      // code given by Professor Bakker
      if (node->size >= aligned_size && node->type == FREE)
      {
        int leftover_size = 0;

        node->type = USED;
        leftover_size = node->size - aligned_size;
        node->size = aligned_size;

        if (leftover_size > 0)
        {
          struct Node *previous_next = node->next;
          struct Node *leftover_node = (struct Node *)malloc(sizeof(struct Node));

          leftover_node->arena = node->arena + size;
          leftover_node->type = FREE;
          leftover_node->size = leftover_size;
          leftover_node->next = previous_next;

          node->next = leftover_node;
        }
        previous_node = node;
        return (void *)node->arena;
      }
      node = node->next;
  }
  return NULL;
}

//DONE
void mavalloc_free( void * ptr ){
  struct Node *current = alloc_list;

  // for every node change the type to FREE
  while(current != NULL){
    if(current -> arena == ptr){
      if(current -> type == FREE){
        printf("Nodes already freed\n");
      }
      current -> type = FREE;
      break;
    }
    current = current -> next;
  }

  // free all nodes
  current = alloc_list;
  while(current != NULL){
    if(current -> type == FREE && current -> next && current -> next -> type == FREE){
      struct Node * free_cur = current->next;
      current->size = current->size + current->next->size;
      current->next = current->next->next;
      free(free_cur);
      continue;
    }
    current = current -> next;
  }
  return;
}

// DONE
// code given by Professor Bakker
int mavalloc_size( ){
  int number_of_nodes = 0;
  struct Node * ptr = alloc_list;

  //. for every pointer increment the number of nodes by 1
  while(ptr != NULL)
  {
    number_of_nodes ++;
    ptr = ptr -> next;
  }

  return number_of_nodes;
}
