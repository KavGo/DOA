/* 
crypto.c

Implementations for cryptography primatives and functions
  making use of them.

Skeleton written by Aidan Dang for COMP20007 Assignment 2 2022
  with Minor modifications by Grady Fitzpatrick
  implementation by <You>
*/
#include <crypto.h>
#include <sponge.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The sponge's rate, in bytes. This represents the maximum number of bytes
// which can be read from or written the start of the sponge's state before a
// permutation must occur.
#define RATE 16
// Delimiter byte value used after absorption of the message
#define DELIMITER_A 0xAD
// Delimiter byte used at the end of the last-used block
#define DELIMITER_B 0X77

// Helpful min function that might be useful.
uint64_t min(uint64_t a, uint64_t b) { return a < b ? a : b; }

void hash(uint8_t *output, uint64_t output_len, uint8_t const *msg,
          uint64_t msg_len) {
  sponge_t sponge;
  sponge_init(&sponge);

  // TODO: fill the rest of this function.
  int i;
  int permute_flag=0;

  /* Permute in blocks of RATE bytes */
  for (i=0;i<msg_len/RATE;i++){
    /* permute after every write*/
    if (permute_flag) {
      sponge_permute(&sponge);
    }else {
      permute_flag=1;
    }
    sponge_write(&sponge,msg+(RATE*i),RATE,true);
  }
  
  /* If there a remaining bytes */
  if (i*RATE<=msg_len) { 
    if (permute_flag) {
      sponge_permute(&sponge);  
    }
    
    sponge_write(&sponge,msg+(RATE*i),msg_len%RATE,true);
  }

  /* Demarcation step */
  sponge_demarcate(&sponge, msg_len%RATE, DELIMITER_A);
  sponge_demarcate(&sponge, RATE - 1, DELIMITER_B);

  
  /* Squeezing step */
  for (i=0;i<output_len/RATE;i++) {
    sponge_permute(&sponge);
    sponge_read(output+(RATE*i),&sponge,RATE);
  }

  if (i*RATE<output_len) {

    sponge_permute(&sponge);
    sponge_read(output+(RATE*i),&sponge,output_len%RATE);
  }
  
}

void mac(uint8_t *tag, uint64_t tag_len, uint8_t const *key, uint8_t const *msg,
         uint64_t msg_len) {
  sponge_t sponge;
  sponge_init(&sponge);

  // TODO: fill the rest of this function.
  // Your implementation should like very similar to that of the hash
  // function's, but should include a keying phase before the absorbing phase.
  // If you wish, you may also treat this as calculating the hash of the key
  // prepended to the message.

  int permute_flag=0;
  int i;
  
  /* Prepend key */
  sponge_write(&sponge,key,RATE,true);
  sponge_permute(&sponge);
  sponge_write(&sponge,key+RATE,RATE,true);
  sponge_permute(&sponge);

  /* Hash */
  /* Permute in blocks of RATE bytes */
  for (i=0;i<msg_len/RATE;i++){
    /* permute after every write*/
    if (permute_flag) {
      sponge_permute(&sponge);
    }else {
      permute_flag=1;
    }
    sponge_write(&sponge,msg+(RATE*i),RATE,true);
  }
  
  /* If there a remaining bytes */
  if (i*RATE<=msg_len) {
    if (permute_flag) {
      sponge_permute(&sponge);
    }
    
    sponge_write(&sponge,msg+(RATE*i),msg_len%RATE,true);
  }

  /* Demarcation step */
  sponge_demarcate(&sponge, msg_len%RATE, DELIMITER_A);
  sponge_demarcate(&sponge, RATE - 1, DELIMITER_B);

  
  /* Squeezing step */
  for (i=0;i<tag_len/RATE;i++) {
    sponge_permute(&sponge);
    sponge_read(tag+(RATE*i),&sponge,RATE);
  }

  if (i*RATE<tag_len) {
    sponge_permute(&sponge);
    sponge_read(tag+(RATE*i),&sponge,tag_len%RATE);
  }

}

