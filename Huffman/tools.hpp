#ifndef HUFFMAN_TOOLS_H
#define HUFFMAN_TOOLS_H

#include <string>
#include <iostream>
#include "arbre.hpp"

using namespace std;

void compression(const char *fal, const char *nf);
void debug_compression(const char *fal, const char *nf);
void decompression(const char *fal, const char *nf);
int fichier_existe(const char * const nom);
void init_tampon();
void ajouter_bit_tampon(int bit, FILE *nf);
void ajouter_char_au_tampon(char carac, FILE *nf);
void ajouter_au_tampon(tpn arbre, FILE *nf);
void clear_tampon(FILE *nf);

//void debug(tpn arbre);

#endif //HUFFMAN_TOOLS_H
