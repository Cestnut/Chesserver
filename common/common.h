//enum begins from 1 because values are casted from string with atoi(). 
//atoi returns 0 when an invalid character is met.
typedef enum {CREATE_GAME=1, JOIN_GAME, SPECTATE_GAME, EXIT} client_choice;

void flush_stdin();
