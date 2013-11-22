/* fft_sender.c
 *
 * This file will contain functions that will package fft data to be sent
 * through a socket.
 *
 * Author: Ahmed Ismail
 */

#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// #include "fft_sender.h"
#include "fft_socket_header.h"
//#include "monofft.h"

void genfft(float*, int);

struct fft_header * hdr;

void init_fft(int bytesToNextHeader, int samplesToNextFFT, int ptsPerFFT,
                         int sampFreq, int endTrans)
{
        hdr = (struct fft_header*) malloc(sizeof(struct fft_header));
        hdr->constSync = 0xACFDFFBC;
        hdr->bytesToNextHeader = sizeof(float) * (ptsPerFFT/2+1) + sizeof(struct fft_header);	
        hdr->ptsPerFFT = ptsPerFFT/2+1;		
        hdr->samplesToNextFFT = samplesToNextFFT;
        hdr->sampFreq = sampFreq;
        // -1 when ongoing, 1 to signal end of transmission
        hdr->endTrans = endTrans;

        // RETURN int ERROR CODES
}

void error1(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;


    if (argc < 2)
    {
     fprintf(stderr,"ERROR, no host provided\n");
     exit(1);
    }
    portno = 51717;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error1("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(-1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Error on connect(): %s\n", strerror(errno));
        exit(-1);
    }
    

    ///////////////////////////////////////////////////////////////////////////////////////////

    int bytesToNextHeader = 5; // total amount of space (header+data)
    int samplesToNextFFT = 3; // Num samples to the start of the next FFT
    int ptsPerFFT = 256; // number of points per FFT
    int sampFreq = 4;
    int endTrans = -1;
    
    init_fft(bytesToNextHeader, samplesToNextFFT, ptsPerFFT, sampFreq, endTrans);
    int header_len = sizeof(struct fft_header);
    
        
        float fbuffer[ptsPerFFT/2+1];	///changesssss
        int i, j;

    int k = 0;
    while(1){
    //while(k < 5){
        fprintf(stderr, "Sending header... ");

        n = write(sockfd, (char *) hdr, header_len);
        fprintf(stderr, "Sent header, n = %d\n", n);
        if (n < 0)
             error1("ERROR writing to socket");

        genfft(fbuffer, ptsPerFFT);
        
        printf("Sending fbuffer\n");
        fprintf(stderr, "Sending data... ");
        n = write(sockfd, fbuffer, (ptsPerFFT/2+1) * sizeof(float));	///changesssss
        fprintf(stderr, "Sent data, n = %d\n", n);
        if (n < 0)
             error1("ERROR writing to socket");

       // usleep(500 * 1000);
        k++;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    

    close(sockfd);
    return 0;
}
