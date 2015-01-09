#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

#define true 1
#define false 0

//Standard dynamic array structure used to store the commands 

void vec_init(Vector *vector, int type) {
  vector->size = 0;
  vector->capacity = INITIAL_CAPACITY;
  vector->type = type;

  if(vector->type == 0){
	vector->arr1 = (char**) malloc(vector->capacity * sizeof(char*));
  }else{
	init2(vector);
  }
}

void init2(Vector *vector){
	vector->arr2 = (char*) malloc(vector->capacity * sizeof(char));
}

void vec_append_str(Vector *vector, char* string) {
    vec_double_capacity(vector);

    vector->arr1[vector->size++] = string;
}

void vec_append_c(Vector *vector, char c) {
    vec_double_capacity(vector);

    vector->arr2[vector->size++] = c;
}

char* vec_get_str(Vector *vector, int index) {
  if (index >= vector->size || index < 0) {
    printf("Index %d out of bounds for vector of size %d\n", index, vector->size);
    exit(1);
  }
	return vector->arr1[index];
}

char vec_get_c(Vector *vector, int index) {
  if (index >= vector->size || index < 0) {
    printf("Index %d out of bounds for vector of size %d\n", index, vector->size);
    exit(1);
  }
	return vector->arr2[index];
}

char** get_arr(Vector *vector){
	return vector->arr1;
}

char* get_string(Vector *vector){
	return vector->arr2;
}

void vec_set_str(Vector *vector, int index, char* string) {
  /**while (index >= vector->size) {
    vec_append_str(vector, "");
  }  **/
	vector->arr1[index] = string;
}

void vec_set_c(Vector *vector, int index, char c) {
  /**while (index >= vector->size) {
    vec_append_str(vector, '');
  }  **/
	vector->arr2[index] = c;
}

void vec_double_capacity(Vector *vector) {
  if(vector->size == vector->capacity - 1) {
	vector->capacity *= 2;
	 if(vector->type == 0){	
			vector->arr1 = (char**)realloc(vector->arr1, vector->capacity * sizeof(char*));
	}else{
			vector->arr2 = (char*)realloc(vector->arr2, vector->capacity * sizeof(char));
    }			
  }
}

void free_vec(Vector *vector, int option) {
  int i;
  
  if(vector->type == 0){
		   if(option == 0) for(i = 0; i < vector->size; i++) free(vector->arr1[i]);
		   free(vector->arr1);
  }else{
		   free(vector->arr2);
  }
}

void print_vec(Vector *vector, char* name){
  if(vector->size > 0){
	  int i = 0;
	  for(i = 0; i < vector->size; i++) {
		  if(vector->type == 0){
			 printf("%s[%d] = %s\n",name,i, vector->arr1[i]);
		  }else{
			 printf("%s[%d] = %c\n",name,i, vector->arr2[i]);
		  }
	  }
  }
}

int isEmpty(Vector *vector){
	if(vector->size == 0){
		return true;
	}
	return false;
}

void trim_vec(Vector *vector){
	vector->size--;
	if(vector->type == 0){
		free(vector->arr1[vector->size]);
		vector->arr1[vector->size] = NULL;
	}
}

char* vec_file(Vector *vector, char *string){

	if(vector->type == 0){
		if(vector->size == 0){
			return NULL;
		}
		
		char* fileName = NULL; 
			int i;
			for(i = 0; i < vector->size; i++) {
			    if(strcmp(vector->arr1[i], string) == 0) {
			      free(vector->arr1[i]);
			      fileName = vector->arr1[i + 1];
			      vector->size -= 2;

			      int j;
			      for(j = i; j <= vector->size; j++) { // adds in NULL values
			        vector->arr1[j] = vector->arr1[j + 2];
			      }
			      break;
			    }
			  }
	  
	  return fileName;
  }
  
  return NULL;
}

int has_string(Vector *vector, char* string){
		if(vector->type == 0){
			int i;
			for(i = 0; i < vector->size;i++){
				if(strcmp(vector->arr1[i], string) == 0){
					return true; 
				}
			}
			return false;
	}
	return false;
}
