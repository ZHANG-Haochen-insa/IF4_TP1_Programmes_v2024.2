# TP1 - Programmation Temps Réel avec FreeRTOS sur ESP32

## Description
Ce dépôt contient les programmes du TP1 d'IF4/5 - Programmation temps réel sur microcontrôleur ESP32 avec FreeRTOS.

**Objectif**: Se familiariser avec les mécanismes multi-tâches temps réel offerts par FreeRTOS.

## Prérequis
- Arduino IDE v2 (les programmes ne fonctionneront pas avec la v1)
- Carte ESP32
- Oscilloscope pour observer les signaux
- Câbles pour connecter les broches GPIO à l'oscilloscope (+ masse GND)

## Démarrage de l'environnement
```bash
/opt/arduino-ide_2.3.2_Linux_64bit/arduino-ide
```

## Structure des programmes

### 2.1 Tâches en concurrence

#### **IF4_TP1_Q21a** - Premier essai de tâches concurrentes
- **Description**: Deux tâches avec différentes priorités (task1_prio=1, task2_prio=10)
- **Broches**: GPIO 19 (oscillo1), GPIO 23 (oscillo2)
- **Observations**:
  - Décrire le résultat à l'oscilloscope
  - Observer le moniteur série
  - Comprendre l'impact des priorités sur l'ordonnancement

#### **IF4_TP1_Q21b** - Variante avec priorités égales
- **Modifications par rapport à Q21a**: Étudier l'impact des priorités
- **Expérimentations**:
  - Observer le comportement avec priorités différentes
  - Modifier les priorités pour les mettre à la même valeur
  - Analyser l'impact sur l'ordonnancement

### 2.2 Tâches périodiques

#### **IF4_TP1_Q22a** - Première approche (delay)
- **Méthode**: Utilisation de `delay()` pour créer des tâches périodiques
- **Questions**:
  - Mesurer les périodes réelles des tâches 1 et 2
  - Tester sur un seul cœur vs deux cœurs
  - Évaluer si c'est une bonne approche pour des tâches périodiques

#### **IF4_TP1_Q22b** - Deuxième approche (vTaskDelay)
- **Méthode**: Utilisation de `vTaskDelay()` pour un meilleur contrôle périodique
- **Variable importante**: `nb_iterations` - nombre d'itérations simulant le travail
- **Expérimentations**:
  - Comparer avec Q22a
  - Augmenter progressivement `nb_iterations` et observer le comportement
  - Identifier le critère permettant d'anticiper les problèmes

### 2.3 Synchronisation entre tâches

#### Concepts clés
- **Sémaphore binaire**: Synchronisation entre deux tâches (feu vert)
- **Mutex** (exclusion mutuelle): Protection de ressources partagées avec héritage de priorité

#### **IF4_TP1_Q23a** - Introduction aux sémaphores
- **Description**: Exemple basique de synchronisation avec sémaphore
- **Objectif**: Comprendre le mécanisme de synchronisation

#### **IF4_TP1_Q23b** - Protection de ressources partagées
- **État actuel**: Mutex commentés - accès non protégé aux ressources
- **Problème**: Accès concurrent à la ressource partagée
- **Modification requise**:
  ```cpp
  // Décommenter les lignes suivantes:
  // Lignes 57-58, 65-66 (vTask1)
  // Lignes 92-93, 100-101 (vTask2)
  ```
- **Configuration**: Tâches sur des cœurs différents (Task1 sur core 0, Task2 sur core 1)

#### **IF4_TP1_Q23c** - Inversion de priorité et héritage
- **Tâches**: 3 tâches avec priorités différentes (Haute=20, Inter=10, Basse=5)
- **Scénario**:
  - Tâche Basse accède à une ressource partagée
  - Tâche Haute attend cette ressource
  - Tâche Inter s'exécute entre les deux
- **Modification requise** (ligne 168):
  ```cpp
  // Remplacer:
  xSemaphore = xSemaphoreCreateBinary();
  // Par:
  xSemaphore = xSemaphoreCreateMutex();
  ```
- **Observation**: Différence entre sémaphore binaire et mutex (héritage de priorité)

### 2.4 Les interruptions

#### **IF4_TP1_Q24** - Gestion des interruptions
- **Description**: Introduction aux mécanismes d'interruption
- **Outils**: Oscilloscope + moniteur série

