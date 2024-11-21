#include "../headers/stdes.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdarg.h>

#define MAX_BUFFER_SIZE 1000
char buffer_sys[MAX_BUFFER_SIZE];

FICHIER *ouvrir(char *nom, char mode){
    FICHIER *f = malloc(sizeof(FICHIER));
    if(f == NULL){
        fprintf(stderr, "Erreur lors de l'allocation de mémoire.\n");
        return NULL;
    }
    switch (mode){
    case 'L':
        f->idf = open(nom, O_RDONLY);
        break;
    case 'E':
        f->idf = open(nom, O_WRONLY | O_CREAT | O_TRUNC);
        break;
    default:
        fprintf(stderr, " Le mode de lecteure n'est pas bon \n");
        free(f);
        return NULL;
    }
    if(f->idf == -1){
        fprintf(stderr, "Erreur lors de l'ouverture du fichier.\n");
        free(f);
        return NULL;
    }

    f->buf = malloc(MAX_BUFFER_SIZE);
    if(f->buf == NULL){
        fprintf(stderr, "Erreur lors de l'allocation de mémoire.\n");
        close(f->idf);
        free(f);
        return NULL;
    }
    f->mode = mode;
    f->buf_size= MAX_BUFFER_SIZE;
    f->pos= 0;

    return f;
} 
int fermer(FICHIER *f){
    if (f==NULL){
        return -1;
    }
    if (f->mode == 'E'){
        vider(f);
    }

    close(f->idf);
    free(f->buf);
    free(f);
    return 0;
}

int vider(FICHIER *f) {
    if (f == NULL || f->pos == 0) {
        return 0;
    }
    ssize_t w = write(f->idf, f->buf, f->pos);
    if (w < 0) {
        return -1;
    }
    f->pos = 0;

    return 0;
}

ssize_t lire(void *p, size_t taille, size_t nbelem, FICHIER *f) {
    if (f == NULL || f->mode != 'L') {
        return -1;
    }

    size_t octet_total = taille * nbelem;
    size_t octets_lus = 0;
    char *dest = (char *)p;

    while (octets_lus < octet_total) {
        // Si le tampon est vide, on lit de nouvelles données
        if (f->pos >= f->buf_rempli) {
            ssize_t r = read(f->idf, f->buf, f->buf_size);
            if (r < 0) {
                perror("Erreur de lecture");
                return -1;  // Retourner -1 en cas d'erreur de lecture
            }

            if (r == 0) {
                // Fin du fichier atteinte
                break;  // Sortie si fin du fichier, même si des octets restent à lire
            }

            f->buf_rempli = r;  // Mise à jour de la taille du tampon
            f->pos = 0;  // Réinitialisation de la position dans le tampon
        }

        size_t octet_copie = f->buf_rempli - f->pos;

        // Si on a plus d'octets à lire que ce que le tampon peut fournir, ajuster
        if (octet_copie > octet_total - octets_lus) {
            octet_copie = octet_total - octets_lus;
        }

        // Copier les données dans le tampon de destination
        memcpy(dest + octets_lus, f->buf + f->pos, octet_copie);

        // Mettre à jour la position et le nombre d'octets lus
        f->pos += octet_copie;
        octets_lus += octet_copie;
    }

    // Si on a copié tous les éléments demandés, retourner le nombre d'éléments lus
    return octets_lus / taille;
}

ssize_t ecrire(void *p,size_t taille,size_t nbelem, FICHIER *f) {
    if (f==NULL || f->mode != 'E') {
        return -1;
    }
    size_t octet_total = taille * nbelem;
    size_t octets_ecrits = 0;
    char *src = (char *)p;

    while (octets_ecrits < octet_total) {
        if (f->pos >= f->buf_size) {
            if (f->pos >0) {
               ssize_t w= write(f->idf, f->buf, f->pos);
               if (w < 0){
                return -1;
               }
            }
            f->pos = 0;
        }

        size_t espace_libre = f->buf_size - f->pos;
        size_t octet_copie;

        if(octet_total - octets_ecrits < espace_libre){
            octet_copie= octet_total - octets_ecrits;
        }
        else{
            octet_copie=espace_libre;
        }
        memcpy(f->buf + f->pos, src + octets_ecrits, octet_copie);
        f->pos += octet_copie;
        octets_ecrits += octet_copie;
    }

    return octets_ecrits / taille;
}

