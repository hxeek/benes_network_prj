#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
/*=======================================================================================
define parameter
=======================================================================================*/
#define COLS 4
#define PATH_ORDER 2
#define SUBNETWORK 3
#define POS_INPUT 0
#define POS_OUTPUT 1
#define FLAG_PORT 0
#define FLAG_SWITCH 1
#define SUB_U 0
#define SUB_L 1
#define SWITCH_BAR 0
#define SWITCH_CROSS 1
#define BASE 2
/*=======================================================================================
declare global variable
=======================================================================================*/
int size;
int switch_size;
int double_size;
int loop = 0;
int **input_table;
int **double_size_table;
int *output_switch;
int **path;
int **IO_switch_arr;
int ***subnet_arr;
int num_layer;
int num_stage;
int total_num_of_subnetwork = 0;
int ***total_switch_arr;
int **stage_switch_arr;

/*=======================================================================================
declare sub_function for main
=======================================================================================*/
void allocate_memory(int current_size);
void free_memory(int current_size);
void table_sort(int current_size);
int start_point(int current_size);
void get_used_switch(int port);
void routing_path(int port, int position, int order);
int outFind(int current_size, int order, int port, int position, int flag, int isFirstcall);
void first_layer(int current_size);
void path_divider(int current_size);
void parameter_cal(int current_size);
double logB(double x);
void subnet_table_maker(int current_size, int layer);
void mid_layer(int current_size);
void set_switch(int current_size);
int final_layer(int current_size);
void transfer_switch_arr(int current_size);
/*=======================================================================================
function for dynamic allocate array
=======================================================================================*/
void allocate_memory(int current_size)
{
 switch_size = current_size / 2;
 double_size = current_size * 2;

 input_table = (int **)calloc(current_size, sizeof(int *));
 for (int i = 0; i < current_size; i++)
 {
 input_table[i] = (int *)calloc(2, sizeof(int));
 }

 double_size_table = (int **)calloc(double_size, sizeof(int *));
 for (int i = 0; i < double_size; i++)
 {
 double_size_table[i] = (int *)calloc(2, sizeof(int));
 }

 path = (int **)calloc(current_size, sizeof(int *));
 for (int i = 0; i < current_size; i++)
 {
 path[i] = (int *)calloc(4, sizeof(int));
 }

 output_switch = (int *)calloc(switch_size, sizeof(int));

 IO_switch_arr = (int **)calloc(switch_size, sizeof(int *));
 for (int i = 0; i < switch_size; i++)
 {
 IO_switch_arr[i] = (int *)calloc(2, sizeof(int));
 }
}
void free_memory(int current_size)
{

 for (int i = 0; i < current_size; i++)
 {
 free(input_table[i]);
 }
 free(input_table);

 for (int i = 0; i < double_size; i++)
 {
 free(double_size_table[i]);
 }
 free(double_size_table);

 for (int i = 0; i < current_size; i++)
 {
 free(path[i]);
 }
 free(path);

 free(output_switch);

 for (int i = 0; i < switch_size; i++)
 {
 free(IO_switch_arr[i]);
 }
 free(IO_switch_arr);
}

void subnet_arr_allocation(int current_size)
{

 int ID = total_num_of_subnetwork;

 subnet_arr = (int ***)calloc(ID, sizeof(int **));
 for (int i = 0; i < ID; i++)
 {
 int now_layer = (int)log2(i + 1);
 int row =(int) (current_size / pow(2, now_layer));
 subnet_arr[i] = (int **)calloc(row, sizeof(int *));
 for (int j = 0; j < row; j++)
 {
 subnet_arr[i][j] = (int *)calloc(2, sizeof(int));
 }
 }
}

void subnet_arr_free(int current_size)
{
 int ID = total_num_of_subnetwork;

 for (int i = 0; i < ID; i++)
 {
 int now_layer = (int)log2(i + 1);
 int row = current_size / pow(2, now_layer);
 for (int j = 0; j < row; j++)
 {
 free(subnet_arr[i][j]);
 }
 free(subnet_arr[i]);
 }
 free(subnet_arr);
}

