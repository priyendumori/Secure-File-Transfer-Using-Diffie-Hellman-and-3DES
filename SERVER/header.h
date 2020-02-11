#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <fstream>
#include <openssl/sha.h>
#include <netinet/in.h> 
#include <pthread.h>
#include <dirent.h>
#include <openssl/des.h>

#define ll long long
#define ull unsigned long long


/*
Protocol op codes
*/
#define PUBKEY 10  
#define REQSERV 20
#define ENCMSG 30
#define REQCOMP 40
#define DISCONNECT 50

#define BUFSIZE 1024

/*
Some constant Variable Declared
*/
#define PORT 28781
#define MAX 1024
#define MAX_SIZE 80

using namespace std;
/* Header of a general message */
typedef struct {
    int opcode; /* opcode for a message */
    int s_addr; /* source address */
    int d_addr; /* destination address */
} Hdr;

/* Public key Y A = r X A mod q or Y B = r X B mod q */
typedef struct {
    ll q; /* large prime */
    ll r; /* primitive root */
    ll Y ; /* public key */
} PubKey;

typedef struct {
    char file_name[MAX_SIZE];
} ReqServ;

typedef struct {
    
} ReqCom;

typedef struct {
    ll size;
    char file_name[MAX_SIZE];
    char cipher_block[BUFSIZE];
} EncMsg;

typedef struct {
    
} Disconnect;


typedef union {
    PubKey pubkey;
    ReqServ reqserv;
    ReqCom reqcom;
    EncMsg encmsg;
    Disconnect disconnect;
} AllMsg;

/*A general message */
typedef struct {
    Hdr hdr; /* Header for a message */
    AllMsg body;
} Msg;

