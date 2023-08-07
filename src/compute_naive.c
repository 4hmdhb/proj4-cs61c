#include "compute.h"

// Computes the dot product of vec1 and vec2, both of size n
int32_t dot(uint32_t n, int32_t *vec1, int32_t *vec2) {
  // TODO: implement dot product of vec1 and vec2, both of size n
    int32_t ans = 0;

    for (int i = 0; i < n; i++){
        ans = ans + (vec1[i] * vec2[i]);

    }
  
    return ans;
}


void exchanger(int32_t *data, uint32_t cols, uint32_t rows, offset){
    
    for (int i = 0; i < cols; i++){
        int32_t a = data[i + (offset * cols)];
        int32_t b = data[i + ((rows - offset - 1) * cols)];

        data[i + (offset * cols)] = b;
        data[i + ((rows - offset - 1) * cols)] = a;
    }

}


int32_t *flipper(matrix_t *matrix){

    uint32_t cols = matrix->cols;
    uint32_t rows = matrix->rows;

    int32_t *result = malloc(sizeof(int32_t) * (cols * rows));


    //copy data

    for (int i = 0; i < (cols * rows); i++){
    
        result[i] = matrix->data[i];
    }

    //flip horizontally
    /
    for (int i = 0; i < rows / 2; i++){
        exchanger(result, cols, rows, i);
    }

}

// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
  // TODO: convolve matrix a and matrix b, and store the resulting matrix in
  // output_matrix
    
    uint32_t a_cols = a_matrix->cols;
    uint32_t b_cols = b_matrix->cols;

    int32_t *rev_b = malloc(sizeof(int32_t) * (b_cols));
    if (rev_b == NULL){
        return -1;   
    }
    
    for (int i = 0; i < b_cols; i++){
        rev_b[i] = b_matrix->data[b_cols - i - 1];
    }

    int32_t *output = malloc(sizeof(int32_t) * (a_cols - b_cols + 1));
    if (output == NULL){
        return -1;
    }

    for (int i = 0; i <= (a_cols - b_cols); i++){
        output[i] = dot(b_cols, a_matrix->data + i, rev_b);
        

    }
    
    *output_matrix = malloc(sizeof(matrix_t));
    (*output_matrix)->rows = 1;
    (*output_matrix)->cols = a_cols - b_cols + 1;

    (*output_matrix)->data = output;
    
    free(rev_b);
    
    return 0;
}

// Executes a task
int execute_task(task_t *task) {
  matrix_t *a_matrix, *b_matrix, *output_matrix;

  if (read_matrix(get_a_matrix_path(task), &a_matrix))
    return -1;
  if (read_matrix(get_b_matrix_path(task), &b_matrix))
    return -1;

  if (convolve(a_matrix, b_matrix, &output_matrix))
    return -1;

  if (write_matrix(get_output_matrix_path(task), output_matrix))
    return -1;

  free(a_matrix->data);
  free(b_matrix->data);
  free(output_matrix->data);
  free(a_matrix);
  free(b_matrix);
  free(output_matrix);
  return 0;
}
