#include "header.h"

#define DEFAULT_SERVER "127.0.0.1"

char ctos_buff[MAX];
char stoc_buff[MAX];

char *ip;
int sock,port;

ull des_key[3];

/*
Fast exponential algo to find power
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
Function to test Miller Test
*/
int my_miller(ull value)
{
	if(value%2==0|| value<2)
		return 0;
	ull q = value - 1;
	ull k = 0;

	while(q%2==0)
	{
		q>>=1;
		k++;
	}	 
	for(int i=0;i<15;i++)
	{	
		long long int a= rand() % (value - 4) + 2;
		long long int result = power(a,q,value);
		int flag=1;
		
		for(;q != value - 1 ; q*=2)
		{
			if(result==1 || result == value - 1)
			{
				flag=0;
				break;
			}
			result = result * result % value;
		}	
		if(flag==1)
				return 0;
	}	
	return 1;

}

/*
This function generate large prime number which pass miller test
*/
ull get_prime()
{
	srand(time(NULL));
    // /* Seed */
    // std::random_device rd;
    
    // /* Random number generator */
    // std::default_random_engine generator(rd());
    
    // /* Distribution on which to apply the generator */
    // std::uniform_int_distribution<long long unsigned> distribution(10000000000,100000000001);
    
    // for (int i = 0; i < 10; i++) {
    //     std::cout << distribution(generator) << std::endl;
    // }
	while(1)
	{
        // ll prime = distribution(generator);

        // prime=0;
        // ll mul=1;
        // for(int x = 0; x < 16 ; x++,mul*=10){
        //     int digit = rand()%10;
        //     prime+=mul*digit;
        // }

        // cout<<"trying "<<prime<<endl;
		ull prime= rand() % LLONG_MAX;
        if(my_miller(prime))
			return prime;
	}	
}

/*
This function returns primitive root of n
*/
ull my_Proot(ull n)
{
	ull MaxSize = 1000000;	

	ull prime[MaxSize];
	memset(prime,1,sizeof(prime));
    for(int p=2; p*p<MaxSize;p++)
	{
        
		if(prime[p])
		{
			for(int i=p*2;i<MaxSize; i+=p)
				prime[i]=0;
		}	
	}	
	ull phi=n-1;	
	ll flag=1;
	ll i = 2;
	while(i++)
	{	
		int j;
		for(j=2;j<=sqrt(phi);j++)
		{
			if(prime[j] && phi%prime[j]==0 && power(i,phi/prime[j],n)==1)
			{	
				flag=0;
				break;
			}	
		}	
		if(flag)
			return i;
	}	
	return -1;
}

