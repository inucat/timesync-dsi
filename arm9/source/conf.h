#ifndef CONF_H
#define CONF_H

struct config
{
    char ntp_server[256];
    int ntp_port;
    int offset_minutes;
};

/**
 * Loads the configuration from a file into the provided struct.
 * @param conf Pointer to the configuration struct to be populated.
 * @return 0 on success, -1 on failure.
 */
int load_conf(struct config *conf);

#endif /* CONF_H */