void switch_arr_allocation(int current_size)
{
 int ID = total_num_of_subnetwork;

 total_switch_arr = (int ***)calloc(ID, sizeof(int **));
 for (int i = 0; i < ID; i++)
 {
 int now_layer = (int)log2(i + 1);
 int row = (int)((current_size / 2) / pow(2, now_layer));
 total_switch_arr[i] = (int **)calloc(row, sizeof(int *));
 for (int j = 0; j < row; j++)
 {
 total_switch_arr[i][j] = (int *)calloc(2, sizeof(int));
 }
 }
}

void switch_arr_free(int current_size)
{
 int ID = total_num_of_subnetwork;

 for (int i = 0; i < ID; i++)
 {
 int now_layer = (int)log2(i + 1);
 int row = (current_size / 2) / pow(2, now_layer);
 for (int j = 0; j < row; j++)
 {
 free(total_switch_arr[i][j]);
 }
 free(total_switch_arr[i]);
 }
 free(total_switch_arr);
}

void stage_switch_arr_allocation(int current_size){
    stage_switch_arr = (int **)calloc(current_size/2,sizeof(int*));
    for(int i=0; i<current_size/2;i++)
    stage_switch_arr[i] = (int*)calloc(num_stage,sizeof(int));
}
void stage_switch_arr_free(int current_size){
    for(int i = 0;i<current_size;i++)
    free(stage_switch_arr[i]);
    free(stage_switch_arr);
}

void print_stage_switch(int current_size);
/*=======================================================================================
main function
=======================================================================================*/

bool validation_check_size(int n){
    return n>=8 && (n &(n-1)) == 0;
}
int main()
{

 printf("please insert the size of benes network :");
 scanf("%d", &size);
 while(!validation_check_size(size)){
    printf("the size must be 8 or be a greater power of 2. please enter the validate size :");
    scanf("%d",&size);
 }
 clock_t start_time = clock();
 int current_size = size;
 allocate_memory(current_size);
 parameter_cal(current_size);
 subnet_arr_allocation(current_size);
 switch_arr_allocation(current_size);
 first_layer(current_size);
 mid_layer(current_size / 2);
 final_layer(2);
 stage_switch_arr_allocation(size);
 transfer_switch_arr(size);
 printf("\n\n\n\n");
 print_stage_switch(size);
clock_t end_time = clock();
double elapsed_time = ((double) (end_time-start_time)/CLOCKS_PER_SEC);
printf("Elapsed Time : %f seconds \n",elapsed_time);

}
/*=======================================================================================
define sub_function
=======================================================================================*/

double logB(double x)
{
 return log(x) / log(BASE);
}

void parameter_cal(int current_size)
{
 num_layer = log2(current_size);
 num_stage = log2(current_size) * 2 - 1;

 for (int i = 0; i < num_layer; i++)
 {
 total_num_of_subnetwork = total_num_of_subnetwork + pow(2, i);
 }
}
void get_port_num(int current_size)
{
 int input = -1, output = -1;
 bool *input_taken = (bool *)malloc(current_size * sizeof(bool));
 bool *output_taken = (bool *)malloc(current_size * sizeof(bool));

 memset(input_taken, false, current_size * sizeof(bool));
 memset(output_taken, false, current_size * sizeof(bool));

 printf("Enter input and output port number: \n");
 for (int i = 0; i < current_size; i++)
 {
 printf(" %dst input/output: ", i);
 scanf("%d %d", &input, &output);

 while (input >= current_size || output >= current_size || input < 0 || output < 0 || input_taken[input] || output_taken[output])
 {
 if (input >= current_size || output >= current_size || input < 0 || output < 0)
 {
 printf("Error: Invalid port number! Please enter valid port numbers between 0 and %d.\n", current_size - 1);
 }
 else if (input_taken[input] == true || output_taken[output] == true)
 {
 printf("Error: Duplicate port number detected! Please enter unique port numbers.\n");
 }
 printf(" %dst input/output: ", i);
 scanf("%d %d", &input, &output);
 }

 input_taken[input] = true;
 output_taken[output] = true;

 input_table[i][0] = input;
 input_table[i][1] = output;
 }

 for (int i = 0; i < current_size; i++)
 {
 subnet_arr[0][i][0] = input_table[i][0];
 subnet_arr[0][i][1] = input_table[i][1];
 }
 free(input_taken);
 free(output_taken);
}

