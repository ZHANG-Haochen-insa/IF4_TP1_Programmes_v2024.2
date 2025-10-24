/* Programme IF4_TP1_Q24
 * Auteurs: Thomas Vadebout, Arnaud Lelevé
 * Version 2024.2
 */



// pins sondées à l'oscillo
// mettre les masses sur une des broches GND, sonde voie 1 de l'oscilloscope sur broche 23 et sonde voie 2 sur la broche 19
#define oscillo1 23 
#define oscillo2 19

// priorité des tâches 1 et 2
#define task1_prio 20
#define task2_prio 10

// param oscillations
#define nb_iterations 10
#define GLOBAL_HALF_PERIOD_MS 5

// pins pour déclencher et détecter une int matérielle
// sortie à 3,3V qui alterne toutes les 300ms
#define out_high 12 
// entrée numérique liée à l'interuption 
#define in_intr  5  
 


// variables globales représentant les tâches 1 et 2
TaskHandle_t Task1, Task2;

// Interruption Prototypes;
void IRAM_ATTR interruptWheel1();

SemaphoreHandle_t xBinarySemaphore = NULL;

/**
 * Routine d'interuption
 **/
void IRAM_ATTR interruptRoutine()
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  Serial.println("[INFO] interruptRoutine()");
  xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );
  if( xHigherPriorityTaskWoken != pdFALSE )
    portYIELD_FROM_ISR();
}


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
    delay(half_period_ms)  ;  
  }
}
  
 
/******************************
 * code de la tâche 1
 * ****************************
 */
void vTask1( void *pvParameters )
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
    
  Serial.printf("[INFO] vTask1 / core %d started\n", xPortGetCoreID());

  for (;;) {
    
    
    // simule du boulot périodique   
    alternate(oscillo1, GLOBAL_HALF_PERIOD_MS);        
    
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1000 ) );
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
  Serial.printf("[INFO] vTask2 / core %d started\n", xPortGetCoreID());
  
  // boucle infinie
  for (;;) {
    
    // attend que l'interruption libère le sémaphore
    xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );
    Serial.printf("[INFO] vTask2 called\n");
    
    // simule du boulot     
    alternate(oscillo2, GLOBAL_HALF_PERIOD_MS); 
    
    
  /*  Serial.printf("vTask2 travaille de son côté\n", xPortGetCoreID());
    delay(10);
    
    xSemaphoreTake(xMutex, portMAX_DELAY);
    Serial.printf("vTask2 a verrouillé le mutex\n", xPortGetCoreID());

    // simule du boulot
    Serial.printf("vTask2 travaille sur la ressource partagée\n", xPortGetCoreID());
    alternate(oscillo2, 10);
    Serial.printf("vTask2 a fini de travailler sur la ressource partagée\n", xPortGetCoreID());
    
    Serial.printf("vTask2 déverrouille le mutex\n", xPortGetCoreID());
    xSemaphoreGive(xMutex);
 
    */
    vTaskDelay( pdMS_TO_TICKS( 10 ) );     
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
  
  pinMode(out_high, OUTPUT); // pour avoir un signal haut en permanence sur cette pin
  digitalWrite(out_high, HIGH); 
  
  digitalWrite(oscillo2, LOW); 

  xBinarySemaphore = xSemaphoreCreateBinary();
  
  pinMode(in_intr , INPUT);
  
  attachInterrupt(digitalPinToInterrupt(in_intr), interruptRoutine, CHANGE);
 
  
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
   

  // boucle infinie pour alterner toutes les 300ms la sortie out_high
  //FIXME: pas très propre d'exécuter ce code dans le setup...
  int current=0;
  for( ;; ) {
    
    digitalWrite(out_high, current);     
    Serial.printf("[INFO] current=%d et in=%d\n", current, digitalRead(in_intr));
    current = ! current;
    delay(300);
    }
}




// fonction appelée par défaut et en boucle indéfiniment par l'OS, inhibée quand loop ne finit jamais.
void loop()
{
}
