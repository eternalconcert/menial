
const int BUFFER_SIZE = 256;
const int BUFFER_LIMIT = BUFFER_SIZE - 1;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