void auth_encr(uint8_t *ciphertext, uint8_t *tag, uint64_t tag_len,
               uint8_t const *key, uint8_t const *plaintext,
               uint64_t text_len) {
  sponge_t sponge;
  sponge_init(&sponge);

  // TODO: fill the rest of this function.
  // Your implementation should like very similar to that of the mac function's,
  // but should after each write into the sponge's state, there should
  // immediately follow a read from the sponge's state of the same number of
  // bytes, into the ciphertext buffer.
  int permute_flag=0;
  int i;
  
  /* Prepend key */
  sponge_write(&sponge,key,RATE,true);
  sponge_permute(&sponge);
  sponge_write(&sponge,key+RATE,RATE,true);
  sponge_permute(&sponge);

  /* Hash */
  /* Permute in blocks of RATE bytes */
  for (i=0;i<text_len/RATE;i++) {

    /* permute after every write */
    if (permute_flag) {
      sponge_permute(&sponge);
    }else {
      permute_flag=1;
    }
    sponge_write(&sponge,plaintext+(RATE*i),RATE,true);
    sponge_read(ciphertext+(RATE*i),&sponge,RATE);
  } 
  
  /* If there a remaining bytes */
  if (i*RATE<=text_len) {
    if (permute_flag) {
      sponge_permute(&sponge);
    }
    
    sponge_write(&sponge,plaintext+(RATE*i),text_len%RATE,true);
    sponge_read(ciphertext+(RATE*i),&sponge,text_len%RATE);
  }

  /* Demarcation step */
  sponge_demarcate(&sponge, text_len%RATE, DELIMITER_A);
  sponge_demarcate(&sponge, RATE - 1, DELIMITER_B);
    
  /* Squeezing step */
  for (i=0;i<tag_len/RATE;i++) {
    sponge_permute(&sponge);
    sponge_read(tag+(RATE*i),&sponge,RATE);
  }
  
  if (i*RATE<=tag_len) {
    sponge_permute(&sponge);
    sponge_read(tag+(RATE*i),&sponge,tag_len%RATE);
  }
} 
  
int auth_decr(uint8_t *plaintext, uint8_t const *key, uint8_t const *ciphertext,
              uint64_t text_len, uint8_t const *tag, uint64_t tag_len) {
  sponge_t sponge;
  sponge_init(&sponge);
  
  // TODO: fill the rest of this function.
  // The implementation of this function is left as a challenge. It may assist
  // you to know that a ^ b ^ b = a. Remember to return 0 on success, and 1 on
  // failure.
  int i;
  
  /* Decryption */
  /* Prepend key */
  sponge_write(&sponge,key,RATE,true);
  sponge_permute(&sponge);
  sponge_write(&sponge,key+RATE,RATE,true);
  sponge_permute(&sponge);

  /* Permute in blocks of RATE bytes */
  for (i=0;i<text_len/RATE;i++){
   
    sponge_write(&sponge,ciphertext+(RATE*i),RATE,true);
    sponge_read(plaintext+(RATE*i),&sponge,RATE);
    sponge_write(&sponge,ciphertext+(RATE*i),RATE,false);
    /* permute after write*/
    sponge_permute(&sponge);
    
  }
  
  /* If there a remaining bytes */
  if (i*RATE<=text_len) {
    sponge_write(&sponge,ciphertext+(RATE*i),text_len%RATE,true);
    sponge_read(plaintext+(RATE*i),&sponge,text_len%RATE);
  }
   
  /* Getting the tag for the decrypted message */
  uint8_t *msgtag;
  msgtag = malloc(sizeof(uint8_t)*tag_len);
  uint8_t *dummy;
  dummy = malloc(sizeof(uint8_t)*text_len);
  auth_encr(dummy,msgtag,tag_len,key,plaintext,text_len);

  /* Verify that the message hasn't been tampered with */
  for (i=0;i<tag_len;i++) {
    /* tag mismatch*/
    if (msgtag[i]!=tag[i]) {
      return 1;
    }

  }
  return 0;
} 




