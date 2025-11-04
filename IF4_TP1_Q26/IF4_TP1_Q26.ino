/* Programme IF4_TP1_Q26
 * Auteurs: Thomas Vadebout, Arnaud Lelevé
 * Version 2024.2
 *
 * Synchronisation : Rendez-vous entre 3 tâches
 * - Tâche 1 et Tâche 2 exécutent leur travail en parallèle
 * - Tâche 3 attend que les 2 premières aient fini pour démarrer
 */


// param oscillations
#define nb_iterations 10
#define GLOBAL_HALF_PERIOD_MS 5

// pins sondées à l'oscillo
// mettre la masse sur GND, voie 1 sur broche 23, voie 2 sur broche 19, voie 3 sur broche 18
#define oscillo1 23
#define oscillo2 19
#define oscillo3 18

// priorité des tâches
#define task1_prio 1
#define task2_prio 1
#define task3_prio 2

// variables globales représentant les tâches
TaskHandle_t Task1, Task2, Task3;

// Sémaphore de comptage pour synchroniser les 3 tâches
SemaphoreHandle_t xCountingSemaphore = NULL;

/**
 * simule du travail en alternant la sortie fournie en paramètre
 */
void alternate(uint8_t output, int half_period_ms) {
  int i, etat = LOW;
  for( i = 0; i<nb_iterations; i++ ) {
    if (etat == HIGH)
      etat = LOW;
    else
      etat = HIGH;
    digitalWrite(output, etat);
    delay(half_period_ms);
  }
}

/******************************
 * code de la tâche 1
 * ****************************
 */
void vTask1( void *pvParameters )
{
  Serial.printf("[INFO] vTask1 / core %d started\n", xPortGetCoreID());

  for (;;) {
    Serial.printf("[INFO] vTask1: début du travail\n");

    // simule du travail
    alternate(oscillo1, GLOBAL_HALF_PERIOD_MS);

    Serial.printf("[INFO] vTask1: travail terminé, libération du sémaphore\n");

    // libère le sémaphore pour indiquer que le travail est terminé
    xSemaphoreGive( xCountingSemaphore );

    // affiche le compteur du sémaphore
    UBaseType_t count = uxSemaphoreGetCount( xCountingSemaphore );
    Serial.printf("[INFO] vTask1: compteur sémaphore = %d\n", count);

    // pause avant de recommencer
    vTaskDelay( pdMS_TO_TICKS( 2000 ) );
  }

  vTaskDelete( NULL );
}


/******************************
 * code de la tâche 2
 * ****************************
 */
void vTask2( void *pvParameters )
{
  Serial.printf("[INFO] vTask2 / core %d started\n", xPortGetCoreID());

  for (;;) {
    Serial.printf("[INFO] vTask2: début du travail\n");

    // simule du travail (un peu plus long que Task1)
    alternate(oscillo2, GLOBAL_HALF_PERIOD_MS);
    delay(20); // délai supplémentaire pour Task2

    Serial.printf("[INFO] vTask2: travail terminé, libération du sémaphore\n");

    // libère le sémaphore pour indiquer que le travail est terminé
    xSemaphoreGive( xCountingSemaphore );

    // affiche le compteur du sémaphore
    UBaseType_t count = uxSemaphoreGetCount( xCountingSemaphore );
    Serial.printf("[INFO] vTask2: compteur sémaphore = %d\n", count);

    // pause avant de recommencer
    vTaskDelay( pdMS_TO_TICKS( 2000 ) );
  }

  vTaskDelete( NULL );
}


/******************************
 * code de la tâche 3
 * ****************************
 */
void vTask3( void *pvParameters )
{
  Serial.printf("[INFO] vTask3 / core %d started\n", xPortGetCoreID());

  for (;;) {
    Serial.printf("[INFO] vTask3: en attente de Task1...\n");

    // attend que Task1 libère le sémaphore
    xSemaphoreTake( xCountingSemaphore, portMAX_DELAY );
    Serial.printf("[INFO] vTask3: Task1 a terminé (compteur = %d)\n",
                  uxSemaphoreGetCount( xCountingSemaphore ));

    Serial.printf("[INFO] vTask3: en attente de Task2...\n");

    // attend que Task2 libère le sémaphore
    xSemaphoreTake( xCountingSemaphore, portMAX_DELAY );
    Serial.printf("[INFO] vTask3: Task2 a terminé (compteur = %d)\n",
                  uxSemaphoreGetCount( xCountingSemaphore ));

    Serial.printf("[INFO] vTask3: RENDEZ-VOUS atteint! Les 2 tâches ont terminé\n");
    Serial.printf("[INFO] vTask3: début du travail\n");

    // simule du travail
    alternate(oscillo3, GLOBAL_HALF_PERIOD_MS);

    Serial.printf("[INFO] vTask3: travail terminé\n");
    Serial.println("========================================");

    // pause avant le prochain cycle
    vTaskDelay( pdMS_TO_TICKS( 500 ) );
  }

  vTaskDelete( NULL );
}


/**
 * Appelée au démarrage du programme pour initialiser la liaison série et créer les tâches
 */
void setup()
{
  // initialisation de la communication série via USB PC
  Serial.begin(115200);
  while (!Serial);
  Serial.println("[INFO] Start - Synchronisation rendez-vous 3 tâches");

  // declare 3 sorties TOR
  pinMode(oscillo1, OUTPUT);
  pinMode(oscillo2, OUTPUT);
  pinMode(oscillo3, OUTPUT);

  // création d'un sémaphore de comptage
  // Arguments: valeur initiale = 0, valeur maximale = 2
  xCountingSemaphore = xSemaphoreCreateCounting( 2, 0 );

  if( xCountingSemaphore == NULL ) {
    Serial.println("[ERROR] Impossible de créer le sémaphore de comptage");
    for(;;); // blocage
  }

  Serial.printf("[INFO] Sémaphore de comptage créé (max=2, initial=0)\n");

  /* créé les 3 tâches
   *  cf https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/
   *
   *  vTask1,     Function to implement the task
   *  "Task1",    Name of the task
   *  10000,      Stack size in words
   *  NULL,       Task input parameter
   *  task1_prio, Priority of the task
   *  &Task1,     Task handle.
   *  0);         Core where the task should run
   */
  xTaskCreatePinnedToCore( vTask1, "Task1", 10000, NULL, task1_prio, &Task1, 0);
  xTaskCreatePinnedToCore( vTask2, "Task2", 10000, NULL, task2_prio, &Task2, 1);
  xTaskCreatePinnedToCore( vTask3, "Task3", 10000, NULL, task3_prio, &Task3, 0);

  // Setup() fait partie d'une tâche dont on n'a plus besoin, maintenant que tout est lancé => elle s'autodétruit.
  Serial.print("[INFO] setup task finished\n");
  vTaskDelete( NULL );
}




/*
 * indispensable pour pouvoir compiler même si inutile car la tâche arduino est tuée à la fin de setup
 */
void loop()
{

}