void table_sort(int current_size)
{

 int **temp_table = (int **)calloc(current_size,sizeof(int *));
 for (int i = 0; i < current_size; i++)
 {
 temp_table[i] = (int *)calloc(2,sizeof(int));
 }

 int **inverse_temp_table = (int **)calloc(current_size,sizeof(int *));
 for (int i = 0; i < current_size; i++)
 {
 inverse_temp_table[i] = (int *)calloc(2,sizeof(int));
 }

 for (int i = 0; i < current_size; i++)
 {
 temp_table[i][0] = input_table[i][0];
 temp_table[i][1] = input_table[i][1];
 }
 for (int i = 0; i < current_size - 1; i++)
 {
 int min_idx = i;
 for (int j = i + 1; j < current_size; j++)
 {
 if (temp_table[j][0] < temp_table[min_idx][0])
 {
 min_idx = j;
 }
 }
 if (min_idx != i)
 {
 int temp_in = temp_table[i][0];
 int temp_out = temp_table[i][1];
 temp_table[i][0] = temp_table[min_idx][0];
 temp_table[i][1] = temp_table[min_idx][1];
 temp_table[min_idx][0] = temp_in;
 temp_table[min_idx][1] = temp_out;
 }
 }

 for (int i = 0; i < current_size; i++)
 {
 inverse_temp_table[i][0] = input_table[i][1];
 inverse_temp_table[i][1] = input_table[i][0];
 }
 for (int i = 0; i < current_size - 1; i++)
 {
 int min_idx = i;
 for (int j = i + 1; j < current_size; j++)
 {
 if (inverse_temp_table[j][0] < inverse_temp_table[min_idx][0])
 {
 min_idx = j;
 }
 }
 if (min_idx != i)
 {
 int temp_in = inverse_temp_table[i][0];
 int temp_out = inverse_temp_table[i][1];
 inverse_temp_table[i][0] = inverse_temp_table[min_idx][0];
 inverse_temp_table[i][1] = inverse_temp_table[min_idx][1];
 inverse_temp_table[min_idx][0] = temp_in;
 inverse_temp_table[min_idx][1] = temp_out;
 }
 }

 for (int i = 0; i < current_size; i++)
 {
 double_size_table[i][0] = temp_table[i][0];
 double_size_table[i][1] = temp_table[i][1];
 double_size_table[current_size + i][0] = inverse_temp_table[i][0];
 double_size_table[current_size + i][1] = inverse_temp_table[i][1];
 }
}

int start_point(int current_size)
{
 return double_size_table[current_size][0];
}

void get_used_switch(int port)
{

 int sw_num;

 sw_num = port / 2;

 output_switch[sw_num] = output_switch[sw_num] + 1;
}

