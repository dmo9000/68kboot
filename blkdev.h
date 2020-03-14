int blkdevseek(uint8_t dev, off_t offset);
int blkdevread(uint8_t dev, char *buffer, size_t len);
int blkdevwrite(uint8_t dev, char *buffer, size_t len);


