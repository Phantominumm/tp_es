#ifndef STDES_H
#define STDES_H

#include <stddef.h>
#include <sys/types.h>

// Définition de la structure FICHIER
typedef struct {
    int idf;           // Descripteur du fichier
    char *buf;         // Tampon interne
    size_t buf_size;   // Taille du tampon
    size_t buf_rempli; // Nombre d'octets actuellement dans le tampon
    size_t pos;        // Position actuelle dans le tampon
    char mode;         // Mode d'ouverture ('L' pour lecture, 'E' pour écriture)
} FICHIER;

// Fonctions principales
FICHIER* ouvrir(const char *nom, char mode);
int fermer(FICHIER *f);
ssize_t lire(void *p, size_t taille, size_t nbelem, FICHIER *f);
ssize_t ecrire(const void *p, size_t taille, size_t nbelem, FICHIER *f);

// Fonctions avancées avec format
int fliref(FICHIER *f, const char *format, ...);
int fecriref(FICHIER *f, const char *format, ...);

#endif // STDES_H
