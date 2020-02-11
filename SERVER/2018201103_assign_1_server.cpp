#include "header.h"


// int caesar_key;
FILE *plaintext;
int server_fd;
struct sockaddr_in address;
int addrlen = sizeof(address);
char encoding_scheme[]={' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',',','.','?','0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','!'};

 

/*
    returns size of the file
*/
long long getFileSize(string file)
{
    struct stat sb;
    stat(file.c_str(), &sb);
    return sb.st_size;
}

/*
This Function for fast exponential
*/
ull power(ull base,ull n,ull prime)
{
	unsigned long long result=1;
	unsigned long long y=base;

	while(n>0)
	{
		if(n & 1)
			result = (result * y) % prime;

		y = (y * y) % prime;

		n = n>>1;
	}	
	ull finalresult = result % prime;
	return finalresult; 
    
}

/*
This Function create new socket for server
*/
void createServerSocket()
{
	
	int opt=1;
	// int s_p  ort=PORT;
	server_fd = socket(AF_INET , SOCK_STREAM , 0);	//AF_INEt for ipv4,sock_stream for TCP,0 fpr internet protocol

	if(setsockopt(server_fd , SOL_SOCKET , SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
			//helps in reuse of address and port. optional
	{
		printf("setsockopt\n");
		exit(0);
	}

	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;   //INADDR_ANY for localhos
    address.sin_port = htons( PORT );   //The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
      
    
    if (bind(server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        printf("bind failed");
        exit(0);
    }
    listen(server_fd, 5);
    
}

/*
This Function used ffor initial communication while establishing secrete key
Send the data to client 
*/
void sendInitialtoClient(char *stoc_buff,int new_socket)
{
    send(new_socket,stoc_buff,strlen(stoc_buff),0);
}

/*
This Function used ffor initial communication while establishing secrete key
Receive data from client 
*/
void receiveIntialFromClient(char *ctos_buff,int new_socket)
{   
    int i=0;
    i=read( new_socket , ctos_buff, MAX);
    ctos_buff[i]='\0';           
  
}


/*
Write Log into serverlog.txt file 
*/
// void writelog(string str)
// {
//     ofstream myfile("serverlog.txt", std::ios_base::app | std::ios_base::out);
//     myfile << str << endl;
//     myfile.close();
// }


/*
Generate Public key and establish secreate key (caesar key) with given socket_client
*/
void extractDataAndGenerateKeys(int new_socket, vector<ull> &des_key)
{
    for(int it=0;it<3;it++){
        char ctos_buff[MAX];
        char stoc_buff[MAX];
        // receiveIntialFromClient(ctos_buff,new_socket); 
        PubKey recv_key;
        int nbytes = recv(new_socket, &recv_key, sizeof(PubKey), 0);
        
        ull prime_num = recv_key.q;
        ull primitive_root = recv_key.r;
        ull c_public_key = recv_key.Y;

        int i=0,j=0;
        char temp[MAX];

        printf("Prime number            :%llu\n",prime_num);
        printf("Primitive root          :%llu\n\n",primitive_root);
        

        ull s_private_key = rand() % (prime_num - 1) + 1;
        printf("Private key of Server   :%llu\n",s_private_key);

        ull s_public_key = power(primitive_root,s_private_key,prime_num);
        printf("Public key of Server    :%llu\n\n",s_public_key);

        printf("Public key of Client    :%llu\n\n",c_public_key);

        sprintf(stoc_buff,"%llu",s_public_key);
        strncpy(stoc_buff,stoc_buff,strlen(stoc_buff));
        sendInitialtoClient(stoc_buff,new_socket);

        ull shared_key = power(c_public_key, s_private_key , prime_num);
        printf("Shared key is           :%llu\n\n",shared_key);
    
        cout<<"des key of "<<it<<" = "<<shared_key<<endl;
        des_key[it]=shared_key;
    }
}

/*
This function find SHA1 hash of given string
*/
// string findHash(string str)
// {
//     int len = str.length()+1;
//     char * schunk = new char [len];
//     strcpy (schunk, str.c_str());

//     unsigned char hash[SHA_DIGEST_LENGTH];
//     char buf[SHA_DIGEST_LENGTH * 2];
//     SHA1((unsigned char *)schunk, len , hash);

//     for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
//         sprintf((char *)&(buf[i * 2]), "%02x", hash[i]);

//     string ans;
//     for (int i = 0; i < SHA_DIGEST_LENGTH * 2; i++)
//     {
//         ans += buf[i];
//     }
//     return ans;
    
// }

/*
send Msg to client
*/
int sendMsgtoClient(Msg reply, int new_socket)
{
    int status = send(new_socket, &reply, sizeof(Msg), 0);
	return status;
}


bool file_present(string s){
    DIR *dir;
    struct dirent *ent;
    
    if ((dir = opendir ("./files")) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            string current_file = ent->d_name;
            
            struct stat st;
            string file_path = "./files/"+current_file; 
            if( stat(file_path.c_str(),&st) == 0 ){
                if( st.st_mode & S_IFREG ){
                    //it's a file
                    if(current_file==s){
                        closedir (dir);        
                        return true;
                    }
                }
            }
            
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        return false;
    }
    return false;
}

void send_disconnect(int client_socket){
    int src_addr, dest_addr;
	Msg send_msg;

	dest_addr = inet_addr("DEFAULT_SERVER");
	src_addr = inet_addr("12printf7.0.0.1");

	send_msg.hdr.opcode = DISCONNECT;
	send_msg.hdr.s_addr = src_addr;
	send_msg.hdr.d_addr = dest_addr;
	
	Disconnect dis;
	send_msg.body.disconnect = dis;

	send(client_socket, &send_msg, sizeof(Msg), 0);
    // close(client_socket);
}

void send_ENC_MSG(unsigned char out[BUFSIZE], int client_socket, int chunksize){
    // cout<<"in send ENC MSG with cs = "<<chunksize<<endl;
    int src_addr, dest_addr;
	Msg send_msg;

	dest_addr = inet_addr("DEFAULT_SERVER");
	src_addr = inet_addr("127.0.0.1");

	send_msg.hdr.opcode = ENCMSG;
	send_msg.hdr.s_addr = src_addr;
	send_msg.hdr.d_addr = dest_addr;
	
	EncMsg em;
    em.size = chunksize;
    for(int i=0;i<BUFSIZE;i++)
    {
        em.cipher_block[i]=out[i];
    }
    // strcpy((char *)em.cipher_block, (const char *)out);
    send_msg.body.encmsg = em;

	send(client_socket, &send_msg, sizeof(Msg), 0);
}

void send_request_comp(int client_socket){
    int src_addr, dest_addr;
	Msg send_msg;

	dest_addr = inet_addr("DEFAULT_SERVER");
	src_addr = inet_addr("127.0.0.1");

	send_msg.hdr.opcode = REQCOMP;
	send_msg.hdr.s_addr = src_addr;
	send_msg.hdr.d_addr = dest_addr;
	
    ReqCom rq;
    send_msg.body.reqcom = rq;

	send(client_socket, &send_msg, sizeof(Msg), 0);
}

void send_file(string file_name, int client_socket, vector<ull> &des_key){

    file_name = "./files/"+file_name;

    unsigned char in[BUFSIZE], out[BUFSIZE];
    // char *keystr = "Victoria Harbour";
    // char *key1 = "2000141204";
    // char *key2 = "101636102";
    // char *key3 = "1916220121";
    // cout<<"The three keys are:"<<endl;
    // cout<<des_key[0]<<" "<<des_key[1]<<" "<<des_key[2]<<endl;
    // cout<<(char *)to_string(des_key[0]).c_str()<<" "<<(char *)to_string(des_key[1]).c_str()<<" "<<(char *)to_string(des_key[2]).c_str()<<endl;
   
    string s1,s2,s3;
    s1 = to_string(des_key[0]);
    s2 = to_string(des_key[1]);
    s3 = to_string(des_key[2]);

    cout<<"sending file "<<file_name<<" with keys "<<endl;
    // cout<<"STRING KEYS----------------------------------"<<endl;
    cout<<s1<<" "<<s2<<" "<<s3<<endl;


    char* ckey1 = &*(s1).begin();
	char* ckey2 = &*(s2).begin();
    char* ckey3 = &*(s3).begin();


    // char* ckey1 = &*(to_string(des_key[0])).begin();
	// char* ckey2 = &*(to_string(des_key[1])).begin();
    // char* ckey3 = &*(to_string(des_key[2])).begin();

    // char *ckey1 = (char *)to_string(des_key[0]).c_str();
    // char *ckey2 = (char *)to_string(des_key[1]).c_str();
    // char *ckey3 = (char *)to_string(des_key[2]).c_str();

    // cout<<"KEYS----------------------------------"<<endl;
    // cout<<ckey1<<" "<<ckey2<<" "<<ckey3<<endl;

    // cout<<"The three keys are:"<<endl;
    // cout<<des_key[0]<<" "<<des_key[1]<<" "<<des_key[2]<<endl;

    ifstream fin;
    int i, num, result;
 
    DES_cblock k1,k2,k3;
    DES_cblock ivsetup = {0xE1, 0xE2, 0xE3, 0xD4, 0xD5, 0xC6, 0xC7, 0xA8};
    DES_key_schedule ks1,ks2,ks3;
    DES_cblock ivec;
 
    memset(in, 0, sizeof(in));
    memset(out, 0, sizeof(out));
    // memset(back, 0, sizeof(back));
 
    DES_string_to_key(ckey1, &k1);
    DES_string_to_key(ckey2, &k2);
    DES_string_to_key(ckey3, &k3);
 
    if ((result = DES_set_key_checked((DES_cblock *)k1, &ks1)) != 0) {
        if (result == -1) {
            printf("ERROR: key parity is incorrect\n");
        } else {
            printf("ERROR: weak or semi-weak key\n");
        }
        exit(1);
    }

    if ((result = DES_set_key_checked((DES_cblock *)k2, &ks2)) != 0) {
        if (result == -1) {
            printf("ERROR: key parity is incorrect\n");
        } else {
            printf("ERROR: weak or semi-weak key\n");
        }
        exit(1);
    }

    if ((result = DES_set_key_checked((DES_cblock *)k3, &ks3)) != 0) {
        if (result == -1) {
            printf("ERROR: key parity is incorrect\n");
        } else {
            printf("ERROR: weak or semi-weak key\n");
        }
        exit(1);
    }

    fin.open(file_name, ios::binary | ios::in);
    if (!fin) {
        printf(" ERROR: opening input file\n");
        exit(1);
    }
    int chuk=0;
    int chunksize=BUFSIZE;
    long long sizeLeftToSend = getFileSize(file_name);
    if (sizeLeftToSend < chunksize)
        chunksize = sizeLeftToSend;
    while (fin.read((char *)in, chunksize))
    {
        sizeLeftToSend -= chunksize;

        memcpy(ivec, ivsetup, sizeof(ivsetup));
        num = 0;
        for (i = 0; i < chunksize; i++) {
            DES_ede3_ofb64_encrypt(&(in[i]), &(out[i]), 1, &ks1, &ks2, &ks3, &ivec, &num);
        }

        if (chunksize == 0)
            break;

        // cout<<"sending chunk "<<(++chuk)<<" of size "<<chunksize<<endl;
        send_ENC_MSG(out, client_socket, chunksize);

        // send_ENC_MSG(in, client_socket, chunksize);

        usleep(1000);
        // sleep(1);
        // memcpy(ivec, ivsetup, sizeof(ivsetup));
        // num = 0;
        // for (i = 0; i < chunksize; i++) {
        //     DES_ede3_ofb64_encrypt(&(out[i]), &(back[i]), 1, &ks1, &ks2, &ks3, &ivec, &num);
        // }
        // 56350

        memset(in, 0, sizeof(in));
        memset(out, 0, sizeof(out));
        if (sizeLeftToSend < chunksize)
            chunksize = sizeLeftToSend;

    }

    fin.close();
    send_request_comp(client_socket);

}

void send_not_found(int client_socket){
    send_disconnect(client_socket);
}

void *serverservice(void *socket_desc)
{
    vector<ull> des_key(3);
    int new_socket = *(int *)socket_desc;
    cout<<"new connection with socket id : "<<new_socket<<endl<<endl;
    extractDataAndGenerateKeys(new_socket,des_key);

    // cout<<"The three keys are:"<<endl;
    // cout<<des_key[0]<<" "<<des_key[1]<<" "<<des_key[2]<<endl;
    int flag=1;
    do
    {
        Msg recv_msg;
        cout<<"Waiting..."<<endl;
        int nbytes = recv(new_socket, &recv_msg, sizeof(Msg), 0);
        if (nbytes == -1) {
            fprintf(stderr, "*** Server error: unable to receive\n");
            return NULL;
        }
        int opcode = recv_msg.hdr.opcode;

        //according to Message opcode call aprropiate function call to handle it
        if(opcode==PUBKEY)
        {
        }
        else if(opcode == REQSERV)
        {

            // cout<<"The three keys are:"<<endl;
            // cout<<des_key[0]<<" "<<des_key[1]<<" "<<des_key[2]<<endl;
            string s = recv_msg.body.reqserv.file_name;
            if(file_present(s)){
                send_file(s,new_socket,des_key);

            }
            else{
                send_not_found(new_socket);
            }
        }
        else if(opcode == ENCMSG)
        {
        }
        else if(opcode == REQCOMP)
        {
        }
        else if(opcode == DISCONNECT)
        {
            send_disconnect(new_socket);
        }
        else
        {
            cout<<"Invalid code receive in server side"<<endl;
            flag=0;
        }
        recv_msg.hdr.opcode=0;
        
    }while(flag);

    return socket_desc;
}

int main()
{
    srand(time(NULL));
    // ReadPasswordFile();
    printf("Waiting for client to connect\n");
   
	createServerSocket();
    pthread_t thread_id;
    int new_socket;
    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Error in accept connection");
            exit(0);
        }

        printf("Connection established\n\n\n");    
        
        if (pthread_create(&thread_id, NULL, serverservice, (void *)&new_socket) < 0)
        {
            perror("\ncould not create thread\n");
        }

    }

	return 0;
}