/*
This function Make connection with server & set sock for socket communication
*/
void connecttoServer()
{
	int s_port=PORT;
	struct sockaddr_in serv_addr;
	sock = socket(AF_INET, SOCK_STREAM,0);
	memset(&serv_addr,'0',sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(s_port);

	inet_pton(AF_INET , "127.0.0.1" , &serv_addr.sin_addr); //converts the character string src into a network address structure in the af address family, then copies the network address structure to dst

	if(connect(sock,(struct  sockaddr *)&serv_addr, sizeof(serv_addr)))
	{
		printf("Connection not established\n");
		exit(0);
	}

}

/*
Send given data to server
*/
void send_to_server(char *s)
{
    strncpy(ctos_buff,s,strlen(s));
    
    send(sock,ctos_buff,strlen(ctos_buff),0);

} 

/*
Receive data from server
*/
void receive_from_server()
{    
    int i=read(sock,stoc_buff, MAX);
    stoc_buff[i]='\0';
}

/*
This function Generate public key & establish secreate key (caesar key) between client and server
*/
void generatePublicNumberforKey()
{
    for(int it=0;it<3;it++){
        ull prime_num=get_prime();
        printf("Prime number		:%llu\n",prime_num);
        cout<<"p is "<<prime_num<<endl;
        ull primitive_root=my_Proot(prime_num);
        printf("Primitive root  	:%llu\n\n",primitive_root);

        ull c_private_key = rand() % (prime_num - 1) + 1;
        printf("Private key of Client 	:%llu\n",c_private_key);

        ull c_public_key = power(primitive_root,c_private_key,prime_num);
        printf("Public key of Client 	:%llu\n\n",c_public_key);
        
        if(it==0)  connecttoServer();

        sprintf(ctos_buff,"%llu$%llu$%llu",prime_num, primitive_root, c_public_key);

        PubKey pk;
        pk.q = prime_num;
        pk.r = primitive_root;
        pk.Y = c_public_key;
        cout<<" PPP - Sending "<<pk.q<<" "<<pk.r<<" "<<pk.Y<<endl;
        send(sock, &pk, sizeof(PubKey), 0);
        // send_to_server(ctos_buff);
        
        receive_from_server();

        ull s_public_key = atol(stoc_buff);
        printf("Public key of Server    :%llu\n\n",s_public_key);

        ull shared_key = power(s_public_key, c_private_key , prime_num);
        printf("Shared key		:%llu\n\n",shared_key);
	
        des_key[it]=shared_key;
    }
}

void send_disconnect(){
    int src_addr, dest_addr;
	Msg send_msg;

	dest_addr = inet_addr("DEFAULT_SERVER");
	src_addr = inet_addr("127.0.0.1");

	send_msg.hdr.opcode = DISCONNECT;
	send_msg.hdr.s_addr = src_addr;
	send_msg.hdr.d_addr = dest_addr;
	
	Disconnect dis;
	send_msg.body.disconnect = dis;

	send(sock, &send_msg, sizeof(Msg), 0);
    // close(sock);
}

void request_file(){

	cout<<"Please enter the file name: ";
	string file_name;
	cin>>file_name;

	string file_path = "./downloads/"+file_name;

	ofstream myfile;
	myfile.open (file_path,ios::out | ios::binary);

	int src_addr, dest_addr;
	Msg send_msg;

	dest_addr = inet_addr("DEFAULT_SERVER");
	src_addr = inet_addr("127.0.0.1");

	/* send the request message REQ to the server */
	printf("Sending the request message REQ to the server\n");          
	send_msg.hdr.opcode = REQSERV;
	send_msg.hdr.s_addr = src_addr;
	send_msg.hdr.d_addr = dest_addr;
	
	ReqServ reqserv;
	strcpy(reqserv.file_name, file_name.c_str());
	send_msg.body.reqserv = reqserv;
	cout<<"Request for file sent"<<endl;
	send(sock, &send_msg, sizeof(Msg), 0);

	int chuk=0;
	Msg recv_msg;
	int nbytes;
	while((nbytes = recv(sock, &recv_msg, sizeof(Msg), 0)) > 0){
		if (nbytes == -1) {
			fprintf(stderr, "*** Server error: unable to receive\n");
		}
		else{
			if(recv_msg.hdr.opcode==DISCONNECT){
				cout<<endl<<"File not found at server"<<endl;
				send_disconnect();
				break;
			}
			else if(recv_msg.hdr.opcode==ENCMSG){
				unsigned char out[BUFSIZE], back[BUFSIZE];
				// char *key1 = "2000141204";
				// char *key2 = "101636102";
				// char *key3 = "1916220121";
				// cout<<"The three keys are:"<<endl;
    			// cout<<des_key[0]<<" "<<des_key[1]<<" "<<des_key[2]<<endl;
    
				char *key1 = (char *)to_string(des_key[0]).c_str();
				char *key2 = (char *)to_string(des_key[1]).c_str();
				char *key3 = (char *)to_string(des_key[2]).c_str();


				// cout<<"KEYS----------------------------------"<<endl;
				// cout<<key1<<" "<<key2<<" "<<key3<<endl;
				int i, num, len, result;
				int n = 0;
			
				DES_cblock k1,k2,k3;
				DES_cblock ivsetup = {0xE1, 0xE2, 0xE3, 0xD4, 0xD5, 0xC6, 0xC7, 0xA8};
				DES_key_schedule ks1,ks2,ks3;
				DES_cblock ivec;
			
				memset(out, 0, sizeof(out));
				memset(back, 0, sizeof(back));
			
				DES_string_to_key(key1, &k1);
				DES_string_to_key(key2, &k2);
				DES_string_to_key(key3, &k3);
			
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

				EncMsg em = recv_msg.body.encmsg;
				long long chunksize = em.size;

				// strcpy((char *)out,(const char *)em.cipher_block);
				for(int i=0;i<chunksize;i++){
					out[i]=em.cipher_block[i];
				}
				// cout<<em.cipher_block<<endl;

				memcpy(ivec, ivsetup, sizeof(ivsetup));
				num = 0;

				for (i = 0; i < chunksize; i++) {
					DES_ede3_ofb64_encrypt(&(out[i]), &(back[i]), 1, &ks1, &ks2, &ks3, &ivec, &num);
				}
				// myfile.write((const char *)out,chunksize);
				// cout<<"writing chunk no "<<(++chuk)<<" of size "<<chunksize<<endl;
				myfile.write((const char *)back,chunksize);
				memset(back, 0, sizeof(back));
        		memset(out, 0, sizeof(out));
		
			}
			else if(recv_msg.hdr.opcode==REQCOMP){
				cout<<"DONE"<<endl;
				break;
			}
		}
	}

	myfile.close();
}

int main(int argc, char* argv[])
{
	//cout<<"char encoding size : "<<sizeof(encoding_scheme)/sizeof(char);

	if(argc!=2)
	{	
		printf("Please provide IP address \n");
		return 0;
	}	
	srand(time(NULL));
	
    generatePublicNumberforKey();

    cout<<"The three keys are:"<<endl;
    cout<<des_key[0]<<" "<<des_key[1]<<" "<<des_key[2]<<endl;

	int flag=1;

	// Menu driven Program for user
	do
	{
		int ch;
		cout<<"\n\n";
		cout<<"1. Request a file "<<endl;
		cout<<"0. Exit"<<endl;
		cout<<"\nEnter your input : "<<endl;
		cin>>ch;
		switch(ch)
		{
			case 1 :
				// loginrequest(argv[1]);
				request_file();
				break;
			case 0:
				flag=0;
				break;
			default:
				cout<<"Please give correct input :("<<endl;
				break;
		}
	}while(flag);
	cout<<"\n\nThank You, Have a happy CryptoDay !!!"<<endl;

	return 0;
}