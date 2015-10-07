#ifndef ENCRYPTION_H
#define ENCRYPTION_H
#include <iostream>
#include <math.h>

int isprime(int number);

int mul(int num1,int num2);

int get_enc_key(int known_num);

int get_dec_key(int phi,int encryption_key);

int encrypt_number(int plain_text,int encryption_key,int known_num);

#endif // ENCRYPTION_H
