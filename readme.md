# Projet de gestion de fichiers (TP Es)

Ce projet implémente une gestion de fichiers personnalisée, offrant des fonctions pour ouvrir, fermer, lire et écrire dans des fichiers. Il inclut également des fonctions formatées pour l'écriture et la lecture dans des fichiers.

## Description des fonctionnalités

Le projet implémente les fonctions suivantes :

- `ouvrir` : Ouvre un fichier en mode lecture ('L') ou écriture ('E').
- `fermer` : Ferme un fichier ouvert.
- `lire` : Lit des données depuis un fichier ouvert en mode lecture.
- `ecrire` : Écrit des données dans un fichier ouvert en mode écriture.
- `fecriref` : Écrit des données dans un fichier ouvert en mode écriture avec un nombre d'argument pouvant changer.
- `fliref` : Lit des données depuis un fichier ouvert en mode lecture avec un nombre d'argument pouvant changer.

Le projet inclut également des tests pour valider le bon fonctionnement de ces fonctions.

## Structure du projet

Le projet est organisé de la manière suivante :
- Dossier "headers" contenant les stdes.h 
- Dossier "src" contenant stdes.c, le programme principal
- Dossier "tests" contenant les programmes de tests

## Execution des tests

avec le makefile:   make ;  make run_tests

