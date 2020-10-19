// Karim Zaher
// CSCE 313 - 201
// Fall 2020

int main(int argc, char* argv[])
{
    // ----- SETUP ----- //
    if (argc != 5) {
        printf("Usage: file_name planet_number cave_number num_threads search_method");
        exit(-1);
    }
    int planetNumber = std::stoi(argv[1]);
    int caveNumber = std::stoi(argv[2]);
    int NUMROBOTS = std::stoi(argv[3]);
    string searchType = argv[4];

    if (planetNumber > 7 || planetNumber < 1) {
        printf("Invalid planet number(Use Planets 1-7)");
        exit(-1);
    }
    printf("Starting CC.exe...\n");
}