#define INITIAL_CAPACITY 10

typedef struct {
  int type;
  int size;     
  int capacity;  
  char **arr1;     // array of strings 
  char *arr2; //array of chars
} Vector;

void vec_init(Vector *vector, int type);

void init2(Vector *vector);

void vec_append_str(Vector *vector, char* string);

void vec_append_c(Vector *vector, char c);

char* vec_get_str(Vector *vector, int index);

char vec_get_c(Vector *vector, int index);

char** get_arr(Vector *vector);

char* get_string(Vector *vector);

void vec_set_str(Vector *vector, int index, char* string);

void vec_set_c(Vector *vector, int index, char c);

void vec_double_capacity(Vector *vector);

void free_vec(Vector *vector, int option);

void print_vec(Vector *vector, char* name);

int isEmpty(Vector *vector);

void trim_vec(Vector *vector);

char* vec_file(Vector *vector, char* string);

int has_string(Vector *vector, char* string);
