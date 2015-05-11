#include "includes.h"

/**
* Fonction de vérification d'adresse IP
*/
gboolean regex_verification(const gchar *ip, gushort mode)
{
    GRegex      *regex;
    GMatchInfo  *match_info;
    GError      *error = NULL;
    gboolean    result;

    switch(mode)
    {
        case IPPORT_ADDR :
            regex = g_regex_new("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                                "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                                "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                                "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?):[0-9]{1,5}$",
                                0, 0,
                                NULL);
            break;

        case IP_ADDR :
            regex = g_regex_new("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                                "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                                "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                                "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$",
                                0, 0,
                                NULL);
            break;

        case MAC_ADDR :
            regex = g_regex_new("^([0-9A-F]{2}[:-]){5}([0-9A-F]{2})$",
                                0, 0,
                                NULL);
            break;

        default :
            return 0;
    }

    g_regex_match_full(regex,
                       ip,
                       -1, 0, 0,
                       &match_info,
                       &error);

    if(g_match_info_matches(match_info))
        result = TRUE;
    else
        result = FALSE;

    g_match_info_free(match_info);
    g_regex_unref(regex);

    return result;
}


/**
* Fonction de vérification d'une adresse IP lors de l'ajout d'un nouveau device
* Renvoi TRUE si l'IP n'est pas déjà déjà utilisée par un device ce trouvant dans la liste chaînée
*/
gboolean authorize_ip(Devices *devices, const gchar *ip)
{
    Devices *temp = devices;

    while(temp != NULL)
    {
        if(strcmp(temp->element.ip, ip) == 0)
            return FALSE;
        temp = temp->next;
    }
    return TRUE;
}

/**
* Fonction d'affichage d'un message d'erreur
*/
void warning_dialog(GtkWidget *window, gchar *sentence)
{
    GtkWidget *warning;

    warning = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "%s", sentence);

    gtk_window_set_title(GTK_WINDOW(warning), "Warning");
    gtk_dialog_run(GTK_DIALOG(warning));

    gtk_widget_destroy(warning);
}

/**
* Fonction utilisée pour masquer une fenêtre et en afficher une autre
*/
void hide_window(GtkWidget *window1, GtkWidget *window2)
{
    gtk_widget_hide_all(window1);
    gtk_widget_show_all(window2);
}

/**
 * Revoie un tableau de chaînes en fonction du délimiteur spécifié.
 * Le tableau renvoyé doit être libéré après utilisation.
 */
char **explode(char *src, const char *delim, unsigned short *size)
{
    char **array = NULL; //Tableau de retour
    unsigned short sizeArr = 0; //Taille du tableau de retour

    char *part = NULL; //Portion de chaîne extraite grâce au délimiteur
    unsigned short sizePart; //Taille de cette sous-chaîne

    char *str = src; //Copie de la chaîne d'origine pour traitement
    unsigned short sizeDelim = strlen(delim); //Taille du délimiteur, pour décaler le pointeur après chaque extraction

    //Tant qu'on trouve le délimiteur dans la chaîne de traitement...
    while((part = strstr(str, delim)) != NULL)
    {
        sizePart = part-str;

        //Si la chaine trouvé n'est pas vide
        if(sizePart != 0)
        {
            //On alloue une case supplémentaire à notre tableau de chaînes
            sizeArr++;
            array = realloc(array, sizeof(char*) * sizeArr);

            //On alloue la chaine du tableau
            array[sizeArr-1] = malloc(sizeof(char) * (sizePart+1));
            strncpy(array[sizeArr-1], str, sizePart);
            array[sizeArr-1][sizePart] = '\0';
        }

        //On décale le pointeur str pour reprendre la boucle après le délimiteur
        part = part + sizeDelim;
        str = part;
    }

    //Si on ne trouve plus de délimiteur mais que la chaîne n'est pas vide, on récupère ce qu'il reste
    if(strlen(str) != 0)
    {
        unsigned short lastcr;

        sizePart = strlen(str);
        //Vaut 1 si le dernier caractère de la chaîne est un retour chariot
        lastcr = (str[sizePart-1] == '\n');

        sizeArr++;
        array = realloc(array,sizeof(char*) * sizeArr);

        //Si le dernier caractère de la chaîne est un \n, +0 car il sera écrasé par un \0 ; sinon, +1 car on ajoutera un \0 à la fin
        array[sizeArr-1]= malloc(sizeof(char) * (sizePart + !lastcr));
        strncpy(array[sizeArr-1],str,sizePart);

        //Si le dernier caractère de la chaîne est un \n, on le remplace par un \0, sinon on ajoute le \0 à la fin de la chaîne
        array[sizeArr-1][sizePart - lastcr] = '\0';
    }

    //On met à jour le pointeur size avec le nombre de chaînes contenues dans le tableau
    *size = sizeArr;

    return array;
}

