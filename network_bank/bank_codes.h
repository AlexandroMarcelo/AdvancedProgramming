//ALexandro Francisco Marcelo Gonzalez A01021383
//30/03/2019

// The different types of operations available
typedef enum valid_operations {CHECK, DEPOSIT, WITHDRAW, TRANSFER, EXIT} operation_t;

// The types of responses available
typedef enum valid_responses {OK, DEPOSIT_OK, INSUFFICIENT, NO_ACCOUNT, TRANSFER_OK, BYE, ERROR} response_t;
