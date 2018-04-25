#include <assert.h>
#include <stddef.h>
#include <iostream>
#include "arbre.hpp"
#include <stdlib.h>     /* malloc, free, rand */

void init_arbre(t_arbre *arbre)
{
	int i;
	for (i = 0; i < 515; i++)
		arbre->ordres[i] = TPN_NULL;
	for (i = 0; i < 256; i++)
		arbre->caracteres[i] = TPN_NULL;
	/* Initialisation de l'arbre
	 * On crée la racine et on lui ajoute le caractère virtuel de fin
	 * en fils gauche et le caractère inconnu en fils droit. */
	/*cree_noeud(TPN_NULL, cree_feuille(FAKE_EOF, 1, 1), cree_feuille(UNKNOWN_CHAR, 2, 0), 0);*/
	/*FAKE_EOF = -2 et UNKNOWN CHAR = -1*/
	arbre->racine = cree_noeud(TPN_NULL, cree_feuille(-1, 1, 1), cree_feuille(-1, 2, 0), 0);
	/* On a l'arbre suivant
	 *          O
	 *         / \
	 *        FE UC
	 * FE: FAKE_EOF, ordre(1), poids(1)
	 * UC: UNKNOWN_CHAR, ordre(2), poids(0) */
	arbre->ordres[0] = arbre->racine;
	arbre->ordres[1] = noeud_fg(arbre->racine);
	arbre->ordres[2] = noeud_fd(arbre->racine);
	arbre->pfi = noeud_fd(arbre->racine);
    arbre->pffe = noeud_fg(arbre->racine);
}

/** @brief Créée une feuille et la renvoie
 *
 * Créée une feuille, son parent est mis à jour par
 * ce dernier.
 * @param val  valeur de l'élément
 * @param parent le parent de la feuille
 */
tpn cree_feuille(short val, int ordre, int poids)
{
    tpn feuille = (tpn)malloc(sizeof(t_noeud));
    feuille->val = val;
    feuille->ord_gal = ordre;
    feuille->poids = poids;
    feuille->fg = TPN_NULL;
    feuille->fd = TPN_NULL;
    return feuille;
}

/** @brief Créée un noeud et le renvoi
 *
 * Mets automatiquement les parents des enfants à jour.
 * @param parent  le parent du noeud
 * @param fg  le fils gauche du noeud
 * @param fd  le fils droit du noeud
 * @param ordre  son ordre de gallager
 * @param poids  son poids
 */
tpn cree_noeud(tpn parent, tpn fg, tpn fd, int ordre)
{
    assert(fg != TPN_NULL && fd != TPN_NULL);

    tpn noeud = (tpn)malloc(sizeof(t_noeud));
    noeud->parent = parent;
    noeud->ord_gal = ordre;
    noeud->fg = fg;
    noeud->fd = fd;
    noeud->poids = 1;
    // Mise à jour des parents des feuilles
    fg->parent = noeud;
    fd->parent = noeud;
    return noeud;
}

/** @brief Ajout une nouvelle feuille
 *
 * Maintient l'ordre de Gallager
 * @param arbre  l'arbre en question
 * @param c  le caractère à ajouter
 */
void ajout_feuille(t_arbre *arbre, unsigned char c)
{
	/* Et on ajoute la nouvelle feuille en créant un
	 * nouveau noeud.
	 * Car on sait que la feuille UNKNOWN_CHAR sera toujours
	 * le fils droit d'un noeud et qu'il faut donc créer un
	 * nouveau noeud. */
	tpn noeud = cree_noeud(arbre->pfi->parent, cree_feuille((short)c, arbre->pfi->ord_gal+1, 1), arbre->pfi, arbre->pfi->ord_gal);
	noeud->parent->fd = noeud; // Ne pas oublier de mettre à jour le parent.
	arbre->caracteres[(int)c] = noeud_fg(noeud); // Ajout du caractère dans la liste
	// Ne pas oublier de mettre à jour l'ordre de Gallager
	noeud_fd(noeud)->ord_gal += 2;
	arbre->ordres[noeud->ord_gal] = noeud;
	arbre->ordres[noeud_fd(noeud)->ord_gal] = noeud_fd(noeud);
	arbre->ordres[noeud_fg(noeud)->ord_gal] = noeud_fg(noeud);
	// Maintenir l'ordre de Galager
	incrementer_element(arbre, noeud->parent);
}

/** @brief Incrément le poids d'une feuille/noeud en maintenant Gallager
 *
 * Maintient l'ordre de Gallager
 * @param arbre  l'arbre en question
 * @param elem  l'élément qui va être augmenté
 */
