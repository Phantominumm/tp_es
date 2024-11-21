#include "../headers/stdes.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_ouvrir() {
    // Test de l'ouverture en mode lecture
    FICHIER *f1 = ouvrir("lire.txt", 'L');
    assert(f1 != NULL);
    assert(f1->mode == 'L');
    fermer(f1);
    printf("ouverture en mode lecture fonctionne\n");

    // Test de l'ouverture en mode écriture
    FICHIER *f2 = ouvrir("ecrire.txt", 'E');
    assert(f2 != NULL);
    assert(f2->mode == 'E');
    fermer(f2);
    printf("ouverture en mode écriture fonctionne\n");

    // Test de l'ouverture avec un fichier inexistant
    FICHIER *f3 = ouvrir("./inexistant.txt", 'L');
    assert(f3 == NULL);
    printf("ouverture d'un fichier inexistant fonctionne\n");
}
void test_fermer() {
    // Test de fermeture d'un fichier ouvert en mode lecture
    FICHIER *f1 = ouvrir("lire.txt", 'L');
    assert(f1 != NULL);
    assert(fermer(f1) == 0);

    // Test de fermeture d'un fichier ouvert en mode écriture
    FICHIER *f2 = ouvrir("ecrire.txt", 'E');
    assert(f2 != NULL);
    assert(fermer(f2) == 0);
    printf("fermeture des fichiers fonctionne\n");
}
void test_lire() {
    FICHIER *f = ouvrir("lire.txt", 'L');
    char buffer[100];
    
    // Lire des données du fichier
    ssize_t octets_lus = lire(buffer, sizeof(char), sizeof(buffer), f);
    assert(octets_lus > 0);  // S'assurer qu'il y a bien des données lues
    printf("Données lues: %s --finLecture\n", buffer);
    
    fermer(f);
}

void test_ecrire() {
    FICHIER *f = ouvrir("ecrire.txt", 'E');
    const char *data = "Test d'écriture dans le fichier";

    // Écrire dans le fichier
    ssize_t octets_ecrits = ecrire((void*)data, sizeof(char), strlen(data), f);
    assert(octets_ecrits == strlen(data));  // Vérifiez que le nombre d'octets écrits correspond à la longueur de la chaîne
    
    fermer(f);

     // Ouvrir à nouveau le fichier en mode lecture standard (fopen)
    FILE *fichier = fopen("ecrire.txt", "r");
    assert(fichier != NULL);  // Vérifie si l'ouverture a réussi

    char buffer[1024];  // Un buffer assez grand pour contenir le contenu du fichier
    size_t n = fread(buffer, 1, sizeof(buffer)-1, fichier);  // Lire le contenu dans le buffer
    buffer[n] = '\0';  // Ajouter le caractère de fin de chaîne pour afficher comme une chaîne C

    // Afficher le contenu du fichier
    printf("Contenu du fichier ecrire.txt: %s\n", buffer);
    
}
void test_fecriref() {
    FICHIER *f = ouvrir("fecriref.txt", 'E');
    // Utilisation de fecriref pour écrire avec un format
    assert(fecriref(f," %c %s %d \n", 'a', "bonjour",12));
    fermer(f);

     // Ouvrir à nouveau le fichier en mode lecture standard (fopen)
    FILE *fichier = fopen("fecriref.txt", "r");
    assert(fichier != NULL);  // Vérifie si l'ouverture a réussi

    char buffer[1024];  // Un buffer assez grand pour contenir le contenu du fichier
    size_t n = fread(buffer, 1, sizeof(buffer)-1, fichier);  // Lire le contenu dans le buffer
    buffer[n] = '\0';  // Ajouter le caractère de fin de chaîne pour afficher comme une chaîne C

    // Afficher le contenu du fichier
    printf("Contenu du fichier fecriref.txt: %s\n", buffer);

}
void test_fliref() {
    FICHIER *f = ouvrir("fecriref.txt", 'L');
    char c;
    char str[50];
    int num;

    // Lire le fichier avec un format
    int matches = fliref(f, " %c %s %d", &c, str, &num);
    assert(matches == 3);  // Vérifiez que trois éléments ont été lus
    printf("Données lues: %c, %s, %d\n", c, str, num);

    fermer(f);
}


int main() {
    test_ouvrir();
    test_fermer();
    test_lire();
    test_ecrire();
    test_fecriref();
    test_fliref();
    return 0;
}