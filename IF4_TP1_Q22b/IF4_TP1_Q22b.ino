/* Programme IF4_TP1_Q22b
 * Auteurs: Thomas Vadebout, Arnaud Lelevé
 * Version 2024.2
 */
// périodes des tâches en ms
int task1_period = 10;
int task2_period = 20;

int nb_iterations = 10000;

// pins sondées à l'oscillo
// NE PAS OUBLIER LA MASSE (GND) !!
uint8_t oscillo1     = 23; //19;
uint8_t oscillo2     = 19; //5;


// priorité des tâches 1 et 2
int task1_prio = 1;
int task2_prio = 10;

// variables globales représentant les tâches 1 et 2
TaskHandle_t Task1, Task2;


/******************************
 * code de la tâche 1
 * ****************************
 */
void vTask1( void *pvParameters )
{
  bool etat = LOW;
  int i = 0;
  TickType_t xLastWakeTime = xTaskGetTickCount(); //mémorise l'heure actuelle une première fois
  
  Serial.printf("vTask1 / core %d started\n", xPortGetCoreID());
  
  // boucle infinie
  for( ;; )
  {    
    //simule du travail
    for( i = 0; i<nb_iterations; i++ ) {
      if (etat == HIGH)
        etat = LOW;
      else
        etat = HIGH;
      digitalWrite(oscillo1, etat);     
    }

    //endort la tâche pour qu'elle se réveille dans task1_period ms depuis dernier réveil
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( task1_period ) ); 
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
  int i;
  TickType_t xLastWakeTime = xTaskGetTickCount(); //mémorise l'heure actuelle une première fois


  Serial.printf("vTask2 / core %d started\n", xPortGetCoreID());
  
  // boucle infinie
  for( ;; )
  {
    //simule du travail
    for( i = 0; i<nb_iterations; i++ ) {
      if (etat == HIGH)
        etat = LOW;
      else
        etat = HIGH;
      digitalWrite(oscillo2, etat);      
    }

    //endort la tâche pour qu'elle se réveille dans task2_period ms depuis dernier réveil
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( task2_period ) ); // réveille dans task1_period ms depuis dernier réveil
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

  // declare 2 sorties TOR, celles accessibles à côté de la prise USB
  pinMode(oscillo1, OUTPUT);
  pinMode(oscillo2, OUTPUT);
  
  /* créé 1 tâche en forçant l'allocation au noyau 0 
   *  cf https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/
  
   *  vTask1,     Function to implement the task 
      "Task1",    Name of the task 
      10000,      Stack size in words 
      NULL,       Task input parameter 
      task1_prio, Priority of the task 
      &Task1,     Task handle. 
      0);         Core where the task should run 
  */
  xTaskCreatePinnedToCore( vTask1, "Task1", 10000, NULL, task1_prio, &Task1, 0);
  xTaskCreatePinnedToCore( vTask2, "Task2", 10000, NULL, task2_prio, &Task2, 1); 
   

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
