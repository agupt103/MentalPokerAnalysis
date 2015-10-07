#include "encryption.h"
int isprime(int number){
    // function to test prime numbers
    // @param number - the number to be tested
    // @return the result
    //
    int count;
    count=2;
    if(number<2)
        return 0;
    while(count<=(number/2))
    {
        if(number%count==0)
            return 0;
        count++;
    }
    return 1;

}

int get_enc_key(int known_num){
    // function returns encryption key
    // @param known_num - Number decided upon by both players
    // @return - Encryption key
    //
    int count;
    srand( time(0)); // This will ensure a really randomized number by help of time

    count=2; // Randomizing the number between 1-known_num/2 and 2.
    //std::cout<<count<<" "<<known_num<<std::endl;
    while(count<known_num)
    {
        if(known_num%count!=0)
            return count;
        count++;
    }
    return 0;
}

int get_dec_key(int phi,int encryption_key){
    // function returns encryption key
    // @param phi - product of (num1 - 1) and (num2 - 1)
    // @param encryption_key - encryption key
    // @return decryption ke
    //
    int a1,a2,a3,b1,b2,b3,t1,t2,t3,q;

    a1=1;
    a2=0;
    a3=phi;

    b1=0;
    b2=1;
    b3=encryption_key;

    while(b3!=1)                           //evaluating decryption key using encryption key
    {
        q=a3/b3;

        t1=a1-(q*b1);
        t2=a2-(q*b2);
        t3=a3-(q*b3);

        a1=b1;
        a2=b2;
        a3=b3;

        b1=t1;
        b2=t2;
        b3=t3;

    }
    if(b2<0)
        b2=b2+phi;
    return b2;

}

int encrypt_number(int plain_text,int encryption_key,int known_num){
    // function to encrypt the cards
    // @param plain_text - the number to be encrypted
    // @param encryption_key - The encryption key
    // @param known_num - The number decided by the players
    int count,t;
    count=1;
    t=1;
    while(count<= encryption_key)                             //loop computes the cipher text
    {
                    t=t*plain_text;
                    t=t%known_num;
                    count++;
    }
    return (t%known_num);

}