void incrementer_element(t_arbre *arbre, tpn elem)
{
    assert(elem != TPN_NULL && arbre != NULL);
    // Si c'est la racine
    if (elem->parent == TPN_NULL)
    {
        elem->poids++;
    }
    else
    {
        /* Chercher dans l'ordre de Gallager celui de même poids
         * puis l'échanger. */
        int ordre = elem->ord_gal;
        while (ordre > 0 && arbre->ordres[ordre-1]->poids == elem->poids)
        {
            ordre--;
        }
        // Si l'ordre est différent, on doit échanger
        if (ordre != elem->ord_gal)
        {
            /* Mets à jour:
             *     leurs parents
             *     l'ordre de Gallager
             */
            permuter(elem, arbre->ordres[ordre], arbre);
            elem->poids++; // Maj du poids
            incrementer_element(arbre, elem->parent);
        }
        else
        {
            elem->poids++;
            incrementer_element(arbre, elem->parent);
        }
    }
}

/** @brief Retourne vrai si le paramètre est une feuille
 *
 * Une feuille est un noeud qui a ses deux fils à NULL
 * @param element  l'élément à tester
 */
int est_feuille(tpn element)
{
    assert(element != TPN_NULL);
    /* Normalement seul la feuille gauche devrait être à tester car par définition
     * un noeud a obligatoirement 2 fils, sinon c'est une feuille.
     */
    return ((element->fg == TPN_NULL) && (element->fd == TPN_NULL));
}

/** @brief Retourne vrai si la feuille ou noeud est un fils gauche
 *
 * @param elem  l'élément à tester
 */
int est_fg(tpn elem)
{
    if (elem->parent != TPN_NULL && noeud_fg(elem->parent) == elem)
    {
        return 1;
    }
    else
        return 0;
}

/** @brief Retourne le fils gauche du noeud
 *
 * @param noeud  le noeud dont on veut le fils
 */
tpn noeud_fg(tpn noeud)
{
    assert((noeud != TPN_NULL) && !est_feuille(noeud));
    return noeud->fg;
}

/** @brief Retourne le fils droit du noeud
 *
 * @param noeud  le noeud dont on veut le fils
 */
tpn noeud_fd(tpn noeud)
{
    assert((noeud != TPN_NULL) && !est_feuille(noeud));
    return noeud->fd;
}

/** @brief Retourne la valeur de l'élément
 *
 * @param elem  l'élément dont on veut la valeur
 */
char elem_valeur(tpn elem)
{
    assert(elem != TPN_NULL);
    return elem->val;
}

/** @brief Retourne l'ordre de l'élément
 *
 * @param elem  l'élément dont on veut l'ordre
 */
int elem_ordre(tpn elem)
{
    assert(elem != TPN_NULL);
    return elem->ord_gal;
}

/** @brief Retourne le poids de l'élément
 *
 * @param elem  l'élément dont on veut le poids
 */
int elem_poids(tpn elem)
{
    assert(elem != TPN_NULL);
    return elem->poids;
}

/** @brief Permute les 2 éléments
 *
 * Echange leur parents, mets à jour leur ordres, le tableau d'ordres et les fils
 * des parents.
 * @param elem1  le 1er élément à permuter
 * @param elem2  le 2ème élément à permuter
 */
void permuter(tpn elem1, tpn elem2, t_arbre *arbre)
{
    assert(elem1 != TPN_NULL && elem2 != TPN_NULL);

    // Temporaires
    tpn tmp_parent = elem1->parent;
	int tmp_ordre = elem1->ord_gal;

    // Echange dans l'ordre de Gallager
	arbre->ordres[elem1->ord_gal] = elem2;
	arbre->ordres[elem2->ord_gal] = elem1;

    // Echange élément 1
    elem1->parent = elem2->parent;
	elem1->ord_gal = elem2->ord_gal;
    if (elem1->parent->fg == elem2)
        elem1->parent->fg = elem1;
    else
        elem1->parent->fd = elem1;

    elem2->parent = tmp_parent;
    elem2->ord_gal = tmp_ordre;
    if (elem2->parent->fg == elem1)
        elem2->parent->fg = elem2;
    else
        elem2->parent->fd = elem2;
}

/** @brief Libère toute la mémoire réservé pour l'arbre
 *
 * @param racine  pointeur sur la racine de l'arbre à libérer
 */
void liberer_arbre(tpn racine)
{
	if (est_feuille(racine))
	{
		free(racine);
	}
	else
	{
		liberer_arbre(racine->fg);
		liberer_arbre(racine->fd);
		free(racine);
	}
}

/** @brief calcule le nombre de feuille d'un arbre
 *
 * fonction récursive qui renvoie le nombre de feuilles d'un arbre 
 * @param arbre arbre
 */

int nb_feuilles(tpn a){
	if(est_feuille(a)){
		return 1;
	} else{
		return (nb_feuilles(a->fg)+nb_feuilles(a->fd));
	}
}


/** @brief Calcul et renvoie la profondeur d'un arbre
 *
 * fonction récursive qui renvoie la profondeur d'un arbre 
 * @param arbre arbre sur lequel on veux calculer la profondeur de l'arbre
 */
int profondeur(tpn arbre)
{
	int b,c;
	if(est_feuille(arbre)){
		return 0;	
	} else {
		b=nb_feuilles(arbre->fg);
		c=nb_feuilles(arbre->fd);
		return (1 + (b<c)?b:c);
		/* if (b<c){
			return b;
		}else{
			return c;
		}
		*/
	}
}
