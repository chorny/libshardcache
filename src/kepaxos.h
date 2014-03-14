//
// Key-based Egalitarian Paxos
//

#ifndef __CONSENSUS__H__
#define __CONSENSUS__H__

#include <sys/types.h>
#include <stdint.h>

typedef struct __kepaxos_cmd_s kepaxos_cmd_t;
typedef struct __kepaxos_s kepaxos_t;

typedef int (*kepaxos_send_callback_t)(char **recipients,
                                       int num_recipients,
                                       void *cmd,
                                       size_t cmd_len,
                                       void *priv);

typedef int (*kepaxos_commit_callback_t)(unsigned char type,
                                         void *key,
                                         size_t klen,
                                         void *data,
                                         size_t dlen,
                                         int leader,
                                         void *priv);

typedef int (*kepaxos_recover_callback_t)(char *peer,
                                          void *key,
                                          size_t klen,
                                          uint64_t seq,
                                          int32_t prio,
                                          void *priv);

typedef struct {
    kepaxos_send_callback_t send;
    kepaxos_commit_callback_t commit;
    kepaxos_recover_callback_t recover;
    void *priv;
} kepaxos_callbacks_t;

kepaxos_t *kepaxos_context_create(char *dbfile,
                                  char **peers,
                                  int num_peers,
                                  int my_index,
                                  int timeout,
                                  kepaxos_callbacks_t *callbacks);

void kepaxos_context_destroy(kepaxos_t *ke);

int kepaxos_run_command(kepaxos_t *ke,
                        unsigned char type,
                        void *key,
                        size_t klen,
                        void *data,
                        size_t dlen);

int kepaxos_received_command(kepaxos_t *ke, void *cmd, size_t cmdlen, void **response, size_t *response_len);
int kepaxos_received_response(kepaxos_t *ke, void *res, size_t reslen);

uint64_t kepaxos_ballot(kepaxos_t *ke); // returns the current ballot

#endif