int outFind(int current_size, int order, int port, int position, int flag, int isFirstcall)
{
 if (isFirstcall)
 {
 port = start_point(current_size);
 position = POS_OUTPUT;
 flag = FLAG_PORT;
 order = 0;
 get_used_switch(port);
 }
 else
 {
 if (flag == FLAG_PORT)
 {
 if (position == POS_INPUT) // input -> output
 {
 int row = port;
 port = double_size_table[row][1];
 position = POS_OUTPUT;
 flag = FLAG_SWITCH;
 get_used_switch(port);
 }
 else if (position == POS_OUTPUT) // output -> input
 {
 int row = port + current_size;
 port = double_size_table[row][1];
 position = POS_INPUT;
 flag = FLAG_SWITCH;
 }
 }
 else if (flag == FLAG_SWITCH)
 {
 order = order + 1;
 if (position == POS_INPUT)
 {
 if (port % 2 == 0)
 {
 int row = port;
 port = double_size_table[row + 1][0];
 position = POS_INPUT;
 flag = FLAG_PORT;
 }
 else if (port % 2 == 1)
 {
 int row = port;
 port = double_size_table[row - 1][0];
 position = POS_INPUT;
 flag = FLAG_PORT;
 }
 }
 else if (position == POS_OUTPUT)
 {
 if (output_switch[port / 2] == 2)
 {
 loop = loop + 1;
 for (int i = 0; i < switch_size; i++)
 {
 if (output_switch[i] == 0)
 {
 port = i * 2;
 break;
 }
 }

 int row = port + current_size;
 port = double_size_table[row][0];
 position = POS_OUTPUT;
 flag = FLAG_PORT;
 get_used_switch(port);
 }
 else
 {
 if (port % 2 == 0)
 {
 int row = port + current_size;
 port = double_size_table[row + 1][0];
 position = POS_OUTPUT;
 flag = FLAG_PORT;
 get_used_switch(port);
 }
 else if (port % 2 == 1)
 {
 int row = port + current_size;
 port = double_size_table[row - 1][0];
 position = POS_OUTPUT;
 flag = FLAG_PORT;
 get_used_switch(port);
 }
 }
 }
 }
 }
 routing_path(port, position, order);
 for (int i = 0; i < current_size; i++)
 {
 path[i][PATH_ORDER] = i;
 }
 if (path[current_size - 1][POS_OUTPUT] != 0)
 return 0;
 else
 return outFind(current_size, order, port, position, flag, 0);
}
void routing_path(int port, int position, int order)
{
 if (position == POS_INPUT)
 {
 path[order][POS_INPUT] = port;
 }

 else if (position == POS_OUTPUT)
 {
 path[order][POS_OUTPUT] = port;
 }
}
void set_subnetwork(int current_size)
{

 for (int i = 0; i < current_size; i++)
 {
 if (i % 2 == 0)
 {
 path[i][SUBNETWORK] = SUB_U;
 }
 else
 {
 path[i][SUBNETWORK] = SUB_L;
 }
 }
}
void set_switch(int current_size)
{
 for (int i = 0; i < current_size / 2; i++)
 {
 if (path[2 * i][0] % 2 == 0)
 {
 IO_switch_arr[path[2 * i][0] / 2][0] = SWITCH_BAR;
 }

 else if (path[2 * i][0] % 2 == 1)
 {
 IO_switch_arr[path[2 * i][0] / 2][0] = SWITCH_CROSS;
 }

 if (path[2 * i][1] % 2 == 0)
 {
 IO_switch_arr[path[2 * i][1] / 2][1] = SWITCH_BAR;
 }
 else if (path[2 * i][1] % 2 == 1)
 {
 IO_switch_arr[path[2 * i][1] / 2][1] = SWITCH_CROSS;
 }
 }
}
// 완성된 path의 subnetwork 분리를 위해 2로 나누는 작업을 진행
void path_divider(int current_size)
{
 for (int i = 0; i < current_size; i++)
 {
 path[i][POS_INPUT] = path[i][POS_INPUT] / 2;
 path[i][POS_OUTPUT] = path[i][POS_OUTPUT] / 2;
 }
}
void subnet_table_maker(int current_size, int ID)
{ // 여기 들어가는 current_size는 first_layer의 size 그대로!

 for (int i = 0; i < current_size; i++)
 {
 if (path[i][SUBNETWORK] == SUB_U)
 {
 subnet_arr[ID * 2 - 1][i / 2][POS_INPUT] = path[i][POS_INPUT];
 subnet_arr[ID * 2 - 1][i / 2][POS_OUTPUT] = path[i][POS_OUTPUT];
 }
 else
 {
 subnet_arr[ID * 2][i / 2][POS_INPUT] = path[i][POS_INPUT];
 subnet_arr[ID * 2][i / 2][POS_OUTPUT] = path[i][POS_OUTPUT];
 }
 }
}
void total_switch_arr_maker(int current_size, int ID)
{

 for (int i = 0; i < current_size / 2; i++)
 {
 total_switch_arr[ID][i][0] = IO_switch_arr[i][0];
 total_switch_arr[ID][i][1] = IO_switch_arr[i][1];
 }
}

void first_layer(int current_size)
{
 get_port_num(current_size);
 table_sort(current_size);
 start_point(current_size);
 outFind(current_size, 0, 0, 0, 0, 1);
 set_subnetwork(current_size);
 set_switch(current_size);
 path_divider(current_size);
 subnet_table_maker(current_size, 1);
 total_switch_arr_maker(current_size, 0);
}

