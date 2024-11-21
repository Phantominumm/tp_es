# Nom de l'exécutable
TARGET = test_stdes

# Répertoires
SRC_DIR = src
TEST_DIR = tests

# Fichiers sources
SRC_FILES = $(SRC_DIR)/stdes.c $(TEST_DIR)/test_stdes.c

# Répertoires d'inclusion
INCLUDE_DIRS = -I$(SRC_DIR)

# Options de compilation
CFLAGS = -Wall -g $(INCLUDE_DIRS)

# Commande de compilation
$(TARGET): $(SRC_FILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC_FILES)

# Commande pour exécuter les tests
run_tests: $(TARGET)
	./$(TARGET)

# Nettoyage des fichiers générés
clean:
	rm -f $(TARGET)

# Phonie
.PHONY: clean run_tests
