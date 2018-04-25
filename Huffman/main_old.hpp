#ifndef MAIN_OLD_H
#define MAIN_OLD_H

/* Les différents choix pour le menu.
 * Attention, les nombres associé aux choix doivent être positif et
 * doivent se suivre (on peut changer leur ordre mais pas mettre
 * d'écart entre eux.) */
typedef enum e_choice
{
	COMPRESS = 1, DECOMPRESS = 2, COMPRESS_MSG = 3, TEST_DEBUG = 4, CHOICE_NBR
} e_choice;

void print_menu();

#endif //MAIN_OLD_H