void mid_layer(int current_size)
{ // 여기 들어가는 current)_size는 first_layer의 절반인가? 맞네.
 // 생성된_새로운 라우팅 테이블을 구분해야겠지?

 for (int layer = 1; layer < num_layer - 1; layer++)
 {
    allocate_memory(current_size);
    for (int j = pow(2, layer) - 1; j < pow(2, layer + 1) - 1; j++) // 해당 계층의 layer 내에 모든 subnetwork를 정해야함.
    // 2^layer ~ 2^(layer+1)-1 로 설정하면 한 레이어의 서브네트워크를 훑을수 있음 2~3, 4~7,8~15 등등
        {
        
        for (int i = 0; i < current_size; i++) // 행의 size만큼!
            {
            input_table[i][0] = subnet_arr[j][i][0];
            input_table[i][1] = subnet_arr[j][i][1];
            }
        table_sort(current_size);
        start_point(current_size);
        outFind(current_size, 0, 0, 0, 0, 1);
        set_switch(current_size);
        set_subnetwork(current_size);
        path_divider(current_size);
        subnet_table_maker(current_size, j+1);
        total_switch_arr_maker(current_size, j);
        allocate_memory(current_size);
        }
 current_size = current_size / 2;
 
 }
}

int final_layer(int current_size)
{

 allocate_memory(current_size); //re_allocation about the new current_size:2.(final layer)

 for (int j = pow(2, num_layer - 1) - 1; j < pow(2, num_layer) - 1; j++)
 {
    for (int i = 0; i < current_size; i++)
        {
            input_table[i][0] = subnet_arr[j][i][0];
            input_table[i][1] = subnet_arr[j][i][1];
        }
    for (int i = 0; i < current_size; i++)
    {
        if (input_table[i][0] != input_table[i][1])
        {
            total_switch_arr[j][0][0] = 1;
            total_switch_arr[j][0][1] = 1;
        }
        else
        {
            total_switch_arr[j][0][0] = 0;
            total_switch_arr[j][0][1] = 0;
        }
    }
 }
 return 0;
}

void transfer_switch_arr(int current_size) {
    int sw_size = current_size / 2;
    int total_layer = log2(current_size);

    for (int layer = 0; layer < total_layer - 1; layer++) { // except last layer
        int num_subnets = pow(2, layer); 
        int switches_per_subnet = sw_size / num_subnets;

        for (int subnet = 0; subnet < num_subnets; subnet++) {
            int ID = pow(2, layer) - 1 + subnet;

            for (int row = 0; row < switches_per_subnet; row++) {
                int switch_idx = subnet * switches_per_subnet + row;

                stage_switch_arr[switch_idx][layer] = total_switch_arr[ID][row][0];
                stage_switch_arr[switch_idx][total_layer * 2 - 2 - layer] = total_switch_arr[ID][row][1];
            }
        }
    } 

    // Handle the last layer
    int num_subnets_last_layer = pow(2, total_layer - 1); 
    int switches_per_subnet_last_layer = sw_size / num_subnets_last_layer;

    for (int subnet = 0; subnet < num_subnets_last_layer; subnet++) {
        int ID = pow(2, total_layer - 1) - 1 + subnet;

        for (int row = 0; row < switches_per_subnet_last_layer; row++) {
            int switch_idx = subnet * switches_per_subnet_last_layer + row;
            
            // Using the 0th column as requested
            stage_switch_arr[switch_idx][total_layer - 1] = total_switch_arr[ID][row][0];
        }
    }
}

void print_stage_switch(int current_size){
    int rows = current_size / 2;
    int total_layer = log2(current_size); // layer count
    int cols = total_layer * 2 - 1;      // stage_num = layer * 2 - 1

// Print the headers first
    for (int j = 0; j < cols; j++) {
        printf("[stage %d]\t", j);
    }
    printf("\n"); // Move to the next line after printing headers

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (stage_switch_arr[i][j] == 0) {
                printf("[  bar  ]\t");
            } else {
                printf("[ cross ]\t");
            }
        }
        printf("\n"); // Move to the next line after printing each row
    }
}