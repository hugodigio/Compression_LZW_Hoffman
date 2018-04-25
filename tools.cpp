#include "tools.hpp"
#include "arbre.hpp"
#include <cstdio>
#include <assert.h>
#include <time.h>

/* Déclaration dans le .c en static car on en veut pas que ces variables
 * soient visibles ailleurs que dans ce fichier. */
static char glb_stat_tampon;
static int gbl_stat_compteur_tampon;

void afficher_ordre(t_arbre* arbre);

void debug_compression(const char *fal, const char *nf)
{
	/* Longueur du fichier.
	 * Attention: un fichier excédant la longueur que peut contenir cette
	 * variable entrainera un résultat inconnu. */
	long int tailleFichier = 0;
	// Utilisé pour parcourir le fichier
	long int i;
	// caractère actuel
	unsigned char c;
	// Contient l'arbre de Gallager, son ordre, etc.
	t_arbre arbre;
	// Pointeur sur le fichier à lire.
	FILE *pfl = fopen(fal, "r");
	FILE *pfe = fopen(nf, "w");

	// Test de l'ouverture du fichier
	if (pfl == NULL)
	{
		perror ("Erreur d'ouverture du fichier.");
	}
	else if (pfe == NULL)
	{
		/* Si impossible de créer un fichier pour écrire,
		 * on ferme celui qui a été ouvert pour la lecture. */
		fclose(pfl);
		perror ("Erreur d'ouverture du fichier.");
	}
	else
	{
		fseek (pfl, 0, SEEK_END);   // non-portable
		tailleFichier = ftell(pfl);
		rewind(pfl); // Important, remet le curseur en début de fichier
		printf ("Le fichier contient: %ld caractères.\n", tailleFichier);
		
		// Création de l'arbre de base (racine + feuille EOF et caractère inconnu)
		init_arbre(&arbre);
		/* On s'assure que les glb_stat_tampon soient réinitialisé (cas de plusieurs
		 * compression effectué d'affilé. */
		init_tampon();

		// Lecture du complète du fichier.
		for (i = 0; i < tailleFichier; i++)
		{
			/* getc à la place de fgetc car est parfois implémenté en tant que
			 * macro, ce qui peut induire un gain de performance. */
			c = getc(pfl);
			// Opérations sur l'arbre d'Huffman
			tpn code = arbre.caracteres[(int)c];

			// Si le code n'est pas présent dans l'arbre
			if (code == TPN_NULL)
			{
				ajouter_au_tampon(arbre.pfi, pfe);
				ajouter_char_au_tampon(c, pfe);
				ajout_feuille(&arbre, c);
			}
			// On doit rééquilibrer l'arbre et incrémenter de 1 succésivement
			else
			{
				ajouter_au_tampon(code, pfe);
				incrementer_element(&arbre, code);
			}
			// Affiche l'ordre de Gallager
			afficher_ordre(&arbre);
		}
		ajouter_au_tampon(arbre.pffe, pfe);
		clear_tampon(pfe);
		liberer_arbre(arbre.racine);
		fclose(pfl);
		fclose(pfe);
	}
}

/** @brief Compresse le fichier vers un autre
 *
 * Le fichier va être compression via l'algorithme de Huffman adaptatif.
 *
 * @param fal  chaine de caractère étant le nom du fichier à lire
 * @param nf  chaine de caractère étant le nom du fichier à sauvegarder
 * TODO: faire en sorte que la fonction soit portable (lecture taille fichier)
 */
