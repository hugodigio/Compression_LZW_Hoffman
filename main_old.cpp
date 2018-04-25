#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include "main_old.hpp"
#include "constants.hpp"
#include "tools.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	int i;
	// Si il y a des arguments
	if (argc > 1)
	{
		for (i = 1; i < argc; i++)
		{
			// Déclaré aussi comme ca se réset tout seul
			char nom_compresse[256] = {'\0'};
			if (fichier_existe(argv[i]))
			{
				/* On regarde si le fichier est à compresser ou
				 * décompresser. */
				if (strstr(argv[i], ".huff") != NULL)
				{
					// Contient notre extension, c'est à décompresser
					strcpy(nom_compresse, argv[i]);
					nom_compresse[strlen(nom_compresse)-5] = '\0';
					decompression(argv[i], nom_compresse);
				}
				else
				{
					// C'est à compresser
					// Si nom assez court
					if (strlen(argv[i])+1 < 256-6)
					{
						strcpy(nom_compresse, argv[i]);
						strcat(nom_compresse, ".huff");
					}
					// Sinon on coupe le nom
					else
					{
						strncpy(nom_compresse, argv[i], 256-6);
						strcat(nom_compresse, ".huff");
					}
					compression(argv[i], nom_compresse);
				}
			}
		}
	}
	else
	{
		print_menu();
	}

	cout << "\n" << endl;
	return 1;
}

void display_titre()
{
	cout << "\n" << endl;
	cout << "\t\t--------------------\n" << endl;
	cout << "\t\t|Hoffman  Compressor|\n" << endl;
	cout << "\t\t--------------------\n" << endl;
	cout << "\n" << endl;
	cout << "\n" << endl;
}

void fichier_destination(char* fichierDestination)
{
	//ATTENTION, il faut rajouter la vérification du nom de fichier. un fichier ne doit pas contenir
	//certains caractaires comme par exemple: / \ : * ? > < " |

		//system(CLEAR);
		display_titre();
		cout << "\tQuel est nom de fichier de destination ? (max chemin de 512 caracteres)\n" << endl;
		cout << "\n\t Nom: " << endl;
		scanf("%s",fichierDestination);
}

void print_menu()
{
	int choix;
	char destination[512] = {0};
	char fichier_a_compresser[512] = {0};
	char message[1024];
	FILE* fichier = NULL;

	do
	{
		//system(CLEAR);
		display_titre();
		cout << "\t\tQue voulez vous faire ?\n" << endl;
		cout << "\t\n" << endl;
		cout << "\t" << COMPRESS << " - Compresser un fichier" << endl;
		cout << "\t" << DECOMPRESS << " - Decompresser un fichier" << endl;
		cout << "\t" << COMPRESS_MSG << " - Compresser un message (dans un fichier)" << endl;
		cout << "\t" << TEST_DEBUG << "- Tester la fonction debug" << endl;
		cout << "\t0 - Quitter" << endl;
		cout << endl << "\tChoix :" << endl;
		scanf("%d", &choix);
		cout <<choix<<endl;
	}
	while ((choix == 0 && choix < CHOICE_NBR));

	switch (choix)
	{
		case COMPRESS:
			//system(CLEAR);
			cout << "\tEntrez le chemin du fichier à compresser" << endl;
			scanf("%s", fichier_a_compresser);
			if (fichier_existe(fichier_a_compresser))
			{ cout<<"fichier existe"<<endl;
				if (strlen(fichier_a_compresser)+1 < 256-6)
				{
					strcpy(destination, fichier_a_compresser);
					strcat(destination, ".huff");
				}
				// Sinon on coupe le nom
				else
				{cout<<"fichier n'existe pas"<<endl;
					strncpy(destination, fichier_a_compresser, 256-6);
					strcat(destination, ".huff");
				}
				compression(fichier_a_compresser, destination);
				cout << endl << "Votre fichier se trouve à l'emplacement suivant: " <<  endl << destination << endl;
			}
		break;
		case COMPRESS_MSG:
			choix=0;
			do
			{
				//system(CLEAR);

				display_titre();
				cout << endl << "\tQuel est votre message ?\n" << endl;
				cout << endl << "\tmessage (max 512 caracteres): " << endl;
				scanf("%s", message);
				ofstream fichier("input.txt");
				//ecrit dans un fichier temporaire le message rentre par l'utisateur
				if(fichier)
				{
					fichier << message ;
					fichier.close();

				}

        		//system(CLEAR);

				display_titre();
				cout << "\tou voulez vous compresser ?" << endl;
				cout << "1- Dans un fichier" << endl;
				cout << "2- Dans la console" << endl;
				cout << endl << "\tchoix :" << endl;
				scanf("%d",&choix);

			}while(choix!=1 && choix!=2);

			if(choix == 1)
			{
				fichier_destination(destination);
				compression("input.txt", destination);
			} else {
				strcpy(destination, "tmp.huff");
				compression("input.txt", destination);
			}
			remove("input.txt");
			if(choix == 2)
			{
				fichier = fopen(destination, "r");
 				int caractereActuel = 0;
			    if (fichier != NULL)
			    {
			        // Boucle de lecture des caractères un à un
			        do
			        {
			            caractereActuel = fgetc(fichier); // On lit le caractère
			            cout << caractereActuel <<endl; // On l'affic << endl;
			        } while (caractereActuel != EOF); // On continue tant que fgetc n'a pas retourné EOF (fin de fichier)

			        fclose(fichier);
			    }
			    remove(destination);
			}
		break;
		case DECOMPRESS:
			//system(CLEAR);
			cout << "\tEntrez le chemin du fichier à decompresser" << endl;
			scanf("%s", fichier_a_compresser);
			if (fichier_existe(fichier_a_compresser))
			{
				strcpy(destination, fichier_a_compresser);
				destination[strlen(destination)-5] = '\0';
				decompression(fichier_a_compresser, destination);
				cout << endl << "Votre fichier se trouve à l'emplacement suivant: " << endl <<  destination << endl;
			}
		break;
		case TEST_DEBUG:
			choix=0;
			do
			{
				//system(CLEAR);

				display_titre();
				cout << "\tQuel est votre message ?" << endl;
				cout << endl << "\tmessage (max 512 caracteres): " << endl;
				scanf("%s", message);
				ofstream fichier("input.txt");
				//ecrit dans un fichier temporaire le message rentre par l'utisateur
				if (fichier)
				{
					fichier << message ;
					fichier.close();

				}

        		//system(CLEAR);

				display_titre();
				cout << "\tou voulez vous compresser ?" << endl;
				cout << "1- Dans un fichier" << endl;
				cout << "2- Dans la console" << endl;
				cout << endl << "\tchoix :" << endl;
				scanf("%d",&choix);

			}while(choix!=1 && choix!=2);

			if(choix == 1)
			{
				fichier_destination(destination);
				debug_compression("input.txt", destination);
			} else {
				strcpy(destination, "tmp.huff");
				debug_compression("input.txt", destination);
			}
			remove("input.txt");
			if(choix == 2)
			{
				fichier = fopen(destination, "r");
 				int caractereActuel = 0;
			    if (fichier != NULL)
			    {
			        // Boucle de lecture des caractères un à un
			        do
			        {
			            caractereActuel = fgetc(fichier); // On lit le caractère
			            cout << caractereActuel <<endl; // On l'affiche
			        } while (caractereActuel != EOF); // On continue tant que fgetc n'a pas retourné EOF (fin de fichier)

			        fclose(fichier);
			    }
			    remove(destination);
			}
		break;
		default:
		break;
	}
}
