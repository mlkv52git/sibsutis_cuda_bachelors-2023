const unsigned int window_width  = 1024;//512;
const unsigned int window_height = 1024;//512;
#define L 128
#define M 128
const int num_of_verticies=16*L*M;
enum bufferNames{POSITIONS, VELOCITIES, GRID,TEXTURE, MAP, NUM_OF_BUFFERS};
void checkErrors(std::string desc); 
