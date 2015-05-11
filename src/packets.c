#include "includes.h"

/**
* Fonction de formatage des packets avant l'envoi au serveur
*/
char *formatPackets(unsigned short nb, ...)
{
    char            *packet;
    va_list         params;
    unsigned short  i;

    packet = malloc(SOCKET_BUFFER * sizeof(char));
    va_start(params, nb);

    strcpy(packet, va_arg(params, char *));

    for(i=1; i<nb; i++)
    {
        strcat(packet, PACKET_DELIMITER);
        strcat(packet, va_arg(params, char *));
    }
    strcat(packet, PACKET_END);

    va_end(params);

    return packet;
}

/**
* Fonction de chiffrement des packets avant l'envoi
*/
void cipherPacket(Project *project, char **packet)
{
    /*guint       bin_size = 0;
    gushort     *binary = NULL;

    int i, j, k, nb_car_bin = 0, rest = 0;

    char *encoded = NULL, *tmp = NULL, *bin_result = NULL;

    tmp = malloc(project->matrix.columns * sizeof(char));

    bin_size = strlen(*packet);
    //Lit le fichier source, convertit les caractères en binaire puis les stockent dans binary
    binary = char_to_bin(*packet, &bin_size);

    //Si le nombre de bits n'est pas un multiple du nombre de colonnes de la matrice,
    //alors on rajoute autant de 0 à la fin de "binary" qu'il manque de bits.
    //Y a peut être des réallocations à faire dans certains cas de "binary" pour ajouter ces 0...
    rest = bin_size % project->matrix.columns;
    g_print("REST --> %d\n", rest);
    if(rest != 0)
    {
        for(i = 0; i < rest; i++)
        {
            binary[bin_size+i] = '0';
            bin_size++;
        }
    }

    //On récupère le nombre de bits que stockera bin_result
    nb_car_bin = project->matrix.columns * (bin_size / project->matrix.lines);
    g_print("NB CAR BIN --> %d\n", nb_car_bin);
    bin_result = malloc(nb_car_bin * sizeof(char));

    //Découpage du message binaire en groupe de X bits avec X = matrix_line
    for(i = 0; i < (bin_size / project->matrix.lines); i++)
    {
        //Réinitialisation du tableau temp_result à '0'
        for(j = 0; j < project->matrix.columns; j++)
            tmp[j] = '0';

        //codage de la portion du message
        for(j = 0; j < project->matrix.lines; j++)//On se deplace de ligne en ligne
        {
            //Si le bit à coder est à 1, alors on fait les traitements suivants
            if(binary[(i * project->matrix.lines) + j] == 1)
            {
                //on se deplace de bit en bit sur la ligne en cours
                for(k = 0; k < project->matrix.columns; k++)
                {
                    //Si le bit en cours de la matrice est à 1 alors on regarde le contenu dans le tableau temp_result
                    if(project->matrix.matrix[j][k] == 1)
                        tmp[k] = (tmp[k] == '0') ? '1' : '0';
                }
            }
        }

        //enregistrement de temp_result dans le tableau bin_result
        for(j = 0; j < project->matrix.columns; j++)
            bin_result[i * project->matrix.columns + j] = tmp[j];
    }

    free(tmp);
    tmp = malloc(8 * sizeof(char));

    encoded = malloc((nb_car_bin/8 +1) * sizeof(char));

    //Convertion de chaque octet en caractère
    for(i = 0; i < nb_car_bin/8; i++)
    {
        //Récupération octet par octet de bin_result dans tmp
        strncpy(tmp, bin_result+i*8, 8);

        //Convertion de tmp (binaire) en char
        //g_print("%lu\n", strtol(tmp, 0, 2));

        encoded[i] = strtol(tmp, NULL, 2);
    }
    encoded[i+1] = '\0';

    free(binary);
    free(bin_result);
    free(tmp);
    free(*packet);

    *packet = base64_encode((const unsigned char *)encoded, (nb_car_bin/8 +1), (size_t *)&bin_size);

    free(encoded);*/
    char *temp = *packet;
    size_t size = 0;


    *packet = base64_encode((const unsigned char *)temp, strlen(temp), &size);
    free(temp);
}

/**
* Fonction de déchiffrement des packets après la réception
*/
void uncipherPacket(Project *project, char **packet)
{
    /*guint       bin_size;
    gushort     *binary;

    int i, nb = 0, size_tab = 1, x = 0;
    char tmp[8], *bin_result = NULL, *decoded = NULL, *unbased = NULL;

    unbased = base64_decode((const unsigned char *)*packet, strlen(*packet), (size_t *)&bin_size);

    binary = char_to_bin(unbased, &bin_size);

    bin_result = malloc(sizeof(char));

    for(i = 0; i < bin_size; i++)
    {
        if(nb >= size_tab)
        {
            size_tab *= 2;
            bin_result = realloc(bin_result, size_tab * sizeof(char));
        }

        //Récupération des N premiers bits de chaque groupe de bits
        if(i % project->matrix.columns == x)
        {
            bin_result[nb] = (binary[i] == 0) ? '0' : '1';

            nb++;
            x++;

            if(x >= project->matrix.lines)
                x = 0;
        }
    }

    decoded = malloc((nb/8) * sizeof(char));
    //Convertion de chaque octet en caractère
    for(i = 0; i < nb/8; i++)
    {
        //Récupération octet par octet de bin_result dans temp_result
        strncpy(tmp, bin_result+i*8, 8);

        //Convertion de tmp (binaire) en char
        decoded[i] = strtol(tmp, NULL, 2);
    }

    free(binary);
    free(bin_result);
    free(*packet);

    *packet = decoded;*/

    unsigned int       bin_size;
    char *unbased = NULL;

    unbased = base64_decode((const unsigned char *)*packet, strlen(*packet), (size_t *)&bin_size);
    free(*packet);
    *packet = unbased;
}
