int blkdevseek(int dev, off_t offset);
int blkdevread(int dev, char *buffer, size_t len);
int blkdevwrite(int dev, char *buffer, size_t len);


