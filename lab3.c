int main(int argc, char* argv[])
{
    FILE* input_file;

    const char* file_name = (argc == 2) ? argv[1] : DEFAULT_FILE_NAME;

    for (int index = 0; index < 2; ++index) {

        fprintf(stdout, "Real UID:%u\nEffective UID:%u\n", getuid(), geteuid());
        input_file = open_file(file_name);
        if (input_file == NULL) {
            return (EXIT_FAILURE); 
        }

        if (setuid(getuid()) < 0) {
            perror("Error");
            return (EXIT_FAILURE);
        }

        if (fclose(input_file) == EOF) {
            perror("Error");
            return (EXIT_FAILURE);
        }
    }

    return (EXIT_SUCCESS);
}