### 2.5 Communication entre tâches

#### **IF4_TP1_Q25** - Files d'attente (Queues)
- **Description**: Communication inter-tâches via queues
- **Travail supplémentaire**:
  - Tester l'exemple n°7 de http://tvaira.free.fr/esp32/esp32-freertos.html
  - Modifier les paramètres et commenter les résultats

### 2.6 Synchronisation - Exercice final

#### **À créer** - Rendez-vous entre 3 tâches
Créer un nouveau programme avec:
- **Tâche 1**: Exécute un travail
- **Tâche 2**: Exécute un autre travail en parallèle
- **Tâche 3**: Attend que les tâches 1 et 2 aient terminé avant de démarrer

**Indices**:
- `uxSemaphoreGetCount()` : http://www.openrtos.net/uxSemaphoreGetCount.html
- `xSemaphoreCreateCounting()` : Sémaphore compteur (optionnel)

## Broches GPIO utilisées

| Programme | Oscillo 1 | Oscillo 2 |
|-----------|-----------|-----------|
| Q21a      | GPIO 19   | GPIO 23   |
| Q21b      | GPIO 19   | GPIO 23   |
| Q22a      | GPIO 19   | GPIO 23   |
| Q22b      | GPIO 19   | GPIO 23   |
| Q23a      | -         | -         |
| Q23b      | GPIO 23   | GPIO 19   |
| Q23c      | -         | -         |
| Q24       | À vérifier| -         |
| Q25       | À vérifier| -         |

**⚠️ IMPORTANT**: Ne pas oublier de connecter la masse (GND) à l'oscilloscope!

## Concepts FreeRTOS abordés

### Création de tâches
```cpp
xTaskCreate(
    vTask1,          // Fonction de la tâche
    "vTask1",        // Nom (arbitraire)
    10000,           // Taille mémoire (en words)
    NULL,            // Paramètre transmis
    task1_prio,      // Priorité statique
    NULL             // Handle (optionnel)
);
```

### Tâches sur un cœur spécifique
```cpp
xTaskCreatePinnedToCore(
    vTask1,          // Fonction
    "Task1",         // Nom
    10000,           // Stack size
    NULL,            // Paramètre
    task1_prio,      // Priorité
    &Task1,          // Handle
    0                // Cœur (0 ou 1)
);
```

### Délais
- `delay(ms)`: Délai bloquant simple
- `vTaskDelay(pdMS_TO_TICKS(ms))`: Délai avec libération du CPU

### Sémaphores et Mutex
```cpp
// Créer un sémaphore binaire
xSemaphore = xSemaphoreCreateBinary();

// Créer un mutex (avec héritage de priorité)
xMutex = xSemaphoreCreateMutex();

// Prendre le sémaphore/mutex
xSemaphoreTake(xSemaphore, portMAX_DELAY);

// Libérer le sémaphore/mutex
xSemaphoreGive(xSemaphore);
```

## Compte rendu
- **Format**: Manuscrit
- **Délai**: 1 semaine jour pour jour après la séance
- **Contenu**: Observations, explications, réponses aux questions
- **Importance**: Utile pour le TP2 et l'examen

## Évaluation
Les enseignants évalueront:
1. La préparation du travail
2. L'avancement pendant la séance de TP
3. Le compte rendu manuscrit

## Ressources utiles
- Documentation FreeRTOS: https://www.freertos.org/
- Stack size: https://www.freertos.org/FAQMem.html#StackSize
- ESP32 + FreeRTOS exemples: http://tvaira.free.fr/esp32/esp32-freertos.html
- Sémaphore counting: http://www.openrtos.net/uxSemaphoreGetCount.html

## Notes importantes
- Tous les programmes se terminent par `vTaskDelete(NULL)` dans setup() car la tâche Arduino n'est plus nécessaire
- La fonction `loop()` est vide mais obligatoire pour la compilation
- L'ESP32 dispose de 2 cœurs, permettant du vrai parallélisme
- Les priorités plus élevées s'exécutent en premier (task_prio=10 > task_prio=1)

## Troubleshooting
- Si le moniteur série n'affiche rien: vérifier le baud rate (115200)
- Si l'oscilloscope ne montre rien: vérifier les connexions GPIO et GND
- Si la compilation échoue: vérifier que Arduino IDE v2 est utilisé
