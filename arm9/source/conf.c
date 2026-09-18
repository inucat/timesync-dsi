#include "conf.h"

#include <ctype.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONF_FILE_PATH "/timesync-dsi.conf"
#define KEY_SERVER "server"
#define KEY_PORT "port"
#define KEY_OFFSET "offset_minutes"

#define DEFAULT_NTP_SERVER "time.google.com"
#define DEFAULT_NTP_PORT 123
#define DEFAULT_OFFSET_MINUTES 0

#define MAX_LINE_LENGTH 128

static char *trim(char *s)
{
    if (!s)
        return s;

    while (*s && isspace((int)*s))
        s++;

    char *end = s + strlen(s);

    while (end > s && isspace((int)end[-1]))
        end--;

    *end = '\0';

    return s;
}

int load_conf(struct config *conf)
{
    // Set default values
    strncpy(conf->ntp_server, DEFAULT_NTP_SERVER, sizeof(conf->ntp_server) - 1);
    conf->ntp_port = DEFAULT_NTP_PORT;
    conf->offset_minutes = DEFAULT_OFFSET_MINUTES;

    if (!fatInitDefault())
    {
        return -1;
    }

    FILE *file = fopen(CONF_FILE_PATH, "r");
    if (file == NULL)
    {
        return -2;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *key, *value;

        key = trim(strtok(line, "="));
        value = trim(strtok(NULL, "="));

        if (!key || !value || strlen(value) == 0)
        {
            continue;
        }

        if (strcasecmp(key, KEY_SERVER) == 0)
        {
            strncpy(conf->ntp_server, value, sizeof(conf->ntp_server) - 1);
            conf->ntp_server[sizeof(conf->ntp_server) - 1] = '\0';
            continue;
        }
        if (strcasecmp(key, KEY_PORT) == 0)
        {
            conf->ntp_port = strtol(value, NULL, 10);
            continue;
        }
        if (strcasecmp(key, KEY_OFFSET) == 0)
        {
            conf->offset_minutes = strtol(value, NULL, 10);
            continue;
        }
    }

    return 0;
}
