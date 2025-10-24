// Programme IF4_TP1_Q21a

// pins sondées à l'oscillo
// NE PAS OUBLIER LA MASSE (GND) !!
uint8_t oscillo1     = 19;
uint8_t oscillo2     = 23;

// priorité des tâches 1 et 2
int task1_prio = 1;
int task2_prio = 10;

/******************************
 * code de la tâche 1
 * ****************************
 */
void vTask1( void *pvParameters )
{
  bool etat = LOW;

  Serial.printf("vTask1 / core %d started\n", xPortGetCoreID());
  
  // boucle infinie
  for( ;; )
  {
    if (etat == HIGH)
      etat = LOW;
    else
      etat = HIGH;
    digitalWrite(oscillo1, etat);       
  }

  // si la tâche devait se terminer d'elle même, alors finir avec:
  // vTaskDelete( NULL );
}

/******************************
 * code de la tâche 2
 * ****************************
 */
void vTask2( void *pvParameters )
{
  bool etat = LOW;

  Serial.printf("vTask2 / core %d started\n", xPortGetCoreID());
  
  // boucle infinie
  for( ;; )
  {
    if (etat == HIGH)
      etat = LOW;
    else
      etat = HIGH;
    digitalWrite(oscillo2, etat);
    //delay(50);   
    
  }

  // si la tâche devait se terminer d'elle même, alors finir avec:
  // vTaskDelete( NULL );
}


/**
 * Appelée au démarrage du programme pour initialiser la liaison série et créer les tâches
 */
void setup()
{
  // initialisation de la communication série via USB PC
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Start");

  // déclare 2 sorties TOR, celles accessibles à côté de la prise USB
  pinMode(oscillo1, OUTPUT);
  pinMode(oscillo2, OUTPUT);

  // Création des tâches :
  // 1er argument: la fonction affectée à chaque tâche
  // 2e: son nom, arbitraire 
  // 3e: la taille mémoire à allouer à la tâche, unité: word 16 ou 32 bits selon le CPU
  //     pour toute question: https://www.freertos.org/FAQMem.html#StackSize
  // 4e: paramètre éventuel transmis à la tâche, ici yenapas.
  // 5e: priorité (statique)
  // 6e: optionnel

  
  xTaskCreate(vTask1, "vTask1", 10000, NULL, task1_prio, NULL);
  xTaskCreate(vTask2, "vTask2", 10000, NULL, task2_prio, NULL);

  // Setup() fait partie d'une tâche dont on n'a plus besoin, maintenant que tout est lancé => elle s'autodétruit. 
  Serial.print("[INFO] Startup finished\n");
  TaskHandle_t setup_task_t = xTaskGetCurrentTaskHandle();
  vTaskDelete(setup_task_t); 
}




/*
 * indispensable pour pouvoir compiler même si inutile car la tâche arduino est tuée à la fin de setup
 */
void loop()
{

}