void compression(const char *fal, const char *nf)
{
	time_t debut, fin;
	time(&debut); // On stock le temps de début
	/* Longueur du fichier.
	 * Attention: un fichier excédant la longueur que peut contenir cette
	 * variable entrainera un résultat inconnu. */
	long int tailleFichier = 0;
	// Utilisé pour parcourir le fichier
	long int i;
	// caractère actuel
	unsigned char c;
	// Contient l'arbre de Gallager, son ordre, etc.
	t_arbre arbre;
	// Pointeur sur le fichier à lire.
	FILE *pfl = fopen(fal, "r");
	FILE *pfe = fopen(nf, "w");

	// Test de l'ouverture du fichier
	if (pfl == NULL)
	{
		perror ("Erreur d'ouverture du fichier.");
	}
	else if (pfe == NULL)
	{
		/* Si impossible de créer un fichier pour écrire,
		 * on ferme celui qui a été ouvert pour la lecture. */
		fclose(pfl);
		perror ("Erreur d'ouverture du fichier.");
	}
	else
	{
		fseek (pfl, 0, SEEK_END);   // non-portable
		tailleFichier = ftell(pfl);
		rewind(pfl); // Important, remet le curseur en début de fichier
		printf ("Le fichier contient: %ld caractères.\n", tailleFichier);
		
		// Création de l'arbre de base (racine + feuille EOF et caractère inconnu)
		init_arbre(&arbre);
		/* On s'assure que les glb_stat_tampon soient réinitialisé (cas de plusieurs
		 * compression effectué d'affilé. */
		init_tampon();

		// Lecture du complète du fichier.
		for (i = 0; i < tailleFichier; i++)
		{
			/* getc à la place de fgetc car est parfois implémenté en tant que
			 * macro, ce qui peut induire un gain de performance. */
			c = getc(pfl);
			// Opérations sur l'arbre d'Huffman
			tpn code = arbre.caracteres[(int)c];

			// Si le code n'est pas présent dans l'arbre
			if (code == TPN_NULL)
			{
				ajouter_au_tampon(arbre.pfi, pfe);
				ajouter_char_au_tampon(c, pfe);
				ajout_feuille(&arbre, c);
			}
			// On doit rééquilibrer l'arbre et incrémenter de 1 succésivement
			else
			{
				ajouter_au_tampon(code, pfe);
				incrementer_element(&arbre, code);
			}
		}
		ajouter_au_tampon(arbre.pffe, pfe);
		clear_tampon(pfe);
		liberer_arbre(arbre.racine);
		fclose(pfl);
		long int tailleCompresse = ftell(pfe);
		fclose(pfe);
		time(&fin);
		double temps = difftime(fin, debut);
		printf("Fichier %s\n", fal);
		printf("Temps de compression: %.3f secondes, compresse a %.3f%%, vitesse moyenne: %.3fko/s", \
			temps, (1.f-(double)tailleCompresse/(double)tailleFichier)*100.f, (tailleCompresse/1000.f)/temps);
	}
}

/** @brief Décompresse le fichier vers un autre
 *
 * Le fichier va être décompressé via l'algorithme de Huffman adaptatif.
 *
 * @param fal  chaine de caractère étant le nom du fichier à lire
 * @param nf  chaine de caractère étant le nom du fichier à sauvegarder
 * TODO: faire en sorte que la fonction soit portable (lecture taille fichier)
 */
void decompression(const char *fal, const char *nf)
{
	time_t debut, fin;
	time(&debut); // On stock le temps de début
	/* Longueur du fichier.
	 * Attention: un fichier excédant la longueur que peut contenir cette
	 * variable entrainera un résultat inconnu. */
	long int tailleFichier = 0;
	// Utilisé pour parcourir le fichier
	long int i = 0;
	// Utilisé pour buffer
	int j = (sizeof(char)*8)-1;
	// caractère actuel
	unsigned char carac;
	// Contient l'arbre de Gallager, son ordre, etc.
	t_arbre arbre;
	tpn feuille;
	// Pointeur sur le fichier à lire.
	FILE *pfl = fopen(fal, "r");
	FILE *pfe = fopen(nf, "w");

	// Test de l'ouverture du fichier
	if (pfl == NULL)
	{
		perror ("Erreur d'ouverture du fichier.");
	}
	else if (pfe == NULL)
	{
		/* Si impossible de créer un fichier pour écrire,
		 * on ferme celui qui a été ouvert pour la lecture. */
		fclose(pfl);
		perror ("Erreur d'ouverture du fichier.");
	}
	else
	{
		// Si la feuille est le caractère inconnu, on lit
		// les 8 prochains bit et création du noeud avec le char
		// et maintient de l'ordre

		// Si la feuille est un caractère, on incrémente ce dernier

		// Si la feuille mène à la fin de fichier, on stop la lecture.
		// Variante, stop à la fin de fichier et si on a lu caractère
		// de fin, decompression OK sinon ERREUR (fichier corrompu)

		fseek (pfl, 0, SEEK_END);   // non-portable
		tailleFichier = ftell(pfl);
		rewind(pfl); // Important, remet le curseur en début de fichier
		printf ("Le fichier contient: %ld caractères.\n", tailleFichier);
		
		// Création de l'arbre de base (racine + feuille EOF et caractère inconnu)
		init_arbre(&arbre);

		if (tailleFichier != 0)
		{
			carac = getc(pfl);
			tailleFichier++;
		}
		// Lecture du complète du fichier.
		while (i < tailleFichier)
		{
			// On lit un caractère et on décode son binaire pour
			// tomber sur une feuille de l'arbre de Gallager.
			// Tant que pas tombé sur feuille et buffer pas vide
			feuille = arbre.racine;
			while (!est_feuille(feuille) && i < tailleFichier)
			{
				if (j < 0)
		        {
		        	// Rempli buffer
		        	carac = getc(pfl);
		        	j = (sizeof(char)*8)-1;
		        	i++;
		        }
		        if (carac&(1u<<j))
		        {
		        	// Aller à droite
		        	feuille = feuille->fd;
		        }
		        else
		        {
		        	// Aller à gauche
		        	feuille = feuille->fg;
		        }
		        j--;
			}
			// Si la feuille est le caractère inconnu, on lit
			// les 8 prochains bit et création du noeud avec le char
			// et maintient de l'ordre
			//UNKNOWN_CHAR == -1
			if (feuille->val == -1)
			{
				// Lire 8 prochain bit
				int compteurChar = (sizeof(char)*8)-1;
				char charLu = 0;
				while (compteurChar >= 0 && i < tailleFichier)
				{
					if (j < 0)
					{
						carac = getc(pfl);
						j = (sizeof(char)*8)-1;
						i++;
					}
					if (carac&(1u<<j))
					{
						charLu = charLu|(1u<<compteurChar);
						compteurChar--;
						j--;
					}
					else
					{
						compteurChar--;
						j--;
					}
				}
				// charLu contient le char a ajouter
				putc(charLu, pfe);
				ajout_feuille(&arbre, charLu);
			}
			else if (feuille->val == -2) //FAKE_EOF == -2
			{
				// Arrete tout
				i = tailleFichier;
			}
			// Si la feuille est un caractère, on incrémente ce dernier
			else
			{
				putc((char)feuille->val, pfe);
				incrementer_element(&arbre, feuille);
			}
		}
		liberer_arbre(arbre.racine);
		fclose(pfl);
		fclose(pfe);
		time(&fin);
		printf("Temps de decompression: %3f secondes.", difftime(fin, debut));
	}
}

