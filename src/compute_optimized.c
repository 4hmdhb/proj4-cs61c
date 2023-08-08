#include <omp.h>
#include <x86intrin.h>

#include "compute.h"

// Computes the dot product of vec1 and vec2, both of size n
int32_t dot(uint32_t n, int32_t *vec1, int32_t *vec2) {

    // TODO: implement dot product of vec1 and vec2, both of size n
    int32_t ans = 0;
    __m256i ans_vector = _mm256_setzero_si256();

    for (int i = 0; i < n / 8; i++){
        __m256i a_vector = _mm256_loadu_si256( (__m256i *) vec1 + (i * 8));
        __m256i b_vector = _mm256_loadu_si256( (__m256i *) vec2 + (i * 8));

        ans_vector = _mm256_add_epi32(ans_vector, _mm256_mullo_epi32(a_vector, b_vector));
        

    }
    
    int tmp_arr[8] = {0,0,0,0,0,0,0,0};
    _mm256_store_si256( (__m256i *) tmp_arr, ans_vector);

    for(int i = 0; i<8; i++){
        ans = ans +tmp_arr[i];
    }

    for (int i = n/8 * 8; i < n; i++){
        ans = ans + vec1[i] * vec2[i];
    }

    return ans;
  return -1;
}


uint32_t do_mult(int32_t *a_matrix, int32_t *b_matrix, int32_t col_a, int col_b, int row_b){
    int sum = 0;

    int size = col_b * row_b;

    #pragma omp parallel for
    for (int i = 0; i < size; i++){
        int a_row = i / col_b;
        int a_col = i % col_b;
        sum = sum + (a_matrix[a_row * col_a + a_col] * b_matrix[i]);
        
    }
    return sum;
}


void exchanger(int32_t *data, uint32_t cols, uint32_t rows, int32_t offset){
    
    
    for (int i = 0; i < cols; i++){
        int32_t a = data[i + (offset * cols)];
        int32_t b = data[i + ((rows - offset - 1) * cols)];

        data[i + (offset * cols)] = b;
        data[i + ((rows - offset - 1) * cols)] = a;
    }

}


void flipper(matrix_t *matrix){

    uint32_t cols = matrix->cols;
    uint32_t rows = matrix->rows;


    //flip vertically
    
    for (int i = 0; i < rows / 2; i++){
        exchanger(matrix->data, cols, rows, i);
    }

    
    //flip horizontally
    for (int i = 0; i < rows; i++){
        for (int y = 0; y < cols / 2; y++){
            int a = matrix->data[i * cols + y];
            int b = matrix->data[((i+1) * cols) - 1 - y];
            matrix->data[i * cols + y] = b;
            matrix->data[((i+1) * cols) - 1 -y] = a;
        }

    }


}


// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
  // TODO: convolve matrix a and matrix b, and store the resulting matrix in
  // output_matrix
    uint32_t a_cols = a_matrix->cols;
    uint32_t b_cols = b_matrix->cols;


    uint32_t out_rows = a_matrix->rows - b_matrix->rows + 1;
    uint32_t out_cols = a_matrix->cols - b_matrix->cols + 1;
    
    flipper(b_matrix);
    

    int32_t *output = malloc(sizeof(int32_t) * (out_cols * out_rows));
    if (output == NULL){
        return -1;
    }
    
    
    #pragma omp parallel for 
    for (int i = 0; i < out_rows * out_cols; i++){
        int a_row = (i / out_cols);
        int a_col = i % out_cols;
        output[i] = do_mult(a_matrix->data + (a_row * a_matrix->cols + a_col), b_matrix->data, a_matrix->cols, b_matrix->cols, b_matrix->rows);

    }

    *output_matrix = malloc(sizeof(matrix_t));
    (*output_matrix)->rows = out_rows;
    (*output_matrix)->cols = out_cols;

    (*output_matrix)->data = output;
    
    
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