int fecriref (FICHIER *f, const char *format, ...){
    if (!f || !format) {
        return -1;
    }

    char buffer[1024]; // Tampon temporaire pour la sortie
    va_list args;

    // Démarrage du traitement des arguments
    va_start(args, format);

    // Utilisation de vsnprintf pour traiter la chaîne de format et les arguments
    int length = vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    // Vérification des erreurs de formatage ou de débordement de tampon
    if (length < 0 || length >= sizeof(buffer)) {
        return -1;
    }

    // Écriture de la chaîne formatée dans le fichier
    return ecrire(buffer, sizeof(char), length, f);
}

int ecriref (const char *format, ...){
    return 0;
}

ssize_t lire_une_ligne(void *p, size_t taille, size_t nbelem, FICHIER *f) {
    if (f == NULL || f->mode != 'L') {
        return -1;
    }

    size_t octets_lus = 0;
    char *dest = (char *)p;
    size_t octets_restants = nbelem - 1;  // Pour laisser de la place pour '\0'

    // Tant qu'il reste de l'espace dans le tampon et que nous ne rencontrons pas de saut de ligne
    while (octets_restants > 0) {
        // Lire dans le tampon à partir du fichier
        ssize_t r = lire(f->buf, sizeof(char), f->buf_size, f);  // Lire le tampon
        if (r < 0) {
            perror("Erreur de lecture");
            return -1;
        }

        // Si rien n'est lu, on sort
        if (r == 0) {
            break;
        }

        // Copie des données du tampon dans le tampon final (sans dépasser nbelem)
        for (ssize_t i = 0; i < r && octets_lus < octets_restants; i++) {
            char c = f->buf[i];

            // Si on rencontre un saut de ligne, on s'arrête
            if (c == '\n') {
                dest[octets_lus++] = c;
                break;  // Fin de ligne trouvée
            }

            dest[octets_lus++] = c;
        }

        // Si un saut de ligne a été rencontré, on arrête de lire
        if (octets_lus < nbelem - 1 && f->buf[r-1] == '\n') {
            break;
        }
    }

    dest[octets_lus] = '\0';  // Terminer la chaîne de caractères
    return octets_lus;        // Retourner le nombre d'octets lus
}


int fliref(FICHIER *f, const char *format, ...) {
    if (!f || !format) {
        return -1;
    }

    char buffer[1024];
    int octet_lus, matches = 0;
    va_list args;

    // Lire le fichier et mettre dans le tampon
    //octet_lus = lire_une_ligne(buffer, sizeof(char), sizeof(buffer) - 1, f);
    octet_lus = lire(buffer, sizeof(char), sizeof(buffer) - 1, f);
    if (octet_lus <= 0) {
        return -1;
    }

    va_start(args, format);

    // Utiliser vsscanf pour traiter la chaîne de format et les arguments
    matches = vsscanf(buffer, format, args);
    if (matches < 1){
        fprintf(stderr, "Erreur de format dans le formatage du fichier\n");
        return -1;
    }

    va_end(args);

    // Retourner le nombre de fois où le format a été reconnu
    return matches;
}

/*
int main (int argc, char **argv)
{
    / PREMIERE IMPLEMENTATION
FICHIER *f1;
FICHIER *f2;
char c;
if (argc != 3)
exit (-1);
f1 = ouvrir (argv[1], 'L');
if (f1 == NULL)
exit (-1);
f2 = ouvrir (argv[2], 'E');
if (f2 == NULL)
exit (-1);
while (lire (&c, 1, 1, f1) == 1){
    ecrire (&c, 1,1, f2);
}
fermer (f1);
fermer (f2); /

FICHIER *f1;
f1 = ouvrir(argv[1], 'E');
if (f1 == NULL){
    exit (-1);
}


fecriref (f1, " %c %s %d \n", 'a', "bonjour",12);
fecriref (f1, " %d \n", -1257);
fermer(f1);

FICHIER *f2;
f2=ouvrir(argv[1], 'L');

if (f2 == NULL){
    exit (-1);
}
char c;
char str[50];
int num;
 // Test reading the first line
    if (fliref(f2, " %c %s %d", &c, str, &num) == 3) {
        printf("Lecture ligne 1 du fichier: '%c', '%s', %d\n", c, str, num);
    } else {
        fprintf(stderr, "Erreur lecture sur la premiere ligne\n");
    }
    // Test reading the second line
    if (fliref(f2, " %d", &num) == 1) {
        printf("Lecture ligne 2: %d \n", num);
    } else {
        fprintf(stderr, "Erreur lecture sur la 2eme ligne\n");
    }

    fermer(f2);
return 0;
}
*/