/** @brief Retourne 1 si le fichier existe
 *
 * @param nom  le chemin complet vers le fichier
 */
int fichier_existe(const char * const nom)
{
	FILE* file = fopen(nom, "r");
	int existe = file != NULL ? 1 : 0;
	if (file != NULL) fclose(file);
	return existe;
}

/** @brief Initialise les variables global static
 *
 */
void init_tampon()
{
	glb_stat_tampon = 0;
	gbl_stat_compteur_tampon = 0;
}

/** @brief Ajoute le bit en paramètre au tampon puis verifie si le tampon est plein.
 *
 * @param bit Valeur du bit à ajouter (doit être 0 ou 1)
 * @param nf  pointeur du fichier où l'on écrit le tampon
 */
void ajouter_bit_tampon(int bit, FILE *nf)
{
	assert(bit == 1 || bit == 0);
	glb_stat_tampon = glb_stat_tampon << 1;
	glb_stat_tampon = glb_stat_tampon | bit;
	gbl_stat_compteur_tampon++;

	if(gbl_stat_compteur_tampon == 8){
		putc(glb_stat_tampon, nf);
		init_tampon();
	}
}

/** @brief Ajoute une lettre (char) au tampon
 *
 * @param carac  le caractère à mettre dans le tampon
 * @param nf  pointeur du fichier où l'on écrit le tampon
 */
void ajouter_char_au_tampon(char carac, FILE *nf)
{
	int i;
	for(i=(sizeof(char)*8)-1; i>=0; i--)
	{
        if (carac&(1u<<i))
        {
        	ajouter_bit_tampon(1, nf);
        }
        else
        {
        	ajouter_bit_tampon(0, nf);
        }
	}
}

/** @brief Ajoute au tampon la séquence binaire d'une lettre
 *
 * Ecrit la séquence binaire correspondant à la lettre
 * dans le tampon.
 *
 * @param carac  le caractère à mettre dans le tampon
 * @param nf  pointeur du fichier où l'on écrit le tampon
 * TODO: Reprendre la fonction
 */
void ajouter_au_tampon(tpn arbre, FILE *nf)
{
	tpn tmp;
	int chemin[512];
	int taille_tab = 0;

	//on récupère le chemin binaire en partant de la feuille
	//et en remontant jusqu'a la racine.
	while (arbre->parent!=NULL)
	{
		tmp = arbre;
		arbre = arbre->parent;
		if (arbre->fg == tmp)
		{
			chemin[taille_tab]=0;
			taille_tab++;
		} 
		else if(arbre->fd == tmp)
		{
			chemin[taille_tab]=1;
			taille_tab++;
		}
	}

	//boucle qui écrit dans le glb_stat_tampon le chemin dans l'ordre
	//car on écrit le chemin de la racine vers la feuille.
	//cependant il a été récupéré de la feuille à la racine.
	while(taille_tab!=0)
	{
		taille_tab--;
		ajouter_bit_tampon(chemin[taille_tab], nf);
	}
}

/** @brief Vide le tampon en rajoutant du bourrage si besoin
 *
 * @param nf  pointeur du fichier où l'on écrit le tampon
 */
void clear_tampon(FILE *nf)
{
	glb_stat_tampon = glb_stat_tampon << (8 - gbl_stat_compteur_tampon);
	gbl_stat_compteur_tampon = 8;
	putc(glb_stat_tampon, nf);
	init_tampon();
}

void afficher_ordre(t_arbre* arbre)
{
	int indice = 0;
	printf("Affichage ordre et +\n");
	while (arbre->ordres[indice] != TPN_NULL && indice < 515)
	{
		indice++;
	}
	printf("\n");
}