/**
* Fonction de conversion String to Binary
*/
gushort *char_to_bin(const char *str, unsigned int *size)
{
    gushort     *binary;
    int         i, j, bit, size_tab = 1;
    unsigned int len;



    binary = malloc(sizeof(gushort));
    len = *size;
    *size = 0;

    //Convertit en bit le message caractère par caractère
    for(i = 0; i < len; i++)
    {
        for(j = 7; j >= 0; j--)
        {
            bit = (str[i] & (1 << j)) ? 1 : 0;

            if(*size >= size_tab)
            {
                size_tab *= 2;
                binary = realloc(binary, size_tab * sizeof(gushort));
            }

            binary[*size] = bit;

            (*size)++;
        }
    }

    return binary;
}

/**
* Fonction de chargement de la matrice
*/
void load_matrix(Project *project)
{
    project->matrix.matrix[0][0] = 1;
    project->matrix.matrix[0][1] = 0;
    project->matrix.matrix[0][2] = 0;
    project->matrix.matrix[0][3] = 0;
    project->matrix.matrix[0][4] = 1;
    project->matrix.matrix[0][5] = 1;
    project->matrix.matrix[0][6] = 0;
    project->matrix.matrix[0][7] = 0;

    project->matrix.matrix[1][0] = 0;
    project->matrix.matrix[1][1] = 1;
    project->matrix.matrix[1][2] = 0;
    project->matrix.matrix[1][3] = 0;
    project->matrix.matrix[1][4] = 0;
    project->matrix.matrix[1][5] = 0;
    project->matrix.matrix[1][6] = 1;
    project->matrix.matrix[1][7] = 0;

    project->matrix.matrix[2][0] = 0;
    project->matrix.matrix[2][1] = 0;
    project->matrix.matrix[2][2] = 1;
    project->matrix.matrix[2][3] = 0;
    project->matrix.matrix[2][4] = 1;
    project->matrix.matrix[2][5] = 1;
    project->matrix.matrix[2][6] = 1;
    project->matrix.matrix[2][7] = 1;

    project->matrix.matrix[3][0] = 0;
    project->matrix.matrix[3][1] = 0;
    project->matrix.matrix[3][2] = 0;
    project->matrix.matrix[3][3] = 1;
    project->matrix.matrix[3][4] = 0;
    project->matrix.matrix[3][5] = 0;
    project->matrix.matrix[3][6] = 0;
    project->matrix.matrix[3][7] = 1;

    project->matrix.columns = 8;
    project->matrix.lines   = 4;

}

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length)
{
    int mod_table[] = {0, 2, 1}, i, j;

    char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                            'w', 'x', 'y', 'z', '0', '1', '2', '3',
                            '4', '5', '6', '7', '8', '9', '+', '/'};

    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = malloc(((*output_length)+1) *sizeof(char));
    if (encoded_data == NULL) return NULL;

    for (i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    encoded_data[*output_length] = '|';
    encoded_data[*output_length+1] = '\0';

    return encoded_data;
}


char *base64_decode(const unsigned char *data, size_t input_length, size_t *output_length)
{
    int i, j;
    char *decoding_table = build_decoding_table();

    if (decoding_table == NULL) return NULL;

    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    char *decoded_data = malloc(((*output_length)+1) *sizeof(char));
    if (decoded_data == NULL) return NULL;

    for (i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    free(decoding_table);

    decoded_data[*output_length] = '\0';

    return decoded_data;
}


char *build_decoding_table()
{
    int i;
    char *decoding_table = NULL;

    char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                            'w', 'x', 'y', 'z', '0', '1', '2', '3',
                            '4', '5', '6', '7', '8', '9', '+', '/'};

    decoding_table = malloc(256);

    for (i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;

    return decoding_table;
}
