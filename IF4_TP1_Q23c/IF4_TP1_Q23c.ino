/* Programme IF4_TP1_Q23c
 * Auteurs: Thomas Vadebout, Arnaud Lelevé
 * Version 2024.2
 */

  
// priorité des tâches 1 et 2
int taskHaute_prio = 20;
int taskInter_prio = 10;
int taskBasse_prio = 5;


// variables globales représentant les tâches 1 et 2
TaskHandle_t Task1, Task2, Task3;

SemaphoreHandle_t xSemaphore = NULL;
 
  
// Permet de specifier le niveau d'optimisation que le compilateur fera au code (-O0 = aucune optimisation)
// Pour plus d'informations sur l'optimisation du compilateur gcc : https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
#pragma GCC optimize("-O0")

void pass() {
  // Ne fait rien, mais prend tout de même un temps au 
  // processeur pour être traité si -O0 est spécifié.

  // Si -O1, -O2 ou -O3 est specifié, le compilateur 
  // supprimera cette partie car il voit qu'elle n'est 
  // pas utile

  // Le paramètre de compilateur -O permet de modifier 
  // le niveau d'optimisation du compilateur
}

void pass_more(){
    for (int i = 0 ; i<1000000;i++){
      pass();
    }
}

void doing_a_lot_of_useless_things_to_simulate_work(char * job_name, int how_many_work){
    for (int i = 0 ; i<how_many_work;i++){
      pass_more();
      Serial.printf("Travaille sur : %s\n", job_name);
    }
}


 
/******************************
 * code de la tâche HAUTE
 * ****************************
 */
void vTaskHaute( void *pvParameters )
{
  
  Serial.printf("tâche priorité HAUTE se lance / core %d\n", xPortGetCoreID());

  vTaskDelay(1000);

  long start_time = xTaskGetTickCount();
  long total_exec_time;

  Serial.printf("La tâche priorité HAUTE souhaite acceder à la ressource partagée\n");

  if (xSemaphoreTake( xSemaphore, portMAX_DELAY ) == pdTRUE ) {

      Serial.printf("La tâche priorité HAUTE accède à la ressource partagée\n");

      doing_a_lot_of_useless_things_to_simulate_work("HAUTE_PARTAGEE", 5);

      Serial.printf("La tâche priorité HAUTE a fini son travail avec la ressource partagée\n");
      
      total_exec_time = xTaskGetTickCount() - start_time;
      
      Serial.printf("La tâche priorité HAUTE a fini son travail en : %ld ms\n", total_exec_time);

      xSemaphoreGive( xSemaphore );

  }

  // si la tâche devait se terminer d'elle même, alors finir avec:
  vTaskDelete( NULL );
}


/******************************
 * code de la tâche BASSE
 * ****************************
 */
void vTaskBasse( void *pvParameters )
{

  Serial.printf("tâche priorité BASSE se lance / core %d\n", xPortGetCoreID());

  vTaskDelay(100);

  long start_time = xTaskGetTickCount();
  long total_exec_time;

  Serial.printf("La tâche priorité BASSE souhaite acceder à la ressource partagée\n");

  if (xSemaphoreTake( xSemaphore, portMAX_DELAY ) == pdTRUE ) {

      Serial.printf("La tâche priorité BASSE accède à la ressource partagée\n");

      doing_a_lot_of_useless_things_to_simulate_work("BASSE_PARTAGEE", 8);

      Serial.printf("La tâche priorité BASSE a fini son travail avec la ressource partagée\n");

      total_exec_time = xTaskGetTickCount() - start_time;

      Serial.printf("La tâche priorité BASSE a fini son travail en : %ld ms\n", total_exec_time);

      xSemaphoreGive( xSemaphore );

  }


  // si la tâche devait se terminer d'elle même, alors finir avec:
  vTaskDelete( NULL );
}


/******************************
 * code de la tâche INTERMEDIAIRE
 * ****************************
 */
void vTaskInter( void *pvParameters )
{
  
  Serial.printf("tâche priorité INTERMEDIAIRE se lance / core %d\n", xPortGetCoreID());

  vTaskDelay(500);

  long start_time = xTaskGetTickCount();

  Serial.printf("La tâche priorité INTERMEDIAIRE commence son travail\n");

  doing_a_lot_of_useless_things_to_simulate_work("INTERMEDIAIRE", 10);

  long total_exec_time = xTaskGetTickCount() - start_time;

  Serial.printf("La tâche priorité INTERMEDIAIRE a fini son travail en : %ld ms\n", total_exec_time);


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
  Serial.println("Start");
  

  xSemaphore = xSemaphoreCreateBinary();
  //xSemaphore = xSemaphoreCreateMutex();

  xSemaphoreGive( xSemaphore );

  xTaskCreatePinnedToCore( vTaskHaute, "TaskPrioHaute", 10000, NULL, taskHaute_prio, &Task1, 0);
  xTaskCreatePinnedToCore( vTaskBasse, "TaskPrioBasse", 10000, NULL, taskBasse_prio, &Task2, 0); 
  xTaskCreatePinnedToCore( vTaskInter, "TaskPrioInter", 10000, NULL, taskInter_prio, &Task3, 0); 
   
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